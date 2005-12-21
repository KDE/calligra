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

#include "kexisimpleprintingwindow.h"
#include "kexisimpleprintingwindowbase.h"
#include "kexisimpleprintpreviewwindow.h"

#include <core/keximainwindow.h>
#include <kexiutils/utils.h>

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
#include <qwhatsthis.h>

#include <kexiutils/tristate.h>

KexiSimplePrintingWindow::KexiSimplePrintingWindow( KexiMainWindow *mainWin, QWidget *parent, 
	QMap<QString,QString>* args )
	: KexiViewBase( mainWin, parent, "KexiSimplePrintingWindow" )
	, m_previewWindow(0)
	, m_printPreviewNeedsReloading(false)
{
	loadSetup();
	// object to print
	bool ok = args;
	int objectId;
	if (ok)
		objectId = (*args)["identifier"].toInt();
	ok = objectId<=0;
	m_item = mainWin->project()->item( objectId );
	ok = m_item;

	m_preview = false;
	if (ok)
		m_preview = (*args)["printPreview"]=="1";

	// settings
//! @todo default?
	m_unit = KLocale::Metric == KGlobal::locale()->measureSystem() ? KoUnit::U_CM : KoUnit::U_INCH;

	// GUI
	QVBoxLayout *lyr = new QVBoxLayout(this);
	m_contents = new KexiSimplePrintingWindowBase(this, "KexiSimplePrintingWindowBase");
	lyr->addWidget(m_contents);

	setViewWidget(m_contents, true);
//	setFocusPolicy(WheelFocus);
	m_contents->setFocusProxy(m_contents->headerTitleLineEdit);

	m_contents->printButton->setIconSet( KStdGuiItem::print().iconSet(KIcon::Small) );
	m_contents->printButton->setText( KStdGuiItem::print().text() );
	connect(m_contents->printButton, SIGNAL(clicked()), this, SLOT(print()));

	m_contents->printPreviewButton->setIconSet( SmallIconSet("filequickprint") );
	m_contents->printPreviewButton->setText( i18n("Print Previe&w...") );
	connect(m_contents->printPreviewButton, SIGNAL(clicked()), this, SLOT(printPreview()));

	m_contents->iconLabel->setFixedWidth(32+6);
	m_contents->iconLabel->setPixmap( DesktopIcon("document", 32) );
	QWhatsThis::add(m_contents->headerTitleFontButton, i18n("Changes font for title text."));
	connect(m_contents->headerTitleFontButton, SIGNAL(clicked()), 
		this, SLOT(slotChangeTitleFont()));

	if (m_item) {
		m_origCaptionLabelText = m_contents->captionLabel->text();
		m_contents->headerTitleLineEdit->setText( m_item->captionOrName() );
		if (m_item->mimeType()=="kexi/query") {
			m_contents->openDataLink->setText( i18n("Open This Query") );
			m_origCaptionLabelText = i18n("<h2>Page setup for printing \"%1\" query data</h2>");
		}
		m_contents->captionLabel->setText( m_origCaptionLabelText.arg(m_item->name()) );
	}
	m_contents->headerTitleLineEdit->setFont( m_settings.pageTitleFont );

	QWhatsThis::add(m_contents->openDataLink, 
		i18n("Shows data for table or query associated with this page setup."));
	connect(m_contents->openDataLink, SIGNAL(leftClickedURL()), this, SLOT(slotOpenData())); 

	QWhatsThis::add(m_contents->saveSetupLink, i18n("Saves settings for this setup as default."));
	connect(m_contents->saveSetupLink, SIGNAL(leftClickedURL()), this, SLOT(slotSaveSetup()));
#if !KDE_IS_VERSION(3,5,1) && !defined(Q_WS_WIN)
	//a fix for problems with focusable KUrlLabel on KDElibs<=3.5.0
	m_contents->openDataLink->setFocusPolicy(NoFocus);
	m_contents->saveSetupLink->setFocusPolicy(NoFocus);
#endif

	QWhatsThis::add(m_contents->addDateTimeCheckbox, i18n("Adds date and time to the header."));
	QWhatsThis::add(m_contents->addPageNumbersCheckbox, i18n("Adds page numbers to the footer."));
	QWhatsThis::add(m_contents->addTableBordersCheckbox, i18n("Adds table borders."));
	
#ifdef KEXI_NO_UNFINISHED 
	m_contents->addDateTimeCheckbox->hide();
	m_contents->addPageNumbersCheckbox->hide();
#endif

	updatePageLayoutAndUnitInfo();
	QWhatsThis::add(m_contents->changePageSizeAndMarginsButton, 
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

	m_engine = new KexiSimplePrintingEngine(m_settings, this);

	if (m_preview)
		QTimer::singleShot(50, this, SLOT(printPreview()));
	else
		QTimer::singleShot(50, this, SLOT(print()));
}

KexiSimplePrintingWindow::~KexiSimplePrintingWindow()
{
	delete m_previewWindow;
}

void KexiSimplePrintingWindow::loadSetup()
{
	m_settings = KexiSimplePrintingSettings(); //this will set defaults

	KConfig *config = kapp->config();
	config->setGroup("Simple Printing");
	if (config->hasKey("pageTitleFont"))
		m_settings.pageTitleFont = config->readFontEntry("pageTitleFont");
//! @todo system default?
	if (config->hasKey("pageFormat"))
		m_settings.pageLayout.format = KoPageFormat::formatFromString( 
			config->readEntry("pageFormat" ) );
	if (config->readEntry("pageOrientation", "portrait").lower()=="landscape")
		m_settings.pageLayout.orientation = PG_LANDSCAPE;
	else
		m_settings.pageLayout.orientation = PG_PORTRAIT;
	if (config->hasKey("pageWidth"))
		m_settings.pageLayout.ptWidth = config->readDoubleNumEntry("pageWidth");
	if (config->hasKey("pageHeight"))
		m_settings.pageLayout.ptHeight = config->readDoubleNumEntry("pageHeight");
	if (config->hasKey("pageLeftMargin"))
		m_settings.pageLayout.ptLeft = config->readDoubleNumEntry("pageLeftMargin");
	if (config->hasKey("pageRightMargin"))
		m_settings.pageLayout.ptRight = config->readDoubleNumEntry("pageRightMargin");
	if (config->hasKey("pageTopMargin"))
		m_settings.pageLayout.ptTop = config->readDoubleNumEntry("pageTopMargin");
	if (config->hasKey("pageBottomMargin"))
		m_settings.pageLayout.ptBottom = config->readDoubleNumEntry("pageBottomMargin");
	m_settings.addPageNumbers = config->readBoolEntry("addPageNumbersToPage", true);
	m_settings.addDateAndTime = config->readBoolEntry("addDateAndTimePage", true);
	m_settings.addTableBorders = config->readBoolEntry("addTableBorders", false);
}

void KexiSimplePrintingWindow::slotSaveSetup()
{
	KConfig *config = kapp->config();
	config->setGroup("Simple Printing");
	config->writeEntry( "pageTitleFont", m_settings.pageTitleFont );
	config->writeEntry( "pageFormat", KoPageFormat::formatString( m_settings.pageLayout.format ) );
	config->writeEntry("pageOrientation", 
		m_settings.pageLayout.orientation == PG_PORTRAIT ? "portrait" : "landscape");
	config->writeEntry("pageWidth", m_settings.pageLayout.ptWidth);
	config->writeEntry("pageHeight", m_settings.pageLayout.ptHeight);
	config->writeEntry("pageLeftMargin", m_settings.pageLayout.ptLeft);
	config->writeEntry("pageRightMargin", m_settings.pageLayout.ptRight);
	config->writeEntry("pageTopMargin", m_settings.pageLayout.ptTop);
	config->writeEntry("pageBottomMargin", m_settings.pageLayout.ptBottom);
	config->writeEntry("addPageNumbersToPage", m_settings.addPageNumbers);
	config->writeEntry("addDateAndTimePage", m_settings.addDateAndTime);
	config->writeEntry("addTableBorders", m_settings.addTableBorders);
	config->sync();
	setDirty(false);
}

void KexiSimplePrintingWindow::updatePageLayoutAndUnitInfo()
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

void KexiSimplePrintingWindow::print()
{
	KexiDB::Connection *conn = m_mainWin->project()->dbConnection();
	KexiDB::TableOrQuerySchema tableOrQuery(conn, m_item->identifier());

	KexiSimplePrintingEngine engine(m_settings, this);
	QString errorMessage;
	if (!engine.init(*conn, tableOrQuery, errorMessage)) {
		if (!errorMessage.isEmpty())
			KMessageBox::sorry(this, errorMessage, i18n("Printing")); 
		return;
	}

	//setup printing
#ifdef Q_WS_WIN
	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation( m_settings.pageLayout.orientation == PG_PORTRAIT 
		? QPrinter::Portrait : QPrinter::Landscape );
	printer.setPageSize( 
		(KPrinter::PageSize)KoPageFormat::printerPageSize( m_settings.pageLayout.format ) );
#else
	KPrinter printer;
	printer.setOrientation( m_settings.pageLayout.orientation == PG_PORTRAIT 
		? KPrinter::Portrait : KPrinter::Landscape );
	printer.setPageSize( 
		(KPrinter::PageSize)KoPageFormat::printerPageSize( m_settings.pageLayout.format ) );
#endif
	if ( !printer.setup( this ) ) {
//! @todo msg
		return;
	}
	QPainter painter;

	if (!painter.begin(&printer)) {
//! @todo msg
		return;
	}

	uint pageNumber = 0;
	for(;!engine.eof(); ++pageNumber) {
		if (pageNumber>0)
			printer.newPage();
		if (!engine.paintPage(pageNumber, painter)) {
			printer.abort();
			break;
		}
	}

	// stop painting, this will automatically send the print data to the printer
	if (!painter.end())
		return;

	engine.done();
}

void KexiSimplePrintingWindow::printPreview()
{
	if (!m_previewWindow) {
		KexiDB::Connection *conn = m_mainWin->project()->dbConnection();
		KexiDB::TableOrQuerySchema tableOrQuery(conn, m_item->identifier());
		QString errorMessage;
		if (!m_engine->init(*conn, tableOrQuery, errorMessage)) {
			if (!errorMessage.isEmpty())
				KMessageBox::sorry(this, errorMessage, i18n("Print Preview")); 
			return;
		}
		m_previewWindow = new KexiSimplePrintPreviewWindow(
			*m_engine, m_item->captionOrName(), 0, 
			Qt::WStyle_Customize|Qt::WStyle_NormalBorder|Qt::WStyle_Title|
			Qt::WStyle_SysMenu|Qt::WStyle_MinMax|Qt::WStyle_ContextHelp);
		connect(m_previewWindow, SIGNAL(printRequested()), this, SLOT(print()));
		m_previewWindow->show();
		KDialog::centerOnScreen(m_previewWindow);
	}

	if (m_printPreviewNeedsReloading) {//dirty
		m_engine->clear();
		m_previewWindow->setFullWidth();
		m_previewWindow->goToPage(0);
		m_printPreviewNeedsReloading = false;
	}

	m_previewWindow->show();
	m_previewWindow->raise();
	m_previewWindow->setPagesCount(INT_MAX); //will be properly set on demand
}

void KexiSimplePrintingWindow::slotOpenData()
{
	m_mainWin->openObject(m_item);
}

void KexiSimplePrintingWindow::slotChangeTitleFont()
{
	if (QDialog::Accepted != KFontDialog::getFont(m_settings.pageTitleFont, false, this))
		return;
	m_contents->headerTitleLineEdit->setFont( m_settings.pageTitleFont );
	setDirty(true);
}

void KexiSimplePrintingWindow::slotChangePageSizeAndMargins()
{
	KoHeadFoot headfoot; //dummy

	if (QDialog::Accepted != KoPageLayoutDia::pageLayout( 
		m_settings.pageLayout, headfoot, FORMAT_AND_BORDERS | DISABLE_UNIT, m_unit, this ))
		return;

	//update
	updatePageLayoutAndUnitInfo();
	setDirty(true);
}

void KexiSimplePrintingWindow::setDirty(bool set)
{
	m_contents->saveSetupLink->setEnabled(set);
	m_printPreviewNeedsReloading = true;
}

void KexiSimplePrintingWindow::slotAddPageNumbersCheckboxToggled(bool set)
{
	m_settings.addPageNumbers = set;
	setDirty(true);
}

void KexiSimplePrintingWindow::slotAddDateTimeCheckboxToggled(bool set)
{
	m_settings.addDateAndTime = set;
	setDirty(true);
}

void KexiSimplePrintingWindow::slotAddTableBordersCheckboxToggled(bool set)
{
	m_settings.addTableBorders = set;
	setDirty(true);
}

#include "kexisimpleprintingwindow.moc"
