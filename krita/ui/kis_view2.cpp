/*
 *  This file is part of KimageShop^WKrayon^WKrita
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <koch@kde.org>
 *                1999 Carsten Pfeiffer <pfeiffer@kde.org>
 *                2002 Patrick Julien <freak@codepimps.org>
 *                2003-2011 Boudewijn Rempt <boud@valdyas.org>
 *                2004 Clarence Dang <dang@kde.org>
 *                2011 José Luis Vergara <pentalis@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdio.h>

#include "kis_view2.h"
#include <QPrinter>

#include <QDesktopServices>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QRect>
#include <QWidget>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QApplication>
#include <QPrintDialog>
#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QScrollBar>
#include <QMainWindow>

#include <kio/netaccess.h>
#include <kmenubar.h>
#include <ktoolbar.h>
#include <kstatusbar.h>
#include <ktoggleaction.h>
#include <kaction.h>
#include <kactionmenu.h>
#include <klocale.h>
#include <kmenu.h>
#include <kservice.h>
#include <kservicetypetrader.h>
#include <kstandardaction.h>
#include <kurl.h>
#include <KoXMLGUIWindow.h>
#include <KoXMLGUIFactory.h>
#include <kmessagebox.h>
#include <kactioncollection.h>

#include <KoToolRegistry.h>
#include <KoStore.h>
#include <KoMainWindow.h>
#include <KoSelection.h>
#include <KoToolBoxFactory.h>
#include <KoZoomHandler.h>
#include <KoViewConverter.h>
#include <KoView.h>
#include <KoDockerManager.h>
#include <KoDockRegistry.h>
#include <KoResourceServerProvider.h>
#include <KoCompositeOp.h>
#include <KoTemplateCreateDia.h>
#include <KoCanvasController.h>
#include <KoDocumentEntry.h>
#include <KoProperties.h>
#include <KoPart.h>

#include <kis_image.h>
#include <kis_undo_adapter.h>
#include "kis_composite_progress_proxy.h"
#include <kis_layer.h>

#include "canvas/kis_canvas2.h"
#include "canvas/kis_canvas_controller.h"
#include "canvas/kis_grid_manager.h"
#include "canvas/kis_perspective_grid_manager.h"
#include "dialogs/kis_dlg_blacklist_cleanup.h"
#include "kis_canvas_resource_provider.h"
#include "kis_config.h"
#include "kis_config_notifier.h"
#include "kis_control_frame.h"
#include "kis_coordinates_converter.h"
#include "kis_doc2.h"
#include "kis_factory2.h"
#include "kis_filter_manager.h"
#include "kis_group_layer.h"
#include "kis_image_manager.h"
#include "kis_mask_manager.h"
#include "kis_mimedata.h"
#include "kis_node.h"
#include "kis_node_manager.h"
#include "kis_painting_assistants_decoration.h"
#include <kis_paint_layer.h>
#include "kis_paintop_box.h"
#include "kis_print_job.h"
#include "kis_progress_widget.h"
#include "kis_resource_server_provider.h"
#include "kis_selection.h"
#include "kis_selection_manager.h"
#include "kis_shape_layer.h"
#include "kis_shape_controller.h"
#include "kis_statusbar.h"
#include "kis_zoom_manager.h"
#include "kra/kis_kra_loader.h"
#include "widgets/kis_floating_message.h"
#include "kis_image_view.h"
#include "kis_zoom_manager.h"

#include <QPoint>
#include <kapplication.h>
#include "kis_node_commands_adapter.h"
#include <kis_paintop_preset.h>
#include "kis_action_manager.h"
#include "input/kis_input_profile_manager.h"
#include "kis_canvas_controls_manager.h"
#include "kis_mainwindow_observer.h"

class StatusBarItem
{
public:
    StatusBarItem() // for QValueList
        : m_widget(0),
            m_connected(false),
            m_hidden(false) {}

    StatusBarItem(QWidget * widget, int stretch, bool permanent)
        : m_widget(widget),
            m_stretch(stretch),
            m_permanent(permanent),
            m_connected(false),
            m_hidden(false) {}

    bool operator==(const StatusBarItem& rhs) {
        return m_widget == rhs.m_widget;
    }

    bool operator!=(const StatusBarItem& rhs) {
        return m_widget != rhs.m_widget;
    }

    QWidget * widget() const {
        return m_widget;
    }

    void ensureItemShown(KStatusBar * sb) {
        Q_ASSERT(m_widget);
        if (!m_connected) {
            if (m_permanent)
                sb->addPermanentWidget(m_widget, m_stretch);
            else
                sb->addWidget(m_widget, m_stretch);

            if(!m_hidden)
                m_widget->show();

            m_connected = true;
        }
    }
    void ensureItemHidden(KStatusBar * sb) {
        if (m_connected) {
            m_hidden = m_widget->isHidden();
            sb->removeWidget(m_widget);
            m_widget->hide();
            m_connected = false;
        }
    }
private:
    QWidget * m_widget;
    int m_stretch;
    bool m_permanent;
    bool m_connected;
    bool m_hidden;
};

class BlockingUserInputEventFilter : public QObject
{
    bool eventFilter(QObject *watched, QEvent *event)
    {
        Q_UNUSED(watched);
        if(dynamic_cast<QWheelEvent*>(event)
                || dynamic_cast<QKeyEvent*>(event)
                || dynamic_cast<QMouseEvent*>(event)) {
            return true;
        }
        else {
            return false;
        }
    }
};

class KisView2::KisView2Private
{

public:

    KisView2Private()
        : filterManager(0)
        , statusBar(0)
        , selectionManager(0)
        , controlFrame(0)
        , nodeManager(0)
        , imageManager(0)
        , gridManager(0)
        , perspectiveGridManager(0)
        , paintingAssistantsDecoration(0)
        , actionManager(0)
        , mainWindow(0)
        , currentImageView(0)
        , canvasResourceProvider(0)
        , canvasResourceManager(0)
    {
    }

    ~KisView2Private() {

        delete filterManager;
        delete selectionManager;
        delete nodeManager;
        delete imageManager;
        delete gridManager;
        delete perspectiveGridManager;
        delete paintingAssistantsDecoration;
        delete statusBar;
        delete actionManager;
        delete canvasControlsManager;
        delete canvasResourceProvider;
        delete canvasResourceManager;
    }

public:

    KisFilterManager *filterManager;
    KisStatusBar *statusBar;
    KAction *totalRefresh;
    KAction *mirrorCanvas;
    KAction *createTemplate;
    KAction *saveIncremental;
    KAction *saveIncrementalBackup;
    KAction *openResourcesDirectory;
    KAction *rotateCanvasRight;
    KAction *rotateCanvasLeft;
    KAction *resetCanvasTransformations;
    KToggleAction *wrapAroundAction;
    KisSelectionManager *selectionManager;
    KisControlFrame *controlFrame;
    KisNodeManager *nodeManager;
    KisImageManager *imageManager;
    KisGridManager *gridManager;
    KisCanvasControlsManager *canvasControlsManager;
    KisPerspectiveGridManager * perspectiveGridManager;
    KisPaintingAssistantsDecoration *paintingAssistantsDecoration;
    BlockingUserInputEventFilter blockingEventFilter;
    KisFlipbook *flipbook;
    KisActionManager* actionManager;
    QMainWindow* mainWindow;
    KisImageView* currentImageView;
    KisCanvasResourceProvider* canvasResourceProvider;
    KoCanvasResourceManager* canvasResourceManager;
    QList<StatusBarItem> statusBarItems;
};


KisView2::KisView2(QWidget *parent)
    : KoXMLGUIClient(),
      d(new KisView2Private())
{
    d->mainWindow = dynamic_cast<QMainWindow*>(parent);

    setXMLFile(QString("%1.rc").arg(qAppName()));

    d->canvasResourceProvider = new KisCanvasResourceProvider(0);
    d->canvasResourceManager = new KoCanvasResourceManager();
    d->canvasResourceProvider->setResourceManager(d->canvasResourceManager);

    createActions();
    createManagers();

    d->controlFrame = new KisControlFrame(this);

    //Check to draw scrollbars after "Canvas only mode" toggle is created.
    this->showHideScrollbars();


    //Workaround, by default has the same shortcut as hide/show dockers
    if (mainWindow()) {

        mainWindow()->setDockNestingEnabled(true);
        actionCollection()->addAction(KStandardAction::KeyBindings, "keybindings", mainWindow()->guiFactory(), SLOT(configureShortcuts()));

        connect(mainWindow(), SIGNAL(documentSaved()), this, SLOT(slotDocumentSaved()));
        KAction *action = dynamic_cast<KAction*>(mainWindow()->actionCollection()->action("view_toggledockers"));
        if (action) {
            action->setShortcut(QKeySequence(), KAction::DefaultShortcut);
            action->setShortcut(QKeySequence(), KAction::ActiveShortcut);
        }

        KoCanvasController *dummy = new KoDummyCanvasController(actionCollection());
        KoToolManager::instance()->registerTools(actionCollection(), dummy);

        KoToolBoxFactory toolBoxFactory;
        QDockWidget* toolbox = mainWindow()->createDockWidget(&toolBoxFactory);
        toolbox->setMinimumWidth(60);

        mainWindow()->dockerManager()->setIcons(false);
    }

    d->statusBar = new KisStatusBar(this);
    QTimer::singleShot(0, this, SLOT(makeStatusBarVisible()));

    connect(d->nodeManager, SIGNAL(sigNodeActivated(KisNodeSP)),
            d->controlFrame->paintopBox(), SLOT(slotCurrentNodeChanged(KisNodeSP)));

    connect(KoToolManager::instance(), SIGNAL(inputDeviceChanged(KoInputDevice)),
            d->controlFrame->paintopBox(), SLOT(slotInputDeviceChanged(KoInputDevice)));

    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*,int)),
            d->controlFrame->paintopBox(), SLOT(slotToolChanged(KoCanvasController*,int)));

    connect(d->nodeManager, SIGNAL(sigNodeActivated(KisNodeSP)),
            resourceProvider(), SLOT(slotNodeActivated(KisNodeSP)));

    connect(resourceProvider()->resourceManager(), SIGNAL(canvasResourceChanged(int,QVariant)),
            d->controlFrame->paintopBox(), SLOT(slotCanvasResourceChanged(int,QVariant)));

    loadPlugins();

    KisInputProfileManager::instance()->loadProfiles();

#if 0
    //check for colliding shortcuts
    QSet<QKeySequence> existingShortcuts;
    foreach(QAction* action, actionCollection()->actions()) {
        if(action->shortcut() == QKeySequence(0)) {
            continue;
        }
        dbgUI << "shortcut " << action->text() << " " << action->shortcut();
        Q_ASSERT(!existingShortcuts.contains(action->shortcut()));
        existingShortcuts.insert(action->shortcut());
    }
#endif

    KoResourceServer<KisPaintOpPreset> * rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    KisPaintOpPreset *preset = rserver->resourceByName("Basic_tip_default");
    if (!preset) {
        if (rserver->resources().isEmpty()) {
            KMessageBox::error(mainWindow(), i18n("Krita cannot find any brush presets and will close now. Please check your installation.", i18n("Critical Error")));
            exit(0);
        }
        preset = rserver->resources().first();
    }
    if (preset) {
        paintOpBox()->resourceSelected(preset);
    }

    foreach(const QString & docker, KoDockRegistry::instance()->keys()) {
        KoDockFactoryBase *factory = KoDockRegistry::instance()->value(docker);
        if (mainWindow())
            mainWindow()->createDockWidget(factory);
    }
    foreach(KoCanvasObserverBase* observer, mainWindow()->canvasObservers()) {
        KisMainwindowObserver* mainwindowObserver = dynamic_cast<KisMainwindowObserver*>(observer);
        if (mainwindowObserver) {
            mainwindowObserver->setCanvasResourceProvider(d->canvasResourceProvider);
        }
    }

    d->actionManager->updateGUI();

    connect(mainWindow(), SIGNAL(themeChanged()), this, SLOT(updateIcons()));
    updateIcons();

}


KisView2::~KisView2()
{
    if (d->filterManager->isStrokeRunning()) {
        d->filterManager->cancel();
    }

    {
        KConfigGroup group(KGlobal::config(), "krita/shortcuts");
        foreach(KActionCollection *collection, KActionCollection::allCollections()) {
            collection->setConfigGroup("krita/shortcuts");
            collection->writeSettings(&group);
        }
    }
    delete d;
}

void KisView2::setCurrentView(KoView *view)
{
    KisImageView *imageView = qobject_cast<KisImageView*>(view);
    imageView->setParentView(this);

    if (imageView) {
        if (d->currentImageView) {

            KisDoc2* doc = qobject_cast<KisDoc2*>(d->currentImageView->document());
            if (doc) {
                doc->disconnect(this);
            }
            canvasController()->proxyObject->disconnect(d->statusBar);
            d->nodeManager->disconnect(doc->image());

            d->rotateCanvasRight->disconnect();
            d->rotateCanvasLeft->disconnect();
            d->resetCanvasTransformations->disconnect();
            d->mirrorCanvas->disconnect();
            d->wrapAroundAction->disconnect();
            canvasControllerWidget()->disconnect(SIGNAL(toolOptionWidgetsChanged(QList<QPointer<QWidget> >)), mainWindow()->dockerManager());
        }

        // Wait for the async image to have loaded
        KisDoc2* doc = qobject_cast<KisDoc2*>(view->document());
//        connect(canvasController()->proxyObject, SIGNAL(documentMousePositionChanged(QPointF)), d->statusBar, SLOT(documentMousePositionChanged(QPointF)));

        d->currentImageView = imageView;
        imageView->canvasBase()->setResourceManager(d->canvasResourceManager);

        connect(d->nodeManager, SIGNAL(sigNodeActivated(KisNodeSP)), doc->image(), SLOT(requestStrokeEnd()));
        connect(d->rotateCanvasRight, SIGNAL(triggered()), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(rotateCanvasRight15()));
        connect(d->rotateCanvasLeft, SIGNAL(triggered()),dynamic_cast<KisCanvasController*>(canvasController()), SLOT(rotateCanvasLeft15()));
        connect(d->resetCanvasTransformations, SIGNAL(triggered()), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(resetCanvasTransformations()));
        connect(d->mirrorCanvas, SIGNAL(toggled(bool)), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(mirrorCanvas(bool)));
        connect(d->wrapAroundAction, SIGNAL(toggled(bool)), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(slotToggleWrapAroundMode(bool)));
        connect(canvasControllerWidget(), SIGNAL(toolOptionWidgetsChanged(QList<QPointer<QWidget> >)), mainWindow()->dockerManager(), SLOT(newOptionWidgets(QList<QPointer<QWidget> >)));

        imageView->canvasBase()->addDecoration(d->gridManager);
        imageView->canvasBase()->addDecoration(d->perspectiveGridManager);
        imageView->canvasBase()->addDecoration(d->paintingAssistantsDecoration);

        canvasControllerWidget()->activate();
    }

    d->filterManager->setView(imageView);
    d->selectionManager->setView(imageView);
    d->nodeManager->setView(imageView);
    d->imageManager->setView(imageView);
    d->canvasControlsManager->setView(imageView);
    d->actionManager->setView(imageView);
    d->gridManager->setView(imageView);
    d->statusBar->setView(imageView);

    actionManager()->updateGUI();
}

KoZoomController *KisView2::zoomController() const
{
    return d->currentImageView->zoomController();
}

KisImageWSP KisView2::image() const
{
    if (d && d->currentImageView) {
        return d->currentImageView->image();
    }
    return 0;
}

KisCanvasResourceProvider * KisView2::resourceProvider()
{
    return d->canvasResourceProvider;
}

KisCanvas2 *KisView2::canvasBase() const
{
    if (d && d->currentImageView) {
        return d->currentImageView->canvasBase();
    }
    return 0;
}

QWidget *KisView2::canvas() const
{
    if (d && d->currentImageView && d->currentImageView->canvasBase()->canvasWidget()) {
        return d->currentImageView->canvasBase()->canvasWidget();
    }
    return 0;
}

KisStatusBar *KisView2::statusBar() const
{
    return d->statusBar;
}

void KisView2::addStatusBarItem(QWidget * widget, int stretch, bool permanent)
{
    StatusBarItem item(widget, stretch, permanent);
    KStatusBar * sb = mainWindow()->statusBar();
    if (sb) {
        item.ensureItemShown(sb);
    }
    d->statusBarItems.append(item);
}

void KisView2::removeStatusBarItem(QWidget * widget)
{
    KStatusBar *sb = mainWindow()->statusBar();

    int itemCount = d->statusBarItems.count();
    for (int i = itemCount-1; i >= 0; --i) {
        StatusBarItem &sbItem = d->statusBarItems[i];
        if (sbItem.widget() == widget) {
            if (sb) {
                sbItem.ensureItemHidden(sb);
            }
            d->statusBarItems.removeOne(sbItem);
            break;
        }
    }
}

KisPaintopBox* KisView2::paintOpBox() const
{
    return d->controlFrame->paintopBox();
}

KoProgressUpdater* KisView2::createProgressUpdater(KoProgressUpdater::Mode mode)
{
    return new KisProgressUpdater(d->statusBar->progress(), document()->progressProxy(), mode);
}

KisSelectionManager * KisView2::selectionManager()
{
    return d->selectionManager;
}

KoCanvasController * KisView2::canvasController()
{
    if (d && d->currentImageView) {
        return d->currentImageView->canvasController();
    }
    return 0;
}

KisCanvasController *KisView2::canvasControllerWidget()
{
    if (d && d->currentImageView) {
        return dynamic_cast<KisCanvasController*>(canvasController());
    }
    return 0;
}

KisNodeSP KisView2::activeNode()
{
    if (d->nodeManager) {
        return d->nodeManager->activeNode();
    }
    return 0;
}

KisLayerSP KisView2::activeLayer()
{
    if (d->nodeManager) {
        return d->nodeManager->activeLayer();
    }
    return 0;
}

KisPaintDeviceSP KisView2::activeDevice()
{
    if (d->nodeManager) {
        return d->nodeManager->activePaintDevice();
    }
    return 0;
}

KisZoomManager * KisView2::zoomManager()
{
    if (d->currentImageView) {
        return d->currentImageView->zoomManager();
    }
    return 0;
}

KisFilterManager * KisView2::filterManager()
{
    return d->filterManager;
}

KisImageManager * KisView2::imageManager()
{
    return d->imageManager;
}

KisSelectionSP KisView2::selection()
{
    KisLayerSP layer = activeLayer();
    if (layer)
        return layer->selection(); // falls through to the global
    // selection, or 0 in the end
    if (image()) {
        return image()->globalSelection();
    }
    return 0;
}

bool KisView2::selectionEditable()
{
    KisLayerSP layer = activeLayer();
    if (layer) {
        KoProperties properties;
        QList<KisNodeSP> masks = layer->childNodes(QStringList("KisSelectionMask"), properties);
        if (masks.size() == 1) {
            return masks[0]->isEditable();
        }
    }
    // global selection is always editable
    return true;
}

KisUndoAdapter * KisView2::undoAdapter()
{
    if (!document()) return 0;

    KisImageWSP image = document()->image();
    Q_ASSERT(image);

    return image->undoAdapter();
}


void KisView2::slotLoadingFinished()
{
    if (!document()) return;

    /**
     * Cold-start of image size/resolution signals
     */
    slotImageResolutionChanged();
    if (resourceProvider()) {
        resourceProvider()->slotImageSizeChanged();
    }
    if (d->nodeManager) {
        d->nodeManager->nodesUpdated();
    }

    connect(image(), SIGNAL(sigSizeChanged(const QPointF&, const QPointF&)), resourceProvider(), SLOT(slotImageSizeChanged()));

    connect(image(), SIGNAL(sigResolutionChanged(double,double)),
            resourceProvider(), SLOT(slotOnScreenResolutionChanged()));
    connect(zoomManager()->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)),
            resourceProvider(), SLOT(slotOnScreenResolutionChanged()));

    connect(image(), SIGNAL(sigSizeChanged(const QPointF&, const QPointF&)), this, SLOT(slotImageSizeChanged(const QPointF&, const QPointF&)));
    connect(image(), SIGNAL(sigResolutionChanged(double,double)), this, SLOT(slotImageResolutionChanged()));
    connect(image(), SIGNAL(sigNodeChanged(KisNodeSP)), this, SLOT(slotNodeChanged()));
    connect(image()->undoAdapter(), SIGNAL(selectionChanged()), selectionManager(), SLOT(selectionChanged()));

    /*
     * WARNING: Currently we access the global progress bar in two ways:
     * connecting to composite progress proxy (strokes) and creating
     * progress updaters. The latter way should be deprecated in favour
     * of displaying the status of the global strokes queue
     */
    //image()->compositeProgressProxy()->addProxy(m_d->statusBar->progress()->progressProxy());

    d->currentImageView->canvasBase()->initializeImage();

    if (d->controlFrame) {
        connect(image(), SIGNAL(sigColorSpaceChanged(const KoColorSpace*)), d->controlFrame->paintopBox(), SLOT(slotColorSpaceChanged(const KoColorSpace*)));
    }

    if (image()->locked()) {
        // If this is the first view on the image, the image will have been locked
        // so unlock it.
        image()->blockSignals(false);
        image()->unlock();
    }

    KisNodeSP activeNode = document()->preActivatedNode();
    document()->setPreActivatedNode(0); // to make sure that we don't keep a reference to a layer the user can later delete.

    if (!activeNode) {
        activeNode = image()->rootLayer()->firstChild();
    }

    while (activeNode && !activeNode->inherits("KisLayer")) {
        activeNode = activeNode->nextSibling();
    }

    if (activeNode) {
        d->nodeManager->slotNonUiActivatedNode(activeNode);
    }


    // get the assistants and push them to the manager
    QList<KisPaintingAssistant*> paintingAssistants = document()->preLoadedAssistants();
    foreach (KisPaintingAssistant* assistant, paintingAssistants) {
        d->paintingAssistantsDecoration->addAssistant(assistant);
    }

    /**
     * Dirty hack alert
     */
    if (mainWindow() && zoomManager() && zoomManager()->zoomController())
        zoomManager()->zoomController()->setAspectMode(true);

    if (d->currentImageView && d->currentImageView->viewConverter())
        d->currentImageView->viewConverter()->setZoomMode(KoZoomMode::ZOOM_PAGE);

    if (d->paintingAssistantsDecoration){
        foreach(KisPaintingAssistant* assist, document()->preLoadedAssistants()){
            d->paintingAssistantsDecoration->addAssistant(assist);
        }
        d->paintingAssistantsDecoration->setVisible(true);
    }
    updateGUI();

    emit sigLoadingFinished();
}


void KisView2::createActions()
{

    // krita/krita.rc must also be modified to add actions to the menu entries

    d->saveIncremental = new KAction(i18n("Save Incremental &Version"), this);
    d->saveIncremental->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S));
    actionCollection()->addAction("save_incremental_version", d->saveIncremental);
    connect(d->saveIncremental, SIGNAL(triggered()), this, SLOT(slotSaveIncremental()));

    d->saveIncrementalBackup = new KAction(i18n("Save Incremental Backup"), this);
    d->saveIncrementalBackup->setShortcut(Qt::Key_F4);
    actionCollection()->addAction("save_incremental_backup", d->saveIncrementalBackup);
    connect(d->saveIncrementalBackup, SIGNAL(triggered()), this, SLOT(slotSaveIncrementalBackup()));

    connect(mainWindow(), SIGNAL(documentSaved()), this, SLOT(slotDocumentSaved()));

    d->saveIncremental->setEnabled(false);
    d->saveIncrementalBackup->setEnabled(false);

    d->totalRefresh = new KAction(i18n("Total Refresh"), this);
    actionCollection()->addAction("total_refresh", d->totalRefresh);
    d->totalRefresh->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_R));
    connect(d->totalRefresh, SIGNAL(triggered()), this, SLOT(slotTotalRefresh()));

    KAction *tabletDebugger = new KAction(i18n("Toggle Tablet Debugger"), this);
    actionCollection()->addAction("tablet_debugger", tabletDebugger );
    tabletDebugger->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(tabletDebugger, SIGNAL(triggered()), this, SLOT(toggleTabletLogger()));

    d->createTemplate = new KAction( i18n( "&Create Template From Image..." ), this);
    actionCollection()->addAction("createTemplate", d->createTemplate);
    connect(d->createTemplate, SIGNAL(triggered()), this, SLOT(slotCreateTemplate()));

    d->mirrorCanvas = new KToggleAction(i18n("Mirror View"), this);
    d->mirrorCanvas->setChecked(false);
    actionCollection()->addAction("mirror_canvas", d->mirrorCanvas);
    d->mirrorCanvas->setShortcut(QKeySequence(Qt::Key_M));

    d->openResourcesDirectory = new KAction(i18n("Open Resources Folder"), this);
    d->openResourcesDirectory->setToolTip(i18n("Opens a file browser at the location Krita saves resources such as brushes to."));
    d->openResourcesDirectory->setWhatsThis(i18n("Opens a file browser at the location Krita saves resources such as brushes to."));
    actionCollection()->addAction("open_resources_directory", d->openResourcesDirectory);
    connect(d->openResourcesDirectory, SIGNAL(triggered()), SLOT(openResourcesDirectory()));

    d->rotateCanvasRight = new KAction(i18n("Rotate Canvas Right"), this);
    actionCollection()->addAction("rotate_canvas_right", d->rotateCanvasRight);
    d->rotateCanvasRight->setShortcut(QKeySequence("Ctrl+]"));

    d->rotateCanvasLeft = new KAction(i18n("Rotate Canvas Left"), this);
    actionCollection()->addAction("rotate_canvas_left", d->rotateCanvasLeft);
    d->rotateCanvasLeft->setShortcut(QKeySequence("Ctrl+["));

    d->resetCanvasTransformations = new KAction(i18n("Reset Canvas Transformations"), this);
    actionCollection()->addAction("reset_canvas_transformations", d->resetCanvasTransformations);
    d->resetCanvasTransformations->setShortcut(QKeySequence("Ctrl+'"));

    d->wrapAroundAction = new KToggleAction(i18n("Wrap Around Mode"), this);
    actionCollection()->addAction("wrap_around_mode", d->wrapAroundAction);
    d->wrapAroundAction->setShortcut(QKeySequence(Qt::Key_W));


    KToggleAction *tAction = new KToggleAction(i18n("Show Status Bar"), this);
    tAction->setCheckedState(KGuiItem(i18n("Hide Status Bar")));
    tAction->setChecked(true);
    tAction->setToolTip(i18n("Shows or hides the status bar"));
    actionCollection()->addAction("showStatusBar", tAction);
    connect(tAction, SIGNAL(toggled(bool)), this, SLOT(showStatusBar(bool)));


    tAction = new KToggleAction(i18n("Show Canvas Only"), this);
    tAction->setCheckedState(KGuiItem(i18n("Return to Window")));
    tAction->setToolTip(i18n("Shows just the canvas or the whole window"));
    QList<QKeySequence> shortcuts;
    shortcuts << QKeySequence(Qt::Key_Tab);
    tAction->setShortcuts(shortcuts);
    tAction->setChecked(false);
    actionCollection()->addAction("view_show_just_the_canvas", tAction);
    connect(tAction, SIGNAL(toggled(bool)), this, SLOT(showJustTheCanvas(bool)));

    //Workaround, by default has the same shortcut as mirrorCanvas
    KAction *a = dynamic_cast<KAction*>(actionCollection()->action("format_italic"));
    if (a) {
        a->setShortcut(QKeySequence(), KAction::DefaultShortcut);
        a->setShortcut(QKeySequence(), KAction::ActiveShortcut);
    }

    a = new KAction(i18n("Cleanup removed files..."), this);
    actionCollection()->addAction("edit_blacklist_cleanup", a);
    connect(a, SIGNAL(triggered()), this, SLOT(slotBlacklistCleanup()));
}


void KisView2::createManagers()
{
    // Create the managers for filters, selections, layers etc.
    d->actionManager = new KisActionManager(this);

    d->filterManager = new KisFilterManager(this);
    d->filterManager->setup(actionCollection(), actionManager());

    d->selectionManager = new KisSelectionManager(this);
    d->selectionManager->setup(actionCollection(), actionManager());

    d->nodeManager = new KisNodeManager(this);
    d->nodeManager->setup(actionCollection(), actionManager());

    d->imageManager = new KisImageManager(this);
    d->imageManager->setup(actionCollection(), actionManager());

    d->gridManager = new KisGridManager(this);
    d->gridManager->setup(actionCollection());

    d->perspectiveGridManager = new KisPerspectiveGridManager(this);
    d->perspectiveGridManager->setup(actionCollection());

    d->paintingAssistantsDecoration = new KisPaintingAssistantsDecoration(this);
    d->paintingAssistantsDecoration->setup(actionCollection());

    d->canvasControlsManager = new KisCanvasControlsManager(this);
    d->canvasControlsManager->setup(actionCollection(), actionManager());
}

void KisView2::updateGUI()
{
    d->nodeManager->updateGUI();
    d->selectionManager->updateGUI();
    d->filterManager->updateGUI();
    zoomManager()->updateGUI();
    d->gridManager->updateGUI();
    d->perspectiveGridManager->updateGUI();
    d->actionManager->updateGUI();
}

void KisView2::slotBlacklistCleanup()
{
    KisDlgBlacklistCleanup dialog;
    dialog.exec();
}


void KisView2::slotImageSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint)
{
    if (!d->currentImageView) return;
    d->currentImageView->resetImageSizeAndScroll(true, oldStillPoint, newStillPoint);
    zoomManager()->updateGUI();
}

void KisView2::slotImageResolutionChanged()
{
    if (!d->currentImageView) return;
    d->currentImageView->resetImageSizeAndScroll(false);
    zoomManager()->updateGUI();
}

void KisView2::slotNodeChanged()
{
    updateGUI();
}

void KisView2::loadPlugins()
{
    // Load all plugins
    KService::List offers = KServiceTypeTrader::self()->query(QString::fromLatin1("Krita/ViewPlugin"),
                                                              QString::fromLatin1("(Type == 'Service') and "
                                                                                  "([X-Krita-Version] == 28)"));
    KService::List::ConstIterator iter;
    for (iter = offers.constBegin(); iter != offers.constEnd(); ++iter) {
        KService::Ptr service = *iter;
        dbgUI << "Load plugin " << service->name();
        QString error;

        KoXMLGUIClient* plugin =
                dynamic_cast<KoXMLGUIClient*>(service->createInstance<QObject>(this, QVariantList(), &error));
        if (plugin) {
            insertChildClient(plugin);
        } else {
            errKrita << "Fail to create an instance for " << service->name() << " " << error;
        }
    }
}


KoPrintJob * KisView2::createPrintJob()
{
    return new KisPrintJob(image());
}

KisNodeManager * KisView2::nodeManager()
{
    return d->nodeManager;
}

KisActionManager* KisView2::actionManager()
{
    return d->actionManager;
}

KisPerspectiveGridManager* KisView2::perspectiveGridManager()
{
    return d->perspectiveGridManager;
}

KisGridManager * KisView2::gridManager()
{
    return d->gridManager;
}

KisPaintingAssistantsDecoration* KisView2::paintingAssistantsDecoration()
{
    return d->paintingAssistantsDecoration;
}

QMainWindow* KisView2::qtMainWindow() const
{
    if(d->mainWindow)
        return d->mainWindow;
    return mainWindow();
}

void KisView2::setQtMainWindow(QMainWindow* newMainWindow)
{
    d->mainWindow = newMainWindow;
}

KisDoc2 *KisView2::document() const
{
    if (d->currentImageView && d->currentImageView->document()) {
        return qobject_cast<KisDoc2*>(d->currentImageView->document());
    }
    return 0;
}

void KisView2::slotTotalRefresh()
{
    if (!d->currentImageView || !d->currentImageView->canvasBase()) return;

    KisConfig cfg;
    d->currentImageView->canvasBase()->resetCanvas(cfg.useOpenGL());
}

void KisView2::slotCreateTemplate()
{
    if (!document()) return;

    KoTemplateCreateDia::createTemplate("krita_template", ".kra",
                                        KisFactory2::componentData(), document(), mainWindow());
}

void KisView2::slotDocumentSaved()
{
    d->saveIncremental->setEnabled(true);
    d->saveIncrementalBackup->setEnabled(true);
}

void KisView2::slotSaveIncremental()
{
    if (!document()) return;

    bool foundVersion;
    bool fileAlreadyExists;
    bool isBackup;
    QString version = "000";
    QString newVersion;
    QString letter;
    QString fileName = document()->localFilePath();

    // Find current version filenames
    // v v Regexp to find incremental versions in the filename, taking our backup scheme into account as well
    // Considering our incremental version and backup scheme, format is filename_001~001.ext
    QRegExp regex("_\\d{1,4}[.]|_\\d{1,4}[a-z][.]|_\\d{1,4}[~]|_\\d{1,4}[a-z][~]");
    regex.indexIn(fileName);     //  Perform the search
    QStringList matches = regex.capturedTexts();
    foundVersion = matches.at(0).isEmpty() ? false : true;

    // Ensure compatibility with Save Incremental Backup
    // If this regex is not kept separate, the entire algorithm needs modification;
    // It's simpler to just add this.
    QRegExp regexAux("_\\d{1,4}[~]|_\\d{1,4}[a-z][~]");
    regexAux.indexIn(fileName);     //  Perform the search
    QStringList matchesAux = regexAux.capturedTexts();
    isBackup = matchesAux.at(0).isEmpty() ? false : true;

    // If the filename has a version, prepare it for incrementation
    if (foundVersion) {
        version = matches.at(matches.count() - 1);     //  Look at the last index, we don't care about other matches
        if (version.contains(QRegExp("[a-z]"))) {
            version.chop(1);             //  Trim "."
            letter = version.right(1);   //  Save letter
            version.chop(1);             //  Trim letter
        } else {
            version.chop(1);             //  Trim "."
        }
        version.remove(0, 1);            //  Trim "_"
    } else {
        // ...else, simply add a version to it so the next loop works
        QRegExp regex2("[.][a-z]{2,4}$");  //  Heuristic to find file extension
        regex2.indexIn(fileName);
        QStringList matches2 = regex2.capturedTexts();
        QString extensionPlusVersion = matches2.at(0);
        extensionPlusVersion.prepend(version);
        extensionPlusVersion.prepend("_");
        fileName.replace(regex2, extensionPlusVersion);
    }

    // Prepare the base for new version filename
    int intVersion = version.toInt(0);
    ++intVersion;
    QString baseNewVersion = QString::number(intVersion);
    while (baseNewVersion.length() < version.length()) {
        baseNewVersion.prepend("0");
    }

    // Check if the file exists under the new name and search until options are exhausted (test appending a to z)
    do {
        newVersion = baseNewVersion;
        newVersion.prepend("_");
        if (!letter.isNull()) newVersion.append(letter);
        if (isBackup) {
            newVersion.append("~");
        } else {
            newVersion.append(".");
        }
        fileName.replace(regex, newVersion);
        fileAlreadyExists = KIO::NetAccess::exists(fileName, KIO::NetAccess::DestinationSide, mainWindow());
        if (fileAlreadyExists) {
            if (!letter.isNull()) {
                char letterCh = letter.at(0).toLatin1();
                ++letterCh;
                letter = QString(QChar(letterCh));
            } else {
                letter = 'a';
            }
        }
    } while (fileAlreadyExists && letter != "{");  // x, y, z, {...

    if (letter == "{") {
        KMessageBox::error(mainWindow(), "Alternative names exhausted, try manually saving with a higher number", "Couldn't save incremental version");
        return;
    }
    document()->setSaveInBatchMode(true);
    document()->saveAs(fileName);
    document()->setSaveInBatchMode(false);


    if (mainWindow()) {
        mainWindow()->updateCaption();
    }
}

void KisView2::slotSaveIncrementalBackup()
{
    if (!document()) return;

    bool workingOnBackup;
    bool fileAlreadyExists;
    QString version = "000";
    QString newVersion;
    QString letter;
    QString fileName = document()->localFilePath();

    // First, discover if working on a backup file, or a normal file
    QRegExp regex("~\\d{1,4}[.]|~\\d{1,4}[a-z][.]");
    regex.indexIn(fileName);     //  Perform the search
    QStringList matches = regex.capturedTexts();
    workingOnBackup = matches.at(0).isEmpty() ? false : true;

    if (workingOnBackup) {
        // Try to save incremental version (of backup), use letter for alt versions
        version = matches.at(matches.count() - 1);     //  Look at the last index, we don't care about other matches
        if (version.contains(QRegExp("[a-z]"))) {
            version.chop(1);             //  Trim "."
            letter = version.right(1);   //  Save letter
            version.chop(1);             //  Trim letter
        } else {
            version.chop(1);             //  Trim "."
        }
        version.remove(0, 1);            //  Trim "~"

        // Prepare the base for new version filename
        int intVersion = version.toInt(0);
        ++intVersion;
        QString baseNewVersion = QString::number(intVersion);
        QString backupFileName = document()->localFilePath();
        while (baseNewVersion.length() < version.length()) {
            baseNewVersion.prepend("0");
        }

        // Check if the file exists under the new name and search until options are exhausted (test appending a to z)
        do {
            newVersion = baseNewVersion;
            newVersion.prepend("~");
            if (!letter.isNull()) newVersion.append(letter);
            newVersion.append(".");
            backupFileName.replace(regex, newVersion);
            fileAlreadyExists = KIO::NetAccess::exists(backupFileName, KIO::NetAccess::DestinationSide, mainWindow());
            if (fileAlreadyExists) {
                if (!letter.isNull()) {
                    char letterCh = letter.at(0).toLatin1();
                    ++letterCh;
                    letter = QString(QChar(letterCh));
                } else {
                    letter = 'a';
                }
            }
        } while (fileAlreadyExists && letter != "{");  // x, y, z, {...

        if (letter == "{") {
            KMessageBox::error(mainWindow(), "Alternative names exhausted, try manually saving with a higher number", "Couldn't save incremental backup");
            return;
        }
        QFile::copy(fileName, backupFileName);
        document()->saveAs(fileName);

        if (mainWindow()) mainWindow()->updateCaption();
    }
    else { // if NOT working on a backup...
        // Navigate directory searching for latest backup version, ignore letters
        const quint8 HARDCODED_DIGIT_COUNT = 3;
        QString baseNewVersion = "000";
        QString backupFileName = document()->localFilePath();
        QRegExp regex2("[.][a-z]{2,4}$");  //  Heuristic to find file extension
        regex2.indexIn(backupFileName);
        QStringList matches2 = regex2.capturedTexts();
        QString extensionPlusVersion = matches2.at(0);
        extensionPlusVersion.prepend(baseNewVersion);
        extensionPlusVersion.prepend("~");
        backupFileName.replace(regex2, extensionPlusVersion);

        // Save version with 1 number higher than the highest version found ignoring letters
        do {
            newVersion = baseNewVersion;
            newVersion.prepend("~");
            newVersion.append(".");
            backupFileName.replace(regex, newVersion);
            fileAlreadyExists = KIO::NetAccess::exists(backupFileName, KIO::NetAccess::DestinationSide, mainWindow());
            if (fileAlreadyExists) {
                // Prepare the base for new version filename, increment by 1
                int intVersion = baseNewVersion.toInt(0);
                ++intVersion;
                baseNewVersion = QString::number(intVersion);
                while (baseNewVersion.length() < HARDCODED_DIGIT_COUNT) {
                    baseNewVersion.prepend("0");
                }
            }
        } while (fileAlreadyExists);

        // Save both as backup and on current file for interapplication workflow
        document()->setSaveInBatchMode(true);
        QFile::copy(fileName, backupFileName);
        document()->saveAs(fileName);
        document()->setSaveInBatchMode(false);

        if (mainWindow()) mainWindow()->updateCaption();
    }
}

void KisView2::disableControls()
{
    // prevents possible crashes, if somebody changes the paintop during dragging by using the mousewheel
    // this is for Bug 250944
    // the solution blocks all wheel, mouse and key event, while dragging with the freehand tool
    // see KisToolFreehand::initPaint() and endPaint()
    d->controlFrame->paintopBox()->installEventFilter(&d->blockingEventFilter);
    foreach(QObject* child, d->controlFrame->paintopBox()->children()) {
        child->installEventFilter(&d->blockingEventFilter);
    }
}

void KisView2::enableControls()
{
    d->controlFrame->paintopBox()->removeEventFilter(&d->blockingEventFilter);
    foreach(QObject* child, d->controlFrame->paintopBox()->children()) {
        child->removeEventFilter(&d->blockingEventFilter);
    }
}

void KisView2::showStatusBar(bool toggled)
{
    if (d->currentImageView && d->currentImageView->statusBar()) {
        d->currentImageView->statusBar()->setVisible(toggled);
    }
}

#if defined HAVE_OPENGL && defined Q_OS_WIN32
#include <QGLContext>
#endif

void KisView2::showJustTheCanvas(bool toggled)
{
    KisConfig cfg;

/**
 * Workaround for a broken Intel video driver on Windows :(
 * See bug 330040
 */
#if defined HAVE_OPENGL && defined Q_OS_WIN32

    if (toggled && cfg.useOpenGL()) {
        QString renderer((const char*)glGetString(GL_RENDERER));
        bool failingDriver = renderer.startsWith("Intel(R) HD Graphics");

        if (failingDriver &&
            cfg.hideStatusbarFullscreen() &&
            cfg.hideDockersFullscreen() &&
            cfg.hideTitlebarFullscreen() &&
            cfg.hideMenuFullscreen() &&
            cfg.hideToolbarFullscreen() &&
            cfg.hideScrollbarsFullscreen()) {
            int result =
                KMessageBox::warningYesNo(this,
                                          "Intel(R) HD Graphics video adapters "
                                          "are known to have problems with running "
                                          "Krita in pure canvas only mode. At least "
                                          "one UI control must be shown to "
                                          "workaround it.\n\nShow the scroll bars?",
                                          "Failing video adapter",
                                          KStandardGuiItem::yes(),
                                          KStandardGuiItem::no(),
                                          "messagebox_WorkaroundIntelVideoOnWindows");

            if (result == KMessageBox::Yes) {
                cfg.setHideScrollbarsFullscreen(false);
            }
        }
    }

#endif /* defined HAVE_OPENGL && defined Q_OS_WIN32 */

    KoMainWindow *mainWin = mainWindow();
    if (!mainWin) {
        dbgUI << "Unable to switch to canvas-only mode, main window not found";
        return;
    }

    if (cfg.hideStatusbarFullscreen()) {
        if(mainWin->statusBar() && mainWin->statusBar()->isVisible() == toggled) {
            mainWin->statusBar()->setVisible(!toggled);
        }
    }

    if (cfg.hideDockersFullscreen()) {
        KToggleAction* action = qobject_cast<KToggleAction*>(mainWin->actionCollection()->action("view_toggledockers"));
        if (action && action->isChecked() == toggled) {
            action->setChecked(!toggled);
        }
    }

    if (cfg.hideTitlebarFullscreen()) {
        if(toggled) {
            mainWin->setWindowState(mainWin->windowState() | Qt::WindowFullScreen);
        } else {
            mainWin->setWindowState(mainWin->windowState() & ~Qt::WindowFullScreen);
        }
    }

    if (cfg.hideMenuFullscreen()) {
        if (mainWin->menuBar()->isVisible() == toggled) {
            mainWin->menuBar()->setVisible(!toggled);
        }
    }

    if (cfg.hideToolbarFullscreen()) {
        QList<QToolBar*> toolBars = mainWin->findChildren<QToolBar*>();
        foreach(QToolBar* toolbar, toolBars) {
            if (toolbar->isVisible() == toggled) {
                toolbar->setVisible(!toggled);
            }
        }
    }

    showHideScrollbars();

    if (toggled) {
        // show a fading heads-up display about the shortcut to go back
        KisFloatingMessage *floatingMessage = new KisFloatingMessage(i18n("Going into Canvas-Only mode.\nPress %1 to go back.",
                                                                          actionCollection()->action("view_show_just_the_canvas")->shortcut().toString())
                                                                     , mainWin);
        floatingMessage->showMessage();
    }
}

void KisView2::toggleTabletLogger()
{
    if (d->currentImageView && d->currentImageView->canvasBase()) {
        d->currentImageView->canvasBase()->toggleTabletLogger();
    }
}

void KisView2::openResourcesDirectory()
{
    QString dir = KStandardDirs::locateLocal("data", "krita");
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void KisView2::updateIcons()
{
    QColor background = mainWindow()->palette().background().color();
    bool useDarkIcons = background.value() > 100;
    QString prefix = useDarkIcons ? QString("dark_") : QString("light_");

    QStringList whitelist;
    whitelist << "ToolBox" << "KisLayerBox";

    QStringList blacklistedIcons;
    blacklistedIcons << "editpath" << "artistictext-tool" << "view-choose";

    if (mainWindow()) {
        QList<QDockWidget*> dockers = mainWindow()->dockWidgets();
        foreach(QDockWidget* dock, dockers) {
            kDebug() << "name " << dock->objectName();
            if (!whitelist.contains(dock->objectName())) {
                continue;
            }

            QObjectList objects;
            objects.append(dock);
            while (!objects.isEmpty()) {
                QObject* object = objects.takeFirst();
                objects.append(object->children());

                QAbstractButton* button = dynamic_cast<QAbstractButton*>(object);
                if (button && !button->icon().name().isEmpty()) {
                    QString name = button->icon().name();
                    name = name.remove("dark_").remove("light_");

                    if (!blacklistedIcons.contains(name)) {
                        QString iconName = prefix + name;
                        KIcon icon = koIcon(iconName.toLatin1());
                        button->setIcon(icon);
                    }
                }
            }
        }
    }
}

void KisView2::makeStatusBarVisible()
{
    d->mainWindow->statusBar()->setVisible(true);
}

void KisView2::showFloatingMessage(const QString message, const QIcon& icon)
{
    // Yes, the @return is correct. But only for widget based KDE apps, not QML based ones
    if (mainWindow()) {
        KisFloatingMessage *floatingMessage = new KisFloatingMessage(message, mainWindow()->centralWidget());
        floatingMessage->setShowOverParent(true);
        floatingMessage->setIcon(icon);
        floatingMessage->showMessage();
    }
#if QT_VERSION >= 0x040700
    emit floatingMessageRequested(message, icon.name());
#endif
}


KoMainWindow *KisView2::mainWindow() const
{
    return qobject_cast<KoMainWindow*>(d->mainWindow);
}

void KisView2::showHideScrollbars()
{
    if (!canvasController()) return;

    KisConfig cfg;
    bool toggled = actionCollection()->action("view_show_just_the_canvas")->isChecked();

    if ((toggled && cfg.hideScrollbarsFullscreen()) || (!toggled && cfg.hideScrollbars())) {
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
}

#include "kis_view2.moc"
