/* This file is part of the KDE project
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
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
#include "KoCellToolFactory.h"
#include "KoCellTool.h"
#include "RemoveSheetCommand.h"

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
#include <tables/Map.h>
#include <tables/DocBase.h>
#include <tables/part/View.h>
#include <tables/Sheet.h>
#include <tables/ui/Selection.h>
#include <words/part/KWView.h>

#include <KMimeType>
#include <KLocale>
#include <kparts/part.h>
#include <kparts/componentfactory.h>
#include <kparts/event.h>
#include <kundo2stack.h>
#include <kmimetypetrader.h>

#include <QApplication>
#include <QDesktopServices>
#include <QFile>
#include <QFileInfo>
#include <QtDebug>
#include <QSplashScreen>
#include <QTimer>

/*!
* extensions
*/
const QString EXT_PPS("pps");
const QString EXT_PPSX("ppsx");
const QString EXT_PPT("ppt");
const QString EXT_PPTX("pptx");
const QString EXT_ODP("odp");
const QString EXT_DOC("doc");
const QString EXT_DOCX("docx");
const QString EXT_ODT("odt");
const QString EXT_TXT("txt");
const QString EXT_RTF("rtf");
const QString EXT_ODS("ods");
const QString EXT_XLS("xls");
const QString EXT_XLSX("xlsx");

KoAbstractApplicationOpenDocumentArguments::KoAbstractApplicationOpenDocumentArguments()
    : editing(false), openAsTemplates(false)
{
}

KoAbstractApplicationController::KoAbstractApplicationController(QObject *application)
    : m_application(application),
      m_firstChar(true),
      m_searchTextIndex(0),
      m_searchWholeWords(false),
      m_centralWidget(0),
      m_doc(0),
      m_isLoading(false),
      m_editingMode(false),
      m_type(NoDocument),
      m_currentPage(1),
      m_prevCurrentPage(0),
      m_view(0),
      m_textDocument(0),
      m_editor(0),
      m_canvasController(0),
      m_cellTool(0),
      m_cellToolFactory(0),
      m_splash(0),
      m_onlyDisplayDocumentNameInTitle(false)
{
    Q_ASSERT_X(m_application, "KoAbstractApplicationController",
             "Application object must be provided to the controller");
    m_application->installEventFilter(this);
}

KoAbstractApplicationController::~KoAbstractApplicationController()
{
    closeDocument();
    delete m_splash;
}

QStringList KoAbstractApplicationController::supportedExtensions() const
{
    if (m_supportedExtensions.isEmpty()) {
        //Add Txt extension after adding ascii filter to calligra package
        /*extensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT \*/
        m_supportedExtensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT
            << EXT_PPT << EXT_PPTX << EXT_ODP << EXT_PPS << EXT_PPSX
            << EXT_ODS << EXT_XLS << EXT_XLSX;
    }
    return m_supportedExtensions;
}

QStringList KoAbstractApplicationController::supportedFilters() const
{
    if (m_supportedFilters.isEmpty()) {
        foreach (const QString& ext, supportedExtensions()) {
            m_supportedFilters.append(QLatin1String("*.") + ext);
        }
    }
    return m_supportedFilters;
}

bool KoAbstractApplicationController::isSupportedExtension(const QString& extension) const
{
    QList<QString> extensions;
    //Add Txt extension after adding ascii filter to calligra package
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

bool KoAbstractApplicationController::openDocument(const QString &fileName)
{
    KoAbstractApplicationOpenDocumentArguments args;
    args.documentsToOpen.append(fileName);
    return openDocuments(args);
}

bool KoAbstractApplicationController::openDocumentAsTemplate(const QString &fileName)
{
    KoAbstractApplicationOpenDocumentArguments args;
    args.documentsToOpen.append(fileName);
    args.openAsTemplates = true;
    return openDocuments(args);
}

bool KoAbstractApplicationController::openDocuments(
    const KoAbstractApplicationOpenDocumentArguments& args)
{
    //check if the file exists
    const bool isNewDocument = args.documentsToOpen.isEmpty() || args.openAsTemplates;
    const QString fileName = args.documentsToOpen.isEmpty()
                             ? QString() : args.documentsToOpen.first();
    if (!isNewDocument && !QFile(fileName).exists()) {
        showMessage(InformationMessage, i18n("Document \"%1\" does not exist.",
                                             QFileInfo(fileName).fileName()));
        return false;
    }

    //check if the format is supported for opening
    if (!isSupportedExtension( KMimeType::extractKnownExtension(fileName) )) {
        showMessage(UnsupportedFileTypeMessage,
                    i18n("Could not open document \"%1\".\n"
                         "This type is not supported.", QFileInfo(fileName).fileName()));
        qWarning() << "Currently this file format is not supported";
        return false;
    }

    //if the current instance has a document open start a new one
    if (m_doc) {
        return startNewInstance(args);
    }

    showUiBeforeDocumentOpening(isNewDocument); // <-- FOR IMPLEMENTATION

    setProgressIndicatorVisible(true);
    QString mimetype = KMimeType::findByPath(fileName)->name();
    kDebug() << "mimetype:" << mimetype;

    QString error;
    m_isLoading = true;

    m_doc = KMimeTypeTrader::createPartInstanceFromQuery<KoDocument>(
                mimetype, 0, 0, QString(), QVariantList(), &error);

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
    m_cellToolFactory = new KoCellToolFactory(this);
   // m_spreadEdit->setCellTool(m_cellTool)
    KoToolRegistry::instance()->add(m_cellToolFactory);

    m_view = m_doc->createView();
    if (KWView *v = dynamic_cast<KWView*>(m_view)) {
        v->toggleViewFrameBorders(false);
    }
    QList<KoCanvasControllerWidget*> controllers = m_view->findChildren<KoCanvasControllerWidget*>();
    if (controllers.isEmpty()) {
        setProgressIndicatorVisible(false);
        m_isLoading = false;
        return false; // Panic
    }

    m_canvasController = controllers.first();

    if (!m_canvasController) {
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
        connect(m_doc, SIGNAL(pageSetupChanged()),
                this, SLOT(handleDocumentPageSetupChanged()));
    }

    if (m_type == SpreadsheetDocument) {
        KoToolManager::instance()->addController(m_canvasController);
        QApplication::sendEvent(m_view, new KParts::GUIActivateEvent(true));
        Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(m_view);
        m_cellTool = dynamic_cast<KoCellTool *>(
            KoToolManager::instance()->toolById(tablesView->selection()->canvas(), cellToolFactoryId()));
        tablesView->showTabBar(false);
#ifdef Q_WS_MAEMO_5
        tablesView->setStyleSheet("* { color:white; } ");
#endif

        //set the central widget. Note: The central widget for spreadsheet is m_view.
        setCentralWidget(tablesView);
    }
    else if(m_type == TextDocument && isTextDocumentExtension(ext)) {
        m_editor = KoTextEditor::getTextEditorFromCanvas(wordsView()->canvasBase());
        if (canvasControllerWidget()) {
            setCentralWidget(canvasControllerWidget());
        }
    }
    else if (m_type == PresentationDocument) {
        if (canvasControllerWidget()) {
            setCentralWidget(canvasControllerWidget());
        }
    } else {
        //condition required for plain text document that is opened.
        if (canvasControllerWidget()) {
            setCentralWidget(canvasControllerWidget());
        }
    }

    updateWindowTitle(fname);

    if (canvasControllerWidget())
        canvasControllerWidget()->setProperty("FingerScrollable", true);

    QTimer::singleShot(250, this, SLOT(handleDocumentPageSetupChanged()));

    KoCanvasBase *canvas = m_canvasController->canvas();
    connect(
        canvas->resourceManager(), SIGNAL(resourceChanged(int, const QVariant &)),
        this, SLOT(resourceChanged(int, const QVariant &)));
    connect(
        KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*, int)),
        this, SLOT(activeToolChanged(KoCanvasController*, int)));

    setProgressIndicatorVisible(false);
    m_isLoading = false;

    if (mainWindow()) {
        if (m_splash && !mainWindow()->isActiveWindow()) {
            mainWindow()->show();
            if (canvasControllerWidget())
                m_splash->finish(canvasControllerWidget());
            m_splash = 0;
        }
   }

   //This is a hack for the uppercase problem.
   m_firstChar = true;

   //When the user opens a new document it should open in the edit mode directly
   if (isNewDocument) {
       //This timer is required, otherwise a new spread sheet will
       //have an unwanted widget in the view.
       QTimer::singleShot(1, this, SLOT(showEditingMode()));
       if (m_type == TextDocument) {
            if (m_centralWidget)
                m_centralWidget->setInputMethodHints(Qt::ImhNoAutoUppercase);
       }
   } else {
       //activate the pan tool.
       setEditingMode(args.editing);
   }

    updateActions();

    if (args.documentsToOpen.count() > 1) {
        // open the other documents
        KoAbstractApplicationOpenDocumentArguments newArgs(args);
        newArgs.documentsToOpen.removeFirst();
        return startNewInstance(newArgs);
    }
    return true;
}

bool KoAbstractApplicationController::openScheduledDocument()
{
    const bool result = openDocument(m_fileNameToOpen);
    m_fileNameToOpen.clear();
    return result;
}

void KoAbstractApplicationController::closeDocument()
{
    if (m_doc == 0)
        return;

    updateWindowTitle(QString());
    setCentralWidget(0);
    m_searchTextPositions.clear();
    // the presentation and text document instances seem to require different ways to do cleanup
    switch (documentType()) {
    case PresentationDocument:
        KoToolManager::instance()->removeCanvasController(m_canvasController);
        delete m_doc;
        m_doc = 0;
        delete m_view;
        m_view = 0;
        delete m_canvasController;
        m_canvasController = 0;
        break;
    case SpreadsheetDocument:
        KoToolManager::instance()->removeCanvasController(m_canvasController);
//??        delete m_undostack;
//??        m_undostack = 0;
        delete m_view;
        m_view=0;
        delete m_doc;
        m_doc = 0;
        break;
    default:
        KoToolManager::instance()->removeCanvasController(m_canvasController);
        if (wordsView())
            wordsView()->canvasBase()->toolProxy()->deleteSelection();
//??        delete m_undostack;
//??        m_undostack = 0;
        delete m_doc;
        m_doc = 0;
        delete m_view;
        m_view = 0;
        m_editor = 0;
        delete m_canvasController;
        m_canvasController = 0;
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

    kDebug() << mainWindow();
    if (m_splash && !mainWindow()->isActiveWindow()) {
        mainWindow()->show();
        m_splash->finish(mainWindow());
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
    QTimer::singleShot(100, this, SLOT(openScheduledDocument()));
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
    /* js: why is this needed?
    if (m_fileName.isEmpty()) {
        m_fileName = NEW_WORDDOC;
    }*/
    QString fileName = getSaveFileName();
    if (fileName.isEmpty())
        return false; // cancel
    return saveDocumentInternal(fileName);
}

bool KoAbstractApplicationController::saveDocumentInternal(QString fileName)
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
#warning temporary rename!
        fileName.replace(".doc", ".odt", Qt::CaseInsensitive);
        fileName.replace(".ppt", ".odp", Qt::CaseInsensitive);
        fileName.replace(".xls", ".ods", Qt::CaseInsensitive);
        newURL.setPath(fileName);
        KMimeType::Ptr mime = KMimeType::findByUrl(newURL);
        kFatal() << mime;
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
        Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(m_doc);
        return kspreadDoc->map()->count();
    }
    return m_doc->pageCount();
}

KWView* KoAbstractApplicationController::wordsView() const
{
    return qobject_cast<KWView *>(m_view);
}

void KoAbstractApplicationController::handleDocumentPageSetupChanged()
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
    if (!m_canvasController)
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
    if (!m_canvasController)
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
            m_canvasController->pan(QPoint(0, 1));
            cur_page = m_currentPage;
        }
        m_prevCurrentPage = m_currentPage;
        triggerAction("page_next");
        return;
    }
}

void KoAbstractApplicationController::goToNextSheet()
{
    Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(m_view);
    if (!tablesView)
        return;
    Calligra::Tables::Sheet *sheet = tablesView->activeSheet();
    if (!sheet)
        return;
    Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(m_doc);
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->nextSheet(sheet);
    if (!sheet)
        return;
    tablesView->setActiveSheet(sheet);
}

void KoAbstractApplicationController::goToNextSlide()
{
    emit nextSlide();
}

void KoAbstractApplicationController::goToPreviousSlide()
{
    emit previousSlide();
}

void KoAbstractApplicationController::goToPreviousSheet()
{
    Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(m_view);
    if (!tablesView)
        return;
    Calligra::Tables::Sheet *sheet = tablesView->activeSheet();
    if (!sheet)
        return;
    Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(m_doc);
    if (!kspreadDoc)
        return;
    sheet = kspreadDoc->map()->previousSheet(sheet);
    if (!sheet)
        return;
    tablesView->setActiveSheet(sheet);
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

    if (set) {
        switch (documentType()) {
        case SpreadsheetDocument:
            KoToolManager::instance()->switchToolRequested(cellToolFactoryId());
            break;
        default:
            KoToolManager::instance()->switchToolRequested(textToolFactoryId());
        }

    }
    else {
        KoToolManager::instance()->switchToolRequested(panToolFactoryId());
    }
    m_editingMode = set;
    if (KWView *v = dynamic_cast<KWView*>(m_view)) {
        v->toggleViewFrameBorders(set);
    }
    return true;
}

bool KoAbstractApplicationController::editingMode() const
{
    return m_editingMode;
}

bool KoAbstractApplicationController::setEditingMode()
{
    return setEditingMode(true);
}

bool KoAbstractApplicationController::setViewingMode()
{
    return setEditingMode(false);
}

void KoAbstractApplicationController::setDocumentModified(bool set)
{
    if (m_doc)
        m_doc->setModified(set);
}

bool KoAbstractApplicationController::isDocumentModified() const
{
    return m_doc ? m_doc->isModified() : false;
}

bool KoAbstractApplicationController::triggerAction(const char* name)
{
    if (m_view) {
        // the cast in the next line is no longer needed for
        // calligra revision 1004085 and newer
        QAction* action = ((KXMLGUIClient*)m_view)->action(name);
        if (action) {
            action->activate(QAction::Trigger);
            return true;
        }
    }
    return false;
}

bool KoAbstractApplicationController::eventFilter(QObject *watched, QEvent *event)
{
    Q_UNUSED(watched);
    switch (event->type()) {
    case QEvent::Close:
        if (handleCloseEvent(static_cast<QCloseEvent*>(event)))
            return true;
        break;
    default:;
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

void KoAbstractApplicationController::resourceChanged(int key, const QVariant& value)
{
    if (KoCanvasResource::CurrentPage == key) {
        const int prevPage = m_currentPage;
        m_currentPage = value.toInt();
        handleCurrentPageChanged(prevPage);
    }
}

void KoAbstractApplicationController::addSheet()
{
    if (documentType() == SpreadsheetDocument && m_view) {
        setDocumentModified(true);
        Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(m_view);
        tablesView->insertSheet();
    }
}

bool KoAbstractApplicationController::removeCurrentSheet()
{
    if (documentType() == SpreadsheetDocument && m_view) {
        if (askQuestion(ConfirmSheetDeleteQuestion, i18n("Do you want to delete the sheet?")) == QMessageBox::Yes) {
            Calligra::Tables::View *kspreadView = qobject_cast<Calligra::Tables::View*>(m_view);
            Calligra::Tables::DocBase *kspreadDoc = qobject_cast<Calligra::Tables::DocBase*>(m_doc);
            kspreadView->selection()->emitCloseEditor(false); // discard changes
            kspreadDoc->setModified(true);
            setDocumentModified(true);
            Calligra::Tables::Sheet* tbl = kspreadView->activeSheet();
            KUndo2Command* command = new RemoveSheetCommand(tbl);
            kspreadDoc->addCommand(command);
            return true;
        }
    }
    return false;
}

QString KoAbstractApplicationController::currentSheetName() const
{
    if (documentType() == SpreadsheetDocument && m_view) {
            Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(m_view);
        return tablesView->activeSheet()->sheetName();
    }
    return QString();
}

KoCellTool* KoAbstractApplicationController::cellTool() const
{
    return m_cellTool;
}

void KoAbstractApplicationController::activeToolChanged(KoCanvasController* canvas,
                                                        int uniqueToolId)
{
   Q_UNUSED(uniqueToolId);
   QObject *canvasObject = dynamic_cast<QObject*>(canvas);
   QString newTool = KoToolManager::instance()->activeToolId();
   kDebug() << "-------------------------------" << newTool;
   // only Pan tool or Text tool should ever be the active tool, so if
   // another tool got activated, switch back to pan tool
    if (newTool != panToolFactoryId() && newTool != textToolFactoryId() && newTool != cellToolFactoryId() 
        && newTool != "InteractionTool") {
        KoToolManager::instance()->switchToolRequested(panToolFactoryId());
    }
    else
        kDebug() << "-ACCEPTED-";
    canvasObject->setProperty("FingerScrollable", true);
}

void KoAbstractApplicationController::updateWindowTitle(const QString& fileName)
{
    if (fileName.isEmpty())
        setWindowTitle(applicationName());
    else if (m_onlyDisplayDocumentNameInTitle)
        setWindowTitle(fileName);
    else
        setWindowTitle(i18n("%1 - %2", fileName, applicationName()));
}

void KoAbstractApplicationController::setOnlyDisplayDocumentNameInTitle(bool set)
{
    if (m_onlyDisplayDocumentNameInTitle == set)
        return;
    m_onlyDisplayDocumentNameInTitle = set;
    updateWindowTitle(m_fileName);
}

void KoAbstractApplicationController::setHorizontalScrollBarVisible(bool set)
{
    if (canvasControllerWidget())
        canvasControllerWidget()->setHorizontalScrollBarPolicy(set ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(view());
    if (tablesView) {
//! @todo not enough
// this breaks panning!        tablesView->showHorizontalScrollBar(set);
    }
}

void KoAbstractApplicationController::setVerticalScrollBarVisible(bool set)
{
    if (canvasControllerWidget())
        canvasControllerWidget()->setVerticalScrollBarPolicy(set ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
    Calligra::Tables::View *tablesView = qobject_cast<Calligra::Tables::View*>(view());
    if (tablesView) {
//! @todo not enough
// this breaks panning!        tablesView->showVerticalScrollBar(false);
    }
}

QString KoAbstractApplicationController::panToolFactoryId() { return QLatin1String("PanTool"); }
QString KoAbstractApplicationController::textToolFactoryId() { return QLatin1String("TextToolFactory_ID"); }
QString KoAbstractApplicationController::cellToolFactoryId() { return QLatin1String("KoAbstractionCellToolFactory"); }
