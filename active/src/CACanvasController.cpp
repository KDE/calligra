/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2010-2011 Jarosław Staniek <staniek@kde.org>
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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
 */


#include "CACanvasController.h"
#include "CACanvasItem.h"
#include "CADocumentController.h"
#include "CAAbstractDocumentHandler.h"

#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include <KDE/KActionCollection>
#include <KDebug>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QGraphicsWidget>

#include <cmath>

CACanvasController::CACanvasController (QDeclarativeItem* parent)
    : QDeclarativeItem (parent), KoCanvasController (0), m_zoomHandler (0), m_zoomController (0),
      m_caDocumentHandler(0), m_currentPoint (QPoint (0, 0)), m_documentSize (QSizeF (0, 0))
{
    setFlag (QGraphicsItem::ItemHasNoContents, false);
    setClip (true);
}

void CACanvasController::setVastScrolling (qreal factor)
{
    //kDebug() << factor;
}

void CACanvasController::setZoomWithWheel (bool zoom)
{
    //kDebug() << zoom;
}

void CACanvasController::updateDocumentSize (const QSize& sz, bool recalculateCenter)
{
    m_caCanvasItem->updateDocumentSize(sz, recalculateCenter);
    KoCanvasController::setDocumentSize(sz);
}

void CACanvasController::setScrollBarValue (const QPoint& value)
{
    //kDebug() << value;
}

QPoint CACanvasController::scrollBarValue() const
{
    return QPoint();
}

void CACanvasController::pan (const QPoint& distance)
{
    setCameraX(cameraX() + distance.x());
    setCameraY(cameraY() + distance.y());
}

QPointF CACanvasController::preferredCenter() const
{
    //TODO: is this the correct value?
    return QPointF(width()/2, height()/2);
}

void CACanvasController::setPreferredCenter (const QPointF& viewPoint)
{
    //kDebug() << viewPoint;
}

void CACanvasController::recenterPreferred()
{
}

void CACanvasController::zoomTo (const QRect& rect)
{
    //kDebug() << rect;
}

void CACanvasController::zoomBy (const QPoint& center, qreal zoom)
{
    proxyObject->emitZoomRelative(zoom, center);
    canvas()->canvasItem()->update();
}

void CACanvasController::zoomOut (const QPoint& center)
{
    //kDebug() << center;
}

void CACanvasController::zoomIn (const QPoint& center)
{
    //kDebug() << center;
}

void CACanvasController::ensureVisible (KoShape* shape)
{
    ensureVisible(shape->boundingRect(), true);
}

void CACanvasController::ensureVisible (const QRectF& rect, bool smooth)
{
    setCameraX(rect.x());
    setCameraY(rect.y());
}

int CACanvasController::canvasOffsetY() const
{
    return 0;
}

int CACanvasController::canvasOffsetX() const
{
    return 0;
}

int CACanvasController::visibleWidth() const
{
    if (m_caCanvasItem) return m_caCanvasItem->width();
    return 0;
}

int CACanvasController::visibleHeight() const
{
    if (m_caCanvasItem) return m_caCanvasItem->height();
    return 0;
}

KoCanvasBase* CACanvasController::canvas() const
{
    return m_caCanvasItem->koCanvas();
}

KoCanvasControllerProxyObject* CACanvasController::canvasControllerProxyObject()
{
    return proxyObject;
}

QObject* CACanvasController::caCanvasItem()
{
    return dynamic_cast<QObject*>(m_caCanvasItem);
}

void CACanvasController::setCACanvasItem(QObject* caCanvas)
{
    m_caCanvasItem = static_cast<CACanvasItem*>(caCanvas);
}

void CACanvasController::setCanvas (KoCanvasBase* canvas)
{
    canvas->setCanvasController(this);
    m_caCanvasItem->setKoCanvas(canvas);
    emit caCanvasItemChanged();
}

void CACanvasController::setDrawShadow (bool drawShadow)
{
    //kDebug() << drawShadow;
}

QSize CACanvasController::viewportSize() const
{
    return QSize(width(), height());
}

void CACanvasController::scrollContentsBy (int dx, int dy)
{
    //kDebug() << dx << dy;
}

int CACanvasController::cameraX() const
{
    return m_currentPoint.x();
}

int CACanvasController::cameraY() const
{
    return m_currentPoint.y();
}

void CACanvasController::setCameraX (int cameraX)
{
    cameraX -= std::min(m_caCanvasItem->width()/2, width()/2);
    if (m_caDocumentHandler && m_caDocumentHandler->preferredZoomMode() == KoZoomMode::ZOOM_WIDTH) {
        cameraX = 0;
    }
    m_currentPoint.setX (cameraX);
    emit cameraXChanged();
    centerToCamera();
}

void CACanvasController::setCameraY (int cameraY)
{
    //cameraY -= std::min(m_caCanvasItem->height()/2, height()/2);
    if (m_caDocumentHandler && m_caDocumentHandler->preferredZoomMode() == KoZoomMode::ZOOM_PAGE) {
        cameraY = 0;
    }
    m_currentPoint.setY (cameraY);
    emit cameraYChanged();
    centerToCamera();
}

void CACanvasController::centerToCamera()
{
    return;
    if (proxyObject) {
        proxyObject->emitMoveDocumentOffset (m_currentPoint);
    }
    updateCanvas();
}

CACanvasController::~CACanvasController()
{
}

void CACanvasController::zoomToFit()
{
}

void CACanvasController::updateCanvas()
{
    emit needCanvasUpdate();
}

void CACanvasController::setDocumentHandler(CAAbstractDocumentHandler* documentHandler)
{
    m_caDocumentHandler = documentHandler;
}

void CACanvasController::geometryChanged (const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_caCanvasItem->koCanvas() && zoomController() && m_caDocumentHandler) {
        zoomController()->setZoomMode(m_caDocumentHandler->preferredZoomMode());
        emit proxyObject->emitSizeChanged(newGeometry.size().toSize());
        updateCanvas();
    }
    QDeclarativeItem::geometryChanged (newGeometry, oldGeometry);
}

KoZoomController* CACanvasController::zoomController()
{
    return m_zoomController;
}

KoZoomHandler* CACanvasController::zoomHandler()
{
    return m_zoomHandler;
}

void CACanvasController::setZoomHandler (KoZoomHandler* zoomHandler)
{
    if (!m_zoomController) {
        m_zoomHandler = zoomHandler;
        m_zoomController = new KoZoomController(this, zoomHandler, new KActionCollection(this));
        zoomController()->setZoom(m_caDocumentHandler->preferredZoomMode(), 1);
    }
}

void CACanvasController::setZoom(qreal zoom)
{
}

qreal CACanvasController::zoom() const
{
    return m_zoom;
}

void CACanvasController::updateZoomValue(KoZoomMode::Mode mode, qreal zoom)
{
    m_zoom = zoom;
}

#include "CACanvasController.moc"

