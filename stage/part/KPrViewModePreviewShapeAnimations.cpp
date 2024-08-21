/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrViewModePreviewShapeAnimations.h"

// Stage Headers
#include "KPrPageSelectStrategyActive.h"
#include "KPrShapeManagerAnimationStrategy.h"
#include "KPrView.h"
#include "animations/KPrAnimationCache.h"
// Calligra Headers
#include <KPrViewModePresentation.h>
#include <KoCanvasController.h>
#include <KoPACanvasBase.h>
#include <KoPAPageBase.h>
#include <KoPAViewBase.h>
#include <KoPageLayout.h>
#include <KoShapeManager.h>
#include <KoShapePaintingContext.h>
#include <KoZoomController.h>
// Qt Headers
#include <QPainter>

KPrViewModePreviewShapeAnimations::KPrViewModePreviewShapeAnimations(KoPAViewBase *view, KoPACanvasBase *canvas)
    : KoPAViewMode(view, canvas)
    , m_savedViewMode(nullptr)
{
    connect(&m_timeLine, &QTimeLine::finished, this, &KPrViewModePreviewShapeAnimations::activateSavedViewMode);
    m_timeLine.setEasingCurve(QEasingCurve(QEasingCurve::Linear));
    m_timeLine.setUpdateInterval(20);
}

KPrViewModePreviewShapeAnimations::~KPrViewModePreviewShapeAnimations()
{
    delete m_animationCache;
}

void KPrViewModePreviewShapeAnimations::paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect)
{
#ifdef NDEBUG
    Q_UNUSED(canvas);
#else
    Q_ASSERT(m_canvas == canvas);
#endif

    painter.translate(-m_canvas->documentOffset());
    painter.setRenderHint(QPainter::Antialiasing);
    QRect clipRect = paintRect.translated(m_canvas->documentOffset()).toRect();
    painter.setClipRect(clipRect);
    painter.translate(m_canvas->documentOrigin().x(), m_canvas->documentOrigin().y());
    KoViewConverter *converter = m_view->viewConverter(m_canvas);
    KoShapePaintingContext context;
    view()->activePage()->paintBackground(painter, *converter, context);
    canvas->shapeManager()->paint(painter, *converter, true);
}

void KPrViewModePreviewShapeAnimations::tabletEvent(QTabletEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePreviewShapeAnimations::mousePressEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePreviewShapeAnimations::mouseDoubleClickEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePreviewShapeAnimations::mouseMoveEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePreviewShapeAnimations::mouseReleaseEvent(QMouseEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePreviewShapeAnimations::shortcutOverrideEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModePreviewShapeAnimations::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModePreviewShapeAnimations::keyReleaseEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
}

void KPrViewModePreviewShapeAnimations::wheelEvent(QWheelEvent *event, const QPointF &point)
{
    Q_UNUSED(event);
    Q_UNUSED(point);
}

void KPrViewModePreviewShapeAnimations::activate(KoPAViewMode *previousViewMode)
{
    m_savedViewMode = previousViewMode; // store the previous view mode
    m_animationCache = new KPrAnimationCache();
    m_canvas->shapeManager()->setPaintingStrategy(
        new KPrShapeManagerAnimationStrategy(m_canvas->shapeManager(), m_animationCache, new KPrPageSelectStrategyActive(m_canvas)));

    // the update of the canvas is needed so that the old page gets drawn fully before the effect starts

    const KoPageLayout &layout = activePageLayout();

    QSizeF pageSize(layout.width, layout.height);

    // calculate size of union page + viewport
    QSizeF documentMinSize(view()->zoomController()->documentSize());

    // create a rect out of it with origin in tp left of page
    QRectF documentRect(QPointF((documentMinSize.width() - layout.width) * -0.5, (documentMinSize.height() - layout.height) * -0.5), documentMinSize);

    QPointF offset = -documentRect.topLeft();
    m_canvas->setDocumentOrigin(offset);
    m_view->zoomController()->setPageSize(pageSize);

    m_canvas->resourceManager()->setResource(KoCanvasResourceManager::PageSize, pageSize);
    m_canvas->repaint();

    m_timeLine.setDuration(m_shapeAnimation->duration());
    m_timeLine.setCurrentTime(0);
    m_animationCache->clear();
    m_animationCache->setPageSize(view()->zoomController()->pageSize());
    qreal zoom;
    view()->zoomHandler()->zoom(&zoom, &zoom);
    m_animationCache->setZoom(zoom);
    m_shapeAnimation->init(m_animationCache, 0);
    m_animationCache->startStep(0);
    m_timeLine.start();
    connect(&m_timeLine, &QTimeLine::valueChanged, this, &KPrViewModePreviewShapeAnimations::animate);
}

void KPrViewModePreviewShapeAnimations::deactivate()
{
    if (m_timeLine.state() == QTimeLine::Running) { // there are still shape animations running
        m_timeLine.stop();
    }
    m_savedViewMode = nullptr;
    m_shapeAnimation->deactivate();
    m_canvas->shapeManager()->setPaintingStrategy(new KoShapeManagerPaintingStrategy(m_canvas->shapeManager()));
    delete (m_animationCache);
    m_animationCache = nullptr;
    disconnect(&m_timeLine, &QTimeLine::valueChanged, this, &KPrViewModePreviewShapeAnimations::animate);
}

void KPrViewModePreviewShapeAnimations::updateActivePage(KoPAPageBase *page)
{
    m_view->setActivePage(page);
}

void KPrViewModePreviewShapeAnimations::setShapeAnimation(KPrShapeAnimation *shapeAnimation)
{
    m_shapeAnimation = shapeAnimation;
    if (m_savedViewMode) { // stop the previous playing
        activateSavedViewMode();
    }
}

void KPrViewModePreviewShapeAnimations::stopAnimation()
{
    if (m_shapeAnimation) {
        m_timeLine.stop();
    }
}

void KPrViewModePreviewShapeAnimations::activateSavedViewMode()
{
    if (KPrView *view = dynamic_cast<KPrView *>(m_view)) {
        if (m_savedViewMode == view->presentationMode()) {
            view->showNormal();
            return;
        }
    }
    m_view->setViewMode(m_savedViewMode);
}

void KPrViewModePreviewShapeAnimations::animate()
{
    m_animationCache->next();
    m_shapeAnimation->setCurrentTime(m_timeLine.currentTime());
    canvas()->repaint();
}
