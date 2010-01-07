/*
 * This file is part of Maemo 5 Office UI for KOffice
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Manikandaprasad N C <manikandaprasad.chandrasekar@nokia.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "Common.h"
#include "ZoomDialog.h"
#include "HildonMenu.h"
#include "NotifyDialog.h"
#include "AboutDialog.h"

#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QLineEdit>
#include <QCheckBox>
#include <QScrollBar>
#include <QTimer>
#include <QIcon>
#include <QPushButton>
#include <QSize>
#include <QTextDocument>
#include <QTextCursor>
#include <QPair>
#include <QDesktopServices>
#include <QMenuBar>
#include <QX11Info>
#include <QShortcut>
#include <QProcess>

#include <kfileitem.h>
#include <kparts/part.h>
#include <kparts/componentfactory.h>

#include <KoView.h>
#include <KoCanvasBase.h>
#include <KoDocumentInfo.h>
#include <kdemacros.h>
#include <KoCanvasController.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>
#include <KoToolProxy.h>
#include <KoTool.h>
#include <KoCanvasResourceProvider.h>
#include <KoToolManager.h>
#include <KoShape.h>
#include <KoShapeManager.h>
#include <KoShapeUserData.h>
#include <KoTextShapeData.h>
#include <KoSelection.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoPAView.h>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

MainWindow::MainWindow(Splash *aSplash, QWidget *parent)
        : QMainWindow(parent),
        m_ui(new Ui::MainWindow),
        m_search(NULL),
        m_doc(NULL),
        m_view(NULL),
        m_controller(NULL),
        m_vPage(0),
        m_hPage(0),
        m_pressed(false),
        m_isViewToolBar(true),
        m_fsTimer(NULL),
        m_fsButton(NULL),
        m_fsIcon(FS_BUTTON_PATH),
        m_fsPPTBackButton(NULL),
        m_fsPPTForwardButton(NULL),
        m_currentPage(1),
        m_index(0),
        m_wholeWord(false),
        m_type(Text),
        m_splash(aSplash),
        m_panningCount(0),
        m_isLoading(false)
{
    init();
}

void MainWindow::init()
{
    m_ui->setupUi(this);

    QMenuBar* menu = menuBar();
    menu->addAction(m_ui->actionOpen);
    menu->addAction(m_ui->actionAbout);

    m_search = new QLineEdit(this);
    m_ui->SearchToolBar->insertWidget(m_ui->actionSearchOption, m_search);
    m_exactMatchCheckBox = new QCheckBox(i18n("Exact Match"), this);
    m_ui->SearchToolBar->insertWidget(m_ui->actionSearchOption, m_exactMatchCheckBox);
    m_ui->SearchToolBar->hide();

    connect(m_search, SIGNAL(returnPressed()), SLOT(nextWord()));
    connect(m_search, SIGNAL(textEdited(QString)), SLOT(startSearch()));

    connect(m_ui->actionSearch, SIGNAL(toggled(bool)), this, SLOT(toggleToolBar(bool)));
    connect(m_ui->actionOpen, SIGNAL(triggered()), this, SLOT(openFileDialog()));
    connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAboutDialog()));
    connect(m_ui->actionFullScreen, SIGNAL(triggered()), this, SLOT(fullScreen()));

    m_ui->actionZoomIn->setShortcuts(QKeySequence::ZoomIn);
    m_ui->actionZoomIn->setShortcutContext(Qt::ApplicationShortcut);
    m_ui->actionZoomOut->setShortcuts(QKeySequence::ZoomOut);
    m_ui->actionZoomOut->setShortcutContext(Qt::ApplicationShortcut);
    connect(m_ui->actionZoomIn, SIGNAL(triggered()), this, SLOT(zoomIn()));
    connect(m_ui->actionZoomOut, SIGNAL(triggered()), this, SLOT(zoomOut()));
    connect(m_ui->actionZoomLevel, SIGNAL(triggered()), this, SLOT(zoom()));

    connect(m_ui->actionNextPage, SIGNAL(triggered()), this, SLOT(nextPage()));
    connect(m_ui->actionPrevPage, SIGNAL(triggered()), this, SLOT(prevPage()));

    connect(m_ui->actionPanningOn, SIGNAL(triggered()), this, SLOT(toggleSelection()));
    m_ui->actionCopy->setEnabled(false);
    connect(m_ui->actionCopy, SIGNAL(triggered()), this, SLOT(copy()));

    connect(m_ui->actionPrevWord, SIGNAL(triggered()), this, SLOT(previousWord()));
    connect(m_ui->actionNextWord, SIGNAL(triggered()), this, SLOT(nextWord()));
    connect(m_exactMatchCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(searchOptionChanged(int)));

    m_fsTimer = new QTimer(this);
    Q_CHECK_PTR(m_fsTimer);
    connect(m_fsTimer, SIGNAL(timeout()), this, SLOT(fsTimer()));

    m_fsButton = new QPushButton(this);
    Q_CHECK_PTR(m_fsButton);
    m_fsButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
    m_fsButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
    m_fsButton->setIcon(m_fsIcon);
    m_fsButton->hide();
    connect(m_fsButton, SIGNAL(clicked()), SLOT(fsButtonClicked()));
    qApp->installEventFilter(this);

    updateActions();

    /* taking care of Zoom buttons : starts */
    unsigned long val = 1;
    Atom atom = XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", False);
    XChangeProperty(QX11Info::display(), winId(), atom, XA_INTEGER, 32,
                    PropModeReplace,
                    (unsigned char *) &val, 1);
    /* taking care of Zoom buttons : ends */

    m_copyShortcut = new QShortcut(QKeySequence::Copy, this);
    connect(m_copyShortcut, SIGNAL(activated()), this, SLOT(copy()));
}

MainWindow::~MainWindow()
{
    closeDocument();
    delete m_ui;
    m_ui = 0;
}

void MainWindow::openAboutDialog(void)
{
    QList<HildonMenu *> all_dlg = this->findChildren<HildonMenu *>();
    foreach(HildonMenu *menu, all_dlg)
    menu->close();

    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::toggleToolBar(bool show)
{
    if (show) {
        m_ui->viewToolBar->hide();
        m_ui->SearchToolBar->show();
        m_isViewToolBar = false;
        m_search->setFocus();
        m_search->selectAll();
    } else {
        m_search->clearFocus();
        m_ui->SearchToolBar->hide();
        m_ui->viewToolBar->show();
        m_isViewToolBar = true;
        KoToolManager::instance()->switchToolRequested(PanTool_ID);
    }
}

void MainWindow::showFullScreenPresentationIcons(void)
{
    if (!m_controller)
        return;

    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    QSize size(this->frameSize());

    if (m_controller->verticalScrollBar()->isVisible()) {
        QSize vScrlbar = m_controller->verticalScrollBar()->size();
        vScrlbarWidth = vScrlbar.width();
    }

    if (m_controller->horizontalScrollBar()->isVisible()) {
        QSize hScrlbar = m_controller->horizontalScrollBar()->size();
        hScrlbarHeight = hScrlbar.height();
    }

    if (!m_fsPPTBackButton) {
        m_fsPPTBackButton = new QPushButton(this);
        m_fsPPTBackButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
        m_fsPPTBackButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTBackButton->setIcon(QIcon(FS_PPT_BACK_BUTTON_PATH));
        connect(m_fsPPTBackButton, SIGNAL(clicked()), this, SLOT(prevPage()));
        m_fsPPTBackButton->move(size.width() - FS_BUTTON_SIZE*3 - vScrlbarWidth,
                                size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (!m_fsPPTForwardButton) {
        m_fsPPTForwardButton = new QPushButton(this);
        m_fsPPTForwardButton->setStyleSheet(FS_BUTTON_STYLE_SHEET);
        m_fsPPTForwardButton->resize(FS_BUTTON_SIZE, FS_BUTTON_SIZE);
        m_fsPPTForwardButton->setIcon(QIcon(FS_PPT_FORWARD_BUTTON_PATH));
        connect(m_fsPPTForwardButton, SIGNAL(clicked()), this, SLOT(nextPage()));
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    }

    if (m_currentPage < m_doc->pageCount()) {
        m_fsPPTForwardButton->move(size.width() - FS_BUTTON_SIZE*2 - vScrlbarWidth,
                                   size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
        m_fsPPTForwardButton->show();
        m_fsPPTForwardButton->raise();
    }

    if (m_currentPage <= m_doc->pageCount() && m_currentPage != 1) {
        m_fsPPTBackButton->move(size.width() - FS_BUTTON_SIZE*3 - vScrlbarWidth,
                                size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
        m_fsPPTBackButton->show();
        m_fsPPTBackButton->raise();
    }
}

void MainWindow::openFileDialog()
{
    if (m_splash && !this->isActiveWindow()) {
        this->show();
        m_splash->finish(this);
        m_splash = 0;
    }

    QList<HildonMenu *> all_dlg = this->findChildren<HildonMenu *>();
    foreach(HildonMenu *menu, all_dlg)
    menu->close();

    QString file = QFileDialog::getOpenFileName(this, i18n("Open Document"),
                   QDesktopServices::storageLocation(
                       QDesktopServices::DocumentsLocation),
                   FILE_CHOOSER_FILTER);

    if (!file.isNull() && !checkFiletype(file)) {
        return;
    }
    if (!file.isNull() && !m_isLoading) {
        m_fileName = file;
        //closeDocument();
        QTimer::singleShot(100, this, SLOT(doOpenDocument()));
    }
}

void MainWindow::closeDocument()
{
    setWindowTitle(i18n("Office Viewer"));
    if (m_doc == NULL)  return;
    setCentralWidget(0);
    m_positions.clear();
    // the presentation and text document instances seem to require different ways to do cleanup
    if (m_type == Presentation || m_type == Spreadsheet) {
        KoToolManager::instance()->removeCanvasController(m_controller);
        delete m_doc;
    } else {
        delete m_doc;
        KoToolManager::instance()->removeCanvasController(m_controller);
        delete m_view;
    }
    m_doc = NULL;
    m_view = NULL;
    m_currentPage = 1;
    m_controller = NULL;
    m_type = Text;
    updateActions();

    m_ui->actionZoomLevel->setText(i18n("%1 %", 100));
    m_ui->actionPageNumber->setText(i18n("%1 of %2", 0, 0));
}

void MainWindow::doOpenDocument()
{
    openDocument(m_fileName);
}

void MainWindow::raiseWindow(void)
{
    Display *display = QX11Info::display();
    XEvent e;
    Window root = RootWindow(display, DefaultScreen(display));
    memset(&e, 0, sizeof(e));
    e.xclient.type         = ClientMessage;
    e.xclient.window       = effectiveWinId();
    e.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    e.xclient.format       = 32;
    XSendEvent(display, root, False, SubstructureRedirectMask, &e);
    XFlush(display);
}

void MainWindow::openDocument(const QString &fileName)
{
    if (!checkFiletype(fileName))
        return;

    if (m_doc) {
        QStringList args;
        args << fileName;
        QProcess::startDetached(FREOFFICE_APPLICATION_PATH, args);
        return;
    }

    raiseWindow();

    setShowProgressIndicator(true);
    QString mimetype = KMimeType::findByPath(fileName)->name();
    closeDocument();
    int errorCode = 0;
    m_isLoading = true;
    m_doc = KParts::ComponentFactory::createPartInstanceFromQuery<KoDocument>(
                mimetype, QString(),
                0, 0, QStringList(),
                &errorCode);
    if (!m_doc) {
        setShowProgressIndicator(false);
        return;
    }

    KUrl url;
    url.setPath(fileName);
    m_doc->setCheckAutoSaveFile(false);
    m_doc->setAutoErrorHandlingEnabled(true);
    if (!m_doc->openUrl(url)) {
        setShowProgressIndicator(false);
        return;
    }

    m_doc->setReadWrite(false);
    m_doc->setAutoSave(0);
    m_view = m_doc->createView();
    QList<KoCanvasController*> controllers = m_view->findChildren<KoCanvasController*>();
    if (controllers.isEmpty()) {
        setShowProgressIndicator(false);
        return;// Panic
    }
    m_controller = controllers.first();
    if (!m_controller) {
        setShowProgressIndicator(false);
        return;
    }

    QString fname = url.fileName();
    QString ext = KMimeType::extractKnownExtension(fname);
    if (ext.length()) {
        fname.chop(ext.length() + 1);
    }

    if (!QString::compare(ext, EXT_ODP, Qt::CaseInsensitive) ||
            !QString::compare(ext, EXT_PPTX, Qt::CaseInsensitive) ||
            !QString::compare(ext, EXT_PPT, Qt::CaseInsensitive)) {
        m_type = Presentation;
    } else if (!QString::compare(ext, EXT_ODS, Qt::CaseInsensitive) ||
               !QString::compare(ext, EXT_XLSX, Qt::CaseInsensitive) ||
               !QString::compare(ext, EXT_XLS, Qt::CaseInsensitive)) {
        m_type = Spreadsheet;
    } else {
        m_type = Text;
    }

    setWindowTitle(QString("%1 - %2").arg(i18n("Office Viewer"), fname));

    m_controller->setProperty("FingerScrollable", true);
    setCentralWidget(m_controller);
    QTimer::singleShot(250, this, SLOT(updateUI()));

    KoCanvasBase *canvas = m_controller->canvas();
    connect(canvas->resourceProvider(),
            SIGNAL(resourceChanged(int, const QVariant &)),
            this,
            SLOT(resourceChanged(int, const QVariant &)));
    connect(KoToolManager::instance(),
            SIGNAL(changedTool(KoCanvasController*, int)),
            SLOT(activeToolChanged(KoCanvasController*, int)));

    KoToolManager::instance()->switchToolRequested(PanTool_ID);

    setShowProgressIndicator(false);
    m_isLoading = false;
    if (m_splash && !this->isActiveWindow()) {
        this->show();
        m_splash->finish(m_controller);
        m_splash = 0;
    }
}

bool MainWindow::checkFiletype(const QString &fileName)
{
    bool ret = false;
    QList<QString> extensions;
    //Add Txt extension after adding ascii filter to koffice package
    /*extensions << EXT_DOC << EXT_DOCX << EXT_ODT << EXT_TXT \*/
    extensions << EXT_DOC << EXT_DOCX << EXT_ODT  \
    << EXT_PPT << EXT_PPTX << EXT_ODP << EXT_RTF \
    << EXT_ODS << EXT_XLS << EXT_XLSX;
    QString ext = KMimeType::extractKnownExtension(fileName);

    for (int i = 0; i < extensions.size(); i++) {
        if (extensions.at(i) == ext)
            ret = true;
    }

    if (!ret) {
        NotifyDialog dialog(this);
        dialog.exec();
    }
    return ret;
}

void MainWindow::updateUI()
{
    updateActions();
    if (!m_view || !m_ui)
        return;
    int factor = 100;
    QString pageNo = i18n("pg%1 - pg%2", 0, 0);

    if (m_doc->pageCount() > 0) {
        factor = m_view->zoomController()->zoomAction()->effectiveZoom() * 100;
        pageNo = i18n("pg%1 - pg%2", 1, QString::number(m_doc->pageCount()));
    }

    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
    m_ui->actionPageNumber->setText(pageNo);

    m_vPage = m_controller->verticalScrollBar()->pageStep();
    m_hPage = m_controller->horizontalScrollBar()->pageStep();
}

void MainWindow::resourceChanged(int key, const QVariant &value)
{
    if (KoCanvasResource::CurrentPage == key) {
        m_currentPage = value.toInt();
        if (m_type == Presentation && isFullScreen()) {
            if (m_currentPage == 1)
                m_fsPPTBackButton->hide();
            else if (m_currentPage > 1)
                m_fsPPTBackButton->show();
            if (m_currentPage == m_doc->pageCount())
                m_fsPPTForwardButton->hide();
            else if (m_currentPage < m_doc->pageCount())
                m_fsPPTForwardButton->show();
        }

        QString pageNo = i18n("pg%1 - pg%2", QString::number(value.toInt()), QString::number(m_doc->pageCount()));
        m_ui->actionPageNumber->setText(pageNo);
    }
}

void MainWindow::fullScreen()
{
    if (!m_ui)
        return;
    int vScrlbarWidth = 0;
    int hScrlbarHeight = 0;
    m_ui->viewToolBar->hide();
    m_ui->SearchToolBar->hide();
    showFullScreen();
    QSize size(this->frameSize());

    if (m_controller) {
        if (m_controller->verticalScrollBar()->isVisible()) {
            QSize vScrlbar = m_controller->verticalScrollBar()->size();
            vScrlbarWidth = vScrlbar.width();
        }
        if (m_controller->horizontalScrollBar()->isVisible()) {
            QSize hScrlbar = m_controller->horizontalScrollBar()->size();
            hScrlbarHeight = hScrlbar.height();
        }
    }

    m_fsButton->move(size.width() - FS_BUTTON_SIZE - vScrlbarWidth,
                     size.height() - FS_BUTTON_SIZE - hScrlbarHeight);
    m_fsButton->show();
    m_fsButton->raise();
    m_fsTimer->start(3000);

    if (m_type == Presentation)
        showFullScreenPresentationIcons();
}

void MainWindow::zoomIn()
{
    if (!m_view || !m_ui)
        return;
    KoZoomAction *zAction = m_view->zoomController()->zoomAction();
    zAction->zoomIn();
    int factor = zAction->effectiveZoom() * 100;
    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
}

void MainWindow::zoomOut()
{
    if (!m_view || !m_ui)
        return;
    KoZoomAction *zAction = m_view->zoomController()->zoomAction();
    zAction->zoomOut();
    int factor = zAction->effectiveZoom() * 100;
    m_ui->actionZoomLevel->setText(i18n("%1 %", QString::number(factor)));
}

void MainWindow::zoom()
{
    ZoomDialog dlg(this);
    connect(&dlg, SIGNAL(fitPage()), SLOT(zoomToPage()));
    connect(&dlg, SIGNAL(fitPageWidth()), SLOT(zoomToPageWidth()));
    dlg.exec();
}

void MainWindow::zoomToPage()
{
    if (!m_view || !m_ui)
        return;
    m_view->zoomController()->setZoomMode(KoZoomMode::ZOOM_PAGE);
}

void MainWindow::zoomToPageWidth()
{
    if (!m_view || !m_ui)
        return;
    m_view->zoomController()->setZoomMode(KoZoomMode::ZOOM_WIDTH);
}

void MainWindow::prevPage()
{
    if (!m_controller)
        return;
    if ((m_doc->pageCount() > 0) && triggerAction("page_previous"))
        return;
    m_vPage = m_controller->verticalScrollBar()->pageStep();
    m_controller->pan(QPoint(0, -m_vPage));
}

void MainWindow::nextPage()
{
    if (!m_controller)
        return;
    if ((m_doc->pageCount() > 0) && triggerAction("page_next"))
        return;
    m_vPage = m_controller->verticalScrollBar()->pageStep();
    m_controller->pan(QPoint(0, m_vPage));
}

void MainWindow::fsTimer()
{
    if (!m_pressed) {
        m_fsTimer->stop();
        m_fsButton->hide();
        if (m_fsPPTBackButton && m_fsPPTBackButton->isVisible())
            m_fsPPTBackButton->hide();
        if (m_fsPPTForwardButton && m_fsPPTForwardButton->isVisible())
            m_fsPPTForwardButton->hide();
    }
}

void MainWindow::fsButtonClicked()
{
    if (!m_ui)
        return;

    m_fsButton->hide();

    if (m_fsPPTBackButton && m_fsPPTBackButton->isVisible())
        m_fsPPTBackButton->hide();

    if (m_fsPPTForwardButton && m_fsPPTForwardButton->isVisible())
        m_fsPPTForwardButton->hide();

    if (m_isViewToolBar)
        m_ui->viewToolBar->show();
    else
        m_ui->SearchToolBar->show();

    showNormal();
}

void MainWindow::toggleSelection()
{
    if (!m_doc || !m_view)
        return;

    if (KoToolManager::instance()->activeToolId() != TextTool_ID) {
        // before activating the text tool we need to make sure that the selection
        // contains at least one text shape, assuming there is at least one text shape
        // available
        KoCanvasBase *canvas = m_controller->canvas();
        Q_CHECK_PTR(canvas);


        // first check if the current selection already has a text shape in it
        bool hasText = false;
        KoSelection *selection = canvas->shapeManager()->selection();
        foreach(KoShape *shape, selection->selectedShapes()) {
            KoTextShapeData* tsd = qobject_cast<KoTextShapeData*>(shape->userData());
            if (tsd) {
                hasText = true;
                break;
            }
        }

        if (!hasText) {
            // no text in selection, try to find a text shape and add the first
            // one to the selection
            KoShapeManager *manager = canvas->shapeManager();
            Q_CHECK_PTR(manager);
            QList<KoShape*> shapes = manager->shapes();
            foreach(KoShape *shape, shapes) {
                KoTextShapeData* tsd = qobject_cast<KoTextShapeData*>(shape->userData());
                if (tsd) {
                    selection->select(shape);
                    break;
                }
            }
        }

        KoToolManager::instance()->switchToolRequested(TextTool_ID);
    } else {
        KoToolManager::instance()->switchToolRequested(PanTool_ID);
    }
}

static void findTextShapesRecursive(KoShapeContainer* con, KoPAPageBase* page,
                                    QList<QPair<KoPAPageBase*, KoShape*> >& shapes,
                                    QList<QTextDocument*>& docs)
{
    foreach(KoShape* shape, con->childShapes()) {
        KoTextShapeData* tsd = qobject_cast<KoTextShapeData*> (shape->userData());
        if (tsd) {
            shapes.append(qMakePair(page, shape));
            docs.append(tsd->document());
        }
        KoShapeContainer* child = dynamic_cast<KoShapeContainer*>(shape);
        if (child) findTextShapesRecursive(child, page, shapes, docs);
    }
}

void MainWindow::startSearch()
{
    if (!m_search || !m_controller)
        return;

    QString searchString = m_search->text();

    KoCanvasBase *canvas = m_controller->canvas();
    Q_CHECK_PTR(canvas);

    KoPADocument* padoc = qobject_cast<KoPADocument*>(m_doc);
    if (padoc) {
        // loop over all pages starting from current page to get
        // search results in the right order
        int curPage = canvas->resourceProvider()->resource(\
                      KoCanvasResource::CurrentPage).toInt() - 1;
        QList<QPair<KoPAPageBase*, KoShape*> > textShapes;
        QList<QTextDocument*> textDocs;
        for (int page = 0; page < padoc->pageCount(); page++) {
            KoPAPageBase* papage = padoc->pageByIndex(page, false);
            findTextShapesRecursive(papage, papage, textShapes, textDocs);
        };

        findText(textDocs, textShapes, searchString);

        // now find the first search result in the list of positions
        // counting from the current page
        // this is not very efficient...
        bool foundIt = false;
        for (int page = curPage; page < padoc->pageCount(); page++) {
            for (int i = 0; i < m_positions.size(); i++) {
                if (m_positions[i].first.first == padoc->pageByIndex(page, false)) {
                    foundIt = true;
                    m_index = i;
                    highlightText(m_index);
                    break;
                }
            }
            if (foundIt) break;
        }
        if (!foundIt) {
            for (int page = 0; page < curPage; page++) {
                for (int i = 0; i < m_positions.size(); i++) {
                    if (m_positions[i].first.first ==
                            padoc->pageByIndex(page, false)) {
                        foundIt = true;
                        m_index = i;
                        highlightText(m_index);
                        break;
                    }
                }
                if (foundIt) break;
            }
        }
    } else {
        KoShapeManager *manager = canvas->shapeManager();
        Q_CHECK_PTR(manager);
        QList<KoShape*> shapes = manager->shapes();

        int size = shapes.size();
        if (size != 0) {
            QList<KoTextShapeData*> shapeDatas;
            QList<QTextDocument*> textDocs;
            QList<QPair<KoPAPageBase*, KoShape*> > textShapes;
            QSet<QTextDocument*> textDocSet;

            for (int i = 0; i < size; i++) {
                shapeDatas.append(qobject_cast<KoTextShapeData*> \
                                  (shapes.at(i)->userData()));
                if (shapeDatas.at(i) && !textDocSet.contains(\
                        shapeDatas.at(i)->document())) {
                    textDocSet.insert(shapeDatas.at(i)->document());
                    textDocs.append(shapeDatas.at(i)->document());
                    textShapes.append(qMakePair((KoPAPageBase*)(0),
                                                shapes.at(i)));
                }
            }
            findText(textDocs, textShapes, searchString);
        }
    }
}

void MainWindow::findText(QList<QTextDocument*> aDocs,
                          QList<QPair<KoPAPageBase*, KoShape*> > shapes,
                          const QString &aText)
{
    if (aDocs.isEmpty())
        return;

    m_positions.clear();

    for (int i = 0; i < aDocs.size(); i++) {
        QTextDocument* doc = aDocs.at(i);
        KoShape* shape = shapes.at(i).second;

        QTextCursor result(doc);
        do {
            if (!m_wholeWord)
                result = doc->find(aText, result);
            else
                result = doc->find(aText, result, QTextDocument::FindWholeWords);
            if (result.hasSelection()) {
                m_positions.append(qMakePair(qMakePair(shapes.at(i).first, shape),
                                             qMakePair(result.selectionStart(),
                                                       result.selectionEnd())));
            }
        } while (!result.isNull());
    }

    m_index = 0;
    if (!m_positions.isEmpty())
        highlightText(m_index);
    else
        m_ui->actionSearchResult->setText(i18n("%1 of %2", 0, 0));
}

void MainWindow::highlightText(int aIndex)
{
    if (!m_controller || !m_ui)
        return;

    KoCanvasBase *canvas = m_controller->canvas();
    Q_CHECK_PTR(canvas);

    // first potentially go to the correct page
    KoPAPageBase* page = m_positions.at(aIndex).first.first;
    if (page) {
        KoPAView* paview = static_cast<KoPAView*>(m_view);
        if (paview->activePage() != page) {
            paview->doUpdateActivePage(page);
        }
    }

    // secondly set the currently selected text shape to the one containing this search result
    KoSelection *selection = canvas->shapeManager()->selection();
    KoShape* shape = m_positions.at(aIndex).first.second;
    if (selection->count() != 1 || selection->firstSelectedShape() != shape) {
        selection->deselectAll();
        selection->select(shape);
    }
    // ugly hack, but if we don't first disable and than re-enable the text tool
    // it will still keep the wrong textshape selected
    KoToolManager::instance()->switchToolRequested(PanTool_ID);
    KoToolManager::instance()->switchToolRequested(TextTool_ID);

    KoCanvasResourceProvider *provider = canvas->resourceProvider();
    Q_CHECK_PTR(provider);

    QString sizeStr = QString::number(m_positions.size());
    QString indexStr = QString::number(aIndex + 1);

    m_ui->actionSearchResult->setText(i18n("%1 of %2", m_positions.size(), aIndex + 1));

    provider->setResource(KoText::CurrentTextPosition,
                          m_positions.at(aIndex).second.first);
    provider->setResource(KoText::CurrentTextAnchor,
                          m_positions.at(aIndex).second.second);
}

void MainWindow::previousWord()
{
    if (m_positions.isEmpty())
        return;
    if (m_index == 0) {
        m_index = m_positions.size() - 1;
    } else {
        m_index--;
    }
    highlightText(m_index);
}

void MainWindow::nextWord()
{
    if (m_positions.isEmpty())
        return;
    if (m_index == m_positions.size() - 1) {
        m_index = 0;
    } else {
        m_index++;
    }
    highlightText(m_index);
}

void MainWindow::searchOptionChanged(int aCheckBoxState)
{
    if (aCheckBoxState == Qt::Unchecked)
        m_wholeWord = false;
    if (aCheckBoxState == Qt::Checked)
        m_wholeWord = true;


    startSearch();
}

void MainWindow::copy()
{
    m_controller->canvas()->toolProxy()->copy();
}

bool MainWindow::triggerAction(const char* name)
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

void MainWindow::updateActions()
{
    bool docLoaded = m_doc;
    m_ui->actionSearch->setEnabled(docLoaded);
    m_ui->actionFullScreen->setEnabled(docLoaded);
    m_ui->actionZoomIn->setEnabled(docLoaded);
    m_ui->actionZoomOut->setEnabled(docLoaded);
    m_ui->actionZoomLevel->setEnabled(docLoaded);
    m_ui->actionNextPage->setEnabled(docLoaded);
    m_ui->actionPrevPage->setEnabled(docLoaded);
    m_ui->actionPanningOn->setEnabled(docLoaded);
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    // show buttons in full screen mode if user taps anywhere in the screen
    if (event && this->isFullScreen()) {
        if (event->type() == QEvent::MouseButtonPress ||
                event->type() == QEvent::TabletPress) {
            m_pressed = true;
            m_panningCount = 0;
            m_slideChangePossible = true;
            m_fsTimer->start(3000);
            m_pressPos = (reinterpret_cast<QMouseEvent*>(event))->pos();
        } else if (event->type() == QEvent::MouseButtonRelease ||
                   event->type() == QEvent::TabletRelease) {
            m_pressed = false;

            //show buttons only if user just tap the screen without
            //panning document
            if (m_panningCount <= 5) {
                m_fsButton->show();
                m_fsButton->raise();
                if (m_type == Presentation)
                    showFullScreenPresentationIcons();
                m_fsTimer->start(3000);
                m_slideChangePossible = false;
            }

            m_panningCount = 0;
        } else if ((event->type() == QEvent::TabletMove ||
                    event->type() == QEvent::MouseMove) && m_pressed) {
            int sliderMin = m_controller->verticalScrollBar()->minimum();
            int sliderVal = m_controller->verticalScrollBar()->value();
            int sliderMax = m_controller->verticalScrollBar()->maximum();
            if (sliderVal == sliderMin || sliderVal == sliderMax)
                m_panningCount++;
        }

        if (m_type == Presentation && m_slideChangePossible
                && m_panningCount > 5 && (event->type() == QEvent::MouseMove
                                          || event->type() == QEvent::TabletMove)) {
            int sliderMin = m_controller->verticalScrollBar()->minimum();
            int sliderVal = m_controller->verticalScrollBar()->value();
            int sliderMax = m_controller->verticalScrollBar()->maximum();
            QPoint movePos = (reinterpret_cast<QMouseEvent*>(event))->pos();
            if (movePos.y() - m_pressPos.y() > 50 && sliderVal == sliderMin) {
                m_slideChangePossible = false;
                triggerAction("page_previous");
            }
            if (m_pressPos.y() - movePos.y() > 50 && sliderVal == sliderMax) {
                m_slideChangePossible = false;
                triggerAction("page_next");
            }
        }
    }

    // Maemo Qt hardcodes handling of F6 to toggling full screen directly, so
    // override that shortcut to do what we want it to do instead.
    if (event && event->type() == QEvent::Shortcut) {
        QShortcutEvent *qse = reinterpret_cast<QShortcutEvent*>(event);
        if (qse->key() == QKeySequence(Qt::Key_F6)) {
            if (m_ui->actionFullScreen->isEnabled())
                fullScreen();
            return true;
        } else if (qse->key() == QKeySequence(Qt::Key_F4)) {
            showApplicationMenu();
            return true;
        }
    } else if (event->type() == QEvent::ShortcutOverride && isFullScreen()) {
        // somehow shortcuts aren't working properly in full-screen mode...
        QKeyEvent* qke = reinterpret_cast<QKeyEvent*>(event);
        if (qke->key() == Qt::Key_F7) {
            m_ui->actionZoomIn->trigger();
            return true;
        } else if (qke->key() == Qt::Key_F8) {
            m_ui->actionZoomOut->trigger();
            return true;
        }
    }

    // change presentation slide in fullscreen mode if user taps on
    // left or right side of the screen
    if (watched && event && m_type == Presentation && m_doc->pageCount() > 0
            && this->isFullScreen()
            && event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = reinterpret_cast<QMouseEvent*>(event);
        // check that event wasn't from full screen push button
        if (QString::compare("QPushButton", watched->metaObject()->className())) {
            QSize size(this->frameSize());
            if (mouseEvent->x() <= FS_BUTTON_SIZE) {
                triggerAction("page_previous");
            } else if (mouseEvent->x() >= (size.width() - FS_BUTTON_SIZE)) {
                triggerAction("page_next");
            }
        }
    }

    return false;
    //return QMainWindow::eventFilter(watched, event);
}

void MainWindow::showApplicationMenu()
{
    HildonMenu menu(this);
    menu.exec();
}

void MainWindow::activeToolChanged(KoCanvasController* canvas, int)
{
    QString newTool = KoToolManager::instance()->activeToolId();
    // only Pan tool or Text tool should ever be the active tool, so if
    // another tool got activated, switch back to pan tool
    if (newTool != PanTool_ID && newTool != TextTool_ID) {
        KoToolManager::instance()->switchToolRequested(PanTool_ID);
    }

    canvas->setProperty("FingerScrollable", true);
    m_ui->actionCopy->setEnabled(newTool == TextTool_ID);
    m_ui->actionPanningOn->setChecked(newTool == TextTool_ID);
}

void MainWindow::setShowProgressIndicator(bool visible)
{
    unsigned long val = visible ? 1 : 0;
    Atom atom = XInternAtom(QX11Info::display(),
                            "_HILDON_WM_WINDOW_PROGRESS_INDICATOR", False);
    XChangeProperty(QX11Info::display(), winId(), atom, XA_INTEGER,
                    32, PropModeReplace,
                    (unsigned char *) &val, 1);
}

//Function to check if application has been started by DBus
void MainWindow::checkDBusActivation()
{
    if (m_splash && !this->isActiveWindow())
        openFileDialog();
}

void MainWindow::loadScrollAndQuit()
{
    // if no document is loaded, simply quit
    if (m_doc == 0 || m_controller == 0) {
        QTimer::singleShot(1, qApp, SLOT(quit()));
        return;
    }
    // if still loading, wait some more
    if (m_doc->isLoading()) {
        QTimer::singleShot(100, this, SLOT(loadScrollAndQuit()));
        return;
    }
    // when done loading, start proceeding to the next page until the end of the document has been
    // reached
    bool done;
    if (m_type == Presentation) {
        done = m_currentPage == m_doc->pageCount();
    } else {
        QScrollBar *v = m_controller->verticalScrollBar();
        done = v->value() >= v->maximum();
    }
    if (done) {
        QTimer::singleShot(1, qApp, SLOT(quit()));
    } else {
        nextPage();
        QTimer::singleShot(20, this, SLOT(loadScrollAndQuit()));
    }
}
