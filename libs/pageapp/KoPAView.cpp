/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2006-2009 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2009 Inge Wallin <inge@lysator.liu.se>
   SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAView.h"

#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QLabel>
#include <QMimeData>
#include <QTabBar>

#include <KoCanvasControllerWidget.h>
#include <KoCanvasResourceManager.h>
#include <KoColorBackground.h>
#include <KoCopyController.h>
#include <KoCutController.h>
#include <KoDockerManager.h>
#include <KoDrag.h>
#include <KoFind.h>
#include <KoGridData.h>
#include <KoGuidesData.h>
#include <KoIcon.h>
#include <KoInlineTextObjectManager.h>
#include <KoMainWindow.h>
#include <KoModeBoxFactory.h>
#include <KoProperties.h>
#include <KoRuler.h>
#include <KoRulerController.h>
#include <KoSelection.h>
#include <KoShapeController.h>
#include <KoShapeFactoryBase.h>
#include <KoShapeLayer.h>
#include <KoShapeManager.h>
#include <KoShapeRegistry.h>
#include <KoStandardAction.h>
#include <KoTextDocumentLayout.h>
#include <KoToolBoxFactory.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoUnit.h>
#include <KoZoomAction.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include "KoPACanvas.h"
#include "KoPADocument.h"
#include "KoPADocumentStructureDocker.h"
#include "KoPAMasterPage.h"
#include "KoPAOdfPageSaveHelper.h"
#include "KoPAPage.h"
#include "KoPAPastePage.h"
#include "KoPAPrintJob.h"
#include "KoPAViewModeNormal.h"
#include "KoShapeTraversal.h"
#include "commands/KoPAChangeMasterPageCommand.h"
#include "commands/KoPAPageInsertCommand.h"
#include "dialogs/KoPAConfigureDialog.h"
#include "dialogs/KoPAMasterPageDialog.h"
#include "dialogs/KoPAPageLayoutDialog.h"
#include "widgets/KoPageNavigator.h"

#include <KActionCollection>
#include <KActionMenu>
#include <KLocalizedString>
#include <KMessageBox>
#include <KoNetAccess.h>
#include <PageAppDebug.h>
#include <ktoggleaction.h>

#include <QAction>
#include <QFileDialog>
#include <QStatusBar>
#include <QTemporaryFile>
#include <QUrl>
#include <qboxlayout.h>

class Q_DECL_HIDDEN KoPAView::Private
{
public:
    Private(KoPADocument *document)
        : doc(document)
        , canvas(nullptr)
        , activePage(nullptr)
    {
    }

    ~Private() = default;

    // These were originally private in the .h file
    KoPADocumentStructureDocker *documentStructureDocker;

    KoCanvasController *canvasController;
    KoZoomController *zoomController;
    KoCopyController *copyController;
    KoCutController *cutController;

    QAction *editPaste;
    QAction *deleteSelectionAction;

    KToggleAction *actionViewSnapToGrid;
    KToggleAction *actionViewShowMasterPages;

    QAction *actionInsertPage;
    QAction *actionCopyPage;
    QAction *actionDeletePage;

    QAction *actionMasterPage;
    QAction *actionPageLayout;

    QAction *actionConfigure;

    KoRuler *horizontalRuler;
    KoRuler *verticalRuler;
    KToggleAction *viewRulers;

    KoZoomAction *zoomAction;

    KToggleAction *showPageMargins;

    KoFind *find;

    KoPAViewMode *viewModeNormal;

    // This tab bar hidden by default. It could be used to alternate between view modes
    QTabBar *tabBar;

    QGridLayout *tabBarLayout;
    QVBoxLayout *mainLayout;
    QWidget *insideWidget;

    // status bar
    KoPageNavigator *pageNavigator;
    QLabel *status; ///< ordinary status
    QWidget *zoomActionWidget;

    // These used to be protected.
    KoPADocument *doc;
    KoPACanvas *canvas;
    KoPAPageBase *activePage;
};

KoPAView::KoPAView(KoPart *part, KoPADocument *document, KoPAFlags withModeBox, QWidget *parent)
    : KoView(part, document, parent)
    , d(new Private(document))
{
    initGUI(withModeBox);
    initActions();

    if (d->doc->pageCount() > 0)
        doUpdateActivePage(d->doc->pageByIndex(0, false));

    setAcceptDrops(true);
}

KoPAView::~KoPAView()
{
    KoToolManager::instance()->removeCanvasController(d->canvasController);

    removeStatusBarItem(d->status);
    removeStatusBarItem(d->zoomActionWidget);

    delete d->canvasController;
    delete d->zoomController;
    delete d->viewModeNormal;

    delete d;
}

void KoPAView::addImages(const QVector<QImage> &imageList, const QPoint &insertAt)
{
    // get position from event and convert to document coordinates
    QPointF pos = zoomHandler()->viewToDocument(insertAt) + kopaCanvas()->documentOffset() - kopaCanvas()->documentOrigin();

    // create a factory
    KoShapeFactoryBase *factory = KoShapeRegistry::instance()->value("PictureShape");
    if (!factory) {
        warnPageApp << "No picture shape found, cannot drop images.";
        return;
    }

    foreach (const QImage &image, imageList) {
        KoProperties params;
        auto v = QVariant::fromValue(image);
        params.setProperty("qimage", v);

        KoShape *shape = factory->createShape(&params, d->doc->resourceManager());

        if (!shape) {
            warnPageApp << "Could not create a shape from the image";
            return;
        }
        shape->setPosition(pos);
        pos += QPointF(25, 25); // increase the position for each shape we insert so the
                                // user can see them all.
        KUndo2Command *cmd = kopaCanvas()->shapeController()->addShapeDirect(shape);
        if (cmd) {
            KoSelection *selection = kopaCanvas()->shapeManager()->selection();
            selection->deselectAll();
            selection->select(shape);
        }
        kopaCanvas()->addCommand(cmd);
    }
}

void KoPAView::initGUI(KoPAFlags flags)
{
    d->mainLayout = new QVBoxLayout(this);
    d->mainLayout->setContentsMargins({});
    d->mainLayout->setSpacing(0);
    d->tabBarLayout = new QGridLayout;
    d->mainLayout->addLayout(d->tabBarLayout);
    d->insideWidget = new QWidget();
    QGridLayout *gridLayout = new QGridLayout(d->insideWidget);
    gridLayout->setContentsMargins({});
    gridLayout->setSpacing(0);

    d->canvas = new KoPACanvas(this, d->doc, this);
    KoCanvasControllerWidget *canvasController = new KoCanvasControllerWidget(actionCollection(), this);

    if (mainWindow()) {
        // this needs to be done before KoCanvasControllerWidget::setCanvas is called
        KoPADocumentStructureDockerFactory structureDockerFactory(KoDocumentSectionView::ThumbnailMode, d->doc->pageType());
        d->documentStructureDocker = qobject_cast<KoPADocumentStructureDocker *>(mainWindow()->createDockWidget(&structureDockerFactory));
        connect(d->documentStructureDocker, &KoPADocumentStructureDocker::pageChanged, proxyObject, &KoPAViewProxyObject::updateActivePage);
        connect(d->documentStructureDocker, &KoPADocumentStructureDocker::dockerReset, this, &KoPAView::reinitDocumentDocker);
    }

    d->canvasController = canvasController;
    d->canvasController->setCanvas(d->canvas);
    KoToolManager::instance()->addController(d->canvasController);
    KoToolManager::instance()->registerTools(actionCollection(), d->canvasController);

    d->zoomController = new KoZoomController(d->canvasController, zoomHandler(), actionCollection());
    connect(d->zoomController, &KoZoomController::zoomChanged, this, &KoPAView::slotZoomChanged);

    d->zoomAction = d->zoomController->zoomAction();

    // page/slide navigator
    d->pageNavigator = new KoPageNavigator(this);
    addStatusBarItem(d->pageNavigator, 0);

    // set up status bar message
    d->status = new QLabel(QString(), this);
    d->status->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    d->status->setMinimumWidth(300);
    addStatusBarItem(d->status, 1);
    connect(KoToolManager::instance(), &KoToolManager::changedStatusText, d->status, &QLabel::setText);
    d->zoomActionWidget = d->zoomAction->createWidget(statusBar());
    addStatusBarItem(d->zoomActionWidget, 0);

    d->zoomController->setZoomMode(KoZoomMode::ZOOM_PAGE);

    d->viewModeNormal = new KoPAViewModeNormal(this, d->canvas);
    setViewMode(d->viewModeNormal);

    // The rulers
    d->horizontalRuler = new KoRuler(this, Qt::Horizontal, viewConverter(d->canvas));
    d->horizontalRuler->setShowMousePosition(true);
    d->horizontalRuler->setUnit(d->doc->unit());
    d->verticalRuler = new KoRuler(this, Qt::Vertical, viewConverter(d->canvas));
    d->verticalRuler->setUnit(d->doc->unit());
    d->verticalRuler->setShowMousePosition(true);

    new KoRulerController(d->horizontalRuler, d->canvas->resourceManager());

    connect(d->doc, &KoDocument::unitChanged, this, &KoPAView::updateUnit);
    // Layout a tab bar
    d->tabBar = new QTabBar();
    d->tabBarLayout->addWidget(d->insideWidget, 1, 1);
    setTabBarPosition(Qt::Horizontal);

    gridLayout->addWidget(d->horizontalRuler->tabChooser(), 0, 0);
    gridLayout->addWidget(d->horizontalRuler, 0, 1);
    gridLayout->addWidget(d->verticalRuler, 1, 0);
    gridLayout->addWidget(canvasController, 1, 1);

    // tab bar is hidden by default a method is provided to access to the tab bar
    d->tabBar->hide();

    connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasOffsetXChanged, this, &KoPAView::pageOffsetChanged);
    connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasOffsetYChanged, this, &KoPAView::pageOffsetChanged);
    connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::sizeChanged, this, &KoPAView::pageOffsetChanged);
    connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::canvasMousePositionChanged, this, &KoPAView::updateMousePosition);
    d->verticalRuler->createGuideToolConnection(d->canvas);
    d->horizontalRuler->createGuideToolConnection(d->canvas);

    KoMainWindow *mw = mainWindow();
    if (flags & KoPAView::ModeBox) {
        if (mw) {
            KoModeBoxFactory modeBoxFactory(canvasController, qApp->applicationName(), i18n("Tools"));
            QDockWidget *modeBox = mw->createDockWidget(&modeBoxFactory);
            mw->dockerManager()->removeToolOptionsDocker();
            dynamic_cast<KoCanvasObserverBase *>(modeBox)->setObservedCanvas(d->canvas);
        }
    } else {
        if (mw) {
            KoToolBoxFactory toolBoxFactory;
            mw->createDockWidget(&toolBoxFactory);
            connect(canvasController, &KoCanvasControllerWidget::toolOptionWidgetsChanged, mw->dockerManager(), &KoDockerManager::newOptionWidgets);
        }
    }

    connect(shapeManager(), &KoShapeManager::selectionChanged, this, &KoPAView::selectionChanged);
    connect(shapeManager(), &KoShapeManager::contentChanged, this, [this]() {
        updateCanvasSize();
    });
    connect(d->doc, &KoPADocument::shapeAdded, this, [this]() {
        updateCanvasSize();
    });
    connect(d->doc, &KoPADocument::shapeRemoved, this, [this]() {
        updateCanvasSize();
    });
    connect(d->doc, &KoPADocument::update, this, &KoPAView::pageUpdated);
    connect(d->canvas, &KoPACanvas::documentSize, d->canvasController->proxyObject, [this](QSize sz) {
        d->canvasController->proxyObject->updateDocumentSize(sz);
    });
    connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::moveDocumentOffset, d->canvas, &KoPACanvas::slotSetDocumentOffset);
    connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::sizeChanged, this, [this]() {
        updateCanvasSize();
    });

    if (mw) {
        KoToolManager::instance()->requestToolActivation(d->canvasController);
    }

    d->mainLayout->addWidget(statusBar());
}

void KoPAView::initActions()
{
    QAction *action = actionCollection()->addAction(KStandardAction::Cut, "edit_cut", nullptr, nullptr);
    d->cutController = new KoCutController(kopaCanvas(), action);
    action = actionCollection()->addAction(KStandardAction::Copy, "edit_copy", nullptr, nullptr);
    d->copyController = new KoCopyController(kopaCanvas(), action);
    d->editPaste = actionCollection()->addAction(KStandardAction::Paste, "edit_paste", proxyObject, SLOT(editPaste()));
    connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &KoPAView::clipboardDataChanged);
    connect(d->canvas->toolProxy(), &KoToolProxy::toolChanged, this, &KoPAView::clipboardDataChanged);
    clipboardDataChanged();
    actionCollection()->addAction(KStandardAction::SelectAll, "edit_select_all", this, SLOT(editSelectAll()));
    actionCollection()->addAction(KStandardAction::Deselect, "edit_deselect_all", this, SLOT(editDeselectAll()));

    d->deleteSelectionAction = new QAction(koIcon("edit-delete"), i18n("D&elete"), this);
    actionCollection()->addAction("edit_delete", d->deleteSelectionAction);
    d->deleteSelectionAction->setShortcut(QKeySequence("Del"));
    d->deleteSelectionAction->setEnabled(false);
    connect(d->deleteSelectionAction, &QAction::triggered, this, &KoPAView::editDeleteSelection);
    connect(d->canvas->toolProxy(), &KoToolProxy::selectionChanged, d->deleteSelectionAction, &QAction::setEnabled);

    KToggleAction *showGrid = d->doc->gridData().gridToggleAction(d->canvas);
    actionCollection()->addAction("view_grid", showGrid);

    d->actionViewSnapToGrid = new KToggleAction(i18n("Snap to Grid"), this);
    d->actionViewSnapToGrid->setChecked(d->doc->gridData().snapToGrid());
    actionCollection()->addAction("view_snaptogrid", d->actionViewSnapToGrid);
    connect(d->actionViewSnapToGrid, &QAction::triggered, this, &KoPAView::viewSnapToGrid);

    KToggleAction *actionViewShowGuides = KoStandardAction::showGuides(this, SLOT(viewGuides(bool)), this);
    actionViewShowGuides->setChecked(d->doc->guidesData().showGuideLines());
    actionCollection()->addAction(KoStandardAction::name(KoStandardAction::ShowGuides), actionViewShowGuides);

    d->actionViewShowMasterPages = new KToggleAction(i18n("Show Master Pages"), this);
    actionCollection()->addAction("view_masterpages", d->actionViewShowMasterPages);
    connect(d->actionViewShowMasterPages, &QAction::triggered, this, &KoPAView::setMasterMode);

    d->viewRulers = new KToggleAction(i18n("Show Rulers"), this);
    actionCollection()->addAction("view_rulers", d->viewRulers);
    d->viewRulers->setToolTip(i18n("Show/hide the view's rulers"));
    connect(d->viewRulers, &QAction::triggered, proxyObject, &KoPAViewProxyObject::setShowRulers);
    setShowRulers(d->doc->rulersVisible());

    d->showPageMargins = new KToggleAction(i18n("Show Page Margins"), this);
    actionCollection()->addAction("view_page_margins", d->showPageMargins);
    d->showPageMargins->setToolTip(i18n("Show/hide the page margins"));
    connect(d->showPageMargins, &QAction::toggled, this, &KoPAView::setShowPageMargins);
    setShowPageMargins(d->doc->showPageMargins());

    d->actionInsertPage = new QAction(koIcon("document-new"), i18n("Insert Page"), this);
    actionCollection()->addAction("page_insertpage", d->actionInsertPage);
    d->actionInsertPage->setToolTip(i18n("Insert a new page after the current one"));
    d->actionInsertPage->setWhatsThis(i18n("Insert a new page after the current one"));
    connect(d->actionInsertPage, &QAction::triggered, proxyObject, &KoPAViewProxyObject::insertPage);

    d->actionCopyPage = new QAction(i18n("Copy Page"), this);
    actionCollection()->addAction("page_copypage", d->actionCopyPage);
    d->actionCopyPage->setToolTip(i18n("Copy the current page"));
    d->actionCopyPage->setWhatsThis(i18n("Copy the current page"));
    connect(d->actionCopyPage, &QAction::triggered, this, &KoPAView::copyPage);

    d->actionDeletePage = new QAction(i18n("Delete Page"), this);
    d->actionDeletePage->setEnabled(d->doc->pageCount() > 1);
    actionCollection()->addAction("page_deletepage", d->actionDeletePage);
    d->actionDeletePage->setToolTip(i18n("Delete the current page"));
    d->actionDeletePage->setWhatsThis(i18n("Delete the current page"));
    connect(d->actionDeletePage, &QAction::triggered, this, &KoPAView::deletePage);

    d->actionMasterPage = new QAction(i18n("Master Page..."), this);
    actionCollection()->addAction("format_masterpage", d->actionMasterPage);
    connect(d->actionMasterPage, &QAction::triggered, this, &KoPAView::formatMasterPage);

    d->actionPageLayout = new QAction(i18n("Page Layout..."), this);
    actionCollection()->addAction("format_pagelayout", d->actionPageLayout);
    connect(d->actionPageLayout, &QAction::triggered, this, &KoPAView::formatPageLayout);

    actionCollection()->addAction(KStandardAction::Prior, "page_previous", this, SLOT(goToPreviousPage()));
    actionCollection()->addAction(KStandardAction::Next, "page_next", this, SLOT(goToNextPage()));
    actionCollection()->addAction(KStandardAction::FirstPage, "page_first", this, SLOT(goToFirstPage()));
    actionCollection()->addAction(KStandardAction::LastPage, "page_last", this, SLOT(goToLastPage()));
    d->pageNavigator->initActions();

    KActionMenu *actionMenu = new KActionMenu(i18n("Variable"), this);
    foreach (QAction *action, d->doc->inlineTextObjectManager()->createInsertVariableActions(d->canvas))
        actionMenu->addAction(action);
    actionCollection()->addAction("insert_variable", actionMenu);

    QAction *am = new QAction(i18n("Import Document..."), this);
    actionCollection()->addAction("import_document", am);
    connect(am, &QAction::triggered, this, &KoPAView::importDocument);

    d->actionConfigure = new QAction(koIcon("configure"), i18n("Configure..."), this);
    actionCollection()->addAction("configure", d->actionConfigure);
    connect(d->actionConfigure, &QAction::triggered, this, &KoPAView::configure);
    // not sure why this isn't done through KStandardAction, but since it isn't
    // we ought to set the MenuRole manually so the item ends up in the appropriate
    // menu on OS X:
    d->actionConfigure->setMenuRole(QAction::PreferencesRole);

    d->find = new KoFind(this, d->canvas->resourceManager(), actionCollection());
    connect(d->find, &KoFind::findDocumentSetNext, this, &KoPAView::findDocumentSetNext);
    connect(d->find, &KoFind::findDocumentSetPrevious, this, &KoPAView::findDocumentSetPrevious);

    if (actionCollection()->action("object_group")) {
        actionCollection()->action("object_group")->setShortcut(QKeySequence("Ctrl+G"));
    }
    if (actionCollection()->action("object_ungroup")) {
        actionCollection()->action("object_ungroup")->setShortcut(QKeySequence("Ctrl+Shift+G"));
    }

    connect(d->doc, &KoPADocument::actionsPossible, this, &KoPAView::setActionEnabled);
}

KoCanvasController *KoPAView::canvasController() const
{
    return d->canvasController;
}

KoPACanvasBase *KoPAView::kopaCanvas() const
{
    return d->canvas;
}

KoPADocument *KoPAView::kopaDocument() const
{
    return d->doc;
}

KoPAPageBase *KoPAView::activePage() const
{
    return d->activePage;
}

void KoPAView::updateReadWrite(bool readwrite)
{
    Q_UNUSED(readwrite);
}

KoRuler *KoPAView::horizontalRuler()
{
    return d->horizontalRuler;
}

KoRuler *KoPAView::verticalRuler()
{
    return d->verticalRuler;
}

void KoPAView::setShowPageMargins(bool state)
{
    d->showPageMargins->setChecked(state);
    d->canvas->setShowPageMargins(state);
    d->doc->setShowPageMargins(state);
}

KoZoomController *KoPAView::zoomController() const
{
    return d->zoomController;
}

KoCopyController *KoPAView::copyController() const
{
    return d->copyController;
}

KoCutController *KoPAView::cutController() const
{
    return d->cutController;
}

QAction *KoPAView::deleteSelectionAction() const
{
    return d->deleteSelectionAction;
}

void KoPAView::importDocument()
{
    QFileDialog *dialog = new QFileDialog(/* QT5TODO: QUrl("kfiledialog:///OpenDialog"),*/ this);
    dialog->setObjectName("file dialog");
    dialog->setFileMode(QFileDialog::AnyFile);
    if (d->doc->pageType() == KoPageApp::Slide) {
        dialog->setWindowTitle(i18n("Import Slideshow"));
    } else {
        dialog->setWindowTitle(i18n("Import Document"));
    }

    // TODO make it possible to select also other supported types (than the default format) here.
    // this needs to go via the filters to get the file in the correct format.
    // For now we only support the native mime types
    QStringList mimeFilter;

    mimeFilter << KoOdf::mimeType(d->doc->documentType()) << KoOdf::templateMimeType(d->doc->documentType());

    dialog->setMimeTypeFilters(mimeFilter);
    if (dialog->exec() == QDialog::Accepted) {
        QUrl url(dialog->selectedUrls().first());
        QString tmpFile;
        if (KIO::NetAccess::download(url, tmpFile, nullptr)) {
            QFile file(tmpFile);
            file.open(QIODevice::ReadOnly);
            QByteArray ba;
            ba = file.readAll();

            // set the correct mime type as otherwise it does not find the correct tag when loading
            QMimeData data;
            data.setData(KoOdf::mimeType(d->doc->documentType()), ba);
            KoPAPastePage paste(d->doc, d->activePage);
            if (!paste.paste(d->doc->documentType(), &data)) {
                KMessageBox::error(nullptr, i18n("Could not import\n%1", url.url(QUrl::PreferLocalFile)));
            }
        } else {
            KMessageBox::error(nullptr, i18n("Could not import\n%1", url.url(QUrl::PreferLocalFile)));
        }
    }
    delete dialog;
}

void KoPAView::viewSnapToGrid(bool snap)
{
    d->doc->gridData().setSnapToGrid(snap);
    d->actionViewSnapToGrid->setChecked(snap);
}

void KoPAView::viewGuides(bool show)
{
    d->doc->guidesData().setShowGuideLines(show);
    d->canvas->update();
}

void KoPAView::editPaste()
{
    if (!d->canvas->toolProxy()->paste()) {
        pagePaste();
    }
}

void KoPAView::pagePaste()
{
    const QMimeData *data = QApplication::clipboard()->mimeData();

    KoOdf::DocumentType documentTypes[] = {KoOdf::Graphics, KoOdf::Presentation};

    for (unsigned int i = 0; i < sizeof(documentTypes) / sizeof(KoOdf::DocumentType); ++i) {
        if (data->hasFormat(KoOdf::mimeType(documentTypes[i]))) {
            KoPAPastePage paste(d->doc, d->activePage);
            paste.paste(documentTypes[i], data);
            break;
        }
    }
}

void KoPAView::editDeleteSelection()
{
    d->canvas->toolProxy()->deleteSelection();
}

void KoPAView::editSelectAll()
{
    KoSelection *selection = kopaCanvas()->shapeManager()->selection();
    if (!selection)
        return;
    if (!this->isVisible()) {
        Q_EMIT selectAllRequested();
        return;
    }

    QList<KoShape *> shapes = activePage()->shapes();

    foreach (KoShape *shape, shapes) {
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(shape);

        if (layer) {
            QList<KoShape *> layerShapes(layer->shapes());
            foreach (KoShape *layerShape, layerShapes) {
                selection->select(layerShape);
                layerShape->update();
            }
        }
    }

    selectionChanged();
}

void KoPAView::editDeselectAll()
{
    if (!this->isVisible()) {
        Q_EMIT deselectAllRequested();
        return;
    }

    KoSelection *selection = kopaCanvas()->shapeManager()->selection();
    if (selection)
        selection->deselectAll();

    selectionChanged();
    d->canvas->update();
}

void KoPAView::formatMasterPage()
{
    KoPAPage *page = dynamic_cast<KoPAPage *>(d->activePage);
    Q_ASSERT(page);
    KoPAMasterPageDialog *dialog = new KoPAMasterPageDialog(d->doc, page->masterPage(), d->canvas);

    if (dialog->exec() == QDialog::Accepted) {
        KoPAMasterPage *masterPage = dialog->selectedMasterPage();
        KoPAPage *page = dynamic_cast<KoPAPage *>(d->activePage);
        if (page) {
            KoPAChangeMasterPageCommand *command = new KoPAChangeMasterPageCommand(d->doc, page, masterPage);
            d->canvas->addCommand(command);
        }
    }

    delete dialog;
}

void KoPAView::formatPageLayout()
{
    const KoPageLayout &pageLayout = viewMode()->activePageLayout();

    KoPAPageLayoutDialog dialog(d->doc, pageLayout, d->canvas);

    if (dialog.exec() == QDialog::Accepted) {
        KUndo2Command *command = new KUndo2Command(kundo2_i18n("Change page layout"));
        viewMode()->changePageLayout(dialog.pageLayout(), dialog.applyToDocument(), command);

        d->canvas->addCommand(command);
    }
}

void KoPAView::slotZoomChanged(KoZoomMode::Mode mode, qreal zoom)
{
    Q_UNUSED(zoom);
    if (d->activePage) {
        if (mode == KoZoomMode::ZOOM_PAGE) {
            const KoPageLayout &layout = viewMode()->activePageLayout();
            QRectF pageRect(0, 0, layout.width, layout.height);
            d->canvasController->ensureVisible(d->canvas->viewConverter()->documentToView(pageRect));
        } else if (mode == KoZoomMode::ZOOM_WIDTH) {
            // horizontally center the page
            const KoPageLayout &layout = viewMode()->activePageLayout();
            QRectF pageRect(0, 0, layout.width, layout.height);
            QRect viewRect = d->canvas->viewConverter()->documentToView(pageRect).toRect();
            viewRect.translate(d->canvas->documentOrigin());
            QRect currentVisible(qMax(0, -d->canvasController->canvasOffsetX()),
                                 qMax(0, -d->canvasController->canvasOffsetY()),
                                 d->canvasController->visibleWidth(),
                                 d->canvasController->visibleHeight());
            int horizontalMove = viewRect.center().x() - currentVisible.center().x();
            d->canvasController->pan(QPoint(horizontalMove, 0));
        }
        updateCanvasSize(true);
    }
}

void KoPAView::configure()
{
    openConfiguration();
    // TODO update canvas
}

void KoPAView::openConfiguration()
{
    QPointer<KoPAConfigureDialog> dialog(new KoPAConfigureDialog(this));
    dialog->exec();
    delete dialog;
}

void KoPAView::setMasterMode(bool master)
{
    viewMode()->setMasterMode(master);
    if (mainWindow()) {
        d->documentStructureDocker->setMasterMode(master);
    }
    d->actionMasterPage->setEnabled(!master);

    QList<KoPAPageBase *> pages = d->doc->pages(master);
    d->actionDeletePage->setEnabled(pages.size() > 1);
}

KoShapeManager *KoPAView::shapeManager() const
{
    return d->canvas->shapeManager();
}

KoShapeManager *KoPAView::masterShapeManager() const
{
    return d->canvas->masterShapeManager();
}

void KoPAView::reinitDocumentDocker()
{
    if (mainWindow()) {
        d->documentStructureDocker->setActivePage(d->activePage);
    }
}

void KoPAView::pageUpdated(KoPAPageBase *page)
{
    // if the page was updated its content e.g. master page has been changed. Therefore we need to
    // set the page again to set the shapes of the new master page and get a repaint. Without this
    // changing the master page does not update the page.
    if (d->activePage == page) {
        doUpdateActivePage(page);
    }
}

void KoPAView::updateCanvasSize(bool forceUpdate)
{
    const KoPageLayout &layout = viewMode()->activePageLayout();

    QSizeF pageSize(layout.width, layout.height);
    QSizeF viewportSize = d->canvasController->viewportSize();

    // calculate size of union page + viewport
    QSizeF documentMinSize(qMax(zoomHandler()->unzoomItX(viewportSize.width()), layout.width),
                           qMax(zoomHandler()->unzoomItY(viewportSize.height()), layout.height));

    // create a rect out of it with origin in tp left of page
    QRectF documentRect(QPointF((documentMinSize.width() - layout.width) * -0.5, (documentMinSize.height() - layout.height) * -0.5), documentMinSize);

    // Now make a union with the bounding rect of all shapes
    // Fetch boundingRect like this as a viewmode might have set other shapes than the page
    foreach (KoShape *layer, d->canvas->shapeManager()->shapes()) {
        if (!dynamic_cast<KoShapeLayer *>(layer)) {
            documentRect = documentRect.united(layer->boundingRect());
        }
    }

    QPointF offset = -documentRect.topLeft();
    QPoint scrollChange = d->canvas->documentOrigin() - zoomHandler()->documentToView(offset).toPoint();

    if (forceUpdate || scrollChange != QPoint(0, 0) || d->zoomController->documentSize() != documentRect.size() || d->zoomController->pageSize() != pageSize) {
        d->horizontalRuler->setRulerLength(layout.width);
        d->verticalRuler->setRulerLength(layout.height);
        d->horizontalRuler->setActiveRange(layout.leftMargin, layout.width - layout.rightMargin);
        d->verticalRuler->setActiveRange(layout.topMargin, layout.height - layout.bottomMargin);
        QSizeF documentSize(documentRect.size());
        d->canvas->setDocumentOrigin(offset);
        d->zoomController->setDocumentSize(documentSize);

        d->canvas->resourceManager()->setResource(KoCanvasResourceManager::PageSize, pageSize);

        d->canvas->update();
        QSize documentPxSize(zoomHandler()->zoomItX(documentRect.width()), zoomHandler()->zoomItY(documentRect.height()));
        d->canvasController->proxyObject->updateDocumentSize(documentPxSize);
        // this can trigger a change of the zoom level in "fit to mode" and therefore this needs to be at the end as it calls this function again
        d->zoomController->setPageSize(pageSize);
    }
}

void KoPAView::doUpdateActivePage(KoPAPageBase *page)
{
    bool pageChanged = page != d->activePage;
    setActivePage(page);

    updateCanvasSize(true);

    updatePageNavigationActions();

    if (pageChanged) {
        proxyObject->emitActivePageChanged();
    }

    pageOffsetChanged();
}

void KoPAView::setActivePage(KoPAPageBase *page)
{
    if (!page)
        return;

    bool pageChanged = page != d->activePage;

    shapeManager()->removeAdditional(d->activePage);
    d->activePage = page;
    shapeManager()->addAdditional(d->activePage);
    QList<KoShape *> shapes = page->shapes();
    shapeManager()->setShapes(shapes, KoShapeManager::AddWithoutRepaint);
    // Make the top most layer active
    if (!shapes.isEmpty()) {
        KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(shapes.last());
        shapeManager()->selection()->setActiveLayer(layer);
    }

    // if the page is not a master page itself set shapes of the master page
    KoPAPage *paPage = dynamic_cast<KoPAPage *>(page);
    if (paPage) {
        KoPAMasterPage *masterPage = paPage->masterPage();
        QList<KoShape *> masterShapes = masterPage->shapes();
        masterShapeManager()->setShapes(masterShapes, KoShapeManager::AddWithoutRepaint);
        // Make the top most layer active
        if (!masterShapes.isEmpty()) {
            KoShapeLayer *layer = dynamic_cast<KoShapeLayer *>(masterShapes.last());
            masterShapeManager()->selection()->setActiveLayer(layer);
        }
    } else {
        // if the page is a master page no shapes are in the masterShapeManager
        masterShapeManager()->setShapes(QList<KoShape *>());
    }

    if (mainWindow() && pageChanged) {
        d->documentStructureDocker->setActivePage(d->activePage);
        proxyObject->emitActivePageChanged();
    }

    // Set the current page number in the canvas resource provider
    d->canvas->resourceManager()->setResource(KoCanvasResourceManager::CurrentPage, d->doc->pageIndex(d->activePage) + 1);
}

void KoPAView::navigatePage(KoPageApp::PageNavigation pageNavigation)
{
    KoPAPageBase *newPage = d->doc->pageByNavigation(d->activePage, pageNavigation);

    if (newPage != d->activePage) {
        proxyObject->updateActivePage(newPage);
    }
}

KoPrintJob *KoPAView::createPrintJob()
{
    return new KoPAPrintJob(this);
}

void KoPAView::pageOffsetChanged()
{
    QPoint documentOrigin(d->canvas->documentOrigin());
    d->horizontalRuler->setOffset(d->canvasController->canvasOffsetX() + documentOrigin.x());
    d->verticalRuler->setOffset(d->canvasController->canvasOffsetY() + documentOrigin.y());
}

void KoPAView::updateMousePosition(const QPoint &position)
{
    const QPoint canvasOffset(d->canvasController->canvasOffsetX(), d->canvasController->canvasOffsetY());
    const QPoint viewPos = position - d->canvas->documentOrigin() - canvasOffset;

    d->horizontalRuler->updateMouseCoordinate(viewPos.x());
    d->verticalRuler->updateMouseCoordinate(viewPos.y());

    // Update the selection borders in the rulers while moving with the mouse
    if (d->canvas->shapeManager()->selection() && (d->canvas->shapeManager()->selection()->count() > 0)) {
        QRectF boundingRect = d->canvas->shapeManager()->selection()->boundingRect();
        d->horizontalRuler->updateSelectionBorders(boundingRect.x(), boundingRect.right());
        d->verticalRuler->updateSelectionBorders(boundingRect.y(), boundingRect.bottom());
    }
}

void KoPAView::selectionChanged()
{
    // Show the borders of the selection in the rulers
    if (d->canvas->shapeManager()->selection() && (d->canvas->shapeManager()->selection()->count() > 0)) {
        QRectF boundingRect = d->canvas->shapeManager()->selection()->boundingRect();
        d->horizontalRuler->setShowSelectionBorders(true);
        d->verticalRuler->setShowSelectionBorders(true);
        d->horizontalRuler->updateSelectionBorders(boundingRect.x(), boundingRect.right());
        d->verticalRuler->updateSelectionBorders(boundingRect.y(), boundingRect.bottom());
    } else {
        d->horizontalRuler->setShowSelectionBorders(false);
        d->verticalRuler->setShowSelectionBorders(false);
    }
}

void KoPAView::setShowRulers(bool show)
{
    d->horizontalRuler->setVisible(show);
    d->verticalRuler->setVisible(show);

    d->viewRulers->setChecked(show);
    d->doc->setRulersVisible(show);
}

void KoPAView::insertPage()
{
    KoPAPageBase *page = nullptr;
    if (viewMode()->masterMode()) {
        KoPAMasterPage *masterPage = d->doc->newMasterPage();
        masterPage->setBackground(QSharedPointer<KoColorBackground>(new KoColorBackground(Qt::white)));
        // use the layout of the current active page for the new page
        KoPageLayout &layout = masterPage->pageLayout();
        KoPAMasterPage *activeMasterPage = dynamic_cast<KoPAMasterPage *>(d->activePage);
        if (activeMasterPage) {
            layout = activeMasterPage->pageLayout();
        }
        page = masterPage;
    } else {
        KoPAPage *activePage = static_cast<KoPAPage *>(d->activePage);
        KoPAMasterPage *masterPage = activePage->masterPage();
        page = d->doc->newPage(masterPage);
    }

    KoPAPageInsertCommand *command = new KoPAPageInsertCommand(d->doc, page, d->activePage);
    d->canvas->addCommand(command);

    doUpdateActivePage(page);
}

void KoPAView::copyPage()
{
    QList<KoPAPageBase *> pages;
    pages.append(d->activePage);
    KoPAOdfPageSaveHelper saveHelper(d->doc, pages);
    KoDrag drag;
    drag.setOdf(KoOdf::mimeType(d->doc->documentType()), saveHelper);
    drag.addToClipboard();
}

void KoPAView::deletePage()
{
    if (!isMasterUsed(d->activePage)) {
        d->doc->removePage(d->activePage);
    }
}

void KoPAView::setActionEnabled(int actions, bool enable)
{
    if (actions & ActionInsertPage) {
        d->actionInsertPage->setEnabled(enable);
    }
    if (actions & ActionCopyPage) {
        d->actionCopyPage->setEnabled(enable);
    }
    if (actions & ActionDeletePage) {
        d->actionDeletePage->setEnabled(enable);
    }
    if (actions & ActionViewShowMasterPages) {
        d->actionViewShowMasterPages->setEnabled(enable);
    }
    if (actions & ActionFormatMasterPage) {
        d->actionMasterPage->setEnabled(enable);
    }
}

void KoPAView::setViewMode(KoPAViewMode *mode)
{
    KoPAViewMode *previousViewMode = viewMode();
    KoPAViewBase::setViewMode(mode);

    if (previousViewMode && mode != previousViewMode) {
        disconnect(d->doc, &KoPADocument::shapeAdded, previousViewMode, &KoPAViewMode::addShape);
        disconnect(d->doc, &KoPADocument::shapeRemoved, previousViewMode, &KoPAViewMode::removeShape);
    }
    connect(d->doc, &KoPADocument::shapeAdded, mode, &KoPAViewMode::addShape);
    connect(d->doc, &KoPADocument::shapeRemoved, mode, &KoPAViewMode::removeShape);
}

QPixmap KoPAView::pageThumbnail(KoPAPageBase *page, const QSize &size)
{
    return d->doc->pageThumbnail(page, size);
}

bool KoPAView::exportPageThumbnail(KoPAPageBase *page, const QUrl &url, const QSize &size, const QByteArray &format, int quality)
{
    bool res = false;
    QPixmap pix = d->doc->pageThumbnail(page, size);
    if (!pix.isNull()) {
        // Depending on the desired target size due to rounding
        // errors during zoom the resulting pixmap *might* be
        // 1 pixel or 2 pixels wider/higher than desired: we just
        // remove the additional columns/rows.  This can be done
        // since Stage is leaving a minimal border below/at
        // the right of the image anyway.
        if (size != pix.size()) {
            pix = pix.copy(0, 0, size.width(), size.height());
        }
        // save the pixmap to the desired file
        QUrl fileUrl(url);
        if (fileUrl.scheme().isEmpty()) {
            fileUrl.setScheme("file");
        }
        const bool bLocalFile = fileUrl.isLocalFile();
        QTemporaryFile *tmpFile = bLocalFile ? nullptr : new QTemporaryFile();
        if (bLocalFile || tmpFile->open()) {
            QFile file(bLocalFile ? fileUrl.path() : tmpFile->fileName());
            if (file.open(QIODevice::ReadWrite)) {
                res = pix.save(&file, format, quality);
                file.close();
            }
            if (!bLocalFile) {
                if (res) {
                    res = KIO::NetAccess::upload(tmpFile->fileName(), fileUrl, this);
                }
            }
        }
        if (!bLocalFile) {
            delete tmpFile;
        }
    }
    return res;
}

KoPADocumentStructureDocker *KoPAView::documentStructureDocker() const
{
    return d->documentStructureDocker;
}

void KoPAView::clipboardDataChanged()
{
    const QMimeData *data = QApplication::clipboard()->mimeData();
    bool paste = false;

    if (data) {
        // TODO see if we can use the KoPasteController instead of having to add this feature in each calligra app.
        QStringList mimeTypes = d->canvas->toolProxy()->supportedPasteMimeTypes();
        mimeTypes << KoOdf::mimeType(KoOdf::Graphics);
        mimeTypes << KoOdf::mimeType(KoOdf::Presentation);

        foreach (const QString &mimeType, mimeTypes) {
            if (data->hasFormat(mimeType)) {
                paste = true;
                break;
            }
        }
    }

    d->editPaste->setEnabled(paste);
}

void KoPAView::goToPreviousPage()
{
    navigatePage(KoPageApp::PagePrevious);
}

void KoPAView::goToNextPage()
{
    navigatePage(KoPageApp::PageNext);
}

void KoPAView::goToFirstPage()
{
    navigatePage(KoPageApp::PageFirst);
}

void KoPAView::goToLastPage()
{
    navigatePage(KoPageApp::PageLast);
}

void KoPAView::findDocumentSetNext(QTextDocument *document)
{
    KoPAPageBase *page = nullptr;
    KoShape *startShape = nullptr;
    KoTextDocumentLayout *lay = document ? qobject_cast<KoTextDocumentLayout *>(document->documentLayout()) : 0;
    if (lay != nullptr) {
        startShape = lay->shapes().value(0);
        Q_ASSERT(startShape->shapeId() == "TextShapeID");
        page = d->doc->pageByShape(startShape);
        if (d->doc->pageIndex(page) == -1) {
            page = nullptr;
        }
    }

    if (page == nullptr) {
        page = d->activePage;
        startShape = page;
    }

    KoShape *shape = startShape;

    do {
        // find next text shape
        shape = KoShapeTraversal::nextShape(shape, "TextShapeID");
        // get next text shape
        if (shape != nullptr) {
            if (page != d->activePage) {
                setActivePage(page);
                d->canvas->update();
            }
            KoSelection *selection = kopaCanvas()->shapeManager()->selection();
            selection->deselectAll();
            selection->select(shape);
            // TODO can this be done nicer? is there a way to get the shape id and the tool id from the shape?
            KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");
            break;
        } else {
            // if none is found go to next page and try again
            if (d->doc->pageIndex(page) < d->doc->pages().size() - 1) {
                // TODO use also master slides
                page = d->doc->pageByNavigation(page, KoPageApp::PageNext);
            } else {
                page = d->doc->pageByNavigation(page, KoPageApp::PageFirst);
            }
            shape = page;
        }
        // do until you find the same start shape or you are on the same page again only if there was none
    } while (page != startShape);
}

void KoPAView::findDocumentSetPrevious(QTextDocument *document)
{
    KoPAPageBase *page = nullptr;
    KoShape *startShape = nullptr;
    KoTextDocumentLayout *lay = document ? qobject_cast<KoTextDocumentLayout *>(document->documentLayout()) : 0;
    if (lay != nullptr) {
        startShape = lay->shapes().value(0);
        Q_ASSERT(startShape->shapeId() == "TextShapeID");
        page = d->doc->pageByShape(startShape);
        if (d->doc->pageIndex(page) == -1) {
            page = nullptr;
        }
    }

    bool check = false;
    if (page == nullptr) {
        page = d->activePage;
        startShape = KoShapeTraversal::last(page);
        check = true;
    }

    KoShape *shape = startShape;

    do {
        if (!check || shape->shapeId() != "TextShapeID") {
            shape = KoShapeTraversal::previousShape(shape, "TextShapeID");
        }
        // get next text shape
        if (shape != nullptr) {
            if (page != d->activePage) {
                setActivePage(page);
                d->canvas->update();
            }
            KoSelection *selection = kopaCanvas()->shapeManager()->selection();
            selection->deselectAll();
            selection->select(shape);
            // TODO can this be done nicer? is there a way to get the shape id and the tool id from the shape?
            KoToolManager::instance()->switchToolRequested("TextToolFactory_ID");
            break;
        } else {
            // if none is found go to next page and try again
            if (d->doc->pageIndex(page) > 0) {
                // TODO use also master slides
                page = d->doc->pageByNavigation(page, KoPageApp::PagePrevious);
            } else {
                page = d->doc->pageByNavigation(page, KoPageApp::PageLast);
            }
            shape = KoShapeTraversal::last(page);
            check = true;
        }
        // do until you find the same start shape or you are on the same page again only if there was none
    } while (shape != startShape);
}

void KoPAView::updatePageNavigationActions()
{
    int index = d->doc->pageIndex(activePage());
    int pageCount = d->doc->pages(viewMode()->masterMode()).count();

    actionCollection()->action("page_previous")->setEnabled(index > 0);
    actionCollection()->action("page_first")->setEnabled(index > 0);
    actionCollection()->action("page_next")->setEnabled(index < pageCount - 1);
    actionCollection()->action("page_last")->setEnabled(index < pageCount - 1);
}

bool KoPAView::isMasterUsed(KoPAPageBase *page)
{
    KoPAMasterPage *master = dynamic_cast<KoPAMasterPage *>(page);

    bool used = false;

    if (master) {
        QList<KoPAPageBase *> pages = d->doc->pages();
        foreach (KoPAPageBase *page, pages) {
            KoPAPage *p = dynamic_cast<KoPAPage *>(page);
            Q_ASSERT(p);
            if (p && p->masterPage() == master) {
                used = true;
                break;
            }
        }
    }

    return used;
}

void KoPAView::centerPage()
{
    KoPageLayout &layout = d->activePage->pageLayout();
    QSizeF pageSize(layout.width, layout.height);

    QPoint documentCenter = zoomHandler()->documentToView(QPoint(pageSize.width(), pageSize.height())).toPoint();

    d->canvasController->setPreferredCenter(documentCenter);
    d->canvasController->recenterPreferred();
}

QTabBar *KoPAView::tabBar() const
{
    return d->tabBar;
}

void KoPAView::replaceCentralWidget(QWidget *newWidget)
{
    // hide standard central widget
    d->insideWidget->hide();
    // If there is already a custom central widget, it's hidden and removed from the layout
    hideCustomCentralWidget();
    // layout and show new custom widget
    d->tabBarLayout->addWidget(newWidget, 2, 1);
    newWidget->show();
}

void KoPAView::restoreCentralWidget()
{
    // hide custom central widget
    hideCustomCentralWidget();
    // show standard central widget
    d->insideWidget->show();
}

void KoPAView::hideCustomCentralWidget()
{
    if (d->tabBarLayout->itemAtPosition(2, 1)) {
        if (d->tabBarLayout->itemAtPosition(2, 1)->widget()) {
            d->tabBarLayout->itemAtPosition(2, 1)->widget()->hide();
        }
        d->tabBarLayout->removeItem(d->tabBarLayout->itemAtPosition(2, 1));
    }
}

void KoPAView::setTabBarPosition(Qt::Orientation orientation)
{
    switch (orientation) {
    case Qt::Horizontal:
        d->tabBarLayout->removeWidget(d->tabBar);
        d->tabBar->setShape(QTabBar::RoundedNorth);
        d->tabBarLayout->addWidget(d->tabBar, 0, 1);
        break;
    case Qt::Vertical:
        d->tabBarLayout->removeWidget(d->tabBar);
        d->tabBar->setShape(QTabBar::RoundedWest);
        d->tabBarLayout->addWidget(d->tabBar, 1, 0, 2, 1, Qt::AlignTop);
        break;
    default:
        break;
    }
}

void KoPAView::updateUnit(const KoUnit &unit)
{
    d->horizontalRuler->setUnit(unit);
    d->verticalRuler->setUnit(unit);
    d->canvas->resourceManager()->setResource(KoCanvasResourceManager::Unit, unit);
}
