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

#include "ComponentsKoCanvasController.h"

#include <QGraphicsWidget>
#include <QPoint>
#include <QSize>

#include <QDebug>

#include <KoCanvasBase.h>

using namespace Calligra::Components;

class ComponentsKoCanvasController::Private
{
public:
    Private()
        : canvas(nullptr)
    {
    }
    KoCanvasBase *canvas;
};

ComponentsKoCanvasController::ComponentsKoCanvasController(KActionCollection *actionCollection)
    : KoCanvasController(actionCollection)
    , d(new Private)
{
}

ComponentsKoCanvasController::~ComponentsKoCanvasController()
{
    delete d;
}

void ComponentsKoCanvasController::setVastScrolling(qreal factor)
{
    Q_UNUSED(factor)
}

void ComponentsKoCanvasController::setZoomWithWheel(bool zoom)
{
    Q_UNUSED(zoom)
}

void ComponentsKoCanvasController::updateDocumentSize(const QSize &sz, bool recalculateCenter)
{
    setDocumentSize(sz);
    Q_EMIT documentSizeChanged(sz);
}

void ComponentsKoCanvasController::setScrollBarValue(const QPoint &value)
{
    setDocumentOffset(const_cast<QPoint &>(value));
    proxyObject->emitMoveDocumentOffset(value);
    Q_EMIT documentPositionChanged(value);
}

QPoint ComponentsKoCanvasController::scrollBarValue() const
{
    return QPoint();
}

void ComponentsKoCanvasController::pan(const QPoint &distance)
{
    QPoint offset = documentOffset() + distance;
    setDocumentOffset(offset);
    proxyObject->emitMoveDocumentOffset(offset);
    Q_EMIT documentPositionChanged(offset);
}

QPointF ComponentsKoCanvasController::preferredCenter() const
{
    return QPointF();
}

void ComponentsKoCanvasController::setPreferredCenter(const QPointF &viewPoint)
{
    Q_UNUSED(viewPoint)
}

void ComponentsKoCanvasController::recenterPreferred()
{
}

void ComponentsKoCanvasController::zoomTo(const QRect &rect)
{
    Q_UNUSED(rect)
}

void ComponentsKoCanvasController::zoomBy(const QPoint &center, qreal zoom)
{
    Q_UNUSED(center)
    Q_UNUSED(zoom)
}

void ComponentsKoCanvasController::zoomOut(const QPoint &center)
{
    Q_UNUSED(center)
}

void ComponentsKoCanvasController::zoomIn(const QPoint &center)
{
    Q_UNUSED(center)
}

void ComponentsKoCanvasController::ensureVisible(KoShape *shape)
{
    Q_UNUSED(shape)
}

void ComponentsKoCanvasController::ensureVisible(const QRectF &rect, bool smooth)
{
    Q_UNUSED(rect)
    Q_UNUSED(smooth)
}

int ComponentsKoCanvasController::canvasOffsetY() const
{
    return 0;
}

int ComponentsKoCanvasController::canvasOffsetX() const
{
    return 0;
}

int ComponentsKoCanvasController::visibleWidth() const
{
    return 0;
}

int ComponentsKoCanvasController::visibleHeight() const
{
    return 0;
}

KoCanvasBase *ComponentsKoCanvasController::canvas() const
{
    return d->canvas;
}

void ComponentsKoCanvasController::setCanvas(KoCanvasBase *canvas)
{
    d->canvas = canvas;
    canvas->setCanvasController(this);
    proxyObject->emitCanvasSet(this);
}

void ComponentsKoCanvasController::setDrawShadow(bool drawShadow)
{
    Q_UNUSED(drawShadow)
}

QSize ComponentsKoCanvasController::viewportSize() const
{
    QGraphicsWidget *canvasWidget = dynamic_cast<QGraphicsWidget *>(d->canvas);
    return canvasWidget->size().toSize();
}

void ComponentsKoCanvasController::scrollContentsBy(int dx, int dy)
{
    Q_UNUSED(dx)
    Q_UNUSED(dy)
}

QSize ComponentsKoCanvasController::documentSize()
{
    return KoCanvasController::documentSize();
}
