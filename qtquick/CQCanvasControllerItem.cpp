/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen-Wander Hiemstra <aw.hiemstra@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "CQCanvasControllerItem.h"

#include <QPainter>
#include <QDebug>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>

#include "CQCanvasController.h"
#include "CQCanvasBase.h"

class CQCanvasControllerItem::Private
{
public:
    Private() : canvas(0), canvasController(0), lastX(0), lastY(0) { }

    CQCanvasBase *canvas;
    CQCanvasController *canvasController;

    QSize documentSize;

    float lastX;
    float lastY;
};

CQCanvasControllerItem::CQCanvasControllerItem(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges);
}

CQCanvasControllerItem::~CQCanvasControllerItem()
{
    delete d;
}

void CQCanvasControllerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget* )
{
//     painter->setBrush(QBrush(Qt::cyan));
//     painter->setOpacity(0.5);
//     painter->drawRect(x(), y(), width(), height());
}

QDeclarativeItem* CQCanvasControllerItem::canvas() const
{
    return d->canvas;
}

void CQCanvasControllerItem::setCanvas(QDeclarativeItem* canvas)
{
    Q_ASSERT(canvas);
    if(canvas != d->canvas) {
        d->canvas = qobject_cast<CQCanvasBase*>(canvas);
        Q_ASSERT(d->canvas);

        connect(d->canvas, SIGNAL(canvasControllerChanged()), SLOT(canvasControllerChanged()));
        canvasControllerChanged();

        emit canvasChanged();
    }
}

QSize CQCanvasControllerItem::documentSize() const
{
    return d->documentSize;
}

void CQCanvasControllerItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    Q_UNUSED(newGeometry);
    Q_UNUSED(oldGeometry);
}

QVariant CQCanvasControllerItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if(change == QGraphicsItem::ItemScenePositionHasChanged) {
        QPointF pos = value.toPointF();
        float xDiff = -(pos.x() - d->lastX);
        float yDiff = -(pos.y() - d->lastY);
        d->canvasController->pan(QPoint(xDiff, yDiff));
        d->lastX = pos.x();
        d->lastY = pos.y();
    }

    return value;
}

void CQCanvasControllerItem::updateDocumentSize(const QSize &size)
{
    setSize(size);
    d->documentSize = size;
    emit documentSizeChanged();
}

void CQCanvasControllerItem::canvasControllerChanged()
{
    if(d->canvasController) {
        disconnect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), this, SLOT(updateDocumentSize(QSize)));
    }

    d->canvasController = d->canvas->canvasController();
    if(d->canvasController) {
        connect(d->canvas->canvasController(), SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
        updateDocumentSize(d->canvasController->documentSize());
    }
}
