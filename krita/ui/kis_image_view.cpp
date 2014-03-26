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

#include <KoToolManager.h>
#include <KoMainWindow.h>

#include <kis_image.h>
#include <kis_node.h>
#include <kis_group_layer.h>

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
    {}

    ~Private()
    {
        if (canvasController) {
            KoToolManager::instance()->removeCanvasController(canvasController);
        }

        delete zoomManager;
        delete resourceProvider;
        delete canvasController;
        delete canvas;
        delete viewConverter;
    }

    KisCoordinatesConverter *viewConverter;
    KisCanvasController *canvasController;
    KisCanvasResourceProvider *resourceProvider;
    KisCanvas2 *canvas;
    KisZoomManager *zoomManager;
    KisDoc2 *doc;
    KisView2 *parentView;

};

KisImageView::KisImageView(KoPart *part, KisDoc2 *doc, QWidget *parent)
    : KoView(part, doc, parent)
    , d(new Private())
{
    Q_ASSERT(doc);
    Q_ASSERT(doc->image());

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

    d->resourceProvider = new KisCanvasResourceProvider(this);
    d->resourceProvider->resetDisplayProfile(QApplication::desktop()->screenNumber(this));

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
    connect(d->resourceProvider, SIGNAL(sigDisplayProfileChanged(const KoColorProfile*)), d->canvas, SLOT(slotSetDisplayProfile(const KoColorProfile*)));

    d->canvasController->setCanvas(d->canvas);

    d->resourceProvider->setResourceManager(d->canvas->resourceManager());

    Q_ASSERT(d->canvasController);
    KoToolManager::instance()->addController(d->canvasController);
    KoToolManager::instance()->registerTools(actionCollection(), d->canvasController);

    connect(d->canvasController, SIGNAL(documentSizeChanged()), d->zoomManager, SLOT(slotScrollAreaSizeChanged()));
    setAcceptDrops(true);

    d->zoomManager = new KisZoomManager(this, d->viewConverter, d->canvasController);
    d->zoomManager->setup(actionCollection());

}

KisImageView::~KisImageView()
{
    delete d;
}

void KisImageView::setParentView(KisView2 *view)
{
    d->parentView = view;
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
    return d->resourceProvider;
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

    if (event->mimeData()->hasFormat("application/x-krita-node") || event->mimeData()->hasImage()) {

        KisShapeController *kritaShapeController =
            dynamic_cast<KisShapeController*>(d->doc->shapeController());

        KisNodeSP node =
            KisMimeData::loadNode(event->mimeData(), imageBounds,
                                  pasteCenter, forceRecenter,
                                  kisimage, kritaShapeController);

        if (node) {
            KisNodeCommandsAdapter adapter(d->parentView);
            if (!d->parentView->nodeManager()->activeLayer()) {
                adapter.addNode(node, kisimage->rootLayer() , 0);
            } else {
                adapter.addNode(node,
                                d->parentView->nodeManager()->activeLayer()->parent(),
                                d->parentView->nodeManager()->activeLayer());
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

