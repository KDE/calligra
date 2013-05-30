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
    Private()
        : canvas(0),
          flickable(0),
          canvasController(0),
          lastX(0),
          lastY(0),
          zoom(0.0f),
          zoomChange(0.f),
          zooming(false),
          minimumZoom( -1.f ),
          maximumZoom( 2.f ),
          placeholder(0)
          { }

    CQCanvasBase *canvas;
    QDeclarativeItem* flickable;
    CQCanvasController *canvasController;

    QSize documentSize;

    float lastX;
    float lastY;

    QRectF placeholderTarget;

    qreal zoom;
    qreal zoomChange;
    bool zooming;
    qreal minimumZoom;
    qreal maximumZoom;
    QPointF zoomCenter;

    QPixmap *placeholder;
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
    if(!d->zooming || !d->placeholder || d->placeholder->isNull()) {
        return;
    }

    QPointF offset(d->flickable->property("contentX").toReal(), d->flickable->property("contentY").toReal());


    painter->drawPixmap(QRectF(offset - d->placeholderTarget.topLeft(), d->placeholderTarget.size()) , *d->placeholder, QRectF(QPointF(0, 0), d->placeholder->size()));
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
        d->zoom = tempZoom;
        d->canvas->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, tempZoom);
        emit zoomChanged();
    }
}

qreal CQCanvasControllerItem::minimumZoom() const
{
    return d->minimumZoom;
}

qreal CQCanvasControllerItem::maximumZoom() const
{
    return d->maximumZoom;
}

void CQCanvasControllerItem::setMinimumZoom(qreal newZoom)
{
    if(newZoom != d->minimumZoom) {
        d->minimumZoom = newZoom;
        if( d->minimumZoom > 0.f )
            KoZoomMode::setMinimumZoom( d->minimumZoom );
        emit minimumZoomChanged();
    }
}

void CQCanvasControllerItem::setMaximumZoom(qreal newZoom)
{
    if(newZoom != d->maximumZoom) {
        d->maximumZoom = newZoom;
        KoZoomMode::setMaximumZoom( d->maximumZoom );
        emit maximumZoomChanged();
    }
}

void CQCanvasControllerItem::beginZoomGesture()
{
    if(d->zooming) {
        return;
    }

    d->placeholder = new QPixmap(d->flickable->width(), d->flickable->height());

    d->placeholderTarget.setLeft(0);
    d->placeholderTarget.setTop(0);
    d->placeholderTarget.setWidth(d->flickable->width());
    d->placeholderTarget.setHeight(d->flickable->height());

    QPainter painter;
    painter.begin(d->placeholder);
    d->canvas->render(&painter, QRectF(QPoint(0, 0), d->placeholder->size()));
    painter.end();

    d->canvas->setVisible(false);
    d->zooming = true;
}

void CQCanvasControllerItem::endZoomGesture()
{
    if(!d->zooming)
        return;

    qreal newZoom = d->zoom + d->zoomChange;

    qreal oldX = d->flickable->property("contentX").toReal();
    qreal oldY = d->flickable->property("contentY").toReal();

    qreal xoff = (d->zoomCenter.x() + oldX) * newZoom / d->zoom;
    d->flickable->setProperty("contentX", xoff - d->zoomCenter.x());

    qreal yoff = (d->zoomCenter.y() + oldY ) * newZoom / d->zoom;
    d->flickable->setProperty("contentY", yoff - d->zoomCenter.y());

    setZoom(d->zoom + d->zoomChange);

    delete d->placeholder;
    d->placeholder = 0;
    d->zoomChange = 0.0;
    d->zooming = false;

    d->canvas->setVisible(true);
}

void CQCanvasControllerItem::zoomBy(qreal amount, const QPointF& center)
{
    qreal newZoom = d->zoom + d->zoomChange + amount;
    if(d->zooming && newZoom >= KoZoomMode::minimumZoom() && newZoom <= KoZoomMode::maximumZoom() ) {
        qreal oldWidth = d->placeholderTarget.width();
        qreal oldHeight = d->placeholderTarget.height();

        d->zoomChange += amount;

        d->placeholderTarget.setWidth(d->flickable->width() * (1.0 + d->zoomChange));
        d->placeholderTarget.setHeight(d->flickable->height() * (1.0 + d->zoomChange));

        d->placeholderTarget.moveLeft(d->placeholderTarget.x() + (center.x() * d->placeholderTarget.width() / oldWidth) - center.x());
        d->placeholderTarget.moveTop(d->placeholderTarget.y() + (center.y() * d->placeholderTarget.height() / oldHeight) - center.y());

        d->zoomCenter = center;

        update();
    }
}

void CQCanvasControllerItem::fitToWidth( qreal width )
{
    if( width < 0.01f )
        return;

    if( d->zoom < 0.01f )
        return;

    if( d->documentSize.width() > 0.f && d->documentSize.width() < 2e6 )
        setZoom( width / ( d->documentSize.width() / d->zoom ) );
}

void CQCanvasControllerItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    Q_UNUSED(newGeometry);
    Q_UNUSED(oldGeometry);
}

QVariant CQCanvasControllerItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    Q_UNUSED(value);
    if(change == QGraphicsItem::ItemScenePositionHasChanged && d->canvasController) {
        QPointF pos(d->flickable->property("contentX").toReal(), d->flickable->property("contentY").toReal());
        float xDiff = pos.x() - d->lastX;
        float yDiff = pos.y() - d->lastY;
        d->canvasController->blockSignals(true);
        d->canvasController->pan(QPoint(xDiff, yDiff));
        d->canvasController->blockSignals(false);
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

        //If we have a correct document size, try to set the minimum zoom level, but
        //do not try to set it when we're dealing with a nearly-infinite document. (E.g. Sheets)
        if( d->minimumZoom < 0 && d->documentSize.width() > 0 && d->documentSize.width() < 2e6 )
        {
            qreal minZoom = d->flickable->width() / ( d->documentSize.width() / ( d->zoom > 0.f ? d->zoom : 1.0 ) );

            if( KoZoomMode::minimumZoom() != minZoom )
            {
                KoZoomMode::setMinimumZoom( minZoom );
                setZoom( d->zoom );
            }
        }
    }

    emit documentSizeChanged();
}

void CQCanvasControllerItem::updateDocumentPosition(const QPoint& pos)
{
    if(d->flickable) {
        d->flickable->setProperty("contentX", QVariant::fromValue<qreal>(pos.x()));
        d->flickable->setProperty("contentY", QVariant::fromValue<qreal>(pos.y()));
    }
}

void CQCanvasControllerItem::canvasControllerChanged()
{
    if(d->canvasController) {
        disconnect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), this, SLOT(updateDocumentSize(QSize)));
        disconnect(d->canvasController, SIGNAL(documentPositionChanged(QPoint)), this, SLOT(updateDocumentPosition(QPoint)));
    }

    d->canvasController = d->canvas->canvasController();
    if(d->canvasController) {
        connect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
        connect(d->canvasController, SIGNAL(documentPositionChanged(QPoint)), SLOT(updateDocumentPosition(QPoint)));
        updateDocumentSize(d->canvasController->documentSize());
    }
}
