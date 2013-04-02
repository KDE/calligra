/* This file is part of the KDE project
 *
 * Copyright (C) 2006, 2010 Boudewijn Rempt <boud@valdyas.org>
 * Copyright (C) Lukáš Tvrdý <lukast.dev@gmail.com>, (C) 2010
 * Copyright (C) 2011 Silvio Heinrich <plassy@web.de>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA..
 */

#include "kis_canvas2.h"

#include <QWidget>
#include <QTime>
#include <QLabel>
#include <QMouseEvent>

#include <kis_debug.h>

#include <KoUnit.h>
#include <KoShapeManager.h>
#include <KoColorProfile.h>
#include <KoColorSpaceRegistry.h>
#include <KoCanvasControllerWidget.h>
#include <KoDocument.h>
#include <KoSelection.h>

#include "kis_tool_proxy.h"
#include "kis_coordinates_converter.h"
#include "kis_prescaled_projection.h"
#include "kis_image.h"
#include "kis_doc2.h"
#include "flake/kis_shape_layer.h"
#include "kis_canvas_resource_provider.h"
#include "kis_view2.h"
#include "kis_config.h"
#include "kis_config_notifier.h"
#include "kis_qpainter_canvas.h"
#include "kis_abstract_canvas_widget_factory.h"
#include "kis_qpainter_canvas_widget_factory.h"
#include "kis_group_layer.h"
#include "flake/kis_shape_controller.h"
#include "kis_node_manager.h"
#include "kis_selection.h"
#include "kis_selection_component.h"
#include "flake/kis_shape_selection.h"
#include "kis_image_config.h"

#include "opengl/kis_opengl_canvas2.h"
#include "opengl/kis_opengl_image_textures.h"
#ifdef HAVE_OPENGL
#include <QGLFormat>
#include "opengl2/kis_gl2_canvas.h"
#endif

//Favorite resource Manager
#include <ko_favorite_resource_manager.h>
#include <kis_paintop_box.h>

#include "input/kis_input_manager.h"
#include <opengl2/kis_gl2_canvas_widget_factory.h>

class KisCanvas2::KisCanvas2Private
{

public:

    KisCanvas2Private(KoCanvasBase * parent, KisCoordinatesConverter* coordConverter, KisView2 * view)
        : coordinatesConverter(coordConverter)
        , view(view)
        , canvasWidget(0)
        , canvasItem(0)
        , shapeManager(new KoShapeManager(parent))
        , monitorProfile(0)
        , currentCanvasIsOpenGL(false)
        , toolProxy(new KisToolProxy(parent))
        , favoriteResourceManager(0)
        , vastScrolling(true) {
    }

    ~KisCanvas2Private() {
        delete favoriteResourceManager;
        delete shapeManager;
        delete toolProxy;
    }

    KisCoordinatesConverter *coordinatesConverter;
    KisView2 *view;
    KisCanvasWidgetBase *canvasWidget;
    QGraphicsObject *canvasItem;
    KoShapeManager *shapeManager;
    KoColorProfile *monitorProfile;
    KoColorConversionTransformation::Intent renderingIntent;
    KoColorConversionTransformation::ConversionFlags conversionFlags;
    bool currentCanvasIsOpenGL;
#ifdef HAVE_OPENGL
    bool useTrilinearFiltering;
#endif
    KoToolProxy *toolProxy;
    KoFavoriteResourceManager *favoriteResourceManager;

    bool vastScrolling;

    KisInputManager* inputManager;

    static KisAbstractCanvasWidgetFactory *widgetFactory;
};

KisAbstractCanvasWidgetFactory *KisCanvas2::KisCanvas2Private::widgetFactory = 0;

KisCanvas2::KisCanvas2(KisCoordinatesConverter* coordConverter, KisView2 * view, KoShapeBasedDocumentBase * sc)
    : KoCanvasBase(sc)
    , m_d(new KisCanvas2Private(this, coordConverter, view))
{
    // a bit of duplication from slotConfigChanged()
    KisConfig cfg;
    m_d->vastScrolling = cfg.vastScrolling();

    m_d->inputManager = new KisInputManager(this, m_d->toolProxy);

    KisCanvas2Private::widgetFactory = 0;
    if(cfg.useOpenGL()) {
        if(!KisCanvas2Private::widgetFactory)
            KisCanvas2Private::widgetFactory = new KisGL2CanvasWidgetFactory();
    }
    else {
        if(!KisCanvas2Private::widgetFactory)
            KisCanvas2Private::widgetFactory = new KisQPainterCanvasWidgetFactory();

    }
    m_d->renderingIntent = (KoColorConversionTransformation::Intent)cfg.renderIntent();
    createCanvas(cfg.useOpenGL());

    connect(view->canvasController()->proxyObject, SIGNAL(moveDocumentOffset(QPoint)), SLOT(documentOffsetMoved(QPoint)));
    connect(KisConfigNotifier::instance(), SIGNAL(configChanged()), SLOT(slotConfigChanged()));
    connect(this, SIGNAL(canvasDestroyed(QWidget*)), this, SLOT(slotCanvasDestroyed(QWidget*)));

    /**
     * We switch the shape manager every time vector layer or
     * shape selection is activated. Flake does not expect this
     * and connects all the signals of the global shape manager
     * to the clients in the constructor. To workaround this we
     * forward the signals of local shape managers stored in the
     * vector layers to the signals of global shape manager. So the
     * sequence of signal deliveries is the following:
     *
     * shapeLayer.m_d.canvas.m_shapeManager.selection() ->
     * shapeLayer ->
     * shapeController ->
     * globalShapeManager.selection()
     */

    KisShapeController *kritaShapeController = dynamic_cast<KisShapeController*>(sc);
    connect(kritaShapeController, SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
    connect(kritaShapeController, SIGNAL(currentLayerChanged(const KoShapeLayer*)),
            globalShapeManager()->selection(), SIGNAL(currentLayerChanged(const KoShapeLayer*)));
}

KisCanvas2::~KisCanvas2()
{
    delete m_d;
}

void KisCanvas2::setCanvasWidget(KisCanvasWidgetBase* widget)
{
    QWidget *tmp = widget->widget();
    emit canvasDestroyed(tmp);

    if(m_d->canvasWidget!=0)
        widget->setDecorations(m_d->canvasWidget->decorations());
    m_d->canvasWidget = widget;

//     tmp->setAutoFillBackground(false);
//     widget->setAttribute(Qt::WA_OpaquePaintEvent);
//     widget->setMouseTracking(true);
//     widget->setAcceptDrops(true);
    tmp->installEventFilter(m_d->inputManager);
//     widget->setAttribute(Qt::WA_AcceptTouchEvents, true);
    KoCanvasControllerWidget *controller = dynamic_cast<KoCanvasControllerWidget*>(canvasController());
    if (controller) {
        Q_ASSERT(controller->canvas() == this);
        controller->changeCanvasWidget(tmp);
    }
}

bool KisCanvas2::canvasIsOpenGL()
{
    return m_d->currentCanvasIsOpenGL;
}

void KisCanvas2::gridSize(qreal *horizontal, qreal *vertical) const
{
    Q_ASSERT(horizontal);
    Q_ASSERT(vertical);
    QTransform transform = coordinatesConverter()->imageToDocumentTransform();
    QPointF size = transform.map(QPointF(m_d->view->document()->gridData().gridX(), m_d->view->document()->gridData().gridY()));
    *horizontal = size.x();
    *vertical = size.y();
}

bool KisCanvas2::snapToGrid() const
{
    return m_d->view->document()->gridData().snapToGrid();
}

qreal KisCanvas2::rotationAngle() const
{
    return m_d->coordinatesConverter->rotationAngle();
}

void KisCanvas2::setSmoothingEnabled(bool smooth)
{
    KisQPainterCanvas *canvas = dynamic_cast<KisQPainterCanvas*>(m_d->canvasWidget);
    if (canvas) {
        canvas->setSmoothingEnabled(smooth);
    }
}

void KisCanvas2::addCommand(KUndo2Command *command)
{
    m_d->view->koDocument()->addCommand(command);
}

void KisCanvas2::startMacro(const QString &title)
{
    m_d->view->koDocument()->beginMacro(title);
}

void KisCanvas2::stopMacro()
{
    m_d->view->koDocument()->endMacro();
}

KoShapeManager* KisCanvas2::shapeManager() const
{
    if (!m_d->view) return m_d->shapeManager;
    if (!m_d->view->nodeManager()) return m_d->shapeManager;

    KisLayerSP activeLayer = m_d->view->nodeManager()->activeLayer();
    if (activeLayer && activeLayer->isEditable()) {
        KisShapeLayer * shapeLayer = dynamic_cast<KisShapeLayer*>(activeLayer.data());
        if (shapeLayer) {
            return shapeLayer->shapeManager();
        }
        KisSelectionSP selection = activeLayer->selection();
        if (selection && !selection.isNull()) {
            if (selection->hasShapeSelection()) {
                KoShapeManager* m = dynamic_cast<KisShapeSelection*>(selection->shapeSelection())->shapeManager();
                return m;
            }

        }
    }
    return m_d->shapeManager;
}

KoShapeManager * KisCanvas2::globalShapeManager() const
{
    return m_d->shapeManager;
}

void KisCanvas2::updateInputMethodInfo()
{
    // TODO call (the protected) QWidget::updateMicroFocus() on the proper canvas widget...
}

const KisCoordinatesConverter* KisCanvas2::coordinatesConverter() const
{
    return m_d->coordinatesConverter;
}

KoViewConverter* KisCanvas2::viewConverter() const
{
    return m_d->coordinatesConverter;
}

QWidget* KisCanvas2::canvasWidget()
{
    return m_d->canvasWidget->widget();
}

const QWidget* KisCanvas2::canvasWidget() const
{
    return m_d->canvasWidget->widget();
}

void KisCanvas2::setCanvasItem(QGraphicsObject *canvasItem)
{
    Q_ASSERT(!m_d->canvasItem);
    m_d->canvasItem = canvasItem;
}

QGraphicsObject *KisCanvas2::canvasItem()
{
    return m_d->canvasItem;
}

const QGraphicsObject *KisCanvas2::canvasItem() const
{
    return m_d->canvasItem;
}

KoUnit KisCanvas2::unit() const
{
    return KoUnit(KoUnit::Pixel);
}

KoToolProxy * KisCanvas2::toolProxy() const
{
    return m_d->toolProxy;
}

KisInputManager* KisCanvas2::inputManager() const
{
    return m_d->inputManager;
}

void KisCanvas2::createCanvas(bool /*useOpenGL*/)
{
    KisConfig cfg;
    const KoColorProfile *profile = m_d->view->resourceProvider()->currentDisplayProfile();
    m_d->monitorProfile = const_cast<KoColorProfile*>(profile);

    m_d->conversionFlags = KoColorConversionTransformation::HighQuality;
    if (cfg.useBlackPointCompensation()) m_d->conversionFlags |= KoColorConversionTransformation::BlackpointCompensation;
    if (!cfg.allowLCMSOptimization()) m_d->conversionFlags |= KoColorConversionTransformation::NoOptimization;
    m_d->renderingIntent = (KoColorConversionTransformation::Intent)cfg.renderIntent();

    Q_ASSERT(m_d->renderingIntent < 4);

    setCanvasWidget(m_d->widgetFactory->create(this, m_d->coordinatesConverter, m_d->view));
    m_d->canvasWidget->setMonitorProfile(m_d->monitorProfile,  m_d->renderingIntent, m_d->conversionFlags);
}

void KisCanvas2::connectCurrentImage()
{
    KisImageWSP image = m_d->view->image();

    m_d->coordinatesConverter->setImage(image);
    m_d->canvasWidget->setImage(image);

    connect(image, SIGNAL(sigImageUpdated(QRect)),
            SLOT(startUpdateCanvasProjection(QRect)),
            Qt::DirectConnection);
    connect(this, SIGNAL(sigCanvasCacheUpdated(KisUpdateInfoSP)),
            this, SLOT(updateCanvasProjection(KisUpdateInfoSP)));

    connect(image, SIGNAL(sigSizeChanged(qint32,qint32)),
            SLOT(startResizingImage(qint32,qint32)),
            Qt::DirectConnection);
    connect(this, SIGNAL(sigContinueResizeImage(qint32,qint32)),
            this, SLOT(finishResizingImage(qint32,qint32)));

    startResizingImage(image->width(), image->height());

    emit imageChanged(image);
}

void KisCanvas2::disconnectCurrentImage()
{
    m_d->coordinatesConverter->setImage(0);

//     if (m_d->currentCanvasIsOpenGL) {
// #ifdef HAVE_OPENGL
//         Q_ASSERT(m_d->openGLImageTextures);
//         m_d->openGLImageTextures->disconnect(this);
//         m_d->openGLImageTextures->disconnect(m_d->view->image());
// #else
//         qFatal("Bad use of disconnectCurrentImage(). It shouldn't have happened =(");
// #endif
//     }

    disconnect(SIGNAL(sigCanvasCacheUpdated(KisUpdateInfoSP)));

    // for sigSizeChanged()
    m_d->view->image()->disconnect(this);
}

void KisCanvas2::resetCanvas(bool /*useOpenGL*/)
{
    // we cannot reset the canvas before it's created, but this method might be called,
    // for instance when setting the monitor profile.
    if (!m_d->canvasWidget) {
        return;
    }
    m_d->canvasWidget->update();
}

void KisCanvas2::startUpdateInPatches(QRect imageRect)
{
    if (m_d->currentCanvasIsOpenGL) {
        startUpdateCanvasProjection(imageRect);
    } else {
        KisImageConfig imageConfig;
        int patchWidth = imageConfig.updatePatchWidth();
        int patchHeight = imageConfig.updatePatchHeight();

        for (int y = 0; y < imageRect.height(); y += patchHeight) {
            for (int x = 0; x < imageRect.width(); x += patchWidth) {
                QRect patchRect(x, y, patchWidth, patchHeight);
                startUpdateCanvasProjection(patchRect);
            }
        }
    }
}

void KisCanvas2::setMonitorProfile(KoColorProfile* monitorProfile,
                                   KoColorConversionTransformation::Intent renderingIntent,
                                   KoColorConversionTransformation::ConversionFlags conversionFlags)
{
    KisImageWSP image = this->image();

    Q_ASSERT(renderingIntent < 4);

    m_d->monitorProfile = monitorProfile;
    m_d->renderingIntent = renderingIntent;
    m_d->conversionFlags = conversionFlags;

    image->barrierLock();
    m_d->canvasWidget->setMonitorProfile(monitorProfile, renderingIntent, conversionFlags);
    startUpdateInPatches(image->bounds());
    image->unlock();
}

void KisCanvas2::setDisplayFilter(KisDisplayFilter *displayFilter)
{
    KisImageWSP image = this->image();

    image->barrierLock();
    m_d->canvasWidget->setDisplayFilter(displayFilter);
    startUpdateInPatches(image->bounds());
    image->unlock();

}

void KisCanvas2::startResizingImage(qint32 w, qint32 h)
{
    emit sigContinueResizeImage(w, h);

    QRect imageBounds(0, 0, w, h);
    startUpdateInPatches(imageBounds);
}

void KisCanvas2::finishResizingImage(qint32 w, qint32 h)
{
    m_d->canvasWidget->imageSizeChanged(w, h);
}

void KisCanvas2::startUpdateCanvasProjection(const QRect & rc)
{
    KisUpdateInfoSP info = m_d->canvasWidget->updateCache(rc);
    emit sigCanvasCacheUpdated(info);
}

void KisCanvas2::updateCanvasProjection(KisUpdateInfoSP info)
{
    m_d->canvasWidget->recalculateCache(info);
    QRect vRect = m_d->coordinatesConverter->viewportToWidget(info->dirtyViewportRect()).toAlignedRect();

    if(!vRect.isEmpty()) {
        m_d->canvasWidget->update(vRect);
    }
}

void KisCanvas2::updateCanvas()
{
    m_d->canvasWidget->widget()->update();
}

void KisCanvas2::updateCanvas(const QRectF& documentRect)
{
    // updateCanvas is called from tools, never from the projection
    // updates, so no need to prescale!
    QRect widgetRect = m_d->coordinatesConverter->documentToWidget(documentRect).toAlignedRect();
    widgetRect.adjust(-2, -2, 2, 2);

    if (!widgetRect.isEmpty()) {
        emit updateCanvasRequested(widgetRect);
        m_d->canvasWidget->widget()->update(widgetRect);
    }
}

void KisCanvas2::disconnectCanvasObserver(QObject *object)
{
    KoCanvasBase::disconnectCanvasObserver(object);
    m_d->view->disconnect(object);
}

void KisCanvas2::notifyZoomChanged()
{
    m_d->canvasWidget->notifyZoomChanged();
    updateCanvas(); // update the canvas, because that isn't done when zooming using KoZoomAction
}

void KisCanvas2::preScale()
{
    m_d->canvasWidget->preScale();
}

KoColorProfile *  KisCanvas2::monitorProfile()
{
    return m_d->monitorProfile;
}

KisView2* KisCanvas2::view()
{
    return m_d->view;
}

KisImageWSP KisCanvas2::image()
{
    return m_d->view->image();

}

KisImageWSP KisCanvas2::currentImage()
{
    return m_d->view->image();
}

void KisCanvas2::setCanvasWidgetFactory(KisAbstractCanvasWidgetFactory* factory)
{
    KisCanvas2Private::widgetFactory = factory;
}

void KisCanvas2::documentOffsetMoved(const QPoint &documentOffset)
{
    QPointF offsetBefore = m_d->coordinatesConverter->imageRectInViewportPixels().topLeft();
    m_d->coordinatesConverter->setDocumentOffset(documentOffset);
    QPointF offsetAfter = m_d->coordinatesConverter->imageRectInViewportPixels().topLeft();

    QPointF moveOffset = offsetAfter - offsetBefore;

    m_d->canvasWidget->viewportMoved(moveOffset);

    updateCanvas();
}

void KisCanvas2::slotConfigChanged()
{
    KisConfig cfg;
    m_d->vastScrolling = cfg.vastScrolling();

    // first, assume we're going to crash when switching to opengl
    bool useOpenGL = cfg.useOpenGL();
    if (cfg.canvasState() == "TRY_OPENGL" && useOpenGL) {
        cfg.setCanvasState("OPENGL_FAILED");
    }
    resetCanvas(useOpenGL);
    if (useOpenGL) {
        cfg.setCanvasState("OPENGL_SUCCESS");
    }
}

void KisCanvas2::slotSetDisplayProfile(const KoColorProfile * profile)
{
    KisConfig cfg;
    KoColorConversionTransformation::Intent renderingIntent = (KoColorConversionTransformation::Intent)cfg.renderIntent();
    KoColorConversionTransformation::ConversionFlags conversionFlags = KoColorConversionTransformation::HighQuality;

    if (cfg.useBlackPointCompensation()) conversionFlags |= KoColorConversionTransformation::BlackpointCompensation;
    if (!cfg.allowLCMSOptimization()) conversionFlags |= KoColorConversionTransformation::NoOptimization;

    setMonitorProfile(const_cast<KoColorProfile*>(profile), renderingIntent, conversionFlags);
}

void KisCanvas2::addDecoration(KisCanvasDecoration* deco)
{
    m_d->canvasWidget->addDecoration(deco);
}

KisCanvasDecoration* KisCanvas2::decoration(const QString& id)
{
    return m_d->canvasWidget->decoration(id);
}


QPoint KisCanvas2::documentOrigin() const
{
    return QPoint();
}

QPoint KisCanvas2::documentOffset() const
{
    return m_d->coordinatesConverter->documentOffset();
}

void KisCanvas2::createFavoriteResourceManager(KisPaintopBox* paintopbox)
{
    m_d->favoriteResourceManager = new KoFavoriteResourceManager(paintopbox, canvasWidget());
    connect(this, SIGNAL(favoritePaletteCalled(QPoint)), favoriteResourceManager(), SLOT(slotShowPopupPalette(QPoint)));
    connect(view()->resourceProvider(), SIGNAL(sigFGColorUsed(KoColor)), favoriteResourceManager(), SLOT(slotAddRecentColor(KoColor)));
    connect(view()->resourceProvider(), SIGNAL(sigFGColorChanged(KoColor)), favoriteResourceManager(), SLOT(slotChangeFGColorSelector(KoColor)));
    connect(favoriteResourceManager(), SIGNAL(sigSetFGColor(KoColor)), view()->resourceProvider(), SLOT(slotSetFGColor(KoColor)));
    connect(favoriteResourceManager(), SIGNAL(sigEnableChangeColor(bool)), view()->resourceProvider(), SLOT(slotResetEnableFGChange(bool)));
}

void KisCanvas2::slotCanvasDestroyed(QWidget* w)
{
    if (m_d->favoriteResourceManager != 0)
    {
        m_d->favoriteResourceManager->resetPopupPaletteParent(w);
    }
}

KoFavoriteResourceManager* KisCanvas2::favoriteResourceManager()
{
    return m_d->favoriteResourceManager;
}


bool KisCanvas2::handlePopupPaletteIsVisible()
{
    if (favoriteResourceManager()
            && favoriteResourceManager()->isPopupPaletteVisible()) {

        favoriteResourceManager()->slotShowPopupPalette();
        return true;
    }
    return false;
}

void KisCanvas2::setCursor(const QCursor &cursor)
{
    canvasWidget()->setCursor(cursor);
}

void KisCanvas2::slotSelectionChanged()
{
    KisShapeLayer* shapeLayer = dynamic_cast<KisShapeLayer*>(view()->activeLayer().data());
    if (!shapeLayer) {
        return;
    }
    m_d->shapeManager->selection()->deselectAll();
    foreach(KoShape* shape, shapeLayer->shapeManager()->selection()->selectedShapes()) {
        m_d->shapeManager->selection()->select(shape);
    }
}


#include "kis_canvas2.moc"
