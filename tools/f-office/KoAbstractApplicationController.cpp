/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010 Jarosław Staniek <staniek@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoAbstractApplicationController.h"
#include "Common.h"
#include "FoCellToolFactory.h"
#include "FoCellTool.h"
#include "PreviewDialog.h"
#include "PresentationTool.h"
#include "RemoveSheet.h"

#include <KoDocumentInfo.h>
#include <KoView.h>
#include <KoCanvasBase.h>
#include <kdemacros.h>
#include <KoCanvasControllerWidget.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>
#include <KoToolProxy.h>
#include <KoToolBase.h>
#include <KoResourceManager.h>
#include <KoToolManager.h>
#include <KoShapeManager.h>
#include <KoShapeUserData.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoPADocument.h>
#include <KoTextEditor.h>
#include <KoPAView.h>
#include <KoStore.h>
#include <KoCanvasBase.h>
#include <KoToolRegistry.h>
#include <styles/KoParagraphStyle.h>
#include <styles/KoListLevelProperties.h>
#include <KoList.h>
#include <Map.h>
#include <Doc.h>
#include <part/View.h>
#include <Sheet.h>
#include <kspread/ui/Selection.h>
#include <KWView.h>

#include <KMimeType>
#include <KLocale>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kparts/event.h>
#include <kundostack.h>

#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>
#include <QSplashScreen>

KoAbstractApplicationController::KoAbstractApplicationController()
    : m_firstChar(true),
      m_searchTextIndex(0),
      m_searchWholeWords(false),
      m_doc(0),
      m_isLoading(false),
      m_type(NoDocument),
      m_currentPage(1),
      m_prevCurrentPage(0),
      m_view(0),
      m_textDocument(0),
      m_editor(0),
      m_controller(0),
      m_cellTool(0),
      m_cellToolFactory(0),
      m_storeButtonPreview(0),
      m_splash(0),
      m_presentationTool(0)
{
}

KoAbstractApplicationController::~KoAbstractApplicationController()
{
    closeDocument();
//    delete m_signalReceiver;
    delete m_splash;
}

// QObject* KoAbstractApplicationController::signalReceiver() const
// {
//     return m_signalReceiver;
// }

bool KoAbstractApplicationController::isSupportedExtension(const QString& extension) const
{
    QList<QString> extensions;
    //Add Txt extension after adding ascii filter to koffice package
    /*extensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT \*/
    extensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT
        << EXT_PPT << EXT_PPTX << EXT_ODP << EXT_PPS << EXT_PPSX
        << EXT_ODS << EXT_XLS << EXT_XLSX;
    return extensions.contains(extension.toLower());
}

bool KoAbstractApplicationController::isNativeDocumentExtension(const QString& extension) const
{
    return 0 == QString::compare(extension, EXT_ODT, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_ODP, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_ODS, Qt::CaseInsensitive);
}

bool KoAbstractApplicationController::isTextDocumentExtension(const QString& extension) const
{
    return 0 == QString::compare(extension, EXT_ODT, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_DOC, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_DOCX, Qt::CaseInsensitive);
}

bool KoAbstractApplicationController::isPresentationDocumentExtension(const QString& extension) const
{
    return 0 == QString::compare(extension, EXT_ODP, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_PPS, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_PPSX, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_PPT, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_PPTX, Qt::CaseInsensitive);
}

bool KoAbstractApplicationController::isSpreadsheetDocumentExtension(const QString& extension) const
{
    return 0 == QString::compare(extension, EXT_ODS, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_XLS, Qt::CaseInsensitive)
       ||  0 == QString::compare(extension, EXT_XLSX, Qt::CaseInsensitive);
}

bool KoAbstractApplicationController::openDocument(const QString &fileName, bool isNewDocument)
{
    //check if the file exists
    if (!QFile(fileName).exists()) {
        showMessage(InformationMessage, i18n("No such file exists"));
        return false;
    }

    //check if the format is supported for opening
    if (!isSupportedExtension( KMimeType::extractKnownExtension(fileName) )) {
        showMessage(UnsupportedFileTypeMessage);
        qWarning() << "Currently this file format is not supported";
        return false;
    }

    //if the current instance has a document open start a new one
    if (m_doc) {
        startNewInstance(fileName, isNewDocument);
        return true;
    }

    showUiBeforeDocumentOpening(isNewDocument); // <-- FOR IMPLEMENTATION

    setProgressIndicatorVisible(true);
    QString mimetype = KMimeType::findByPath(fileName)->name();
    int errorCode = 0;
    m_isLoading = true;

    m_doc = KParts::ComponentFactory::createPartInstanceFromQuery<KoDocument>(
                mimetype, QString(),
                0, 0, QStringList(),
                &errorCode);

    if (!m_doc) {
        setProgressIndicatorVisible(false);
        m_isLoading = false;
        return false;
    }

    //for new files the condition to be checked is m_fileName.isEmpty() 
    if (isNewDocument) {
        m_fileName.clear();
    } else {
        m_fileName = fileName;
    }

    KUrl fileUrl;
    fileUrl.setPath(fileName);

    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(true);

    //actual opening of the document happens here.
    if (!m_doc->openUrl(fileUrl)) {
        setProgressIndicatorVisible(false);
        m_isLoading = false;
        return false;
    }

    m_doc->setReadWrite(true);
    m_doc->setAutoSave(0);

    // registering tools
    m_cellToolFactory = new FoCellToolFactory();
   // m_spreadEdit->setCellTool(m_cellTool)
    KoToolRegistry::instance()->add(m_cellToolFactory);

    m_view = m_doc->createView();
    QList<KoCanvasControllerWidget*> controllers = m_view->findChildren<KoCanvasControllerWidget*>();
    if (controllers.isEmpty()) {
        setProgressIndicatorVisible(false);
        m_isLoading = false;
        return false; // Panic
    }

    m_controller = controllers.first();

    if (!m_controller) {
        setProgressIndicatorVisible(false);
        m_isLoading = false;
        return false;
    }

    QString fname = fileUrl.fileName();
    QString ext = KMimeType::extractKnownExtension(fname);
    if (!ext.isEmpty()) {
        fname.chop(ext.length() + 1);
    }

    //file type of the document
    if (isPresentationDocumentExtension(ext)) {
        m_type = PresentationDocument;
    }
    else if (isSpreadsheetDocumentExtension(ext)) {
        m_type = SpreadsheetDocument;
    }
    else {
        m_type = TextDocument;
        // We need to get the page count again after layout rounds.
        QObject::connect(m_doc, SIGNAL(pageSetupChanged()), thisObject(), SLOT(documentPageSetupChanged()));
    }

    if (m_type == SpreadsheetDocument) {
        KoToolManager::instance()->addController(m_controller);
        QApplication::sendEvent(m_view, new KParts::GUIActivateEvent(true));
        KSpread::View *kspreadView = qobject_cast<KSpread::View*>(m_view);
        m_cellTool = dynamic_cast<FoCellTool *>(
            KoToolManager::instance()->toolById(kspreadView->selection()->canvas(), CellTool_ID));
        kspreadView->showTabBar(false);
        kspreadView->setStyleSheet("* { color:white; } ");

        //set the central widget. Note: The central widget for spreadsheet is m_view.
        setCentralWidget(kspreadView);
    }
    else if(m_type == TextDocument && isTextDocumentExtension(ext)) {
        m_editor = qobject_cast<KoTextEditor *>(kwordView()->canvasBase()->toolProxy()->selection());

        //set the central widget here
        setCentralWidget(m_controller);
    }
    else if (m_type == PresentationDocument) {
        //set the central widget here
        setCentralWidget(m_controller);

        //code related to the button previews
        if (m_storeButtonPreview)
            delete m_storeButtonPreview;
        m_storeButtonPreview = new StoreButtonPreview(m_doc, m_view);
        QObject::connect(m_storeButtonPreview, SIGNAL(goToPage(int)), thisObject(), SLOT(goToPage(int)));
    } else {
        //condition required for plain text document that is opened.
        setCentralWidget(m_controller);
    }

    QWidget *thisWidget = dynamic_cast<QWidget*>(this);
    if (thisWidget) {
        thisWidget->setWindowTitle(i18n("%1 - %2", fname, applicationName()));
    }

    m_controller->setProperty("FingerScrollable", true);

    QTimer::singleShot(250, thisObject(), SLOT(documentPageSetupChanged()));

    KoCanvasBase *canvas = m_controller->canvas();
    QObject::connect(
        canvas->resourceManager(), SIGNAL(resourceChanged(int, const QVariant &)),
        thisObject(), SLOT(resourceChanged(int, const QVariant &)));
    QObject::connect(
        KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*, int)),
        thisObject(), SLOT(activeToolChanged(KoCanvasController*, int)));

    setProgressIndicatorVisible(false);
    m_isLoading = false;

    if (thisWidget) {
        if (m_splash && !thisWidget->isActiveWindow()) {
            thisWidget->show();
            m_splash->finish(m_controller);
            m_splash = 0;
        }
   }

   //This is a hack for the uppercase problem.
   m_firstChar = true;

   //When the user opens a new document it should open in the edit mode directly
   if (isNewDocument) {
       //This timer is required, otherwise a new spread sheet will
       //have an unwanted widget in the view.
       QTimer::singleShot(1, thisObject(), SLOT(showEditingMode()));
       if (m_type == TextDocument) {
           centralWidget()->setInputMethodHints(Qt::ImhNoAutoUppercase);
       }
   } else {
       //activate the pan tool.
       setEditingMode(false);
   }

    updateActions();
    return true;
}

bool KoAbstractApplicationController::doOpenDocument()
{
    const bool result = openDocument(m_fileNameToOpen, false);
    m_fileNameToOpen.clear();
    return result;
}

void KoAbstractApplicationController::closeDocument()
{
    if (m_doc == 0)
        return;

    setWindowTitle(applicationName());
    setCentralWidget(0);
    m_searchTextPositions.clear();
    // the presentation and text document instances seem to require different ways to do cleanup
    switch (documentType()) {
    case PresentationDocument:
        KoToolManager::instance()->removeCanvasController(m_controller);
        delete m_doc;
        m_doc = 0;
        delete m_view;
        m_view = 0;
        if (m_controller) {
            delete m_controller;
            m_controller = 0;
        }
        break;
    case SpreadsheetDocument:
        KoToolManager::instance()->removeCanvasController(m_controller);
//??        delete m_undostack;
//??        m_undostack = 0;
        delete m_view;
        m_view=0;
        delete m_doc;
        m_doc = 0;
        break;
    default:
        KoToolManager::instance()->removeCanvasController(m_controller);
        if (kwordView())
            kwordView()->canvasBase()->toolProxy()->deleteSelection();
//??        delete m_undostack;
//??        m_undostack = 0;
        delete m_doc;
        m_doc = 0;
        delete m_view;
        m_view = 0;
        m_editor=0;
        if (m_controller) {
            delete m_controller;
            m_controller = 0;
        }
    }

    setCentralWidget(0);
    m_currentPage = 1;

    updateActions();

    setDocumentModified(false);
}

QString KoAbstractApplicationController::getSaveFileName()
{
    QString filter;
    QString extension;
    switch (documentType()) {
        case TextDocument:
            filter = i18n("Document (%1)", QLatin1String("*.odt"));
            extension = "odt";
            break;
        case PresentationDocument:
            filter = i18n("Presentation (%1)", QLatin1String("*.odp"));
            extension = "odp";
            break;
        case SpreadsheetDocument:
            filter = i18n("SpreadSheet (%1)", QLatin1String("*.ods"));
            extension = "ods";
            break;
        default:
            kFatal() << "Unsupported document type";
            return QString();
    }
    QString fileName = showGetSaveFileNameDialog(i18n("Save File"),
                                         QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
                                         filter + ";;");
    if (fileName.isEmpty())
        return QString();
    if (QFileInfo(fileName).suffix() != extension)
        fileName.append("." + extension);
    return fileName;
}

bool KoAbstractApplicationController::openDocument()
{
    if (m_isLoading)
        return false;

    QWidget *thisWidget = dynamic_cast<QWidget*>(this);
    if (m_splash && !thisWidget->isActiveWindow()) {
        thisWidget->show();
        m_splash->finish(thisWidget);
        m_splash = 0;
    }

    const QString filter
        = i18n("Text Documents (*.doc *.docx *.odt *.txt);;"
               "Presentations (*.ppt *.pptx *.odp *.pps *.ppsx);;"
               "Spreadsheets (*.xls *.xlsx *.ods)");

    const QString file = showGetOpenFileNameDialog(i18n("Open Document"), 
                                                   QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation),
                                                   filter);

    if (file.isEmpty()) {
        return false;
    }
    /* not needed, openDocument(const QString &fileName, bool isNewDocument) checks this
    if (!isSupportedExtension(KMimeType::extractKnownExtension(fileName))) {
        return false;
    }*/
    m_fileNameToOpen = file;
    QTimer::singleShot(100, thisWidget, SLOT(doOpenDocument()));
    return true;
}

bool KoAbstractApplicationController::saveDocument()
{
    if (!m_doc) {
        qWarning() << "No document to save";
        return false;
    }
    if (m_fileName.isEmpty()) {
        return saveDocumentAs();
    }
    return saveDocumentInternal(m_fileName);
}

bool KoAbstractApplicationController::saveDocumentAs()
{
    if (!m_doc) {
        qWarning() << "No document to save";
        return false;
    }
    if (m_fileName.isEmpty()) {
        m_fileName = NEW_WORDDOC;
    }
    QString fileName = getSaveFileName();
    if (fileName.isEmpty())
        return false; // cancel
    return saveDocumentInternal(fileName);
}

bool KoAbstractApplicationController::saveDocumentInternal(const QString& fileName)
{
    if (fileName.isEmpty()) {
        showMessage(InformationWithoutTimeoutMessage, i18n("No file name specified"));
        return false;
    }
    QString ext = KMimeType::extractKnownExtension(fileName);
    bool ok = false;
    if (isNativeDocumentExtension(ext)) {
        ok = m_doc->saveNativeFormat(fileName);
    }
    else if (isSupportedExtension(ext)) { // not native
        showMessage(InformationWithoutTimeoutMessage, i18n("File will be saved in ODF"));
        KUrl newURL;
        newURL.setPath(fileName);
        KMimeType::Ptr mime = KMimeType::findByUrl(newURL);
        QString outputFormatString = mime->name();
        m_doc->setOutputMimeType(outputFormatString.toLatin1(), 0);
        ok = m_doc->saveAs(newURL);
    }
    else {
        showMessage(InformationMessage, i18n("Saving is supported in ODF formats only"));
        return false;
    }

    if (!ok) {
        showMessage(InformationMessage, i18n("The document could not be saved"));
        return false;
    }
    showMessage(InformationMessage, i18n("The document has been saved successfully"));
    m_fileName = fileName;
    setDocumentModified(false);
    return true;
}

int KoAbstractApplicationController::pageCount() const
{
    if (!m_doc)
        return 0;
    if (documentType() == SpreadsheetDocument) {
        KSpread::Doc *kspreadDoc = qobject_cast<KSpread::Doc*>(m_doc);
        return kspreadDoc->map()->count();
    }
    return m_doc->pageCount();
}

KWView* KoAbstractApplicationController::kwordView() const
{
    return qobject_cast<KWView *>(m_view);
}

void KoAbstractApplicationController::documentPageSetupChanged()
{
    updateActions();
}

void KoAbstractApplicationController::goToPage(int page)
{
    if (page >= m_currentPage) {
        for(int i = m_currentPage; i < page; i++) {
            goToNextPage();
        }
    } else {
        for(int i = m_currentPage; i > page; i--) {
            goToPreviousPage();
        }
    }
}

//! these new functions solve problem in action next page which stops moving page from 13th page number
void KoAbstractApplicationController::goToPreviousPage()
{
    if (!m_controller)
        return;
    switch (documentType()) {
    case SpreadsheetDocument:
        goToPreviousSheet();
        return;
    case PresentationDocument:
        goToPreviousSlide();
    default:;
    }

    if (m_currentPage == 1) {
        if (documentType() == PresentationDocument) {
            showMessage(InformationMessage, i18n("First slide reached"));
        } else if (documentType() == TextDocument) {
            showMessage(InformationMessage, i18n("First page reached"));
        }
        return;
    }
    int cur_page = m_currentPage;
    bool check = triggerAction("page_previous");
    if (check) {
        while (cur_page - m_currentPage != 1) {
            goToNextPage();
        }
    }
}

void KoAbstractApplicationController::goToNextPage()
{
    if (!m_controller)
        return;
    switch (documentType()) {
    case SpreadsheetDocument:
        goToNextSheet();
        return;
    case PresentationDocument:
        goToNextSlide();
    default:;
    }

    if (m_currentPage == m_doc->pageCount()) {
        if (documentType() == PresentationDocument) {
            showMessage(InformationMessage, i18n("Last slide reached"));
        } else if (documentType() == TextDocument) {
            showMessage(InformationMessage, i18n("Last page reached"));
        }
        return;
    }
    if (m_prevCurrentPage != m_currentPage) {
        int cur_page = m_currentPage;
        m_prevCurrentPage = m_currentPage;
        triggerAction("page_next");
        if(cur_page == m_currentPage)
            goToNextPage();
        return;
    }
    else {
        int cur_page = m_currentPage;
        int next_page = cur_page+1;
        while(next_page != cur_page && documentType() == TextDocument) {
            m_controller->pan(QPoint(0, 1));
            cur_page = m_currentPage;
        }
        m_prevCurrentPage = m_currentPage;
        triggerAction("page_next");
        return;
    }
}

void KoAbstractApplicationController::goToNextSheet()
{
    KSpread::View *kspreadView = qobject_cast<KSpread::View*>(m_view);
    if (!kspreadView)
        return;
    KSpread::Sheet *sheet = kspreadView->activeSheet();
    if (!sheet)
        return;
    KSpread::Doc *kspreadDoc = qobject_cast<KSpread::Doc*>(m_doc);
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->nextSheet(sheet);
    if (!sheet)
        return;
    kspreadView->setActiveSheet(sheet);
}

void KoAbstractApplicationController::goToPreviousSheet()
{
    KSpread::View *kspreadView = qobject_cast<KSpread::View*>(m_view);
    if (!kspreadView)
        return;
    KSpread::Sheet *sheet = kspreadView->activeSheet();
    if (!sheet)
        return;
    KSpread::Doc *kspreadDoc = qobject_cast<KSpread::Doc*>(m_doc);
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->previousSheet(sheet);
    if (!sheet)
        return;
    kspreadView->setActiveSheet(sheet);
}

void KoAbstractApplicationController::setSplashScreen(QSplashScreen* splash)
{
    delete m_splash;
    m_splash = splash;
}

void KoAbstractApplicationController::toggleVirtualKeyboardVisibility()
{
    setVirtualKeyboardVisible(!isVirtualKeyboardVisible());
}

bool KoAbstractApplicationController::setEditingMode(bool set)
{
    Q_UNUSED(set);
    if (!m_doc)
        return false;

    setVirtualKeyboardVisible(false);
    return true;
}

bool KoAbstractApplicationController::triggerAction(const char* name)
{
    if (m_view) {
        // the cast in the next line is no longer needed for
        // koffice revision 1004085 and newer
        QAction* action = ((KXMLGUIClient*)m_view)->action(name);
        if (action) {
            action->activate(QAction::Trigger);
            return true;
        }
    }
    return false;
}

bool KoAbstractApplicationController::handleCloseEvent(QCloseEvent *event)
{
    setVirtualKeyboardVisible(false);

    if (isDocumentModified()) {
        const QMessageBox::StandardButton result = askQuestion(SaveDiscardCancelQuestion);
        switch (result) {
        case QMessageBox::Save:
            if (saveDocument()) {
                closeDocument();
            }
            break;
        case QMessageBox::Discard:
            closeDocument();
            break;
        case QMessageBox::Cancel:
            break;
        default:
            kFatal() << "Unexpected result" << result;
        }
    }
    if (document()) {
        event->ignore();
        return false;
    }
    return true;
}

void KoAbstractApplicationController::resourceChanged(int key, const QVariant &value)
{
#ifdef Q_WS_MAEMO_5
    if (documentType() == TextDocument && m_ui->actionEdit->isChecked()){
        if (foDocumentRdf)
            foDocumentRdf->findStatements(*textEditor()->cursor(), 1);
    }
#endif
    if (m_presentationTool && m_presentationTool->toolsActivated() && documentType() == PresentationDocument) {
        return;
    }
    if (KoCanvasResource::CurrentPage == key) {
        m_currentPage = value.toInt();
        currentPageChanged();
    }
}

void KoAbstractApplicationController::addSheet()
{
    if (documentType() == SpreadsheetDocument && m_view) {
        setDocumentModified(true);
        KSpread::View *kspreadView = qobject_cast<KSpread::View*>(m_view);
        kspreadView->insertSheet();
    }
}

void KoAbstractApplicationController::removeSheet()
{
    if (documentType() == SpreadsheetDocument && m_view) {
        if (askQuestion(ConfirmSheetDeleteQuestion, i18n("Do you want to delete the sheet?")) == QMessageBox::Yes) {
            KSpread::View *kspreadView = qobject_cast<KSpread::View*>(m_view);
            kspreadView->selection()->emitCloseEditor(false); // discard changes
            kspreadView->doc()->setModified(true);
            setDocumentModified(true);
            KSpread::Sheet* tbl = kspreadView->activeSheet();
            QUndoCommand* command = new RemoveSheet(tbl);
            kspreadView->doc()->addCommand(command);
        }
    }
}

QString KoAbstractApplicationController::currentSheetName() const
{
    if (documentType() == SpreadsheetDocument && m_view) {
            KSpread::View *kspreadView = qobject_cast<KSpread::View*>(m_view);
        return kspreadView->activeSheet()->sheetName();
    }
    return QString();
}

StoreButtonPreview* KoAbstractApplicationController::storeButtonPreview() const
{
    return m_storeButtonPreview;
}

FoCellTool* KoAbstractApplicationController::cellTool() const
{
    return m_cellTool;
}

PresentationTool* KoAbstractApplicationController::presentationTool() const
{
    return m_presentationTool;
}

void KoAbstractApplicationController::setPresentationTool(PresentationTool *tool)
{
    m_presentationTool = tool;
}
