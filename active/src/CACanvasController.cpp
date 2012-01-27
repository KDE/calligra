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

#include <KoCanvasBase.h>
#include <KoShape.h>
#include <KoZoomController.h>
#include <KoZoomHandler.h>

#include <KDebug>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtGui/QGraphicsWidget>

CACanvasController::CACanvasController (QDeclarativeItem* parent)
    : QDeclarativeItem (parent), KoCanvasController (0), m_zoomHandler (0), m_zoomController (0),
      m_canvas (0), m_currentPoint (QPoint (0, 0)), m_documentSize (QSizeF (0, 0))
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
    m_documentSize = sz;
    emit docHeightChanged();
    emit docWidthChanged();
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
    //kDebug() << distance;
}

QPoint CACanvasController::preferredCenter() const
{
    return QPoint();
}

void CACanvasController::setPreferredCenter (const QPoint& viewPoint)
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
    //kDebug() << center << zoom;
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
    setCameraX (shape->position().x());
    setCameraY (shape->position().y());
}

void CACanvasController::ensureVisible (const QRectF& rect, bool smooth)
{
    //kDebug() << rect;
    //kDebug() << canvas()->canvasItem()->size();

    int y = rect.center().y() - height()/2;
    if (y<0) {
        y = 0;
    }
    setCameraY(y);
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
    return 0;
}

int CACanvasController::visibleHeight() const
{
    return 0;
}

KoCanvasBase* CACanvasController::canvas() const
{
    return m_canvas;
}

KoCanvasControllerProxyObject* CACanvasController::canvasControllerProxyObject()
{
    return proxyObject;
}

void CACanvasController::setCanvas (KoCanvasBase* canvas)
{
    QGraphicsWidget* widget = canvas->canvasItem();
    widget->setParentItem (this);
    canvas->setCanvasController (this);
    widget->setVisible (true);
    m_canvas = canvas;

    zoomToFit();
}

void CACanvasController::setDrawShadow (bool drawShadow)
{
    //kDebug() << drawShadow;
}

QSize CACanvasController::viewportSize() const
{
    return QSize();
}

void CACanvasController::scrollContentsBy (int dx, int dy)
{
    //kDebug() << dx << dy;
}

qreal CACanvasController::docHeight() const
{
    if (m_zoomHandler) {
        return m_documentSize.height() * m_zoomHandler->zoomFactorY();
    } else {
        return m_documentSize.height();
    }
}

qreal CACanvasController::docWidth() const
{
    if (m_zoomHandler) {
        return m_documentSize.width() * m_zoomHandler->zoomFactorX();
    } else {
        return m_documentSize.width();
    }
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
    m_currentPoint.setX (cameraX);
    emit cameraXChanged();
    centerToCamera();
}

void CACanvasController::setCameraY (int cameraY)
{
    m_currentPoint.setY (cameraY);
    emit cameraYChanged();
    centerToCamera();
}

void CACanvasController::centerToCamera()
{
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
    emit needsCanvasResize(QSizeF(width(), height()));
    emit docHeightChanged();
    emit docWidthChanged();
}

void CACanvasController::updateCanvas()
{
    emit needCanvasUpdate();
}

void CACanvasController::geometryChanged (const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (m_canvas) {
        QGraphicsWidget* widget = m_canvas->canvasItem();
        widget->setParentItem (this);
        widget->setVisible (true);
        widget->setGeometry (newGeometry);

        zoomToFit();
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

void CACanvasController::setZoomController (KoZoomController* zoomController)
{
    m_zoomController = zoomController;
    connect(m_zoomController, SIGNAL(zoomChanged(KoZoomMode::Mode,qreal)), SLOT(updateZoomValue(KoZoomMode::Mode,qreal)));
}

void CACanvasController::setZoomHandler (KoZoomHandler* zoomHandler)
{
    m_zoomHandler = zoomHandler;
}

void CACanvasController::setZoom(qreal zoom)
{
    if (m_zoomHandler) {
        m_zoomHandler->setZoom(zoom);
        emit zoomChanged();
    }
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

