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

#include "View.h"

#include <QTimer>
#include <QPainter>
#include <QQuickWindow>
#include <QGraphicsWidget>
#include <QStyleOptionGraphicsItem>

#include "Document.h"
#include <KoCanvasController.h>
#include <KoZoomController.h>

using namespace Calligra::Components;

class View::Private
{
public:
    Private(View* qq) : q{qq}, document{nullptr}, canvas{nullptr}
    { }

    void updateCanvas();

    View* q;

    Document* document;
    QGraphicsWidget* canvas;

    QTimer updateTimer;
};

View::View(QQuickItem* parent)
    : QQuickPaintedItem{parent}, d{new Private{this}}
{
}

View::~View()
{
    delete d;
}

void View::paint(QPainter* painter)
{
    if(!d->document || !d->canvas) {
        return;
    }

    //TODO: This should probably be double-buffered to prevent flickering
    QStyleOptionGraphicsItem option;
    option.exposedRect = QRectF{0, 0, width(), height()};
    option.rect = option.exposedRect.toAlignedRect();
    d->canvas->paint(painter, &option);
}

Document* View::document() const
{
    return d->document;
}

void View::setDocument(Document* newValue)
{
    if(newValue != d->document) {
        if(d->document) {
            disconnect(d->document, SIGNAL(requestViewUpdate()), this, SLOT(update()));
        }

        d->document = newValue;
        connect(d->document, &Document::statusChanged, [&]() { d->updateCanvas(); });
        connect(d->document, SIGNAL(requestViewUpdate()), this, SLOT(update()));

        d->updateCanvas();
        emit documentChanged();
    }
}

float View::zoom() const
{
    if(d->document && d->document->zoomController()) {
        return d->document->zoomController()->zoomAction()->effectiveZoom();
    }

    return -1.f;
}

void View::setZoom(float newValue)
{
    if(zoom() == newValue)
        return;

    if(d->document && d->document->zoomController()) {
        d->document->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, newValue);
        update();
    }
}

void View::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    if (d->canvas) {
        d->canvas->setGeometry(newGeometry);
    }
    QQuickPaintedItem::geometryChanged(newGeometry, oldGeometry);
}

void View::Private::updateCanvas()
{
    if(document && document->status() == DocumentStatus::Loaded) {
        canvas = document->canvas();
        canvas->setGeometry(0, 0, q->width(), q->height());
        q->update();
    } else {
        canvas = nullptr;
    }
}
