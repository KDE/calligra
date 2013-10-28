/*
 * This file is part of the KDE project
 *
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

#include "ViewController.h"

#include <QtCore/QDebug>
#include <QTimer>
#include <QQuickWindow>
#include <QPainter>

#include <KoCanvasController.h>

#include "Document.h"
#include "View.h"

using namespace Calligra::Components;

class ViewController::Private
{
public:
    Private()
        : view{nullptr}
        , flickable{nullptr}
        , canvasController{nullptr}
        , lastX{0.f}
        , lastY{0.f}
        , ignoreOffsetChange{false}
        , ignoreFlickableChange{false}
        , minimumZoom{.5f}
        , minimumZoomFitsWidth{false}
        , zoom{1.f}
        , zoomChange{0.f}
        , maximumZoom{2.f}
        , useZoomProxy{true}
        , zoomProxy{nullptr}
        , zoomTimer{nullptr}
    { }

    View* view;
    QQuickItem* flickable;

    KoCanvasController* canvasController;

    float lastX;
    float lastY;
    bool ignoreOffsetChange;
    bool ignoreFlickableChange;

    float minimumZoom;
    bool minimumZoomFitsWidth;
    float zoom;
    float zoomChange;
    float maximumZoom;

    bool useZoomProxy;
    QImage* zoomProxy;
    QTimer* zoomTimer;
};

ViewController::ViewController(QQuickItem* parent)
    : QQuickPaintedItem{parent}, d{new Private}
{
    d->zoomTimer = new QTimer{this};
    d->zoomTimer->setInterval(500);
    d->zoomTimer->setSingleShot(true);
    connect(d->zoomTimer, &QTimer::timeout, this, &ViewController::zoomTimeout);
}

ViewController::~ViewController()
{
    delete d;
}

void ViewController::paint(QPainter* painter)
{
    if(d->zoomProxy) {
        QPoint contentPos{d->flickable->property("contentX").toInt(), d->flickable->property("contentY").toInt()};
        QRect targetRect{contentPos, QSize{int(d->flickable->width() * (1.f + d->zoomChange)), int(d->flickable->height() * (1.f + d->zoomChange))}};
        painter->drawImage(targetRect, *(d->zoomProxy));
    }
}

View* ViewController::view() const
{
    return d->view;
}

void ViewController::setView(View* newView)
{
    if(newView != d->view) {
        if(d->view) {
            if(d->view->document()) {
                if(d->canvasController) {
                    disconnect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::moveDocumentOffset, this, &ViewController::documentOffsetChanged);
                }
                d->view->document()->disconnect(this);
            }
            disconnect(d->view, &View::documentChanged, this, &ViewController::documentChanged);
        }

        d->view = newView;
        connect(d->view, &View::documentChanged, this, &ViewController::documentChanged);

        if(d->view->document()) {
            documentChanged();
        } else {
            d->canvasController = nullptr;
        }

        emit viewChanged();
    }
}

QQuickItem* ViewController::flickable() const
{
    return d->flickable;
}

void ViewController::setFlickable(QQuickItem* item)
{
    if(item != d->flickable) {
        if(item && item->metaObject()->indexOfProperty("contentWidth") == -1) {
            qWarning() << Q_FUNC_INFO << "Item does not seem to be a flickable, ignoring.";
            return;
        }

        d->flickable = item;

        if(item) {
            documentSizeChanged();
            connect(d->flickable, SIGNAL(contentXChanged()), this, SLOT(contentPositionChanged()) );
            connect(d->flickable, SIGNAL(contentYChanged()), this, SLOT(contentPositionChanged()) );
        }
        emit flickableChanged();
    }
}

float ViewController::minimumZoom() const
{
    return d->minimumZoom;
}

void ViewController::setMinimumZoom(float newValue)
{
    if(newValue != d->minimumZoom) {
        d->minimumZoom = newValue;
        emit minimumZoomChanged();
    }
}

bool ViewController::minimumZoomFitsWidth() const
{
    return d->minimumZoomFitsWidth;
}

void ViewController::setMinimumZoomFitsWidth(bool newValue)
{

}

float ViewController::zoom() const
{
    if(d->useZoomProxy && d->zoomProxy) {
        return d->zoom + d->zoomChange;
    }

    return d->zoom;
}

void ViewController::setZoom(float newZoom)
{
    if(newZoom != d->zoom) {
        if(d->useZoomProxy) {
            if(!d->zoomProxy) {
                d->zoomProxy = new QImage{int(d->view->width()), int(d->view->height()), QImage::Format_ARGB32};

                QPainter p;
                p.begin(d->zoomProxy);
                d->view->paint(&p);
                p.end();
            }

            d->zoomChange = newZoom - d->zoom;
            update();
            d->zoomTimer->start();
        } else {
            d->zoom = newZoom;
            d->view->setZoom(d->zoom);
        }

        emit zoomChanged();
    }
}

float ViewController::maximumZoom() const
{
    return d->maximumZoom;
}

void ViewController::setMaximumZoom(float newValue)
{
    if(newValue != d->maximumZoom) {
        d->maximumZoom = newValue;
        emit maximumZoomChanged();
    }
}

bool ViewController::useZoomProxy() const
{
    return d->useZoomProxy;
}

void ViewController::setUseZoomProxy(bool proxy)
{
    if(proxy != d->useZoomProxy) {
        d->useZoomProxy = proxy;

        if(!d->useZoomProxy && d->zoomProxy) {
            delete d->zoomProxy;
            d->zoomProxy = nullptr;
            update();
        }

        emit useZoomProxyChanged();
    }
}

void ViewController::zoomAroundPoint(float amount, const QPointF& point)
{

}

void ViewController::zoomToFitWidth(float width)
{
    
}

void ViewController::contentPositionChanged()
{
    if(!d->canvasController || d->ignoreFlickableChange)
        return;

    float newX = d->flickable->property("contentX").toFloat();
    float newY = d->flickable->property("contentY").toFloat();

    //TODO: The rounding here causes some issues at edges. Need to investigate how to fix it.
    QPointF diff = QPointF{newX - d->lastX, newY - d->lastY};
    d->ignoreOffsetChange = true;
    d->canvasController->pan(diff.toPoint());
    d->ignoreOffsetChange = false;

    d->lastX = newX;
    d->lastY = newY;
}

void ViewController::documentChanged()
{
    connect(d->view->document(), &Document::statusChanged, this, &ViewController::documentStatusChanged);
    connect(d->view->document(), &Document::documentSizeChanged, this, &ViewController::documentSizeChanged);
    documentStatusChanged();
    documentSizeChanged();
}

void ViewController::documentSizeChanged()
{
    if(d->view && d->view->document() && d->flickable) {
        if(!d->canvasController) {
            d->canvasController = d->view->document()->canvasController();
        }

        QSizeF docSize = d->view->document()->documentSize();
        d->flickable->setProperty("contentWidth", docSize.width());
        d->flickable->setProperty("contentHeight", docSize.height());
    }
}

void ViewController::documentStatusChanged()
{
    if(d->view->document()->status() == DocumentStatus::Loaded) {
        d->canvasController = d->view->document()->canvasController();
        connect(d->canvasController->proxyObject, &KoCanvasControllerProxyObject::moveDocumentOffset, this, &ViewController::documentOffsetChanged);
    }
}

void ViewController::documentOffsetChanged(const QPoint& offset)
{
    if(d->ignoreOffsetChange || !d->flickable) {
        return;
    }

    d->ignoreFlickableChange = true;
    d->flickable->setProperty("contentX", offset.x());
    d->flickable->setProperty("contentY", offset.y());
    d->ignoreFlickableChange = false;

    d->lastX = offset.x();
    d->lastY = offset.y();

    QMetaObject::invokeMethod(d->flickable, "returnToBounds");
}

void ViewController::zoomTimeout()
{
    delete d->zoomProxy;
    d->zoomProxy = nullptr;
    d->zoom = d->zoom + d->zoomChange;
    d->view->setZoom(d->zoom);
    update();
}
