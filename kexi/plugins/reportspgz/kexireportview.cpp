/*
 * Kexi Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)                  
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */
#include "kexireportview.h"
#include <qlabel.h>
#include <QBoxLayout>
#include <QScrollArea>
#include <qlayout.h>
#include <kdebug.h>
#include <qprinter.h>
#include <core/KexiMainWindowIface.h>
#include "kexireportpage.h"
#include <orprerender.h>
#include <orprintrender.h>
#include <renderobjects.h>
#include <QPainter>
#include <QPrintDialog>
#include <widget/utils/kexirecordnavigator.h>
#include <core/KexiWindow.h>

#ifdef HAVE_KSPREAD
  #include <krkspreadrender.h>
#endif 


#include <krhtmlrender.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>

KexiReportView::KexiReportView(QWidget *parent)
 : KexiView(parent)
{	
	rpt = 0;
	setObjectName("KexiReportDesigner_DataView");
	scr = new QScrollArea(this);
	scr->setBackgroundRole(QPalette::Dark);
	scr->viewport()->setAutoFillBackground(true);	

	pageSelector = new KexiRecordNavigator(this, 0);
	layout()->addWidget(scr);
	layout()->addWidget(pageSelector);

	pageSelector->setRecordCount(0);
	pageSelector->setInsertingButtonVisible(false);
	pageSelector->setLabelText(i18n("Page"));
	
	
	// -- setup local actions
	QList<QAction*> viewActions;
	QAction* a;
	viewActions << (a = new KAction(KIcon("printer"), i18n("Print"), this ));
	a->setObjectName("pgzkexirpt_print_report");
	a->setToolTip(i18n("Print Report"));
	a->setWhatsThis(i18n("Prints the current report."));
	connect(a, SIGNAL(triggered()), this, SLOT(slotPrintReport()));

	viewActions << (a = new KAction(KIcon("kword"), i18n("Open in KWord"), this ));
	a->setObjectName("pgzkexirpt_open_kword");
	a->setToolTip(i18n("Open the report in KWord"));
	a->setWhatsThis(i18n("Opens the current report in KWord."));
	a->setEnabled(false);
	connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKWord()));
	
	#ifdef HAVE_KSPREAD
	viewActions << (a = new KAction(KIcon("kspread"), i18n("Open in KSpread"), this ));
	a->setObjectName("pgzkexirpt_open_kspread");
	a->setToolTip(i18n("Open the report in KSpread"));
	a->setWhatsThis(i18n("Opens the current report in KSpread."));
	a->setEnabled(true);
	connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKSpread()));
	#endif

	viewActions << (a = new KAction(KIcon("text-html"), i18n("Export to HTML"), this ));
	a->setObjectName("pgzkexirpt_export_html");
	a->setToolTip(i18n("Export the report to HTML"));
	a->setWhatsThis(i18n("Exports the report to a HTML file."));
	a->setEnabled(true);
	connect(a, SIGNAL(triggered()), this, SLOT(slotExportHTML()));
	
	setViewActions(viewActions);
	
	
	connect(pageSelector, SIGNAL(nextButtonClicked()), this, SLOT(nextPage()));
	connect(pageSelector, SIGNAL(prevButtonClicked()), this, SLOT(prevPage()));
	connect(pageSelector, SIGNAL(firstButtonClicked()), this, SLOT(firstPage()));
	connect(pageSelector, SIGNAL(lastButtonClicked()), this, SLOT(lastPage()));
	
}

KexiReportView::~KexiReportView()
{
}

void KexiReportView::nextPage()
{
	if (curPage < pageCount )
	{
		curPage++;
		rptwid->renderPage(curPage);
		pageSelector->setCurrentRecordNumber(curPage);
	}
}

void KexiReportView::prevPage()
{
	if (curPage > 1 )
	{
		curPage--;
		rptwid->renderPage(curPage);
		pageSelector->setCurrentRecordNumber(curPage);
	}
}

void KexiReportView::firstPage()
{
	if (curPage != 1 )
	{
		curPage = 1;
		rptwid->renderPage(curPage);
		pageSelector->setCurrentRecordNumber(curPage);
	}
}

void KexiReportView::lastPage()
{
	if (curPage != pageCount )
	{
		curPage = pageCount;
		rptwid->renderPage(curPage);
		pageSelector->setCurrentRecordNumber(curPage);
	}
}

void KexiReportView::slotPrintReport()
{
	QPrinter printer;
	ORPrintRender pr;
	
   	// do some printer initialization
	pr.setPrinter(&printer);
	pr.setupPrinter(doc, &printer);
	
	QPrintDialog *dialog = new QPrintDialog(&printer, this);
	if (dialog->exec() != QDialog::Accepted)
		return;

	pr.render(doc);
}

void KexiReportView::slotRenderKSpread()
{
#ifdef HAVE_KSPREAD
	KRKSpreadRender ks;
	KUrl saveUrl = KFileDialog::getSaveUrl( KUrl(), QString(), this, i18n("Save Report to.."));
	if (!saveUrl.isValid()) {
		KMessageBox::error(this, i18n("Report not exported.The URL was invalid"), i18n("Not Saved"));
		return;
	}

	if (KIO::NetAccess::exists(saveUrl, KIO::NetAccess::DestinationSide, this)){
		int wantSave = KMessageBox::warningContinueCancel(this, i18n("The file %1 exists.\nDo you wish to overwrite it?", saveUrl.path()), i18n("Warning"), KGuiItem(i18n("Overwrite")));
		if (wantSave != KMessageBox::Continue) {
			return;
		}
	}
	if (!ks.render(doc, saveUrl)) {
		KMessageBox::error(this, i18n("Failed to open %1 in KSpread", saveUrl.prettyUrl()) , i18n("Opening in KSpread failed"));
	}
#endif
}

void KexiReportView::slotExportHTML()
{
	KUrl saveUrl = KFileDialog::getSaveUrl( KUrl(), QString(), this, i18n("Save Report to.."));
	if (!saveUrl.isValid()) {
		KMessageBox::error(this, i18n("Report not exported, no file selected for writing to"), i18n("Not Saved"));
		return;
	}
	if (KIO::NetAccess::exists(saveUrl, KIO::NetAccess::DestinationSide, this)) {
		int wantSave = KMessageBox::warningContinueCancel(this, i18n("The file %1 exists.\nDo you wish to overwrite it?", saveUrl.path()), i18n("Warning"), KGuiItem(i18n("Overwrite")));
		if (wantSave != KMessageBox::Continue) {
		return;
		}
	}

	bool css = (KMessageBox::questionYesNo(this, i18n("Would you like to export using a Cascading Style Sheet which will give output closer to the original, or export using a Table which outputs a much simpler format."), i18n("Export Style"), KGuiItem("CSS"), KGuiItem("Table")) == KMessageBox::Yes);

	KRHtmlRender hr;
	if (!hr.render(doc, saveUrl, css)) {
		KMessageBox::error(this, i18n("Exporting report to %1 failed", saveUrl.prettyUrl()), i18n( "Saving failed" ));
	} else {
		KMessageBox::information(this, i18n("Report saved to %1", saveUrl.prettyUrl()) , i18n("Saved OK"));
	}
}

tristate KexiReportView::beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore)
{
	//kDebug() << tempData()->document << endl;
	return true;
}

tristate KexiReportView::afterSwitchFrom(Kexi::ViewMode mode)
{
	Q_UNUSED( mode );

	kDebug() << endl;
	if (tempData()->reportSchemaChangedInPreviousView) 
	{
		delete rpt;

		rpt = new ORPreRender(tempData()->document, KexiMainWindowIface::global()->project()->dbConnection());
		curPage = 1;
		
		doc = rpt->generate();
		pageCount = doc->pages();
		pageSelector->setRecordCount(pageCount);
		
		rptwid = new KexiReportPage(this, "Page", doc);
		scr->setWidget(rptwid);
		
		tempData()->reportSchemaChangedInPreviousView = false;
	}
	return true;
}

KexiReportPart::TempData* KexiReportView::tempData() const
{
	return static_cast<KexiReportPart::TempData*>(window()->data());
}

