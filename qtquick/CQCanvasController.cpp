/*
 * This file is part of the KDE project
 * 
 * Copyright (C) 2013 Shantanu Tushar <shantanu@kde.org>
 * Copyright (C) 2013 Sujith Haridasan <sujith.h@gmail.com>
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QGraphicsWidget>

#include <QDebug>

#include <KoCanvasBase.h>

class CQCanvasController::Private
{
public:
    Private() : canvas(0) { }
    KoCanvasBase *canvas;
};

CQCanvasController::CQCanvasController(KActionCollection* actionCollection)
    : KoCanvasController(actionCollection), d(new Private)
{
}

CQCanvasController::~CQCanvasController()
{
    delete d;
}

void CQCanvasController::setVastScrolling(qreal factor)
{
}

void CQCanvasController::setZoomWithWheel(bool zoom)
{
}

void CQCanvasController::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
    setDocumentSize(sz);
    emit documentSizeChanged(sz);
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
    QPoint offset = documentOffset() + distance;
    setDocumentOffset(offset);
    proxyObject->emitMoveDocumentOffset(offset);
    emit documentPositionChanged(offset);
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
    return d->canvas;
}

void CQCanvasController::setCanvas(KoCanvasBase* canvas)
{
    d->canvas = canvas;
    canvas->setCanvasController(this);
    proxyObject->emitCanvasSet(this);
}

void CQCanvasController::setDrawShadow(bool drawShadow)
{
}

QSize CQCanvasController::viewportSize() const
{
    QGraphicsWidget *canvasWidget = dynamic_cast<QGraphicsWidget*>(d->canvas);
    return canvasWidget->size().toSize();
}

void CQCanvasController::scrollContentsBy(int dx, int dy)
{
}

QSize CQCanvasController::documentSize()
{
    return KoCanvasController::documentSize();
}

