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

#include "CQCanvasController.h"
#include <KoCanvasBase.h>

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QGraphicsWidget>

#include <QDebug>

CQCanvasController::CQCanvasController(KActionCollection* actionCollection)
    : KoCanvasController(actionCollection)
    , m_canvas(0)
{
}

CQCanvasController::~CQCanvasController()
{
}

void CQCanvasController::setVastScrolling(qreal factor)
{
}

void CQCanvasController::setZoomWithWheel(bool zoom)
{
}

void CQCanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
}

void CQCanvasController::setScrollBarValue(const QPoint& value)
{
}

QPoint CQCanvasController::scrollBarValue() const
{
    return QPoint();
}

void CQCanvasController::pan(const QPoint& distance)
{
}

QPointF CQCanvasController::preferredCenter() const
{
    return QPointF();
}

void CQCanvasController::setPreferredCenter(const QPointF& viewPoint)
{
}

void CQCanvasController::recenterPreferred()
{
}

void CQCanvasController::zoomTo(const QRect& rect)
{
}

void CQCanvasController::zoomBy(const QPoint& center, qreal zoom)
{
}

void CQCanvasController::zoomOut(const QPoint& center)
{
}

void CQCanvasController::zoomIn(const QPoint& center)
{
}

void CQCanvasController::ensureVisible(KoShape* shape)
{
}

void CQCanvasController::ensureVisible(const QRectF& rect, bool smooth)
{
}

int CQCanvasController::canvasOffsetY() const
{
    return 0;
}

int CQCanvasController::canvasOffsetX() const
{
    return 0;
}

int CQCanvasController::visibleWidth() const
{
    return 0;
}

int CQCanvasController::visibleHeight() const
{
    return 0;
}

KoCanvasBase* CQCanvasController::canvas() const
{
    return m_canvas;
}

void CQCanvasController::setCanvas(KoCanvasBase* canvas)
{
    m_canvas = canvas;
    canvas->setCanvasController(this);
}

void CQCanvasController::setDrawShadow(bool drawShadow)
{
}

QSize CQCanvasController::viewportSize() const
{
    QGraphicsWidget *canvasWidget = dynamic_cast<QGraphicsWidget*>(m_canvas);
    return canvasWidget->size().toSize();
}

void CQCanvasController::scrollContentsBy(int dx, int dy)
{
}

