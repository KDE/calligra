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
#include <QToolBar>
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
#include <kstatusbar.h>
#include <ktoggleaction.h>
#include <kaction.h>
#include <klocale.h>
#include <kmenu.h>
#include <kservice.h>
#include <KoServiceLocator.h>
#include <kstandarddirs.h>
#include <kstandardaction.h>
#include <kurl.h>
#include <kxmlguifactory.h>
#include <kmessagebox.h>
#include <kactioncollection.h>

#include <KoToolRegistry.h>
#include <KoStore.h>
#include <KoMainWindow.h>
#include <KoSelection.h>
#include <KoToolBoxFactory.h>
#include <KoZoomHandler.h>
#include <KoToolManager.h>
#include <KoViewConverter.h>
#include <KoView.h>
#include <KoDockerManager.h>
#include <KoDockRegistry.h>
#include <KoResourceServerProvider.h>
#include <KoResourceItemChooserSync.h>
#include <KoDockWidgetTitleBar.h>
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
#include "kis_node_commands_adapter.h"
#include <kis_paintop_preset.h>
#include <kis_signal_compressor.h>
#include "kis_favorite_resource_manager.h"
#include "kis_action_manager.h"
#include "input/kis_input_profile_manager.h"
#include "kis_canvas_controls_manager.h"
#include "kis_mainwindow_observer.h"
#include "kis_main_window.h"
#include "kis_painting_assistants_manager.h"
#include "kis_part2.h"

#include "kis_tooltip_manager.h"
#include <kis_tool_freehand.h>


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
        , paintingAssistantsManager(0)
        , actionManager(0)
        , mainWindow(0)
        , showFloatingMessage(true)
        , currentImageView(0)
        , canvasResourceProvider(0)
        , canvasResourceManager(0)
        , guiUpdateCompressor(0)
    {
    }

    ~KisView2Private() {
        delete filterManager;
        delete selectionManager;
        delete nodeManager;
        delete imageManager;
        delete gridManager;
        delete perspectiveGridManager;
        delete paintingAssistantsManager;
        delete statusBar;
        delete actionManager;
        delete canvasControlsManager;
        delete canvasResourceProvider;
        delete canvasResourceManager;

        /**
         * Push a timebomb, which will try to release the memory after
         * the document has been deleted
         */
        KisPaintDevice::createMemoryReleaseObject()->deleteLater();
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
    KToggleAction *wrapAroundAction;
    KisSelectionManager *selectionManager;
    KisControlFrame *controlFrame;
    KisNodeManager *nodeManager;
    KisImageManager *imageManager;
    KisGridManager *gridManager;
    KisCanvasControlsManager *canvasControlsManager;
    KisPerspectiveGridManager * perspectiveGridManager;
    KisPaintingAssistantsManager *paintingAssistantsManager;
    BlockingUserInputEventFilter blockingEventFilter;
    KisFlipbook *flipbook;
    KisActionManager* actionManager;
    QMainWindow* mainWindow;
    QPointer<KisFloatingMessage> savedFloatingMessage;
    bool showFloatingMessage;
    QPointer<KisImageView> currentImageView;
    KisCanvasResourceProvider* canvasResourceProvider;
    KoCanvasResourceManager* canvasResourceManager;
    QList<StatusBarItem> statusBarItems;
    KisSignalCompressor* guiUpdateCompressor;
};


KisView2::KisView2(QWidget * parent)
    : KXMLGUIClient(),
      m_d(new KisView2Private())
{
    m_d->mainWindow = dynamic_cast<QMainWindow*>(parent);

    setXMLFile(QString("%1.rc").arg(qAppName()));
    m_d->canvasResourceProvider = new KisCanvasResourceProvider(this);
    m_d->canvasResourceManager = new KoCanvasResourceManager();
    m_d->canvasResourceProvider->setResourceManager(m_d->canvasResourceManager);

    m_d->guiUpdateCompressor = new KisSignalCompressor(30, KisSignalCompressor::POSTPONE, this);
    connect(m_d->guiUpdateCompressor, SIGNAL(timeout()), this, SLOT(guiUpdateTimeout()));

    createActions();
    createManagers();

    m_d->controlFrame = new KisControlFrame(this, mainWindow());

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

    m_d->statusBar = new KisStatusBar(this);
    QTimer::singleShot(0, this, SLOT(makeStatusBarVisible()));

    connect(m_d->nodeManager, SIGNAL(sigNodeActivated(KisNodeSP)),
            m_d->controlFrame->paintopBox(), SLOT(slotCurrentNodeChanged(KisNodeSP)));

    connect(KoToolManager::instance(), SIGNAL(inputDeviceChanged(KoInputDevice)),
            m_d->controlFrame->paintopBox(), SLOT(slotInputDeviceChanged(KoInputDevice)));

    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*,int)),
            m_d->controlFrame->paintopBox(), SLOT(slotToolChanged(KoCanvasController*,int)));

    connect(m_d->nodeManager, SIGNAL(sigNodeActivated(KisNodeSP)),
            resourceProvider(), SLOT(slotNodeActivated(KisNodeSP)));

    connect(resourceProvider()->resourceManager(), SIGNAL(canvasResourceChanged(int,QVariant)),
            m_d->controlFrame->paintopBox(), SLOT(slotCanvasResourceChanged(int,QVariant)));

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

    KisPaintOpPresetResourceServer * rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    if (rserver->resources().isEmpty()) {
        KMessageBox::error(mainWindow(), i18n("Krita cannot find any brush presets and will close now. Please check your installation.", i18n("Critical Error")));
        exit(0);
    }


    foreach(const QString & docker, KoDockRegistry::instance()->keys()) {
        KoDockFactoryBase *factory = KoDockRegistry::instance()->value(docker);
        if (mainWindow())
            mainWindow()->createDockWidget(factory);
    }
    foreach(KoCanvasObserverBase* observer, mainWindow()->canvasObservers()) {
        KisMainwindowObserver* mainwindowObserver = dynamic_cast<KisMainwindowObserver*>(observer);
        if (mainwindowObserver) {
            mainwindowObserver->setMainWindow(this);
        }
    }

    m_d->actionManager->updateGUI();

    connect(mainWindow(), SIGNAL(themeChanged()), this, SLOT(updateIcons()));
    updateIcons();
    qDebug() << "KisView2 Constructed";
}


KisView2::~KisView2()
{
    KisConfig cfg;
    if (resourceProvider() && resourceProvider()->currentPreset()) {
        cfg.writeEntry("LastPreset", resourceProvider()->currentPreset()->name());
    }
    cfg.writeEntry("baseLength", KoResourceItemChooserSync::instance()->baseLength());

    if (m_d->filterManager->isStrokeRunning()) {
        m_d->filterManager->cancel();
    }

    // The reason for this is to ensure the shortcuts are saved at the right time,
    // and only the right shortcuts. Gemini has two views at all times, and shortcuts
    // must be handled by the desktopview, but if we use the logic as below, we
    // overwrite the desktop view's settings with the sketch view's
    if(qApp->applicationName() == QLatin1String("kritagemini")) {
        KConfigGroup group(KGlobal::config(), "krita/shortcuts");
        foreach(KActionCollection *collection, KActionCollection::allCollections()) {
            const QObject* obj = dynamic_cast<const QObject*>(collection->parentGUIClient());
            if(obj && qobject_cast<const KisView2*>(obj) && !obj->objectName().startsWith("view_0"))
                break;
            collection->setConfigGroup("krita/shortcuts");
            collection->writeSettings(&group);
        }
    }
    else {
        KConfigGroup group(KGlobal::config(), "krita/shortcuts");
        foreach(KActionCollection *collection, KActionCollection::allCollections()) {
            collection->setConfigGroup("krita/shortcuts");
            collection->writeSettings(&group);
        }
    }
    delete m_d;
}

void KisView2::setCurrentView(KoView *view)
{
    qDebug() << ">>>>>>>>>>>>>setCurrentView" << view;
    bool first = true;
    if (m_d->currentImageView) {
        first = false;
        KisDoc2* doc = qobject_cast<KisDoc2*>(m_d->currentImageView->document());
        if (doc) {
            doc->disconnect(this);
        }
        canvasController()->proxyObject->disconnect(m_d->statusBar);
        m_d->nodeManager->disconnect(doc->image());

        m_d->rotateCanvasRight->disconnect();
        m_d->rotateCanvasLeft->disconnect();
        m_d->mirrorCanvas->disconnect();
        m_d->wrapAroundAction->disconnect();
        canvasControllerWidget()->disconnect(SIGNAL(toolOptionWidgetsChanged(QList<QPointer<QWidget> >)), mainWindow()->dockerManager());
    }

    QPointer<KisImageView>imageView = qobject_cast<KisImageView*>(view);

    if (imageView) {

        imageView->setParentView(this);

        // Wait for the async image to have loaded
        KisDoc2* doc = qobject_cast<KisDoc2*>(view->document());
        //        connect(canvasController()->proxyObject, SIGNAL(documentMousePositionChanged(QPointF)), d->statusBar, SLOT(documentMousePositionChanged(QPointF)));

        m_d->currentImageView = imageView;
        imageView->canvasBase()->setSharedResourceManager(m_d->canvasResourceManager);

        connect(m_d->nodeManager, SIGNAL(sigNodeActivated(KisNodeSP)), doc->image(), SLOT(requestStrokeEnd()));
        connect(m_d->rotateCanvasRight, SIGNAL(triggered()), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(rotateCanvasRight15()));
        connect(m_d->rotateCanvasLeft, SIGNAL(triggered()),dynamic_cast<KisCanvasController*>(canvasController()), SLOT(rotateCanvasLeft15()));
        connect(m_d->mirrorCanvas, SIGNAL(toggled(bool)), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(mirrorCanvas(bool)));
        connect(m_d->wrapAroundAction, SIGNAL(toggled(bool)), dynamic_cast<KisCanvasController*>(canvasController()), SLOT(slotToggleWrapAroundMode(bool)));
        connect(canvasControllerWidget(), SIGNAL(toolOptionWidgetsChanged(QList<QPointer<QWidget> >)), mainWindow()->dockerManager(), SLOT(newOptionWidgets(QList<QPointer<QWidget> >)));

    }

    m_d->filterManager->setView(imageView);
    m_d->selectionManager->setView(imageView);
    m_d->nodeManager->setView(imageView);
    m_d->imageManager->setView(imageView);
    m_d->canvasControlsManager->setView(imageView);
    m_d->actionManager->setView(imageView);
    m_d->gridManager->setView(imageView);
    m_d->statusBar->setView(imageView);
    m_d->paintingAssistantsManager->setView(imageView);
    m_d->perspectiveGridManager->setView(imageView);

    if (m_d->currentImageView) {
        canvasControllerWidget()->activate();
    }
    actionManager()->updateGUI();

    // Restore the last used brush preset
    if (first) {
        KisConfig cfg;
        KisPaintOpPresetResourceServer * rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
        QString lastPreset = cfg.readEntry("LastPreset", QString("Basic_tip_default"));
        KisPaintOpPresetSP preset = rserver->resourceByName(lastPreset);
        if (!preset) {
            preset = rserver->resourceByName("Basic_tip_default");
        }

        if (!preset) {
            preset = rserver->resources().first();
        }
        if (preset) {
            paintOpBox()->resourceSelected(preset.data());
        }

    }
}


KoZoomController *KisView2::zoomController() const
{
    if (m_d->currentImageView) {
        return m_d->currentImageView->zoomController();
    }
    return 0;
}

KisImageWSP KisView2::image() const
{
    if (document()) {
        return document()->image();
    }
    return 0;
}

KisCanvasResourceProvider * KisView2::resourceProvider()
{
    return m_d->canvasResourceProvider;
}

KisCanvas2 * KisView2::canvasBase() const
{
    if (m_d && m_d->currentImageView) {
        return m_d->currentImageView->canvasBase();
    }
    return 0;
}

QWidget* KisView2::canvas() const
{
    if (m_d && m_d->currentImageView && m_d->currentImageView->canvasBase()->canvasWidget()) {
        return m_d->currentImageView->canvasBase()->canvasWidget();
    }
    return 0;
}

KisStatusBar * KisView2::statusBar() const
{
    return m_d->statusBar;
}

void KisView2::addStatusBarItem(QWidget * widget, int stretch, bool permanent)
{
    StatusBarItem item(widget, stretch, permanent);
    KStatusBar * sb = mainWindow()->statusBar();
    if (sb) {
        item.ensureItemShown(sb);
    }
    m_d->statusBarItems.append(item);
}

void KisView2::removeStatusBarItem(QWidget * widget)
{
    KStatusBar *sb = mainWindow()->statusBar();

    int itemCount = m_d->statusBarItems.count();
    for (int i = itemCount-1; i >= 0; --i) {
        StatusBarItem &sbItem = m_d->statusBarItems[i];
        if (sbItem.widget() == widget) {
            if (sb) {
                sbItem.ensureItemHidden(sb);
            }
            m_d->statusBarItems.removeOne(sbItem);
            break;
        }
    }
}

KisPaintopBox* KisView2::paintOpBox() const
{
    return m_d->controlFrame->paintopBox();
}

KoProgressUpdater* KisView2::createProgressUpdater(KoProgressUpdater::Mode mode)
{
    return new KisProgressUpdater(m_d->statusBar->progress(), document()->progressProxy(), mode);
}

KisSelectionManager * KisView2::selectionManager()
{
    return m_d->selectionManager;
}

KoCanvasController * KisView2::canvasController()
{
    if (m_d && m_d->currentImageView) {
        return m_d->currentImageView->canvasController();
    }
    return 0;
}

KisCanvasController *KisView2::canvasControllerWidget()
{
    if (m_d && m_d->currentImageView) {
        return dynamic_cast<KisCanvasController*>(canvasController());
    }
    return 0;
}

KisNodeSP KisView2::activeNode()
{
    if (m_d->nodeManager)
        return m_d->nodeManager->activeNode();
    else
        return 0;
}

KisLayerSP KisView2::activeLayer()
{
    if (m_d->nodeManager)
        return m_d->nodeManager->activeLayer();
    else
        return 0;
}

KisPaintDeviceSP KisView2::activeDevice()
{
    if (m_d->nodeManager)
        return m_d->nodeManager->activePaintDevice();
    else
        return 0;
}

KisZoomManager * KisView2::zoomManager()
{
    if (m_d->currentImageView) {
        return m_d->currentImageView->zoomManager();
    }
    return 0;
}

KisFilterManager * KisView2::filterManager()
{
    return m_d->filterManager;
}

KisImageManager * KisView2::imageManager()
{
    return m_d->imageManager;
}

KisSelectionSP KisView2::selection()
{
    if (m_d->currentImageView) {
        return m_d->currentImageView->selection();
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
    if (nodeManager()) {
        nodeManager()->nodesUpdated();
    }

    connect(image(), SIGNAL(sigSizeChanged(const QPointF&, const QPointF&)), resourceProvider(), SLOT(slotImageSizeChanged()));

    connect(image(), SIGNAL(sigResolutionChanged(double,double)),
            resourceProvider(), SLOT(slotOnScreenResolutionChanged()));
    connect(zoomManager()->zoomController(), SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)),
            resourceProvider(), SLOT(slotOnScreenResolutionChanged()));

    connect(image(), SIGNAL(sigSizeChanged(const QPointF&, const QPointF&)), this, SLOT(slotImageSizeChanged(const QPointF&, const QPointF&)));
    connect(image(), SIGNAL(sigResolutionChanged(double,double)), this, SLOT(slotImageResolutionChanged()));
    connect(image(), SIGNAL(sigNodeChanged(KisNodeSP)), this, SLOT(slotNodeChanged()));

    /*
     * WARNING: Currently we access the global progress bar in two ways:
     * connecting to composite progress proxy (strokes) and creating
     * progress updaters. The latter way should be deprecated in favour
     * of displaying the status of the global strokes queue
     */
    image()->compositeProgressProxy()->addProxy(m_d->statusBar->progress()->progressProxy());
    connect(m_d->statusBar->progress(), SIGNAL(sigCancellationRequested()),
            image(), SLOT(requestStrokeCancellation()));

    m_d->currentImageView->canvasBase()->initializeImage();

    if (m_d->controlFrame) {
        connect(image(), SIGNAL(sigColorSpaceChanged(const KoColorSpace*)), m_d->controlFrame->paintopBox(), SLOT(slotColorSpaceChanged(const KoColorSpace*)));
    }

    if (image()->locked()) {
        // If this is the first view on the image, the image will have been locked
        // so unlock it.
        image()->blockSignals(false);
        image()->unlock();
    }

    // get the assistants and push them to the manager
    // XXX: KOMVC: assistants!!!
    //    QList<KisPaintingAssistant*> paintingAssistants = document()->preLoadedAssistants();
    //    foreach (KisPaintingAssistant* assistant, paintingAssistants) {
    //        m_d->paintingAssistantsDecoration->addAssistant(assistant);
    //    }

    /**
     * Dirty hack alert
     */
    if (mainWindow() && zoomManager() && zoomManager()->zoomController())
        zoomManager()->zoomController()->setAspectMode(true);

    if (m_d->currentImageView && m_d->currentImageView->viewConverter())
        m_d->currentImageView->viewConverter()->setZoomMode(KoZoomMode::ZOOM_PAGE);


    //    if (m_d->paintingAssistantsDecoration){
    //        foreach(KisPaintingAssistant* assist, document()->preLoadedAssistants()){
    //            m_d->paintingAssistantsDecoration->addAssistant(assist);
    //        }
    //        m_d->paintingAssistantsDecoration->setVisible(true);
    //    }

    updateGUI();

    emit sigLoadingFinished();
}

void KisView2::slotSavingFinished()
{
    if(mainWindow())
        mainWindow()->updateCaption();
}

void KisView2::createActions()
{
    m_d->saveIncremental = new KAction(i18n("Save Incremental &Version"), this);
    m_d->saveIncremental->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_S));
    actionCollection()->addAction("save_incremental_version", m_d->saveIncremental);
    connect(m_d->saveIncremental, SIGNAL(triggered()), this, SLOT(slotSaveIncremental()));

    m_d->saveIncrementalBackup = new KAction(i18n("Save Incremental Backup"), this);
    m_d->saveIncrementalBackup->setShortcut(Qt::Key_F4);
    actionCollection()->addAction("save_incremental_backup", m_d->saveIncrementalBackup);
    connect(m_d->saveIncrementalBackup, SIGNAL(triggered()), this, SLOT(slotSaveIncrementalBackup()));

    connect(mainWindow(), SIGNAL(documentSaved()), this, SLOT(slotDocumentSaved()));

    m_d->saveIncremental->setEnabled(false);
    m_d->saveIncrementalBackup->setEnabled(false);

    KAction *tabletDebugger = new KAction(i18n("Toggle Tablet Debugger"), this);
    actionCollection()->addAction("tablet_debugger", tabletDebugger );
    tabletDebugger->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_T));
    connect(tabletDebugger, SIGNAL(triggered()), this, SLOT(toggleTabletLogger()));

    m_d->createTemplate = new KAction( i18n( "&Create Template From Image..." ), this);
    actionCollection()->addAction("createTemplate", m_d->createTemplate);
    connect(m_d->createTemplate, SIGNAL(triggered()), this, SLOT(slotCreateTemplate()));

    m_d->mirrorCanvas = new KToggleAction(i18n("Mirror View"), this);
    m_d->mirrorCanvas->setChecked(false);
    actionCollection()->addAction("mirror_canvas", m_d->mirrorCanvas);
    m_d->mirrorCanvas->setShortcut(QKeySequence(Qt::Key_M));

    m_d->openResourcesDirectory = new KAction(i18n("Open Resources Folder"), this);
    m_d->openResourcesDirectory->setToolTip(i18n("Opens a file browser at the location Krita saves resources such as brushes to."));
    m_d->openResourcesDirectory->setWhatsThis(i18n("Opens a file browser at the location Krita saves resources such as brushes to."));
    actionCollection()->addAction("open_resources_directory", m_d->openResourcesDirectory);
    connect(m_d->openResourcesDirectory, SIGNAL(triggered()), SLOT(openResourcesDirectory()));

    m_d->rotateCanvasRight = new KAction(i18n("Rotate Canvas Right"), this);
    actionCollection()->addAction("rotate_canvas_right", m_d->rotateCanvasRight);
    m_d->rotateCanvasRight->setShortcut(QKeySequence("Ctrl+]"));

    m_d->rotateCanvasLeft = new KAction(i18n("Rotate Canvas Left"), this);
    actionCollection()->addAction("rotate_canvas_left", m_d->rotateCanvasLeft);
    m_d->rotateCanvasLeft->setShortcut(QKeySequence("Ctrl+["));

    m_d->wrapAroundAction = new KToggleAction(i18n("Wrap Around Mode"), this);
    actionCollection()->addAction("wrap_around_mode", m_d->wrapAroundAction);
    m_d->wrapAroundAction->setShortcut(QKeySequence(Qt::Key_W));


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
    // XXX: When the currentlayer changes, call updateGUI on all
    // managers

    m_d->actionManager = new KisActionManager(this);

    m_d->filterManager = new KisFilterManager(this);
    m_d->filterManager->setup(actionCollection(), actionManager());

    m_d->selectionManager = new KisSelectionManager(this);
    m_d->selectionManager->setup(actionCollection(), actionManager());

    m_d->nodeManager = new KisNodeManager(this);
    m_d->nodeManager->setup(actionCollection(), actionManager());

    m_d->imageManager = new KisImageManager(this);
    m_d->imageManager->setup(actionCollection(), actionManager());

    m_d->gridManager = new KisGridManager(this);
    m_d->gridManager->setup(actionCollection());

    m_d->perspectiveGridManager = new KisPerspectiveGridManager(this);
    m_d->perspectiveGridManager->setup(actionCollection());

    m_d->paintingAssistantsManager = new KisPaintingAssistantsManager(this);
    m_d->paintingAssistantsManager->setup(actionCollection());

    m_d->canvasControlsManager = new KisCanvasControlsManager(this);
    m_d->canvasControlsManager->setup(actionCollection(), actionManager());

    // XXX: KOMVC: move mirror decoration to KisImageView, create a manager for it???
    //    m_d->mirrorAxis = new KisMirrorAxis(m_d->resourceProvider, this);
    //    m_d->canvas->addDecoration(m_d->mirrorAxis);
}

void KisView2::updateGUI()
{
    m_d->guiUpdateCompressor->start();
}

void KisView2::slotBlacklistCleanup()
{
    KisDlgBlacklistCleanup dialog;
    dialog.exec();
}

void KisView2::slotImageSizeChanged(const QPointF &oldStillPoint, const QPointF &newStillPoint)
{
    if (!m_d->currentImageView) return;
    m_d->currentImageView->resetImageSizeAndScroll(true, oldStillPoint, newStillPoint);
    zoomManager()->updateGUI();
}

void KisView2::slotImageResolutionChanged()
{
    if (!m_d->currentImageView) return;
    m_d->currentImageView->resetImageSizeAndScroll(false);
    zoomManager()->updateGUI();
}

void KisView2::slotNodeChanged()
{
    updateGUI();
}

void KisView2::loadPlugins()
{
    // Load all plugins
    const KService::List offers = KoServiceLocator::instance()->entries("Krita/ViewPlugin");
    KService::List::ConstIterator iter;
    for (iter = offers.constBegin(); iter != offers.constEnd(); ++iter) {
        KService::Ptr service = *iter;
        dbgUI << "Load plugin " << service->name();
        QString error;

        KXMLGUIClient* plugin =
                dynamic_cast<KXMLGUIClient*>(service->createInstance<QObject>(this, QVariantList(), &error));
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

KisNodeManager * KisView2::nodeManager() const
{
    return m_d->nodeManager;
}

KisActionManager* KisView2::actionManager() const
{
    return m_d->actionManager;
}

KisPerspectiveGridManager* KisView2::perspectiveGridManager() const
{
    return m_d->perspectiveGridManager;
}

KisGridManager * KisView2::gridManager() const
{
    return m_d->gridManager;
}

KisPaintingAssistantsManager* KisView2::paintingAssistantsManager() const
{
    return m_d->paintingAssistantsManager;
}

QMainWindow* KisView2::qtMainWindow() const
{
    if(m_d->mainWindow)
        return m_d->mainWindow;

    //Fallback for when we have not yet set the main window.
    QMainWindow* w = qobject_cast<QMainWindow*>(qApp->activeWindow());
    if(w)
        return w;

    return mainWindow();
}

void KisView2::setQtMainWindow(QMainWindow* newMainWindow)
{
    m_d->mainWindow = newMainWindow;
}

KisDoc2 *KisView2::document() const
{
    if (m_d->currentImageView && m_d->currentImageView->document()) {
        return qobject_cast<KisDoc2*>(m_d->currentImageView->document());
    }
    return 0;
}

void KisView2::slotCreateTemplate()
{
    if (!document()) return;
    KoTemplateCreateDia::createTemplate("krita_template", ".kra",
                                        KisFactory2::componentData(), document(), mainWindow());
}

void KisView2::slotDocumentSaved()
{
    m_d->saveIncremental->setEnabled(true);
    m_d->saveIncrementalBackup->setEnabled(true);
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

    emit sigSavingFinished();
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

        emit sigSavingFinished();
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

        emit sigSavingFinished();
        if (mainWindow()) mainWindow()->updateCaption();
    }
}

void KisView2::disableControls()
{
    // prevents possible crashes, if somebody changes the paintop during dragging by using the mousewheel
    // this is for Bug 250944
    // the solution blocks all wheel, mouse and key event, while dragging with the freehand tool
    // see KisToolFreehand::initPaint() and endPaint()
    m_d->controlFrame->paintopBox()->installEventFilter(&m_d->blockingEventFilter);
    foreach(QObject* child, m_d->controlFrame->paintopBox()->children()) {
        child->installEventFilter(&m_d->blockingEventFilter);
    }
}

void KisView2::enableControls()
{
    m_d->controlFrame->paintopBox()->removeEventFilter(&m_d->blockingEventFilter);
    foreach(QObject* child, m_d->controlFrame->paintopBox()->children()) {
        child->removeEventFilter(&m_d->blockingEventFilter);
    }
}

void KisView2::showStatusBar(bool toggled)
{
    if (m_d->currentImageView && m_d->currentImageView->statusBar()) {
        m_d->currentImageView->statusBar()->setVisible(toggled);
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

    KoMainWindow* main = mainWindow();

    if(!main) {
        dbgUI << "Unable to switch to canvas-only mode, main window not found";
        return;
    }

    if (cfg.hideStatusbarFullscreen()) {
        if(main->statusBar() && main->statusBar()->isVisible() == toggled) {
            main->statusBar()->setVisible(!toggled);
        }
    }

    if (cfg.hideDockersFullscreen()) {
        KToggleAction* action = qobject_cast<KToggleAction*>(main->actionCollection()->action("view_toggledockers"));
        if (action && action->isChecked() == toggled) {
            action->setChecked(!toggled);
        }
    }

    if (cfg.hideTitlebarFullscreen()) {
        if(toggled) {
            main->setWindowState( main->windowState() | Qt::WindowFullScreen);
        } else {
            main->setWindowState( main->windowState() & ~Qt::WindowFullScreen);
        }
    }

    if (cfg.hideMenuFullscreen()) {
        if (main->menuBar()->isVisible() == toggled) {
            main->menuBar()->setVisible(!toggled);
        }
    }

    if (cfg.hideToolbarFullscreen()) {
        QList<QToolBar*> toolBars = main->findChildren<QToolBar*>();
        foreach(QToolBar* toolbar, toolBars) {
            if (toolbar->isVisible() == toggled) {
                toolbar->setVisible(!toggled);
            }
        }
    }

    showHideScrollbars();

    if (toggled) {
        // show a fading heads-up display about the shortcut to go back

        showFloatingMessage(i18n("Going into Canvas-Only mode.\nPress %1 to go back.",
                                 actionCollection()->action("view_show_just_the_canvas")->shortcut().toString()), QIcon());
    }
}

void KisView2::toggleTabletLogger()
{
    if (m_d->currentImageView) {
        m_d->currentImageView->canvasBase()->toggleTabletLogger();
    }
}

void KisView2::openResourcesDirectory()
{
    QString dir = KStandardDirs::locateLocal("data", "krita");
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}

void KisView2::updateIcons()
{
#if QT_VERSION >= 0x040700
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
            KoDockWidgetTitleBar* titlebar = dynamic_cast<KoDockWidgetTitleBar*>(dock->titleBarWidget());
            if (titlebar) {
                titlebar->updateIcons();
            }
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
                    QString name = button->icon().name(); name = name.remove("dark_").remove("light_");

                    if (!blacklistedIcons.contains(name)) {
                        QString iconName = prefix + name;
                        KIcon icon = koIcon(iconName.toLatin1());
                        button->setIcon(icon);
                    }
                }
            }
        }
    }
#endif
}
void KisView2::makeStatusBarVisible()
{
    m_d->mainWindow->statusBar()->setVisible(true);
}

void KisView2::guiUpdateTimeout()
{
    m_d->nodeManager->updateGUI();
    m_d->selectionManager->updateGUI();
    m_d->filterManager->updateGUI();
    if (zoomManager()) {
        zoomManager()->updateGUI();
    }
    m_d->gridManager->updateGUI();
    m_d->perspectiveGridManager->updateGUI();
    m_d->actionManager->updateGUI();
}

void KisView2::showFloatingMessage(const QString message, const QIcon& icon, int timeout, KisFloatingMessage::Priority priority, int alignment)
{
    if(m_d->showFloatingMessage && qtMainWindow()) {
        if (m_d->savedFloatingMessage) {
            m_d->savedFloatingMessage->tryOverrideMessage(message, icon, timeout, priority, alignment);
        } else {
            m_d->savedFloatingMessage = new KisFloatingMessage(message, qtMainWindow()->centralWidget(), false, timeout, priority, alignment);
            m_d->savedFloatingMessage->setShowOverParent(true);
            m_d->savedFloatingMessage->setIcon(icon);
            m_d->savedFloatingMessage->showMessage();
        }
    }
#if QT_VERSION >= 0x040700
    emit floatingMessageRequested(message, icon.name());
#endif
}

KoMainWindow *KisView2::mainWindow() const
{
    return qobject_cast<KoMainWindow*>(m_d->mainWindow);
}


void KisView2::showHideScrollbars()
{
    if (!canvasController()) return;

    KisConfig cfg;
    bool toggled = actionCollection()->action("view_show_just_the_canvas")->isChecked();

    if ( (toggled && cfg.hideScrollbarsFullscreen()) || (!toggled && cfg.hideScrollbars()) ) {
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        dynamic_cast<KoCanvasControllerWidget*>(canvasController())->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    }
}

void KisView2::setShowFloatingMessage(bool show)
{
    m_d->showFloatingMessage = show;
}


#include "kis_view2.moc"
