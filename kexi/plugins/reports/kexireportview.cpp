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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kexireportview.h"
#include "kexireportpage.h"
#include "kexidbreportdata.h"
#include "keximigratereportdata.h"

#include <QLabel>
#include <QBoxLayout>
#include <QScrollArea>
#include <QLayout>
#include <QPainter>
#include <QPrintDialog>
#include <QPrinter>

#include <kdebug.h>
#include "krscriptfunctions.h"
#include <kfiledialog.h>
#include <kio/netaccess.h>

#include <renderobjects.h>
#include <KoReportPreRenderer.h>

#include <widget/utils/kexirecordnavigator.h>
#include <core/KexiWindow.h>
#include <core/KexiMainWindowIface.h>
#include "../scripting/kexiscripting/kexiscriptadaptor.h"

KexiReportView::KexiReportView(QWidget *parent)
        : KexiView(parent)
{
    m_preRenderer = 0;
    setObjectName("KexiReportDesigner_DataView");
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->viewport()->setAutoFillBackground(true);

    m_pageSelector = new KexiRecordNavigator(this, 0);
    layout()->addWidget(m_scrollArea);
    layout()->addWidget(m_pageSelector);

    m_pageSelector->setRecordCount(0);
    m_pageSelector->setInsertingButtonVisible(false);
    m_pageSelector->setLabelText(i18n("Page"));


    // -- setup local actions
    QList<QAction*> viewActions;
    QAction* a;
    viewActions << (a = new KAction(KIcon("printer"), i18n("Print"), this));
    a->setObjectName("print_report");
    a->setToolTip(i18n("Print Report"));
    a->setWhatsThis(i18n("Prints the current report."));
    connect(a, SIGNAL(triggered()), this, SLOT(slotPrintReport()));

#if 0
    viewActions << (a = new KAction(KIcon("kword"), i18n("Open in KWord"), this));
    a->setObjectName("open_in_kword");
    a->setToolTip(i18n("Open the report in KWord"));
    a->setWhatsThis(i18n("Opens the current report in KWord."));
    a->setEnabled(false);
//! @todo connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKWord()));
#endif

#ifdef HAVE_KSPREAD
    viewActions << (a = new KAction(KIcon("kspread"), i18n("Save to KSpread"), this));
    a->setObjectName("save_to_kspread");
    a->setToolTip(i18n("Save the report to a KSpread document"));
    a->setWhatsThis(i18n("Saves the current report to a KSpread document."));
    a->setEnabled(true);
    connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKSpread()));
#endif

    viewActions << (a = new KAction(KIcon("text-html"), i18n("Export as Web Page"), this));
    a->setObjectName("export_as_web_page");
    a->setToolTip(i18n("Export the report as web page"));
    a->setWhatsThis(i18n("Exports the report to a web page file."));
    a->setEnabled(true);
    connect(a, SIGNAL(triggered()), this, SLOT(slotExportHTML()));

    setViewActions(viewActions);


    connect(m_pageSelector, SIGNAL(nextButtonClicked()), this, SLOT(nextPage()));
    connect(m_pageSelector, SIGNAL(prevButtonClicked()), this, SLOT(prevPage()));
    connect(m_pageSelector, SIGNAL(firstButtonClicked()), this, SLOT(firstPage()));
    connect(m_pageSelector, SIGNAL(lastButtonClicked()), this, SLOT(lastPage()));

}

KexiReportView::~KexiReportView()
{
}

void KexiReportView::nextPage()
{
    if (m_currentPage < m_pageCount) {
        m_currentPage++;
        m_reportWidget->renderPage(m_currentPage);
        m_pageSelector->setCurrentRecordNumber(m_currentPage);
    }
}

void KexiReportView::prevPage()
{
    if (m_currentPage > 1) {
        m_currentPage--;
        m_reportWidget->renderPage(m_currentPage);
        m_pageSelector->setCurrentRecordNumber(m_currentPage);
    }
}

void KexiReportView::firstPage()
{
    if (m_currentPage != 1) {
        m_currentPage = 1;
        m_reportWidget->renderPage(m_currentPage);
        m_pageSelector->setCurrentRecordNumber(m_currentPage);
    }
}

void KexiReportView::lastPage()
{
    if (m_currentPage != m_pageCount) {
        m_currentPage = m_pageCount;
        m_reportWidget->renderPage(m_currentPage);
        m_pageSelector->setCurrentRecordNumber(m_currentPage);
    }
}

void KexiReportView::slotPrintReport()
{
    QPrinter printer;
    QPainter painter;
    KoReportRendererBase *renderer;

    renderer = m_factory.createInstance("print");
    QPointer<QPrintDialog> dialog = new QPrintDialog(&printer, this);
    if (dialog->exec() == QDialog::Accepted) {
        KoReportRendererContext cxt;
        cxt.printer = &printer;
        cxt.painter = &painter;
    
        renderer->render(cxt, m_reportDocument);
    }
    delete dialog;
    delete renderer;
}

void KexiReportView::slotRenderKSpread()
{
#ifdef HAVE_KSPREAD
    KoReportRendererBase *renderer;
    KoReportRendererContext cxt;

    renderer = m_factory.createInstance("kspread");
    
    cxt.destinationUrl = KFileDialog::getSaveUrl(KUrl(), QString(), this, i18n("Save Report to.."));
    if (!cxt.destinationUrl.isValid()) {
        KMessageBox::error(this, i18n("Report not exported.The URL was invalid"), i18n("Not Saved"));
        return;
    }

    if (KIO::NetAccess::exists(cxt.destinationUrl, KIO::NetAccess::DestinationSide, this)) {
        int wantSave = KMessageBox::warningContinueCancel(this, i18n("The file %1 exists.\nDo you wish to overwrite it?", cxt.destinationUrl.path()), i18n("Warning"), KGuiItem(i18n("Overwrite")));
        if (wantSave != KMessageBox::Continue) {
            return;
        }
    }
    if (!renderer->render(cxt, m_reportDocument)) {
        KMessageBox::error(this, i18n("Failed to open %1 in KSpread", cxt.destinationUrl.prettyUrl()) , i18n("Opening in KSpread failed"));
    }
#endif
}

void KexiReportView::slotExportHTML()
{
    KoReportRendererContext cxt;
    KoReportRendererBase *renderer;
    
    cxt.destinationUrl = KFileDialog::getSaveUrl(KUrl(), QString(), this, i18n("Export Report as Web Page"));
    if (!cxt.destinationUrl.isValid()) {
        KMessageBox::error(this, i18n("Report not exported, no file selected for writing to"), i18n("Not Exported"));
        return;
    }
    if (KIO::NetAccess::exists(cxt.destinationUrl, KIO::NetAccess::DestinationSide, this)) {
        int wantSave = KMessageBox::warningContinueCancel(this,
            i18n("The file %1 exists.\nDo you wish to overwrite it?", cxt.destinationUrl.path()),
            i18n("Warning"), KGuiItem(i18n("Overwrite")));
        if (wantSave != KMessageBox::Continue) {
            return;
        }
    }

    bool css = (KMessageBox::questionYesNo(this,
        i18n("Would you like to export using a Cascading Style Sheet which will give output closer to the original, "
             "or export using a Table which outputs a much simpler format."), i18n("Export Style"),
             KGuiItem("CSS"), KGuiItem("Table")) == KMessageBox::Yes);

    if (css){
        renderer = m_factory.createInstance("htmlcss");
    }
    else {
        renderer = m_factory.createInstance("htmltable");
    }
    
    if (!renderer->render(cxt, m_reportDocument)) {
        KMessageBox::error(this, i18n("Exporting report to %1 failed", cxt.destinationUrl.prettyUrl()), i18n("Exporting failed"));
    } else {
        KMessageBox::information(this, i18n("Report exported to %1", cxt.destinationUrl.prettyUrl()) , i18n("Exporting Succeeded"));
    }
}

tristate KexiReportView::beforeSwitchTo(Kexi::ViewMode mode, bool &dontStore)
{
    Q_UNUSED(mode);
    Q_UNUSED(dontStore);

    return true;
}

tristate KexiReportView::afterSwitchFrom(Kexi::ViewMode mode)
{
    Q_UNUSED(mode);

    kDebug();
    if (tempData()->reportSchemaChangedInPreviousView) {
	kDebug() << "Schema changed";
        delete m_preRenderer;

	kDebug() << tempData()->reportDefinition.tagName();
	
        m_preRenderer = new KoReportPreRenderer(tempData()->reportDefinition);
        if (m_preRenderer->isValid()) {
            KoReportData *reportData = 0;
            if (!tempData()->connectionDefinition.isNull())  {
                reportData = sourceData(tempData()->connectionDefinition);    
            }
            if (!reportData) {
                reportData = new KexiDBReportData("", 0);
            }
            m_preRenderer->setSourceData(reportData);
            
            m_preRenderer->setName(tempData()->name);
            m_currentPage = 1;

            //Add a kexi object to provide kexidb and extra functionality
            m_kexi = new KexiScriptAdaptor();
            m_preRenderer->registerScriptObject(m_kexi, "Kexi");

            //If using a kexidb source, add a functions scripting object
            if (tempData()->connectionDefinition.attribute("type") == "internal") {
                m_functions = new KRScriptFunctions(reportData, KexiMainWindowIface::global()->project()->dbConnection());
                m_preRenderer->registerScriptObject(m_functions, "field");
            }
            
            m_reportDocument = m_preRenderer->generate();
            if (m_reportDocument) {
                m_pageCount = m_reportDocument->pages();
                m_pageSelector->setRecordCount(m_pageCount);
            }

            m_reportWidget = new KexiReportPage(this, m_reportDocument);
            m_reportWidget->setObjectName("KexiReportPage");
            m_scrollArea->setWidget(m_reportWidget);

        } else {
            KMessageBox::error(this, i18n("Report schema appears to be invalid or corrupt"), i18n("Opening failed"));
        }


        tempData()->reportSchemaChangedInPreviousView = false;
    }
    return true;
}

KoReportData* KexiReportView::sourceData(QDomElement e)
{
    KoReportData *kodata = 0;

    if (e.attribute("type") == "internal") {
        kodata = new KexiDBReportData(e.attribute("source"), KexiMainWindowIface::global()->project()->dbConnection());
    }
    if (e.attribute("type") ==  "external") {
        kodata = new KexiMigrateReportData(e.attribute("source"));
    }

    return kodata;
}

KexiReportPart::TempData* KexiReportView::tempData() const
{
    return static_cast<KexiReportPart::TempData*>(window()->data());
}

