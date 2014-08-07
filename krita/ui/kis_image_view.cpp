/*
 *  Copyright (c) 2014 Boudewijn Rempt <boud@kde.org>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "kis_image_view.h"

#include <QApplication>
#include <QDesktopWidget>

#include <kmenu.h>
#include <kactioncollection.h>

#include <KoToolManager.h>
#include <KoMainWindow.h>

#include <kis_image.h>
#include <kis_node.h>
#include <kis_group_layer.h>
#include <kis_layer.h>
#include <kis_mask.h>
#include <kis_selection.h>

#include "kis_image_manager.h"
#include "kis_node_manager.h"
#include "kis_view2.h"
#include "kis_shape_controller.h"
#include "kis_mimedata.h"
#include "kis_node_commands_adapter.h"
#include "kis_canvas2.h"
#include "kis_doc2.h"
#include "kis_zoom_manager.h"
#include "kis_canvas_resource_provider.h"
#include "kis_canvas_controller.h"
#include "kis_config.h"
#include "kis_mirror_axis.h"
#include "kis_tool_freehand.h"

#include "krita/gemini/ViewModeSwitchEvent.h"

class KisImageView::Private
{

public:
    Private()
        : viewConverter(0)
        , canvasController(0)
        , canvas(0)
        , zoomManager(0)
        , doc(0)
        , parentView(0)
        , mirrorAxis(0)
    {}

    ~Private()
    {
        if (canvasController) {
            KoToolManager::instance()->removeCanvasController(canvasController);
        }

        delete zoomManager;
        delete canvasController;
        delete canvas;
        delete viewConverter;
        delete mirrorAxis;
    }

    KisCoordinatesConverter *viewConverter;
    KisCanvasController *canvasController;
    KisCanvas2 *canvas;
    KisZoomManager *zoomManager;
    KisDoc2 *doc;
    KisView2 *parentView;

    KisNodeSP currentNode;
    KisLayerSP currentLayer;
    KisMaskSP currentMask;

    KisMirrorAxis* mirrorAxis;
};

KisImageView::KisImageView(KoPart *part, KisDoc2 *doc, QWidget *parent)
    : KoView(part, doc, parent)
    , d(new Private())
{
    Q_ASSERT(doc);
    Q_ASSERT(doc->image());

    qDebug() << this << "Creating View for doc" << doc;

    setFocusPolicy(Qt::NoFocus);

    d->doc = doc;
    d->viewConverter = new KisCoordinatesConverter();

    KisConfig cfg;

    d->canvasController = new KisCanvasController(this, actionCollection());
    d->canvasController->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->canvasController->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->canvasController->setDrawShadow(false);
    d->canvasController->setCanvasMode(KoCanvasController::Infinite);
    d->canvasController->setVastScrolling(cfg.vastScrolling());

    KConfigGroup grp(KGlobal::config(), "krita/crashprevention");
    if (grp.readEntry("CreatingCanvas", false)) {
        cfg.setUseOpenGL(false);
    }
    if (cfg.canvasState() == "OPENGL_FAILED") {
        cfg.setUseOpenGL(false);
    }
    grp.writeEntry("CreatingCanvas", true);
    grp.sync();
    d->canvas = new KisCanvas2(d->viewConverter, this, doc->shapeController());
    grp.writeEntry("CreatingCanvas", false);
    grp.sync();

    d->canvasController->setCanvas(d->canvas);

    Q_ASSERT(d->canvasController);
    KoToolManager::instance()->addController(d->canvasController);

    d->zoomManager = new KisZoomManager(this, d->viewConverter, d->canvasController);
    d->zoomManager->setup(actionCollection());

    connect(d->canvasController, SIGNAL(documentSizeChanged()), d->zoomManager, SLOT(slotScrollAreaSizeChanged()));
    setAcceptDrops(true);

    connect(d->doc, SIGNAL(sigLoadingFinished()), this, SLOT(slotLoadingFinished()));
    if (!d->doc->isLoading() || d->doc->image()) {
        slotLoadingFinished();
    }
}

KisImageView::~KisImageView()
{
    qDebug() << "deleting image view" << this;
    delete d;
}

void KisImageView::setParentView(KisView2 *view)
{
    d->parentView = view;
    view->resourceProvider()->resetDisplayProfile(QApplication::desktop()->screenNumber(this));
    connect(view->resourceProvider(), SIGNAL(sigDisplayProfileChanged(const KoColorProfile*)), d->canvas, SLOT(slotSetDisplayProfile(const KoColorProfile*)));

}

KisView2* KisImageView::parentView() const
{
    return d->parentView;
}

KoZoomController *KisImageView::zoomController() const
{
    return d->zoomManager->zoomController();
}

KisZoomManager *KisImageView::zoomManager() const
{
    return d->zoomManager;
}

KoCanvasController *KisImageView::canvasController() const
{
    return d->canvasController;
}

KisCanvasResourceProvider *KisImageView::resourceProvider() const
{
    if (d->parentView) {
        return d->parentView->resourceProvider();
    }
    return 0;
}

KisCanvas2 *KisImageView::canvasBase() const
{
    return d->canvas;
}

KisImageWSP KisImageView::image() const
{
    if (d->doc) {
        return d->doc->image();
    }
    return 0;
}

KisCoordinatesConverter *KisImageView::viewConverter() const
{
    return d->viewConverter;
}

void KisImageView::dragEnterEvent(QDragEnterEvent *event)
{
    dbgUI << "KisView2::dragEnterEvent";
    // Only accept drag if we're not busy, particularly as we may
    // be showing a progress bar and calling qApp->processEvents().
    if (event->mimeData()->hasImage()
            || event->mimeData()->hasUrls()
            || event->mimeData()->hasFormat("application/x-krita-node")) {
        event->accept();
    } else {
        event->ignore();
    }
}

void KisImageView::dropEvent(QDropEvent *event)
{
    KisImageSP kisimage = image();
    Q_ASSERT(kisimage);

    QPoint cursorPos = canvasBase()->coordinatesConverter()->widgetToImage(event->pos()).toPoint();
    QRect imageBounds = kisimage->bounds();
    QPoint pasteCenter;
    bool forceRecenter;

    if (event->keyboardModifiers() & Qt::ShiftModifier &&
        imageBounds.contains(cursorPos)) {

        pasteCenter = cursorPos;
        forceRecenter = true;
    } else {
        pasteCenter = imageBounds.center();
        forceRecenter = false;
    }
    if (event->mimeData()->hasFormat("application/x-krita-node") ||
        event->mimeData()->hasImage())
    {
        KisShapeController *kritaShapeController =
            dynamic_cast<KisShapeController*>(d->doc->shapeController());

        QList<KisNodeSP> nodes =
            KisMimeData::loadNodes(event->mimeData(), imageBounds,
                                  pasteCenter, forceRecenter,
                                  kisimage, kritaShapeController);

        foreach(KisNodeSP node, nodes) {
            if (node) {
                KisNodeCommandsAdapter adapter(parentView());
                if (!parentView()->nodeManager()->activeLayer()) {
                    adapter.addNode(node, kisimage->rootLayer() , 0);
                } else {
                    adapter.addNode(node,
                                    parentView()->nodeManager()->activeLayer()->parent(),
                                    parentView()->nodeManager()->activeLayer());
                }
            }
        }
    }
    else if (event->mimeData()->hasUrls()) {

        QList<QUrl> urls = event->mimeData()->urls();
        if (urls.length() > 0) {

            KMenu popup;
            popup.setObjectName("drop_popup");

            QAction *insertAsNewLayer = new KAction(i18n("Insert as New Layer"), &popup);
            QAction *insertManyLayers = new KAction(i18n("Insert Many Layers"), &popup);

            QAction *openInNewDocument = new KAction(i18n("Open in New Document"), &popup);
            QAction *openManyDocuments = new KAction(i18n("Open Many Documents"), &popup);

            QAction *replaceCurrentDocument = new KAction(i18n("Replace Current Document"), &popup);

            QAction *cancel = new KAction(i18n("Cancel"), &popup);

            popup.addAction(insertAsNewLayer);
            popup.addAction(openInNewDocument);
            popup.addAction(replaceCurrentDocument);
            popup.addAction(insertManyLayers);
            popup.addAction(openManyDocuments);

            insertAsNewLayer->setEnabled(image() && urls.count() == 1);
            openInNewDocument->setEnabled(urls.count() == 1);
            replaceCurrentDocument->setEnabled(image() && urls.count() == 1);
            insertManyLayers->setEnabled(image() && urls.count() > 1);
            openManyDocuments->setEnabled(urls.count() > 1);

            popup.addSeparator();
            popup.addAction(cancel);

            QAction *action = popup.exec(QCursor::pos());

            if (action != 0 && action != cancel) {
                foreach(const QUrl &url, urls) {

                    if (action == insertAsNewLayer || action == insertManyLayers) {
                        d->parentView->imageManager()->importImage(KUrl(url));
                        activateWindow();
                    }
                    else if (action == replaceCurrentDocument) {
                        if (d->doc->isModified()) {
                            d->doc->save();
                        }
                        if (mainWindow() != 0) {
                            /**
                             * NOTE: this is effectively deferred self-destruction
                             */
                            connect(mainWindow(), SIGNAL(loadCompleted()),
                                    mainWindow(), SLOT(close()));

                            mainWindow()->openDocument(url);
                        }
                    } else {
                        Q_ASSERT(action == openInNewDocument || action == openManyDocuments);
                        if (mainWindow()) {
                            mainWindow()->openDocument(url);
                        }
                    }
                }
            }
        }
    }
}

bool KisImageView::event(QEvent *event)
{
    switch(static_cast<int>(event->type()))
    {
        case ViewModeSwitchEvent::AboutToSwitchViewModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            parentView()->canvasControllerWidget()->setFocus();
            qApp->processEvents();

            KisCanvasResourceProvider* provider = resourceProvider();
            syncObject->backgroundColor = provider->bgColor();
            syncObject->foregroundColor = provider->fgColor();
            syncObject->exposure = provider->HDRExposure();
            syncObject->gamma = provider->HDRGamma();
            syncObject->compositeOp = provider->currentCompositeOp();
            syncObject->pattern = provider->currentPattern();
            syncObject->gradient = provider->currentGradient();
            syncObject->node = provider->currentNode();
            syncObject->paintOp = provider->currentPreset();
            syncObject->opacity = provider->opacity();
            syncObject->globalAlphaLock = provider->globalAlphaLock();

            syncObject->documentOffset = parentView()->canvasControllerWidget()->scrollBarValue() - pos();
            syncObject->zoomLevel = zoomController()->zoomAction()->effectiveZoom();
            syncObject->rotationAngle = canvasBase()->rotationAngle();

            syncObject->activeToolId = KoToolManager::instance()->activeToolId();

            syncObject->gridData = &document()->gridData();

            syncObject->mirrorHorizontal = provider->mirrorHorizontal();
            syncObject->mirrorVertical = provider->mirrorVertical();
            syncObject->mirrorAxesCenter = provider->resourceManager()->resource(KisCanvasResourceProvider::MirrorAxesCenter).toPointF();

            KisToolFreehand* tool = qobject_cast<KisToolFreehand*>(KoToolManager::instance()->toolById(canvasBase(), syncObject->activeToolId));
            if(tool) {
                syncObject->smoothingOptions = tool->smoothingOptions();
            }

            syncObject->initialized = true;

            QMainWindow* mainWindow = qobject_cast<QMainWindow*>(qApp->activeWindow());
            if(mainWindow) {
                QList<QDockWidget*> dockWidgets = mainWindow->findChildren<QDockWidget*>();
                foreach(QDockWidget* widget, dockWidgets) {
                    if (widget->isFloating()) {
                        widget->hide();
                    }
                }
            }

            return true;
        }
        case ViewModeSwitchEvent::SwitchedToDesktopModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();
            parentView()->canvasControllerWidget()->setFocus();
            qApp->processEvents();

            if(syncObject->initialized) {
                KisCanvasResourceProvider* provider = resourceProvider();

                provider->resourceManager()->setResource(KisCanvasResourceProvider::MirrorAxesCenter, syncObject->mirrorAxesCenter);
                if (provider->mirrorHorizontal() != syncObject->mirrorHorizontal) {
                    QAction* mirrorAction = actionCollection()->action("hmirror_action");
                    mirrorAction->setChecked(syncObject->mirrorHorizontal);
                    provider->setMirrorHorizontal(syncObject->mirrorHorizontal);
                }
                if (provider->mirrorVertical() != syncObject->mirrorVertical) {
                    QAction* mirrorAction = actionCollection()->action("vmirror_action");
                    mirrorAction->setChecked(syncObject->mirrorVertical);
                    provider->setMirrorVertical(syncObject->mirrorVertical);
                }

                provider->setPaintOpPreset(syncObject->paintOp);
                qApp->processEvents();

                KoToolManager::instance()->switchToolRequested(syncObject->activeToolId);
                qApp->processEvents();

                KisPaintOpPresetSP preset = canvasBase()->resourceManager()->resource(KisCanvasResourceProvider::CurrentPaintOpPreset).value<KisPaintOpPresetSP>();
                preset->settings()->setProperty("CompositeOp", syncObject->compositeOp);
                if(preset->settings()->hasProperty("OpacityValue"))
                    preset->settings()->setProperty("OpacityValue", syncObject->opacity);
                provider->setPaintOpPreset(preset);

                provider->setBGColor(syncObject->backgroundColor);
                provider->setFGColor(syncObject->foregroundColor);
                provider->setHDRExposure(syncObject->exposure);
                provider->setHDRGamma(syncObject->gamma);
                provider->slotPatternActivated(syncObject->pattern);
                provider->slotGradientActivated(syncObject->gradient);
                provider->slotNodeActivated(syncObject->node);
                provider->setOpacity(syncObject->opacity);
                provider->setGlobalAlphaLock(syncObject->globalAlphaLock);
                provider->setCurrentCompositeOp(syncObject->compositeOp);

                document()->gridData().setGrid(syncObject->gridData->gridX(), syncObject->gridData->gridY());
                document()->gridData().setGridColor(syncObject->gridData->gridColor());
                document()->gridData().setPaintGridInBackground(syncObject->gridData->paintGridInBackground());
                document()->gridData().setShowGrid(syncObject->gridData->showGrid());
                document()->gridData().setSnapToGrid(syncObject->gridData->snapToGrid());

                actionCollection()->action("zoom_in")->trigger();
                qApp->processEvents();


                QMainWindow* mainWindow = qobject_cast<QMainWindow*>(qApp->activeWindow());
                if(mainWindow) {
                    QList<QDockWidget*> dockWidgets = mainWindow->findChildren<QDockWidget*>();
                    foreach(QDockWidget* widget, dockWidgets) {
                        if (widget->isFloating()) {
                            widget->show();
                        }
                    }
                }

                zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, syncObject->zoomLevel);
                parentView()->canvasControllerWidget()->rotateCanvas(syncObject->rotationAngle - canvasBase()->rotationAngle());

                QPoint newOffset = syncObject->documentOffset + pos();
                qApp->processEvents();
                parentView()->canvasControllerWidget()->setScrollBarValue(newOffset);

                KisToolFreehand* tool = qobject_cast<KisToolFreehand*>(KoToolManager::instance()->toolById(canvasBase(), syncObject->activeToolId));
                if(tool && syncObject->smoothingOptions) {
                    tool->smoothingOptions()->setSmoothingType(syncObject->smoothingOptions->smoothingType());
                    tool->smoothingOptions()->setSmoothPressure(syncObject->smoothingOptions->smoothPressure());
                    tool->smoothingOptions()->setTailAggressiveness(syncObject->smoothingOptions->tailAggressiveness());
                    tool->smoothingOptions()->setUseScalableDistance(syncObject->smoothingOptions->useScalableDistance());
                    tool->smoothingOptions()->setSmoothnessDistance(syncObject->smoothingOptions->smoothnessDistance());
                    tool->smoothingOptions()->setUseDelayDistance(syncObject->smoothingOptions->useDelayDistance());
                    tool->smoothingOptions()->setDelayDistance(syncObject->smoothingOptions->delayDistance());
                    tool->smoothingOptions()->setFinishStabilizedCurve(syncObject->smoothingOptions->finishStabilizedCurve());
                    tool->smoothingOptions()->setStabilizeSensors(syncObject->smoothingOptions->stabilizeSensors());
                    tool->updateSettingsViews();
                }
            }

            return true;
        }
        default:
            break;
    }

    return QWidget::event( event );
}

void KisImageView::resetImageSizeAndScroll(bool changeCentering,
                                       const QPointF oldImageStillPoint,
                                       const QPointF newImageStillPoint)
{
    const KisCoordinatesConverter *converter = d->canvas->coordinatesConverter();

    QPointF oldPreferredCenter = d->canvasController->preferredCenter();

    /**
     * Calculating the still point in old coordinates depending on the
     * parameters given
     */

    QPointF oldStillPoint;

    if (changeCentering) {
        oldStillPoint =
            converter->imageToWidget(oldImageStillPoint) +
            converter->documentOffset();
    } else {
        QSize oldDocumentSize = d->canvasController->documentSize();
        oldStillPoint = QPointF(0.5 * oldDocumentSize.width(), 0.5 * oldDocumentSize.height());
    }

    /**
     * Updating the document size
     */

    QSizeF size(image()->width() / image()->xRes(), image()->height() / image()->yRes());
    KoZoomController *zc = d->zoomManager->zoomController();
    zc->setZoom(KoZoomMode::ZOOM_CONSTANT, zc->zoomAction()->effectiveZoom());
    zc->setPageSize(size);
    zc->setDocumentSize(size, true);

    /**
     * Calculating the still point in new coordinates depending on the
     * parameters given
     */

    QPointF newStillPoint;

    if (changeCentering) {
        newStillPoint =
            converter->imageToWidget(newImageStillPoint) +
            converter->documentOffset();
    } else {
        QSize newDocumentSize = d->canvasController->documentSize();
        newStillPoint = QPointF(0.5 * newDocumentSize.width(), 0.5 * newDocumentSize.height());
    }

    d->canvasController->setPreferredCenter(oldPreferredCenter - oldStillPoint + newStillPoint);
}

void KisImageView::setCurrentNode(KisNodeSP node)
{
    d->currentNode = node;
}

KisNodeSP KisImageView::currentNode() const
{
    return d->currentNode;
}

void KisImageView::setCurrentLayer(KisLayerSP layer)
{
    d->currentLayer = layer;
}

KisLayerSP KisImageView::currentLayer() const
{
    return d->currentLayer;
}

void KisImageView::setCurrentMask(KisMaskSP mask)
{
    d->currentMask = mask;
}

KisMaskSP KisImageView::currentMask() const
{
   return d->currentMask;
}

KisSelectionSP KisImageView::selection()
{
    KisLayerSP layer = currentLayer();
    if (layer)
        return layer->selection(); // falls through to the global
    // selection, or 0 in the end
    if (image()) {
        return image()->globalSelection();
    }
    return 0;
}

void KisImageView::slotLoadingFinished()
{
    if (!document()) return;

    canvasBase()->initializeImage();

    /**
     * Dirty hack alert
     */
    d->zoomManager->zoomController()->setAspectMode(true);

    if (viewConverter()) {
       viewConverter()->setZoomMode(KoZoomMode::ZOOM_PAGE);
    }
    connect(image(), SIGNAL(sigColorSpaceChanged(const KoColorSpace*)),
            this, SIGNAL(sigColorSpaceChanged(const KoColorSpace*)));
    connect(image(), SIGNAL(sigProfileChanged(const KoColorProfile*)),
            this, SIGNAL(sigProfileChanged(const KoColorProfile*)));
    connect(image(), SIGNAL(sigSizeChanged(QPointF,QPointF)),
            this, SIGNAL(sigSizeChanged(QPointF,QPointF)));


    KisDoc2* doc = static_cast<KisDoc2*>(document());
    KisNodeSP activeNode = doc->preActivatedNode();
    doc->setPreActivatedNode(0); // to make sure that we don't keep a reference to a layer the user can later delete.

    if (!activeNode) {
        activeNode = image()->rootLayer()->lastChild();
    }

    while (activeNode && !activeNode->inherits("KisLayer")) {
        activeNode = activeNode->prevSibling();
    }

    setCurrentNode(activeNode);
}
