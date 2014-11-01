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
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGLWidget>

#include <KoCanvasBase.h>
#include <KoCanvasController.h>
#include <KoZoomMode.h>
#include <KoZoomController.h>

#include "CQCanvasController.h"
#include "CQCanvasBase.h"
#include "CQTextDocumentCanvas.h"

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
          useViewport(false)
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

    bool useViewport;
    QImage placeholder;
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

void CQCanvasControllerItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* , QWidget*)
{
    if (!d->zooming || d->placeholder.isNull()) {
        return;
    }

    QPointF offset(d->flickable->property("contentX").toReal(), d->flickable->property("contentY").toReal());

    painter->drawImage(QRectF(offset - d->placeholderTarget.topLeft(), d->placeholderTarget.size()), d->placeholder, QRectF(QPointF(0, 0), d->placeholder.size()));
}

QDeclarativeItem* CQCanvasControllerItem::canvas() const
{
    return d->canvas;
}

void CQCanvasControllerItem::setCanvas(QDeclarativeItem* canvas)
{
    if (canvas != d->canvas) {
        if (d->canvas) {
            disconnect(d->canvas, SIGNAL(canvasControllerChanged()), this, SLOT(canvasControllerChanged()));
            disconnect(d->canvas, SIGNAL(positionShouldChange(QPoint)), this, SLOT(updateDocumentPosition(QPoint)));
        }

        d->canvas = qobject_cast<CQCanvasBase*>(canvas);
        Q_ASSERT(d->canvas);

        connect(d->canvas, SIGNAL(positionShouldChange(QPoint)), this, SLOT(updateDocumentPosition(QPoint)));
        connect(d->canvas, SIGNAL(canvasControllerChanged()), SLOT(canvasControllerChanged()));
        canvasControllerChanged();

        if (qobject_cast<CQTextDocumentCanvas*>(d->canvas) != 0) {
            d->useViewport = true;
        } else {
            d->useViewport = false;
        }

        emit canvasChanged();
    }
}

QDeclarativeItem* CQCanvasControllerItem::flickable() const
{
    return d->flickable;
}

void CQCanvasControllerItem::setFlickable(QDeclarativeItem* item)
{
    if (item != d->flickable) {
        if (item->metaObject()->indexOfProperty("contentWidth") == -1) {
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
    if (!qFuzzyCompare(d->zoom, tempZoom)) {
        d->zoom = tempZoom;
        if (d->canvas && d->canvas->zoomController()) {
            d->canvas->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, tempZoom);
        }
        emit zoomChanged();
    }
}

void CQCanvasControllerItem::zoomToPage()
{
    if (d->canvas && d->canvas->zoomController()) {
        // This may seem odd, but it ensures that we scale up as well as down
        // when zooming to fit page - without this, it will only scale down,
        // not up. To avoid changing the zooming logic in desktop, we just do
        // it this way. It's non-invasive anyway, and reasonably cheap, and
        // at any rate just covers a corner-case (normally this would only
        // happen when in full-screen mode anyway, but scaling down can be
        // triggered if in touch mode as a window and that is resized)
        d->canvas->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, 10);
        d->canvas->zoomController()->setZoom(KoZoomMode::ZOOM_PAGE, 1.0);
        d->canvas->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, d->canvas->zoomController()->zoomAction()->effectiveZoom());
        d->zoom = d->canvas->zoomController()->zoomAction()->effectiveZoom();
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
    if (newZoom != d->minimumZoom && newZoom < KoZoomMode::maximumZoom() && newZoom > 0.f) {
        d->minimumZoom = newZoom;
        KoZoomMode::setMinimumZoom( d->minimumZoom );
        emit minimumZoomChanged();
    }
}

void CQCanvasControllerItem::setMaximumZoom(qreal newZoom)
{
    if (newZoom != d->maximumZoom && newZoom > KoZoomMode::minimumZoom()) {
        d->maximumZoom = newZoom;
        KoZoomMode::setMaximumZoom( d->maximumZoom );
        emit maximumZoomChanged();
    }
}

void CQCanvasControllerItem::beginZoomGesture()
{
    if (d->zooming) {
        return;
    }

    d->placeholderTarget.setLeft(0.f);
    d->placeholderTarget.setTop(0.f);
    d->placeholderTarget.setWidth(d->flickable->width());
    d->placeholderTarget.setHeight(d->flickable->height());

    if (d->useViewport) {
        QGLWidget* gl = qobject_cast<QGLWidget*>(scene()->views().at(0)->viewport());
        if (!gl) {
            return;
        }

        QRectF scene = d->flickable->mapToScene(QRectF(d->flickable->x(), 0, d->flickable->width(), d->flickable->height())).boundingRect();
        d->placeholder = gl->grabFrameBuffer(true).copy(scene.toRect());
    } else {
        d->placeholder = QImage(d->flickable->width(), d->flickable->height(), QImage::Format_ARGB32_Premultiplied);
        QPainter painter;
        painter.begin(&d->placeholder);
        d->canvas->render(&painter, QRectF(0, 0, d->flickable->width(), d->flickable->height()));
        painter.end();
    }

    d->canvas->setVisible(false);
    d->zooming = true;
}

void CQCanvasControllerItem::endZoomGesture()
{
    if (!d->zooming) {
        return;
    }

    qreal newZoom = d->zoom + d->zoomChange;

    qreal oldX = d->flickable->property("contentX").toReal();
    qreal oldY = d->flickable->property("contentY").toReal();

    qreal xoff = (d->zoomCenter.x() + oldX) * newZoom / d->zoom;
    d->flickable->setProperty("contentX", xoff - d->zoomCenter.x());

    qreal yoff = (d->zoomCenter.y() + oldY ) * newZoom / d->zoom;
    d->flickable->setProperty("contentY", yoff - d->zoomCenter.y());

    setZoom(d->zoom + d->zoomChange);

    d->placeholder = QImage();
    d->zoomChange = 0.0;
    d->zooming = false;

    d->canvas->setVisible(true);
}

void CQCanvasControllerItem::zoomBy(qreal amount, const QPointF& center)
{
    qreal newZoom = d->zoom + d->zoomChange + amount;
    if (d->zooming && newZoom >= KoZoomMode::minimumZoom() && newZoom <= KoZoomMode::maximumZoom() ) {
//         qreal oldWidth = d->placeholderTarget.width();
//         qreal oldHeight = d->placeholderTarget.height();
        qreal oldZoom = d->zoom + d->zoomChange;

        d->zoomChange += amount;

        d->placeholderTarget.setWidth((d->placeholderTarget.width() / oldZoom) * newZoom);
        d->placeholderTarget.setHeight((d->placeholderTarget.height() / oldZoom) * newZoom);
        d->placeholderTarget.moveLeft((center.x() * newZoom / d->zoom) - center.x());
        d->placeholderTarget.moveTop((center.y() * newZoom / d->zoom) - center.y());

        d->zoomCenter = center;

        update();
    }
}

void CQCanvasControllerItem::fitToWidth( qreal width )
{
    if ( width < 0.01f ) {
        return;
    }

    if ( d->zoom < 0.01f ) {
        return;
    }

    if ( d->documentSize.width() > 0.f && d->documentSize.width() < 2e6 ) {
        setZoom( width / ( d->documentSize.width() / d->zoom ) );
    }
}

void CQCanvasControllerItem::returnToBounds()
{
    QPointF pos(d->flickable->property("contentX").toReal(), d->flickable->property("contentY").toReal());
    float xDiff = pos.x() - d->lastX;
    float yDiff = pos.y() - d->lastY;
    d->canvasController->blockSignals(true);
    d->canvasController->pan(QPoint(xDiff, yDiff));
    d->canvasController->blockSignals(false);
    d->lastX = pos.x();
    d->lastY = pos.y();
}

void CQCanvasControllerItem::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    Q_UNUSED(newGeometry);
    Q_UNUSED(oldGeometry);
}

QVariant CQCanvasControllerItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
    Q_UNUSED(value);
    if (change == QGraphicsItem::ItemScenePositionHasChanged && d->canvasController) {
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

    if (d->flickable) {
        d->flickable->setProperty("contentWidth", d->documentSize.width());
        d->flickable->setProperty("contentHeight", d->documentSize.height());

        //If we have a correct document size, try to set the minimum zoom level, but
        //do not try to set it when we're dealing with a nearly-infinite document. (E.g. Sheets)
        if ( d->minimumZoom < 0 && d->documentSize.width() > 0 && d->documentSize.width() < 2e6 )
        {
            qreal minZoom = d->flickable->width() / ( d->documentSize.width() / ( d->zoom > 0.f ? d->zoom : 0.5 ) );

            if ( KoZoomMode::minimumZoom() != minZoom )
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
    if (d->flickable) {
        d->flickable->setProperty("contentX", QVariant::fromValue<qreal>(pos.x()));
        d->flickable->setProperty("contentY", QVariant::fromValue<qreal>(pos.y()));
    }
}

void CQCanvasControllerItem::canvasControllerChanged()
{
    if (d->canvasController) {
        disconnect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), this, SLOT(updateDocumentSize(QSize)));
        disconnect(d->canvasController, SIGNAL(documentPositionChanged(QPoint)), this, SLOT(updateDocumentPosition(QPoint)));
    }

    d->canvasController = d->canvas->canvasController();
    if (d->canvasController) {
        connect(d->canvasController, SIGNAL(documentSizeChanged(QSize)), SLOT(updateDocumentSize(QSize)));
        connect(d->canvasController, SIGNAL(documentPositionChanged(QPoint)), SLOT(updateDocumentPosition(QPoint)));
        updateDocumentSize(d->canvasController->documentSize());
    }
}
