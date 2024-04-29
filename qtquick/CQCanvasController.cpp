/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Shantanu Tushar <shantanu@kde.org>
 * SPDX-FileCopyrightText: 2013 Sujith Haridasan <sujith.h@gmail.com>
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "CQCanvasController.h"

#include <QGraphicsWidget>
#include <QPoint>
#include <QSize>

#include <KoCanvasBase.h>
#include <KoShape.h>

class CQCanvasController::Private
{
public:
    Private()
        : canvas(0)
    {
    }
    KoCanvasBase *canvas;
};

CQCanvasController::CQCanvasController(KActionCollection *actionCollection)
    : KoCanvasController(actionCollection)
    , d(new Private)
{
}

CQCanvasController::~CQCanvasController()
{
    delete d;
}

void CQCanvasController::setVastScrolling(qreal factor)
{
    Q_UNUSED(factor)
}

void CQCanvasController::setZoomWithWheel(bool zoom)
{
    Q_UNUSED(zoom)
}

void CQCanvasController::updateDocumentSize(const QSize &sz, bool recalculateCenter)
{
    Q_UNUSED(recalculateCenter)
    setDocumentSize(sz);
    Q_EMIT documentSizeChanged(sz);
}

void CQCanvasController::setScrollBarValue(const QPoint &value)
{
    Q_UNUSED(value)
}

QPoint CQCanvasController::scrollBarValue() const
{
    return QPoint();
}

void CQCanvasController::pan(const QPoint &distance)
{
    QPoint offset = documentOffset() + distance;
    setDocumentOffset(offset);
    proxyObject->emitMoveDocumentOffset(offset);
    Q_EMIT documentPositionChanged(offset);
}

QPointF CQCanvasController::preferredCenter() const
{
    return QPointF();
}

void CQCanvasController::setPreferredCenter(const QPointF &viewPoint)
{
    Q_UNUSED(viewPoint)
}

void CQCanvasController::recenterPreferred()
{
}

void CQCanvasController::zoomTo(const QRect &rect)
{
    Q_UNUSED(rect)
}

void CQCanvasController::zoomBy(const QPoint &center, qreal zoom)
{
    Q_UNUSED(center)
    Q_UNUSED(zoom)
}

void CQCanvasController::zoomOut(const QPoint &center)
{
    Q_UNUSED(center)
}

void CQCanvasController::zoomIn(const QPoint &center)
{
    Q_UNUSED(center)
}

void CQCanvasController::ensureVisible(KoShape *shape)
{
    Q_UNUSED(shape)
}

void CQCanvasController::ensureVisible(const QRectF &rect, bool smooth)
{
    Q_UNUSED(rect)
    Q_UNUSED(smooth)
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

KoCanvasBase *CQCanvasController::canvas() const
{
    return d->canvas;
}

void CQCanvasController::setCanvas(KoCanvasBase *canvas)
{
    d->canvas = canvas;
    canvas->setCanvasController(this);
    proxyObject->emitCanvasSet(this);
}

void CQCanvasController::setDrawShadow(bool drawShadow)
{
    Q_UNUSED(drawShadow)
}

QSize CQCanvasController::viewportSize() const
{
    QGraphicsWidget *canvasWidget = dynamic_cast<QGraphicsWidget *>(d->canvas);
    return canvasWidget->size().toSize();
}

void CQCanvasController::scrollContentsBy(int dx, int dy)
{
    Q_UNUSED(dx)
    Q_UNUSED(dy)
}

QSize CQCanvasController::documentSize() const
{
    return KoCanvasController::documentSize();
}
