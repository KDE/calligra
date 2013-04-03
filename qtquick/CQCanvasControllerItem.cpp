/*
 * This file is part of the KDE project
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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
#include <KoZoomMode.h>
#include <KoZoomController.h>

#include "CQCanvasController.h"
#include "CQCanvasBase.h"

class CQCanvasControllerItem::Private
{
public:
    Private() : canvas(0), flickable(0), canvasController(0), lastX(0), lastY(0), zoom(1.0f) { }

    CQCanvasBase *canvas;
    QDeclarativeItem* flickable;
    CQCanvasController *canvasController;

    QSize documentSize;

    float lastX;
    float lastY;

    qreal zoom;
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
#if 0
    painter->setBrush(QBrush(Qt::cyan));
    painter->setOpacity(0.5);
    painter->drawRect(x(), y(), width(), height());
#endif
}

QDeclarativeItem* CQCanvasControllerItem::canvas() const
{
    return d->canvas;
}

void CQCanvasControllerItem::setCanvas(QDeclarativeItem* canvas)
{
    if(canvas != d->canvas) {
        if(d->canvas) {
            disconnect(d->canvas, SIGNAL(canvasControllerChanged()), this, SLOT(canvasControllerChanged()));
        }

        d->canvas = qobject_cast<CQCanvasBase*>(canvas);
        Q_ASSERT(d->canvas);

        connect(d->canvas, SIGNAL(canvasControllerChanged()), SLOT(canvasControllerChanged()));
        canvasControllerChanged();

        emit canvasChanged();
    }
}

QDeclarativeItem* CQCanvasControllerItem::flickable() const
{
    return d->flickable;
}

void CQCanvasControllerItem::setFlickable(QDeclarativeItem* item)
{
    if(item != d->flickable) {
        if(item->metaObject()->indexOfProperty("contentWidth") == -1) {
            qWarning() << Q_FUNC_INFO << "item does not look like a flickable, ignoring.";
            return;
        }

        d->flickable = item;
        d->flickable->setProperty("contentWidth", d->documentSize.width());
        d->flickable->setProperty("contentHeight", d->documentSize.height());
        emit flickableChanged();
    }
}

QSize CQCanvasControllerItem::documentSize() const
{
    return d->documentSize;
}

qreal CQCanvasControllerItem::zoom() const
{
    return d->zoom;
}

void CQCanvasControllerItem::setZoom(qreal newZoom)
{
    qreal tempZoom = qBound(KoZoomMode::minimumZoom(), newZoom, KoZoomMode::maximumZoom());
    if(!qFuzzyCompare(d->zoom, tempZoom)) {
        d->canvas->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, tempZoom);
        d->zoom = tempZoom;
        emit zoomChanged();
    }
}

void CQCanvasControllerItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    Q_UNUSED(newGeometry);
    Q_UNUSED(oldGeometry);
}

QVariant CQCanvasControllerItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    if(change == QGraphicsItem::ItemScenePositionHasChanged) {
        QPointF pos = value.toPointF() - d->flickable->scenePos();
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

    if(d->flickable) {
        d->flickable->setProperty("contentWidth", d->documentSize.width());
        d->flickable->setProperty("contentHeight", d->documentSize.height());
    }

    emit documentSizeChanged();
}

void CQCanvasControllerItem::canvasControllerChanged()
{
    if(d->canvasController) {
        disconnect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), this, SLOT(updateDocumentSize(QSize)));
    }

    d->canvasController = d->canvas->canvasController();
    if(d->canvasController) {
        connect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
        updateDocumentSize(d->canvasController->documentSize());
    }
}
