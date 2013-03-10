/*
 * This file is part of the KDE project
 * 
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 * 
 */

#include "CTCanvasController.h"
#include <KoCanvasBase.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QGraphicsWidget>

#include <QDebug>

CTCanvasController::CTCanvasController(KActionCollection* actionCollection)
    : KoCanvasController(actionCollection)
    , m_canvas(0)
{
}

CTCanvasController::~CTCanvasController()
{
}

void CTCanvasController::setVastScrolling(qreal factor)
{
}

void CTCanvasController::setZoomWithWheel(bool zoom)
{
}

void CTCanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
}

void CTCanvasController::setScrollBarValue(const QPoint& value)
{
}

QPoint CTCanvasController::scrollBarValue() const
{
    return QPoint();
}

void CTCanvasController::pan(const QPoint& distance)
{
}

QPointF CTCanvasController::preferredCenter() const
{
    return QPointF();
}

void CTCanvasController::setPreferredCenter(const QPointF& viewPoint)
{
}

void CTCanvasController::recenterPreferred()
{
}

void CTCanvasController::zoomTo(const QRect& rect)
{
}

void CTCanvasController::zoomBy(const QPoint& center, qreal zoom)
{
}

void CTCanvasController::zoomOut(const QPoint& center)
{
}

void CTCanvasController::zoomIn(const QPoint& center)
{
}

void CTCanvasController::ensureVisible(KoShape* shape)
{
}

void CTCanvasController::ensureVisible(const QRectF& rect, bool smooth)
{
}

int CTCanvasController::canvasOffsetY() const
{
    return 0;
}

int CTCanvasController::canvasOffsetX() const
{
    return 0;
}

int CTCanvasController::visibleWidth() const
{
    return 0;
}

int CTCanvasController::visibleHeight() const
{
    return 0;
}

KoCanvasBase* CTCanvasController::canvas() const
{
    return m_canvas;
}

void CTCanvasController::setCanvas(KoCanvasBase* canvas)
{
    m_canvas = canvas;
    canvas->setCanvasController(this);
}

void CTCanvasController::setDrawShadow(bool drawShadow)
{
}

QSize CTCanvasController::viewportSize() const
{
    QGraphicsWidget *canvasWidget = dynamic_cast<QGraphicsWidget*>(m_canvas);
    return canvasWidget->size().toSize();
}

void CTCanvasController::scrollContentsBy(int dx, int dy)
{
}

