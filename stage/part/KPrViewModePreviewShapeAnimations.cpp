/* This file is part of the KDE project
   Copyright (C) 2012 Paul Mendez <paulestebanms@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrViewModePreviewShapeAnimations.h"

//Stage Headers
#include "KPrShapeManagerAnimationStrategy.h"
#include "KPrPageSelectStrategyActive.h"
#include "animations/KPrAnimationCache.h"

//Qt Headers
#include <QPainter>

//Calligra Headers
#include <KoPACanvasBase.h>
#include <KoShapeManager.h>
#include <KoPAViewBase.h>
#include <KoPADocument.h>
#include <KoPAPageBase.h>
#include <KoZoomController.h>

KPrViewModePreviewShapeAnimations::KPrViewModePreviewShapeAnimations(KoPAViewBase *view, KoPACanvasBase *canvas)
    : KoPAViewMode(view, canvas)
    , m_savedViewMode(0)
{
    connect(&m_timeLine, SIGNAL(finished()), this, SLOT(activateSavedViewMode()));
    m_timeLine.setCurveShape(QTimeLine::LinearCurve);
    m_timeLine.setUpdateInterval(20);
}

KPrViewModePreviewShapeAnimations::~KPrViewModePreviewShapeAnimations()
{
    delete m_animationCache;
}

void KPrViewModePreviewShapeAnimations::paint(KoPACanvasBase *canvas, QPainter &painter, const QRectF &paintRect)
{
    painter.setClipRect(paintRect);
    painter.setRenderHint(QPainter::Antialiasing);
    canvas->shapeManager()->paint(painter, *(view()->zoomHandler()), true);
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
    m_savedViewMode = previousViewMode;               // store the previous view mode
    m_animationCache = new KPrAnimationCache();
    m_canvas->shapeManager()->setPaintingStrategy(new KPrShapeManagerAnimationStrategy(m_canvas->shapeManager(), m_animationCache,
                                                       new KPrPageSelectStrategyActive(m_canvas)));

    // the update of the canvas is needed so that the old page gets drawn fully before the effect starts
    canvas()->repaint();

    m_timeLine.setDuration(m_shapeAnimation->duration());
    m_timeLine.setCurrentTime(0);
    m_animationCache->clear();
    //m_animationCache->setPageSize(m_view->activePage()->size());
    m_animationCache->setPageSize(view()->zoomController()->documentSize());
    qreal zoom;
    view()->zoomHandler()->zoom(&zoom, &zoom);
    m_animationCache->setZoom(zoom);
    m_shapeAnimation->init(m_animationCache, 0);
    m_animationCache->startStep(0);
    m_timeLine.start();
    connect(&m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(animate()));
}

void KPrViewModePreviewShapeAnimations::deactivate()
{
    if ( m_timeLine.state() == QTimeLine::Running ) { // there are still shape animations running
            m_timeLine.stop();
    }
    m_savedViewMode = 0;
    m_shapeAnimation->deactivate();
    m_canvas->shapeManager()->setPaintingStrategy(new KoShapeManagerPaintingStrategy(m_canvas->shapeManager()));
    delete (m_animationCache);
    disconnect(&m_timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(animate()));
}

void KPrViewModePreviewShapeAnimations::updateActivePage(KoPAPageBase *page)
{
    m_view->setActivePage(page);
}

void KPrViewModePreviewShapeAnimations::setShapeAnimation(KPrShapeAnimation *shapeAnimation)
{
    m_shapeAnimation = shapeAnimation;
    if(m_savedViewMode) {           //stop the previous playing
        activateSavedViewMode();
    }
}

void KPrViewModePreviewShapeAnimations::activateSavedViewMode()
{
    m_view->setViewMode(m_savedViewMode);
}

void KPrViewModePreviewShapeAnimations::animate()
{
    m_animationCache->next();
    m_shapeAnimation->setCurrentTime(m_timeLine.currentTime());
    canvas()->repaint();
}


