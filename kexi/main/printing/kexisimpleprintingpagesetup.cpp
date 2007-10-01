/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "kexisimpleprintingpagesetup.h"
#include "kexisimpleprintingpagesetupbase.h"
#include "kexisimpleprintpreviewwindow.h"

#include <core/KexiMainWindowIface.h>
#include <kexiutils/utils.h>
#include <kexi_version.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kfontdialog.h>
#include <kurllabel.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kprinter.h>
#include <kpushbutton.h>
#include <kdeversion.h>

#include <qlabel.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qpainter.h>
#include <qcheckbox.h>
#include <q3whatsthis.h>
#include <qtooltip.h>
//Added by qt3to4:
#include <Q3VBoxLayout>
#include <Q3ValueList>

#include <kexiutils/tristate.h>

KexiSimplePrintingCommand::KexiSimplePrintingCommand(
	KexiMainWindow* mainWin, int objectId, QObject* parent)
 : QObject(parent)
 , m_previewEngine(0)
 , m_mainWin(mainWin)
 , m_objectId(objectId)
 , m_previewWindow(0)
 , m_printPreviewNeedsReloading(false)
{
	setObjectName("KexiSimplePrintCommand");
	connect(this, SIGNAL(showPageSetupRequested(KexiPart::Item*)), 
		m_mainWin, SLOT(showPageSetupForItem(KexiPart::Item*)));
}

KexiSimplePrintingCommand::~KexiSimplePrintingCommand()
{
	delete m_previewWindow;
	delete m_previewEngine;
//	delete m_settings;
}


bool KexiSimplePrintingCommand::print(const KexiSimplePrintingSettings& settings, 
	const QString& aTitleText)
{
	m_settings = settings;
	return print(aTitleText);
}

bool KexiSimplePrintingCommand::print(const QString& aTitleText)
{
	KexiDB::Connection *conn = m_mainWin->project()->dbConnection();
	KexiDB::TableOrQuerySchema tableOrQuery(conn, m_objectId);
	if (!tableOrQuery.table() && !tableOrQuery.query()) {
//! @todo item not found
		return false;
	}
	QString titleText(aTitleText.trimmed());
	if (titleText.isEmpty())
		titleText = tableOrQuery.captionOrName();

	KexiSimplePrintingEngine engine(m_settings, this);
	QString errorMessage;
	if (!engine.init(*conn, tableOrQuery, titleText, errorMessage)) {
		if (!errorMessage.isEmpty())
			KMessageBox::sorry(m_mainWin, errorMessage, i18n("Printing"));
		return false;
	}

	//setup printing
#ifdef Q_WS_WIN
	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation( m_settings.pageLayout.orientation == PG_PORTRAIT 
		? QPrinter::Portrait : QPrinter::Landscape );
	printer.setPageSize( 
		(QPrinter::PageSize)KoPageFormat::printerPageSize( m_settings.pageLayout.format ) );

	// "chicken-egg" problem: 
	// we cannot use real from/to values in setMinMax() and setFromTo() 
	// because page count is known after obtaining print settings
	printer.setFromTo(1,1);
#else
	KPrinter printer;
	printer.setOrientation( m_settings.pageLayout.orientation == PG_PORTRAIT 
		? KPrinter::Portrait : KPrinter::Landscape );
	printer.setPageSize( 
		(KPrinter::PageSize)KoPageFormat::printerPageSize( m_settings.pageLayout.format ) );
#endif

	printer.setFullPage(true);
	QString docName( titleText );
	printer.setDocName( docName );
	printer.setCreator(KEXI_APP_NAME);
	if ( !printer.setup( m_mainWin ) ) {
		return true;
	}

	// now we have final settings

//! @todo get printer.pageOrder() (for reversed order requires improved engine)
	QPainter painter;

	if (!painter.begin(&printer)) {
//! @todo msg
		return false;
	}
	engine.calculatePagesCount(painter);

	uint loops, loopsPerPage;
	Q3ValueList<int> pagesToPrint;
	int fromPage = 0;
#ifdef Q_WS_WIN
	int toPage = 0;
	if (QPrinter::PageRange == printer.printRange()) {
		fromPage = printer.fromPage();
		toPage = printer.toPage();
	}
	if (fromPage==0 || toPage==0) {
		fromPage = 0;
		toPage = (int)engine.pagesCount()-1;
	}
	else {
		fromPage--;
		if (toPage > (int)engine.pagesCount())
			toPage = (int)engine.pagesCount();
		toPage--;
	}
	// win32 only supports one range, build the list
	for (int i = fromPage; i<=toPage; i++) {
		pagesToPrint.append(i);
	}
	// on win32 the OS does perform buffering (only when collation is off, each copy needs to be repeated)
	loops = 1;
	loopsPerPage = printer.collateCopies() ? 1 : printer.numCopies();
#else
	// on !win32 print QPrinter::numCopies() times (the OS does not perform buffering)
	pagesToPrint = printer.pageList();
	kDebug() << pagesToPrint << endl;
	if (pagesToPrint.isEmpty()) {
		fromPage = 0;
		for (int i = 0; i<(int)engine.pagesCount(); i++) {
			pagesToPrint.append(i);
		}
	}
	else
		fromPage = pagesToPrint.first();
	if (printer.collate()==KPrinter::Collate) {
		//collation: p1, p2,..pn; p1, p2,..pn; ......; p1, p2,..pn
		loops = printer.numCopies();
		loopsPerPage = 1;
	}
	else {
		//no collation: p1, p1, ..., p1; p2, p2, ..., p2; ......; pn, pn,..pn
		loops = 1; 
		loopsPerPage = printer.numCopies();
	}
//! @todo also look at printer.pageSet() option : all/odd/even pages
#endif
	// now, total number of printed pages is printer.numCopies()*printer.pageList().count()

	kDebug() << "printing..." << endl;
	bool firstPage = true;
	for (uint copy = 0;copy < loops; copy++) {
		kDebug() << "copy " << (copy+1) << " of " << loops << endl;
		uint pageNumber = fromPage;
		Q3ValueList<int>::ConstIterator pagesIt = pagesToPrint.constBegin();
		for(;(int)pageNumber == fromPage || !engine.atEnd(); ++pageNumber) {
			kDebug() << "printing..." << endl;
			if (pagesIt == pagesToPrint.constEnd()) //no more pages to print
				break;
			if ((int)pageNumber < *pagesIt) { //skip pages without printing (needed for computation)
				engine.paintPage(pageNumber, painter, false);
				continue;
			}
			if (*pagesIt < (int)pageNumber) { //sanity
				++pagesIt;
				continue;
			}
			for (uint onePageCounter = 0; onePageCounter < loopsPerPage; onePageCounter++) {
				if (!firstPage)
					printer.newPage();
				else
					firstPage = false;
				kDebug() << "page #" << pageNumber << endl;
				engine.paintPage(pageNumber, painter);
			}
			++pagesIt;
		}
	}
	kDebug() << "end of printing." << endl;

	// stop painting, this will automatically send the print data to the printer
	if (!painter.end())
		return false;

	if (!engine.done())
		return false;

	return true;
}

bool KexiSimplePrintingCommand::showPrintPreview(const KexiSimplePrintingSettings& settings, 
	const QString& aTitleText, bool reload)
{
	m_settings = settings;
	if (!m_previewEngine)
		m_previewEngine = new KexiSimplePrintingEngine(m_settings, this);

	if (reload)
		m_printPreviewNeedsReloading = true;

	bool backToPage0 = true;
	QString titleText(aTitleText.trimmed());
	KexiDB::Connection *conn = m_mainWin->project()->dbConnection();
	KexiDB::TableOrQuerySchema tableOrQuery(conn, m_objectId);
	if (!tableOrQuery.table() && !tableOrQuery.query()) {
//! @todo item not found
		return false;
	}
	if (titleText.isEmpty())
		titleText = tableOrQuery.captionOrName();
	if (!m_previewWindow || m_printPreviewNeedsReloading) {
		QString errorMessage;
		if (!m_previewEngine->init(
			*conn, tableOrQuery, titleText, errorMessage)) {
			if (!errorMessage.isEmpty())
				KMessageBox::sorry(m_mainWin, errorMessage, i18n("Print Preview")); 
			return false;
		}
	}
	if (!m_previewWindow) {
		backToPage0 = false;
		m_previewWindow = new KexiSimplePrintPreviewWindow(
			*m_previewEngine, tableOrQuery.captionOrName(), 0, 
			Qt::WStyle_Customize|Qt::WStyle_NormalBorder|Qt::WStyle_Title|
			Qt::WStyle_SysMenu|Qt::WStyle_MinMax|Qt::WStyle_ContextHelp);
		connect(m_previewWindow, SIGNAL(printRequested()), this, SLOT(print()));
		connect(m_previewWindow, SIGNAL(pageSetupRequested()), this, SLOT(slotShowPageSetupRequested()));
		m_previewWindow->show();
		KDialog::centerOnScreen(m_previewWindow);
		m_printPreviewNeedsReloading = false;
	}

	if (m_printPreviewNeedsReloading) {//dirty
		m_previewEngine->clear();
//! @todo progress bar...
		m_previewEngine->setTitleText( titleText );
		m_previewWindow->setFullWidth();
		m_previewWindow->updatePagesCount();
		m_printPreviewNeedsReloading = false;
	}
	if (backToPage0)
		m_previewWindow->goToPage(0);
	m_previewWindow->show();
	m_previewWindow->raise();
//	m_previewWindow->setPagesCount(INT_MAX); //will be properly set on demand
	return true;
}

void KexiSimplePrintingCommand::slotShowPageSetupRequested()
{
	m_mainWin->raise();
	emit showPageSetupRequested( m_mainWin->project()->item( m_objectId ) );
}

/*void KexiSimplePrintingCommand::setPrintPreviewNeedsReloading()
{
	m_printPreviewNeedsReloading = true;
}*/

//----------------------------------------------------------

KexiSimplePrintingPageSetup::KexiSimplePrintingPageSetup( KexiMainWindow *mainWin, QWidget *parent, 
	QMap<QString,QString>* args )
	: KexiView( mainWin, parent, "KexiSimplePrintingPageSetup" )
	, m_settings( KexiSimplePrintingSettings::load() )
//	, m_command(0)
{
	// object to print
	bool ok = args;
	int objectId;
	if (ok)
		objectId = (*args)["identifier"].toInt();
	ok = objectId<=0;
	m_item = mainWin->project()->item( objectId );
	ok = m_item;

	bool printPreview = false;
	bool print = false;
	bool pageSetup = false;
	if (ok) {
		printPreview = (*args)["action"]=="printPreview";
		print = (*args)["action"]=="print";
		pageSetup = (*args)["action"]=="pageSetup";
		ok = printPreview || print || pageSetup;
	}

	// settings
//! @todo default?
	m_unit = KLocale::Metric == KGlobal::locale()->measureSystem() ? KoUnit::Centimeter : KoUnit::Inch;

	// GUI
	Q3VBoxLayout *lyr = new Q3VBoxLayout(this);
	m_contents = new KexiSimplePrintingPageSetupBase(this, "KexiSimplePrintingPageSetupBase");
	lyr->addWidget(m_contents);

	setViewWidget(m_contents, true);
//	setFocusPolicy(WheelFocus);
	m_contents->setFocusProxy(m_contents->headerTitleLineEdit);

	m_contents->printButton->setIconSet( KStandardGuiItem::print().iconSet(KIconLoader::Small) );
	m_contents->printButton->setText( KStandardGuiItem::print().text() );
	connect(m_contents->printButton, SIGNAL(clicked()), this, SLOT(print()));

	m_contents->printPreviewButton->setIconSet( KIcon("document-print-preview") );
	m_contents->printPreviewButton->setText( i18n("Print Previe&w...") );
	connect(m_contents->printPreviewButton, SIGNAL(clicked()), this, SLOT(printPreview()));

	m_contents->iconLabel->setFixedWidth(32+6);
	m_contents->iconLabel->setPixmap( DesktopIcon("document", 32) );
	Q3WhatsThis::add(m_contents->headerTitleFontButton, i18n("Changes font for title text."));
	connect(m_contents->headerTitleFontButton, SIGNAL(clicked()), 
		this, SLOT(slotChangeTitleFont()));

	if (m_item) {
		m_origCaptionLabelText = m_contents->captionLabel->text();
		m_contents->headerTitleLineEdit->setText( m_item->captionOrName() );
		if (m_item->mimeType()=="kexi/query") {
			m_contents->openDataLink->setText( i18n("Open This Query") );
			m_origCaptionLabelText = I18N_NOOP("<h2>Page setup for printing \"%1\" query data</h2>");
		}
		m_contents->captionLabel->setText( m_origCaptionLabelText.arg(m_item->name()) );
	}
	connect(m_contents->headerTitleLineEdit,SIGNAL(textChanged(const QString&)), 
		this, SLOT(slotTitleTextChanged(const QString&)));
	m_contents->headerTitleLineEdit->setFont( m_settings.pageTitleFont );

	Q3WhatsThis::add(m_contents->openDataLink, 
		i18n("Shows data for table or query associated with this page setup."));
	m_contents->openDataLink->setToolTip( 
		i18n("Shows data for table or query associated with this page setup."));
	connect(m_contents->openDataLink, SIGNAL(leftClickedUrl()), this, SLOT(slotOpenData())); 

	Q3WhatsThis::add(m_contents->saveSetupLink, i18n("Saves settings for this setup as default."));
	connect(m_contents->saveSetupLink, SIGNAL(leftClickedUrl()), this, SLOT(slotSaveSetup()));
#if !KDE_IS_VERSION(3,5,1) && !defined(Q_WS_WIN)
	//a fix for problems with focusable KUrlLabel on KDElibs<=3.5.0
	m_contents->openDataLink->setFocusPolicy(NoFocus);
	m_contents->saveSetupLink->setFocusPolicy(NoFocus);
#endif

	Q3WhatsThis::add(m_contents->addDateTimeCheckbox, i18n("Adds date and time to the header."));
	Q3WhatsThis::add(m_contents->addPageNumbersCheckbox, i18n("Adds page numbers to the footer."));
	Q3WhatsThis::add(m_contents->addTableBordersCheckbox, i18n("Adds table borders."));
	
#ifdef KEXI_NO_UNFINISHED 
	m_contents->addDateTimeCheckbox->hide();
	m_contents->addPageNumbersCheckbox->hide();
#endif

	updatePageLayoutAndUnitInfo();
	Q3WhatsThis::add(m_contents->changePageSizeAndMarginsButton, 
		i18n("Changes page size and margins."));
	connect(m_contents->changePageSizeAndMarginsButton, SIGNAL(clicked()), 
		this, SLOT(slotChangePageSizeAndMargins()));

	connect(m_contents->addPageNumbersCheckbox, SIGNAL(toggled(bool)), 
		this, SLOT(slotAddPageNumbersCheckboxToggled(bool)));
	connect(m_contents->addDateTimeCheckbox, SIGNAL(toggled(bool)), 
		this, SLOT(slotAddDateTimeCheckboxToggled(bool)));
	connect(m_contents->addTableBordersCheckbox, SIGNAL(toggled(bool)), 
		this, SLOT(slotAddTableBordersCheckboxToggled(bool)));

	if (!ok) {
		// no data!
		setEnabled(false);
	}

	m_contents->addPageNumbersCheckbox->setChecked( m_settings.addPageNumbers );
	m_contents->addDateTimeCheckbox->setChecked( m_settings.addDateAndTime );
	m_contents->addTableBordersCheckbox->setChecked( m_settings.addTableBorders );
	setDirty(false);

//	m_engine = new KexiSimplePrintingEngine(m_settings, this);

	//clear it back to false after widgets initialization
	m_printPreviewNeedsReloading = false;

/*	if (printPreview)
		QTimer::singleShot(50, this, SLOT(printPreview()));
	else if (print)
		QTimer::singleShot(50, this, SLOT(print()));*/
	connect(this, SIGNAL(printItemRequested(KexiPart::Item*,const KexiSimplePrintingSettings&,
		const QString&)),
		m_mainWin, SLOT(printItem(KexiPart::Item*,const KexiSimplePrintingSettings&,
		const QString&)));
	connect(this, SIGNAL(printPreviewForItemRequested(KexiPart::Item*, 
		const KexiSimplePrintingSettings&,const QString&,bool)),
		m_mainWin, SLOT(printPreviewForItem(KexiPart::Item*, 
		const KexiSimplePrintingSettings&,const QString&,bool)));
}

KexiSimplePrintingPageSetup::~KexiSimplePrintingPageSetup()
{
}

void KexiSimplePrintingPageSetup::slotSaveSetup()
{
	m_settings.save();
	setDirty(false);
}

void KexiSimplePrintingPageSetup::updatePageLayoutAndUnitInfo()
{
	QString s;
	if (m_settings.pageLayout.format == PG_CUSTOM) {
		s += QString(" (%1 %2 x %3 %4)")
			.arg(m_settings.pageLayout.ptWidth).arg(KoUnit::unitName(m_unit))
			.arg(m_settings.pageLayout.ptHeight).arg(KoUnit::unitName(m_unit));
	}
	else
		s += KoPageFormat::name(m_settings.pageLayout.format);
	s += QString(", ")
	 + (m_settings.pageLayout.orientation == PG_PORTRAIT ? i18n("Portrait") : i18n("Landscape"))
	 + ", " + i18n("margins:")
	 + " " + KoUnit::toUserStringValue(m_settings.pageLayout.ptLeft, m_unit)
	 + "/" + KoUnit::toUserStringValue(m_settings.pageLayout.ptRight, m_unit)
	 + "/" + KoUnit::toUserStringValue(m_settings.pageLayout.ptTop, m_unit)
	 + "/" + KoUnit::toUserStringValue(m_settings.pageLayout.ptBottom, m_unit)
	 + " " + KoUnit::unitName(m_unit);
	m_contents->pageSizeAndMarginsLabel->setText( s );
}

/*void KexiSimplePrintingPageSetup::setupPrintingCommand()
{
	if (!m_command) {
		m_command = new KexiSimplePrintingCommand(
			m_mainWin, m_item->identifier(), m_settings, false/!owned/, this);
	}
}*/

void KexiSimplePrintingPageSetup::print()
{
//	setupPrintingCommand();
//	m_command->print(m_contents->headerTitleLineEdit->text());
	emit printItemRequested(m_item, m_settings, m_contents->headerTitleLineEdit->text());
}

void KexiSimplePrintingPageSetup::printPreview()
{
//	setupPrintingCommand();
//	m_command->showPrintPreview(m_contents->headerTitleLineEdit->text());
	emit printPreviewForItemRequested(m_item, m_settings, 
		m_contents->headerTitleLineEdit->text(), m_printPreviewNeedsReloading);
	m_printPreviewNeedsReloading = false;
}

void KexiSimplePrintingPageSetup::slotOpenData()
{
	bool openingCancelled;
	KexiMainWindowIface::global()->openObject(m_item, Kexi::DataViewMode, openingCancelled);
}

void KexiSimplePrintingPageSetup::slotTitleTextChanged(const QString&)
{
	if (m_contents->headerTitleLineEdit->isModified()) {
		m_printPreviewNeedsReloading = true;
//		if (m_command)
//			m_command->setPrintPreviewNeedsReloading();
	}
		
	m_contents->headerTitleLineEdit->clearModified();
}

void KexiSimplePrintingPageSetup::slotChangeTitleFont()
{
	if (QDialog::Accepted != KFontDialog::getFont(m_settings.pageTitleFont, false, this))
		return;
	m_contents->headerTitleLineEdit->setFont( m_settings.pageTitleFont );
	setDirty(true);
}

void KexiSimplePrintingPageSetup::slotChangePageSizeAndMargins()
{
	KoHeadFoot headfoot; //dummy

#ifdef __GNUC__
#warning TODO reenable when KoPageLayoutDia is ported
#else
#pragma WARNING( TODO reenable when KoPageLayoutDia is ported )
#endif
#if 0//TODO
	if (int(QDialog::Accepted) != KoPageLayoutDia::pageLayout( 
		m_settings.pageLayout, headfoot, FORMAT_AND_BORDERS | DISABLE_UNIT, m_unit, this ))
#endif
		return;

	//update
	updatePageLayoutAndUnitInfo();
	setDirty(true);
}

void KexiSimplePrintingPageSetup::setDirty(bool set)
{
	m_contents->saveSetupLink->setEnabled(set);
//	if (m_command)
//		m_command->setPrintPreviewNeedsReloading();
	if (set)
		m_printPreviewNeedsReloading = true;
}

void KexiSimplePrintingPageSetup::slotAddPageNumbersCheckboxToggled(bool set)
{
	m_settings.addPageNumbers = set;
	setDirty(true);
}

void KexiSimplePrintingPageSetup::slotAddDateTimeCheckboxToggled(bool set)
{
	m_settings.addDateAndTime = set;
	setDirty(true);
}

void KexiSimplePrintingPageSetup::slotAddTableBordersCheckboxToggled(bool set)
{
	m_settings.addTableBorders = set;
	setDirty(true);
}

#include "kexisimpleprintingpagesetup.moc"
