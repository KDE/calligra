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

#include "ViewController.h"

#include <QtCore/QDebug>
#include <QQuickWindow>

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
        , minimumZoom{.5f}
        , minimumZoomFitsWidth{false}
        , zoom{1.f}
        , maximumZoom{2.f}
        , useZoomProxy{true}
    { }

    View* view;
    QQuickItem* flickable;

    KoCanvasController* canvasController;

    float lastX;
    float lastY;

    float minimumZoom;
    bool minimumZoomFitsWidth;
    float zoom;
    float maximumZoom;
    bool useZoomProxy;
};

ViewController::ViewController(QQuickItem* parent)
    : QQuickPaintedItem{parent}, d{new Private}
{
}

ViewController::~ViewController()
{
    delete d;
}

void ViewController::paint(QPainter* painter)
{

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
                disconnect(d->view->document(), &Document::documentSizeChanged, this, &ViewController::documentSizeChanged);
            }
            disconnect(d->view, &View::documentChanged, this, &ViewController::documentChanged);
        }

        d->view = newView;
        connect(d->view, &View::documentChanged, this, &ViewController::documentChanged);

        if(d->view->document()) {
            d->canvasController = d->view->document()->canvasController();
            connect(d->view->document(), &Document::documentSizeChanged, this, &ViewController::documentSizeChanged);
            documentSizeChanged();
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
        if(item->metaObject()->indexOfProperty("contentWidth") == -1) {
            qWarning() << Q_FUNC_INFO << "Item does not seem to be a flickable, ignoring.";
            return;
        }

        d->flickable = item;
        documentSizeChanged();
        connect(d->flickable, SIGNAL(contentXChanged()), this, SLOT(contentPositionChanged()) );
        connect(d->flickable, SIGNAL(contentYChanged()), this, SLOT(contentPositionChanged()) );
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
    return d->zoom;
}

void ViewController::setZoom(float newZoom)
{
    if(newZoom != d->zoom) {
        d->zoom = newZoom;

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
    if(!d->canvasController)
        return;

    float newX = d->flickable->property("contentX").toFloat();
    float newY = d->flickable->property("contentY").toFloat();

    //TODO: The rounding here causes some issues at edges. Need to investigate how to fix it.
    QPointF diff = QPointF{newX - d->lastX, newY - d->lastY};
    d->canvasController->pan(diff.toPoint());

    d->lastX = newX;
    d->lastY = newY;
}

void ViewController::documentChanged()
{
    d->canvasController = d->view->document()->canvasController();
    connect(d->view->document(), &Document::documentSizeChanged, this, &ViewController::documentSizeChanged);
    documentSizeChanged();
}

void ViewController::documentSizeChanged()
{
    if(d->view && d->view->document() && d->flickable) {
        QSizeF docSize = d->view->document()->documentSize();
        d->flickable->setProperty("contentWidth", docSize.width());
        d->flickable->setProperty("contentHeight", docSize.height());
    }
}
