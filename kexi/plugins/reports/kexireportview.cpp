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
#include <KoReportPage.h>
#include "kexidbreportdata.h"

#ifndef KEXI_MOBILE
#include "keximigratereportdata.h"
#endif

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
#include <KRun>

#include <renderobjects.h>
#include <KoReportPreRenderer.h>

#ifndef KEXI_MOBILE
#include <widget/utils/kexirecordnavigator.h>
#endif

#include <core/KexiWindow.h>
#include <core/KexiMainWindowIface.h>
#include "../scripting/kexiscripting/kexiscriptadaptor.h"

KexiReportView::KexiReportView(QWidget *parent)
        : KexiView(parent), m_preRenderer(0), m_reportDocument(0), m_kexi(0), m_functions(0), m_pageCount(0), m_currentPage(0)
{   
    setObjectName("KexiReportDesigner_DataView");
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->viewport()->setAutoFillBackground(true);

    layout()->addWidget(m_scrollArea);
    
#ifndef KEXI_MOBILE
    m_pageSelector = new KexiRecordNavigator(this, 0);
    layout()->addWidget(m_pageSelector);
    m_pageSelector->setRecordCount(0);
    m_pageSelector->setInsertingButtonVisible(false);
    m_pageSelector->setLabelText(i18n("Page"));
    m_pageSelector->setRecordHandler(this);
#endif
    
    // -- setup local actions
    QList<QAction*> viewActions;
    QAction* a;

#ifndef KEXI_MOBILE
    viewActions << (a = new KAction(KIcon("printer"), i18n("Print"), this));
    a->setObjectName("print_report");
    a->setToolTip(i18n("Print Report"));
    a->setWhatsThis(i18n("Prints the current report."));
    connect(a, SIGNAL(triggered()), this, SLOT(slotPrintReport()));
#endif
    
#ifdef KEXI_MOBILE
    viewActions << (a = new KAction(i18n("Export: "), this));
    a->setEnabled(false); //!TODO this is a bit of a dirty way to add what looks like a label to the toolbar! 
    viewActions << (a = new KAction(KIcon("words"), QString(), this));
#else
    viewActions << (a = new KAction(KIcon("words"), i18n("Save to Words"), this));
#endif
    a->setObjectName("save_to_words");
    a->setToolTip(i18n("Save the report to a Words document"));
    a->setWhatsThis(i18n("Save the report to a Words document"));
    a->setEnabled(true);
    connect(a, SIGNAL(triggered()), this, SLOT(slotRenderODT()));

#ifdef KEXI_MOBILE
    viewActions << (a = new KAction(KIcon("kspread"), " ", this));
#else
    viewActions << (a = new KAction(KIcon("kspread"), i18n("Save to KSpread"), this));
#endif
    a->setObjectName("save_to_kspread");
    a->setToolTip(i18n("Save the report to a KSpread document"));
    a->setWhatsThis(i18n("Saves the current report to a KSpread document."));
    a->setEnabled(true);
    connect(a, SIGNAL(triggered()), this, SLOT(slotRenderKSpread()));

#ifdef KEXI_MOBILE
    viewActions << (a = new KAction(KIcon("text-html"), " ", this));
#else
    viewActions << (a = new KAction(KIcon("text-html"), i18n("Export as Web Page"), this));
#endif
    a->setObjectName("export_as_web_page");
    a->setToolTip(i18n("Export the report as web page"));
    a->setWhatsThis(i18n("Exports the report to a web page file."));
    a->setEnabled(true);
    connect(a, SIGNAL(triggered()), this, SLOT(slotExportHTML()));

    setViewActions(viewActions);

#ifndef KEXI_MOBILE
    connect(m_pageSelector, SIGNAL(nextButtonClicked()), this, SLOT(nextPage()));
    connect(m_pageSelector, SIGNAL(prevButtonClicked()), this, SLOT(prevPage()));
    connect(m_pageSelector, SIGNAL(firstButtonClicked()), this, SLOT(firstPage()));
    connect(m_pageSelector, SIGNAL(lastButtonClicked()), this, SLOT(lastPage()));
#endif
}

KexiReportView::~KexiReportView()
{
    kDebug();
    delete m_preRenderer;
    delete m_kexi;
    delete m_functions;
    delete m_reportDocument;
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
    KoReportRendererBase *renderer;
    KoReportRendererContext cxt;

    renderer = m_factory.createInstance("ods");

    if (renderer) {
        cxt.destinationUrl = KFileDialog::getSaveUrl(KUrl(), "*.ods", this, i18n("Save Report to.."));
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
        } else {
            KRun *runner = new KRun(cxt.destinationUrl, this->topLevelWidget());
        }
    }
}

void KexiReportView::slotRenderODT()
{
    KoReportRendererBase *renderer;
    KoReportRendererContext cxt;

    renderer = m_factory.createInstance("odt");

    if (renderer) {
        cxt.destinationUrl = KFileDialog::getSaveUrl(KUrl(), "*.odt", this, i18n("Save Report to.."));
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
            KMessageBox::error(this, i18n("Failed to save %1 to Words", cxt.destinationUrl.prettyUrl()) , i18n("Saving to Words failed"));
        } else {
            KRun *runner = new KRun(cxt.destinationUrl, this->topLevelWidget());
        }
    }
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
        KRun *runner = new KRun(cxt.destinationUrl, this->topLevelWidget());
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
                reportData = new KexiDBReportData(QString(), KexiMainWindowIface::global()->project()->dbConnection());
            }
            m_preRenderer->setSourceData(reportData);
            
            m_preRenderer->setName(tempData()->name);
            m_currentPage = 1;

            //Add a kexi object to provide kexidb and extra functionality
            if(!m_kexi) {
                m_kexi = new KexiScriptAdaptor();
            }
            m_preRenderer->registerScriptObject(m_kexi, "Kexi");

            //If using a kexidb source, add a functions scripting object
            if (tempData()->connectionDefinition.attribute("type") == "internal") {
                //Delete old functions
                if (m_functions) {
                    delete m_functions;
                }
                
                m_functions = new KRScriptFunctions(reportData, KexiMainWindowIface::global()->project()->dbConnection());
                m_preRenderer->registerScriptObject(m_functions, "field");
            }

            if (m_reportDocument) {
                kDebug() << "=======================================Deleting old document";
                delete m_reportDocument;
            }
            
            m_reportDocument = m_preRenderer->generate();
            if (m_reportDocument) {
                m_pageCount = m_reportDocument->pages();
#ifndef KEXI_MOBILE
                m_pageSelector->setRecordCount(m_pageCount);
#endif
            }

            m_reportWidget = new KoReportPage(this, m_reportDocument);
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
#ifndef KEXI_MOBILE
    if (e.attribute("type") ==  "external") {
        kodata = new KexiMigrateReportData(e.attribute("source"));
    }
#endif
    return kodata;
}

KexiReportPart::TempData* KexiReportView::tempData() const
{
    return static_cast<KexiReportPart::TempData*>(window()->data());
}

void KexiReportView::addNewRecordRequested()
{

}

void KexiReportView::moveToFirstRecordRequested()
{
	if (m_currentPage != 1) {
		m_currentPage = 1;
		m_reportWidget->renderPage(m_currentPage);
		#ifndef KEXI_MOBILE
		m_pageSelector->setCurrentRecordNumber(m_currentPage);  
		#endif
	}
}

void KexiReportView::moveToLastRecordRequested()
{
	if (m_currentPage != m_pageCount) {
		m_currentPage = m_pageCount;
		m_reportWidget->renderPage(m_currentPage);
		#ifndef KEXI_MOBILE
		m_pageSelector->setCurrentRecordNumber(m_currentPage);
		#endif
	}
}

void KexiReportView::moveToNextRecordRequested()
{
	if (m_currentPage < m_pageCount) {
		m_currentPage++;
		m_reportWidget->renderPage(m_currentPage);
		#ifndef KEXI_MOBILE
		m_pageSelector->setCurrentRecordNumber(m_currentPage);
		#endif
	}
}

void KexiReportView::moveToPreviousRecordRequested()
{
	if (m_currentPage > 1) {
		m_currentPage--;
		m_reportWidget->renderPage(m_currentPage);
		#ifndef KEXI_MOBILE
		m_pageSelector->setCurrentRecordNumber(m_currentPage);
		#endif
	}
}

void KexiReportView::moveToRecordRequested(uint r)
{

}

long int KexiReportView::currentRecord()
{
    return m_currentPage;
}

long int KexiReportView::recordCount()
{
    return m_pageCount;
}
