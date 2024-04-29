/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "View.h"

#include <QGraphicsWidget>
#include <QPainter>
#include <QQuickWindow>
#include <QStyleOptionGraphicsItem>
#include <QTimer>

#include "Document.h"
#include <KoCanvasController.h>
#include <KoZoomController.h>

using namespace Calligra::Components;

class View::Private
{
public:
    Private(View *qq)
        : q{qq}
        , document{nullptr}
        , canvas{nullptr}
    {
    }

    void updateCanvas();

    View *q;

    Document *document;
    QGraphicsWidget *canvas;

    QTimer updateTimer;
};

View::View(QQuickItem *parent)
    : QQuickPaintedItem{parent}
    , d{new Private{this}}
{
}

View::~View()
{
    delete d;
}

void View::paint(QPainter *painter)
{
    if (!d->document || !d->canvas) {
        return;
    }

    // TODO: This should probably be double-buffered to prevent flickering
    QStyleOptionGraphicsItem option;
    option.exposedRect = QRectF{0, 0, width(), height()};
    option.rect = option.exposedRect.toAlignedRect();
    d->canvas->paint(painter, &option);
}

Document *View::document() const
{
    return d->document;
}

void View::setDocument(Document *newValue)
{
    if (newValue != d->document) {
        if (d->document) {
            disconnect(d->document, SIGNAL(requestViewUpdate()), this, SLOT(update()));
        }

        d->document = newValue;
        connect(d->document, &Document::statusChanged, this, [&]() {
            d->updateCanvas();
        });
        connect(d->document, &Document::requestViewUpdate, this, [&]() {
            update();
        });

        d->updateCanvas();
        Q_EMIT documentChanged();
    }
}

float View::zoom() const
{
    if (d->document && d->document->zoomController()) {
        return d->document->zoomController()->zoomAction()->effectiveZoom();
    }

    return -1.f;
}

void View::setZoom(float newValue)
{
    if (zoom() == newValue)
        return;

    if (d->document && d->document->zoomController()) {
        d->document->zoomController()->setZoom(KoZoomMode::ZOOM_CONSTANT, newValue);
        update();
    }
}

void View::geometryChange(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (d->canvas) {
        d->canvas->setGeometry(newGeometry);
    }
    QQuickPaintedItem::geometryChange(newGeometry, oldGeometry);
}

void View::Private::updateCanvas()
{
    if (document && document->status() == DocumentStatus::Loaded) {
        canvas = document->canvas();
        canvas->setGeometry(0, 0, q->width(), q->height());
        q->update();
    } else {
        canvas = nullptr;
    }
}
