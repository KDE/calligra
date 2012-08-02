/* This file is part of the KDE project
 * Copyright (C) 2012 KO GmbH. Contact: Boudewijn Rempt <boud@kogmbh.com>
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
 */
#include "CanvasControllerDeclarative.h"

#include <limits>

#include <QtCore/QTimer>
#include <QtGui/QGesture>
#include <QtGui/QVector2D>
#include <QtGui/QGraphicsWidget>

#include <KDE/KActionCollection>

#include <KoCanvasBase.h>
#include <KoZoomHandler.h>
#include <KoZoomController.h>
#include <KoToolManager.h>

class CanvasControllerDeclarative::Private
{
public:
    Private( CanvasControllerDeclarative* qq)
        : q(qq)
        , canvas(0)
        , zoomHandler(0)
        , zoomController(0)
        , vastScrollingFactor(0.f)
        , minX(0)
        , minY(0)
        , maxX(0)
        , maxY(0)
        , dragging(false)
        , zoom(1.0)
        , moveThreshold(5)
        , verticalScrollHandle(0)
        , horizontalScrollHandle(0)
    {
    }
    ~Private() { }

    void updateMinMax();
    void updateCanvasSize();
    void checkBounce(const QPoint& offset);
    void frameChanged(int frame);

    void updateScrollHandles();

    void onHeightChanged();
    void onWidthChanged();
    void documentOffsetMoved(const QPoint& point);
    void timerUpdate();

    QString file;

    CanvasControllerDeclarative* q;
    KoCanvasBase* canvas;
    KoZoomHandler* zoomHandler;
    KoZoomController* zoomController;

    QTimer* timer;

    qreal vastScrollingFactor;

    qreal minX;
    qreal minY;
    qreal maxX;
    qreal maxY;

    bool dragging;
    qreal distX;
    qreal distY;

    qreal zoom;
    qreal zoomMax;
    qreal zoomMin;

    QVector2D force;
    QVector2D velocity;

    float mass;
    float dragCoeff;
    float timeStep;
    float springCoeff;
    qreal moveThreshold;

    QDeclarativeItem *verticalScrollHandle;
    QDeclarativeItem *horizontalScrollHandle;

};

CanvasControllerDeclarative::CanvasControllerDeclarative(QDeclarativeItem* parent)
    : QDeclarativeItem(parent)
    , KoCanvasController(0)
    , d(new Private(this))
{
    setClip(true);
    setAcceptedMouseButtons(Qt::LeftButton);
    setFiltersChildEvents(true);
    setAcceptTouchEvents(true);
    grabGesture(Qt::PinchGesture);
    grabGesture(Qt::SwipeGesture);

    d->timer = new QTimer(this);
    d->timer->setInterval(40);
    connect(d->timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));

    d->mass = 10.f;
    d->dragCoeff = 0.05f;
    d->springCoeff = 0.2f;
    d->timeStep = 1000.f / d->timer->interval();

    connect(this, SIGNAL(heightChanged()), this, SLOT(updateCanvasSize()));
    connect(this, SIGNAL(widthChanged()), this, SLOT(updateCanvasSize()));

    connect(proxyObject, SIGNAL(moveDocumentOffset(QPoint)), this, SLOT(documentOffsetMoved(QPoint)));

}

CanvasControllerDeclarative::~CanvasControllerDeclarative()
{
    KoToolManager::instance()->removeCanvasController(this);
}

QString CanvasControllerDeclarative::file() const
{
    return d->file;
}

void CanvasControllerDeclarative::setFile(const QString &f)
{
    d->file = f;
}

void CanvasControllerDeclarative::setVastScrolling(qreal factor)
{
    d->vastScrollingFactor = factor;
    d->updateMinMax();
}

void CanvasControllerDeclarative::setZoomWithWheel(bool zoom)
{
    Q_UNUSED(zoom)
}

void CanvasControllerDeclarative::updateDocumentSize(const QSize& sz, bool recalculateCenter)
{
    Q_UNUSED(recalculateCenter)
    proxyObject->emitSizeChanged(sz);
    KoCanvasController::setDocumentSize(sz);
    d->updateMinMax();
    d->updateScrollHandles();

    emit documentSizeChanged();
}

void CanvasControllerDeclarative::resetDocumentOffset(const QPoint& offset)
{
    QPoint o;
    if(!offset.isNull()) {
        o = offset;
    } else {
        o = QPoint(d->minX, d->maxY);
    }
    setDocumentOffset(o);
    proxyObject->emitMoveDocumentOffset(offset);

    d->updateScrollHandles();
    d->timer->start();
}

void CanvasControllerDeclarative::setScrollBarValue(const QPoint& value)
{
    Q_UNUSED(value)
}

QPoint CanvasControllerDeclarative::scrollBarValue() const
{
    return documentOffset();
}

void CanvasControllerDeclarative::pan(const QPoint& distance)
{
    Q_UNUSED(distance)
}

QPoint CanvasControllerDeclarative::preferredCenter() const
{
    return QPoint();
}

void CanvasControllerDeclarative::setPreferredCenter(const QPoint& viewPoint)
{
    Q_UNUSED(viewPoint)
}

void CanvasControllerDeclarative::recenterPreferred()
{
}

void CanvasControllerDeclarative::zoomTo(const QRect& rect)
{
    Q_UNUSED(rect)
}

void CanvasControllerDeclarative::zoomBy(const QPoint& center, qreal zoom)
{
    qreal tempZoom = d->zoom * zoom;

    tempZoom = qBound(KoZoomMode::minimumZoom(), tempZoom, KoZoomMode::maximumZoom());

    proxyObject->emitZoomBy(zoom);
    d->zoom = tempZoom;

    QPointF offset = documentOffset();
    QPointF position;
    position.rx() = (zoom * -offset.x()) + (1 - zoom) * center.x();
    position.ry() = (zoom * -offset.y()) + (1 - zoom) * center.y();

    QPoint oNew = (-position).toPoint();

    d->updateMinMax();
    d->updateCanvasSize();
    resetDocumentOffset(oNew);
    d->timer->start();

    emit zoomLevelChanged();
}

int CanvasControllerDeclarative::zoomLevel() const
{
    if(!d->zoomHandler)
        return 100;
    return d->zoomHandler->zoomInPercent();
}

void CanvasControllerDeclarative::setZoomLevel(int zoomPercentage)
{
    // Get current position
    QPointF offset = documentOffset();
    // Get current zoom level
    float oldZoomLevel = d->zoomHandler->zoom();

    // Set new zoom level
    float newZoomLevel = qBound(KoZoomMode::minimumZoom(), ((float)zoomPercentage)/100.0,KoZoomMode::maximumZoom());
    d->zoomController->setZoom(KoZoomMode::ZOOM_CONSTANT, newZoomLevel);

    float zoomDif = newZoomLevel / oldZoomLevel;
    offset.rx() = (offset.x() * zoomDif);
    offset.ry() = (offset.y() * zoomDif);

    resetDocumentOffset(offset.toPoint());
    d->updateMinMax();
    d->updateCanvasSize();
    d->timer->start();

    emit zoomLevelChanged();
}

void CanvasControllerDeclarative::zoomOut(const QPoint& center)
{
    if(center.isNull()) {
        zoomBy(QPoint(width()/ 2, height() / 2), sqrt(0.5));
    } else {
        zoomBy(center, sqrt(0.5));
    }
}

void CanvasControllerDeclarative::zoomIn(const QPoint& center)
{
    if(center.isNull()) {
        zoomBy(QPoint(width()/ 2, height() / 2), sqrt(2.0));
    } else {
        zoomBy(center, sqrt(2.0));
    }
}

void CanvasControllerDeclarative::resetZoom()
{
    d->zoomController->setZoom(KoZoomMode::ZOOM_CONSTANT, 1.0);
    d->updateMinMax();
    d->updateCanvasSize();
}

QPoint CanvasControllerDeclarative::getDocumentOffset()
{
    return documentOffset();
}

void CanvasControllerDeclarative::ensureVisible(KoShape* shape)
{
    Q_UNUSED(shape)
}

void CanvasControllerDeclarative::ensureVisible(const QRectF& rect, bool smooth)
{
    QPointF target = rect.center();
    target.rx() -= width() / 2;
    target.ry() -= height() / 2;

    if(!isnan(d->maxX)) {
        if(-target.x() < d->minX) {
            target.setX(-d->minX);
        }
        if(-target.x() > d->maxX) {
            target.setX(-d->maxX);
        }
    } else {
        target.setX(-d->minX);
    }

    if(!isnan(d->maxY)) {
        if(-target.y() < d->minY) {
            target.setY(-d->minY);
        }

        if(-target.y() > d->maxY) {
            target.setY(-d->maxY);
        }
    } else {
        target.setY(-d->minY);
    }

    if(!smooth) {
        resetDocumentOffset(target.toPoint());
    } else {
        QVector2D diff = QVector2D(target) - QVector2D(documentOffset());
        d->force += diff / 10;
        d->timer->start();
    }
}

int CanvasControllerDeclarative::canvasOffsetY() const
{
    return documentOffset().y();
}

int CanvasControllerDeclarative::canvasOffsetX() const
{
    return documentOffset().x();
}

int CanvasControllerDeclarative::visibleWidth() const
{
    return width();
}

int CanvasControllerDeclarative::visibleHeight() const
{
    return height();
}

KoCanvasBase* CanvasControllerDeclarative::canvas() const
{
    return d->canvas;
}

void CanvasControllerDeclarative::setCanvas(KoCanvasBase* canvas)
{
    Q_ASSERT(canvas);

    if (d->canvas) {
        proxyObject->emitCanvasRemoved(this);
        d->canvas->canvasItem()->hide();
        canvas->setCanvasController(0);
        canvas->canvasItem()->removeSceneEventFilter(this);
    }

    d->canvas = canvas;

    KoToolManager::instance()->addController(this);

    zoomController(canvas->viewConverter(), true);

    canvas->setCanvasController(this);
    canvas->canvasItem()->setParentItem(this);
    canvas->canvasItem()->installEventFilter(this);
    canvas->canvasItem()->setAcceptTouchEvents(true);
    canvas->canvasItem()->grabGesture(Qt::PinchGesture);
    canvas->canvasItem()->grabGesture(Qt::SwipeGesture);
    canvas->canvasItem()->show();

    d->updateCanvasSize();
    d->updateMinMax();

    setFocusProxy(d->canvas->canvasItem());
    proxyObject->emitCanvasSet(this);
}

void CanvasControllerDeclarative::setDrawShadow(bool drawShadow)
{
    Q_UNUSED(drawShadow)
}

QSize CanvasControllerDeclarative::viewportSize() const
{
    return QSize(width(), height());
}

void CanvasControllerDeclarative::scrollContentsBy(int dx, int dy)
{
    if(d->minX == d->maxX || d->minY == d->maxY)
        d->updateMinMax();


    QPoint offset = documentOffset();
    if(!isnan(d->maxX)) {
        offset.setX(offset.x() + dx);
    }
    if(!isnan(d->maxY)) {
        offset.setY(offset.y() + dy);
    }

    resetDocumentOffset(offset);
}

QDeclarativeItem* CanvasControllerDeclarative::horizontalScrollHandle()
{
    return d->horizontalScrollHandle;
}

QDeclarativeItem* CanvasControllerDeclarative::verticalScrollHandle()
{
    return d->verticalScrollHandle;
}

void CanvasControllerDeclarative::setHorizontalScrollHandle ( QDeclarativeItem* handle )
{
    d->horizontalScrollHandle = handle;
}

void CanvasControllerDeclarative::setVerticalScrollHandle ( QDeclarativeItem* handle )
{
    d->verticalScrollHandle = handle;
}

QVector2D CanvasControllerDeclarative::force() const
{
    return d->force;
}

void CanvasControllerDeclarative::setForce(const QVector2D& newForce)
{
    d->force = newForce;
}

void CanvasControllerDeclarative::setZoomMax(qreal newZoomMax)
{
    d->zoomMax = newZoomMax;
}

qreal CanvasControllerDeclarative::zoomMax() const
{
    return d->zoomMax;
}

void CanvasControllerDeclarative::setZoomMin(qreal newZoomMin)
{
    d->zoomMin = newZoomMin;
}

qreal CanvasControllerDeclarative::zoomMin() const
{
    return d->zoomMin;
}

KoZoomHandler* CanvasControllerDeclarative::zoomHandler() const
{
    return d->zoomHandler;
}

bool CanvasControllerDeclarative::eventFilter(QObject* target , QEvent* event )
{
    if(target == this || target == d->canvas->canvasItem()) {
        if(event->type() == QEvent::GraphicsSceneMousePress) {
            d->velocity = QVector2D();
            d->timer->stop();
        }
    }
    return QDeclarativeItem::eventFilter(target, event);
}

KoZoomController* CanvasControllerDeclarative::zoomController(KoViewConverter* viewConverter, bool recreate)
{
    if(recreate) {
        delete d->zoomController;
        d->zoomController = 0;
    }

    if(!d->zoomController) {
        d->zoomHandler = dynamic_cast<KoZoomHandler*>(viewConverter);
        if(!d->zoomHandler)
            d->zoomHandler = new KoZoomHandler();

        KActionCollection collection(this);
        d->zoomController = new KoZoomController(this, d->zoomHandler, &collection, 0, this);
    }
    return d->zoomController;
}

void CanvasControllerDeclarative::setMargin(int margin)
{
    KoCanvasController::setMargin(margin);
    d->updateMinMax();
}

void CanvasControllerDeclarative::Private::updateMinMax()
{
    if(q->canvasMode() != KoCanvasController::Infinite ) {
        int halfWindowWidth = q->width() / 2;
        int halfDocWidth = q->documentSize().width() / 2;
        int margin = q->margin();

        minX = halfWindowWidth - (halfDocWidth + margin) + qMin(0, halfWindowWidth - halfDocWidth);
        if(q->width() < q->documentSize().width()) {
            maxX = minX + ( qMax(0, -(halfWindowWidth - halfDocWidth)) + margin) * 2;
        } else {
            maxX = std::numeric_limits<qreal>::quiet_NaN();
        }

        int halfWindowHeight = q->height() / 2;
        int docHeight = q->documentSize().height();
        int halfDocHeight = (docHeight / 2);

        minY = halfWindowHeight - (halfDocHeight + margin) + qMin(0, halfWindowHeight - halfDocHeight);
        if(q->height() < (docHeight + (margin * 2))) {
            maxY = minY + ( qMax(0, -(halfWindowHeight - halfDocHeight)) + margin) * 2;
        } else {
            maxY = std::numeric_limits<qreal>::quiet_NaN();
        }
    } else {
        minX = 1 - q->documentSize().width();
        maxX = 1;
        minY = 1 - q->documentSize().height();
        maxY = 1;
    }
}

void CanvasControllerDeclarative::Private::updateCanvasSize()
{
    if (canvas) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(canvas->canvasItem());
        item->setX(0);
        item->setY(0);
        item->setWidth(q->width());
        item->setHeight(q->height());
        canvas->updateCanvas(canvas->viewConverter()->viewToDocument(QRectF(0, 0, q->width(), q->height())));
        item->update();
        updateMinMax();
        timer->start();
    }
}

void CanvasControllerDeclarative::Private::documentOffsetMoved(const QPoint& point)
{
    Q_UNUSED(point);
    emit q->docMoved();
}

void CanvasControllerDeclarative::Private::timerUpdate()
{
    QVector2D position = QVector2D(q->documentOffset());
    QVector2D totalForce = force - (dragCoeff * velocity);
    force = QVector2D();

    QVector2D accel = totalForce / mass;

    position += velocity;

    bool positionValid = true;

    if(!isnan(maxX)) {
        if(-position.x() < minX) {
            float diff = (position.x() + minX) * springCoeff;
            position.setX(-minX + diff);
            if(qAbs(diff) > moveThreshold) {
                positionValid = false;
            } else {
                position.setX(-minX);
            }
        }
        if(-position.x() > maxX) {
            float diff = (position.x() + maxX) * springCoeff;
            position.setX(-maxX + diff);
            if(qAbs(diff) > moveThreshold) {
                positionValid = false;
            } else {
                position.setX(-maxX);
            }
        }
    } else {
        position.setX(-minX);
    }

    if(!isnan(maxY)) {
        if(-position.y() < minY) {
            float diff = (position.y() + minY) * springCoeff;
            position.setY(-minY + diff);
            if(qAbs(diff) > moveThreshold) {
                positionValid = false;
            } else {
                position.setY(-minY);
            }
        }
        if(-position.y() > maxY) {
            float diff = (position.y() + maxY) * springCoeff;
            position.setY(-maxY + diff);
            if(qAbs(diff) > moveThreshold) {
                positionValid = false;
            } else {
                position.setY(-maxY);
            }
        }
    } else {
        position.setY(-minY);
    }

    velocity += accel * timeStep;

    q->resetDocumentOffset(QPoint(position.x(), position.y()));

    if(qAbs(velocity.x()) < moveThreshold && qAbs(velocity.y()) < moveThreshold && positionValid) {
        timer->stop();
        emit q->hideHorizontalScrollHandle();
        emit q->hideVerticalScrollHandle();
    }
}

void CanvasControllerDeclarative::Private::updateScrollHandles()
{
    if(horizontalScrollHandle) {
        qreal docWidthPerc = q->width() / q->documentSize().width();
        if(docWidthPerc < 1.0) {
            horizontalScrollHandle->setWidth(q->width() * docWidthPerc);
            emit q->showHorizontalScrollHandle();
        } else {
            horizontalScrollHandle->setWidth(q->width());
            emit q->hideHorizontalScrollHandle();
        }

        qreal offset = qreal(q->documentOffset().x()) / q->documentSize().width();
        qreal pos = offset * q->width();
        horizontalScrollHandle->setX(pos);
    }

    if(verticalScrollHandle) {
        qreal docHeightPerc = q->height() / q->documentSize().height();
        if(docHeightPerc < 1.0) {
            verticalScrollHandle->setHeight(q->height() * docHeightPerc);
            emit q->showVerticalScrollHandle();
        } else {
            verticalScrollHandle->setHeight(q->height());
            emit q->hideVerticalScrollHandle();
        }

        qreal offset = qreal(q->documentOffset().y()) / q->documentSize().height();
        qreal pos = offset * q->height();
        verticalScrollHandle->setY(pos);
    }
}

#include "CanvasControllerDeclarative.moc"
