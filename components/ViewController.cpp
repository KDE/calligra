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
#include <KoToolManager.h>

#include <QDebug>
#include <QTimer>
#include <QQuickWindow>
#include <QPainter>
#include <QSGTransformNode>
#include <QSGSimpleTextureNode>

#include <KoCanvasController.h>
#include <KoCanvasBase.h>
#include <KoShapeManager.h>
#include <KoZoomMode.h>
#include "gemini/ViewModeSwitchEvent.h"

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

    ViewController* q;

    View* view;
    QQuickItem* flickable;

    KoCanvasController* canvasController;

    float lastX;
    float lastY;
    bool ignoreOffsetChange;
    bool ignoreFlickableChange;

    float newX;
    float newY;

    float minimumZoom;
    bool minimumZoomFitsWidth;
    float zoom;
    float zoomChange;
    float maximumZoom;

    bool useZoomProxy;
    QImage* zoomProxy;
    QTimer* zoomTimer;
    QVector3D zoomCenter;

    QSizeF documentSize;
};

ViewController::ViewController(QQuickItem* parent)
    : QQuickItem{parent}, d{new Private}
{
    setFlag(QQuickItem::ItemHasContents, true);

    KoZoomMode::setMinimumZoom(d->minimumZoom);
    KoZoomMode::setMaximumZoom(d->maximumZoom);
    
    d->zoomTimer = new QTimer{this};
    d->zoomTimer->setInterval(500);
    d->zoomTimer->setSingleShot(true);
    connect(d->zoomTimer, &QTimer::timeout, this, &ViewController::zoomTimeout);
}

ViewController::~ViewController()
{
    delete d;
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

        flickableWidthChanged();

        d->flickable = item;

        if(item) {
            documentSizeChanged();
            connect(d->flickable, SIGNAL(contentXChanged()), this, SLOT(contentPositionChanged()) );
            connect(d->flickable, SIGNAL(contentYChanged()), this, SLOT(contentPositionChanged()) );
            connect(d->flickable, &QQuickItem::widthChanged, this, &ViewController::flickableWidthChanged);
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
        KoZoomMode::setMinimumZoom(newValue);
        emit minimumZoomChanged();
    }
}

bool ViewController::minimumZoomFitsWidth() const
{
    return d->minimumZoomFitsWidth;
}

void ViewController::setMinimumZoomFitsWidth(bool newValue)
{
    if(newValue != d->minimumZoomFitsWidth) {
        d->minimumZoomFitsWidth = newValue;

        flickableWidthChanged();

        emit minimumZoomFitsWidthChanged();
    }
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
    newZoom = qBound(d->minimumZoom, newZoom, d->maximumZoom);
    if(newZoom != d->zoom) {
        if(d->useZoomProxy && d->view) {
            if(!d->zoomProxy) {
                d->zoomProxy = new QImage{int(d->flickable->width()), int(d->flickable->height()), QImage::Format_ARGB32};

                QPainter p;
                p.begin(d->zoomProxy);
                d->view->paint(&p);
                p.end();

                d->view->setVisible(false);
            }

            if(d->zoomCenter.isNull()) {
                d->zoomCenter = QVector3D{ float(d->flickable->width()) / 2.f, float(d->flickable->height()) / 2.f, 0.f };
            }
            d->zoomChange = newZoom - d->zoom;
            update();
            d->zoomTimer->start();
        } else {
            d->zoom = newZoom;

            if(d->view) {
                d->view->setZoom(d->zoom);
            }
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
        KoZoomMode::setMaximumZoom(newValue);
        emit maximumZoomChanged();
    }
}

bool ViewController::useZoomProxy() const
{    void updateMinimumZoom();

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

bool ViewController::event(QEvent* event)
{
    switch(static_cast<int>(event->type())) {
        case ViewModeSwitchEvent::AboutToSwitchViewModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            if (d->canvasController) {
                syncObject->scrollBarValue = d->canvasController->documentOffset();
                syncObject->zoomLevel = zoom();
                syncObject->activeToolId = KoToolManager::instance()->activeToolId();
                syncObject->shapes = d->canvasController->canvas()->shapeManager()->shapes();
                syncObject->currentIndex = d->view->document()->currentIndex();
                syncObject->initialized = true;
            }

            return true;
        }
        case ViewModeSwitchEvent::SwitchedToTouchModeEvent: {
            ViewModeSynchronisationObject* syncObject = static_cast<ViewModeSwitchEvent*>(event)->synchronisationObject();

            if (d->canvasController && syncObject->initialized) {
                d->canvasController->canvas()->shapeManager()->setShapes(syncObject->shapes);

                KoToolManager::instance()->switchToolRequested("PageToolFactory_ID");
                qApp->processEvents();

                setZoom(syncObject->zoomLevel);

                qApp->processEvents();
                if(syncObject->scrollBarValue.isNull()) {
                    d->view->document()->setCurrentIndex(syncObject->currentIndex);
                }
                else {
                    d->canvasController->setScrollBarValue(syncObject->scrollBarValue);
                }
                emit d->view->document()->requestViewUpdate();
            }

            return true;
        }
    }
    return QQuickItem::event(event);
}

void ViewController::zoomAroundPoint(float amount, float x, float y)
{
    d->zoomCenter = QVector3D{-1 * x, y, 0.f};
    setZoom(zoom() + amount);
}

void ViewController::zoomToFitWidth(float width)
{
    if( width < 0.01f )
        return;

    if( d->zoom < 0.01f )
        return;

    if( d->documentSize.width() > 0.f && d->documentSize.width() < 2e6 )
        setZoom( width / ( d->documentSize.width() / d->zoom ) );
}

QSGNode* ViewController::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData* )
{
    if(!d->zoomProxy) {
        if(node) {
            delete node;
        }
        return 0;
    }

    auto root = static_cast<QSGTransformNode*>(node);
    if(!root) {
        root = new QSGTransformNode{};
    }

    QMatrix4x4 itemToView;
    itemToView.translate(QVector3D{d->flickable->property("contentX").toFloat(), d->flickable->property("contentY").toFloat(), 0.f} + d->zoomCenter);
    root->setMatrix(itemToView);

    auto center = static_cast<QSGTransformNode*>(root->firstChild());
    if(!center) {
        center = new QSGTransformNode{};
        root->appendChildNode(center);
    }

    float newScale = 1.f + d->zoomChange;

    QMatrix4x4 centerToView;
    centerToView.scale(newScale);

    float newWidth = d->zoomProxy->width() * newScale;
    float newHeight = d->zoomProxy->height() * newScale;

    float left = -newWidth * (d->zoomCenter.x() / newWidth);
    float top = -newHeight * (d->zoomCenter.y() / newHeight);

    d->newX = -left;
    d->newY = -top;

    centerToView.translate(left, top);
    center->setMatrix(centerToView);

    auto texNode = static_cast<QSGSimpleTextureNode*>(center->firstChild());
    if(!texNode)     {
        texNode = new QSGSimpleTextureNode{};
        center->appendChildNode(texNode);
    }
    texNode->setRect(d->zoomProxy->rect());

    auto texture = window()->createTextureFromImage(*d->zoomProxy);
    if(texNode->texture()) {
        delete texNode->texture();
    }
    texNode->setTexture(texture);

    return root;
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

    emit d->view->document()->requestViewUpdate();
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

        d->documentSize = d->view->document()->documentSize();
        //Limit the size of this item to always be at least the same size
        //as the flickable, since otherwise we can end up with situations
        //where children cannot interact, for example when using the
        //LinkArea as a child of this item.
        setWidth(qMax(d->flickable->width() - 1, d->documentSize.width()));
        setHeight(qMax(d->flickable->height() - 1, d->documentSize.height()));

        d->flickable->setProperty("contentWidth", width());
        d->flickable->setProperty("contentHeight", height());

        flickableWidthChanged();
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

    float newZoom = d->zoom + d->zoomChange;

    float oldX = d->flickable->property("contentX").toReal();
    float oldY = d->flickable->property("contentY").toReal();


    float z = 1.0 + d->zoomChange;
    d->flickable->setProperty("contentX", oldX + ((d->zoomCenter.x() * z - d->zoomCenter.x())) );
    d->flickable->setProperty("contentY", oldY + ((d->zoomCenter.y() * z - d->zoomCenter.y())) );

    QMetaObject::invokeMethod(d->flickable, "returnToBounds");

    d->zoom = newZoom;

    d->ignoreOffsetChange = true;
    d->view->setZoom(newZoom);
    d->ignoreOffsetChange = false;

    d->view->setVisible(true);
    d->zoomCenter = QVector3D{};
    update();
}

void ViewController::flickableWidthChanged()
{
    if(d->minimumZoomFitsWidth && d->flickable && d->documentSize.width() > 0.f) {
        setMinimumZoom(d->flickable->width() / (d->documentSize.width() / d->zoom));
        setZoom(d->zoom);
    }
}
