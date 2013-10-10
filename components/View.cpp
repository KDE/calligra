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

#include "View.h"

#include <QtCore/QTimer>
#include <QtGui/QPainter>
#include <QtQuick/QQuickWindow>
#include <QtWidgets/QGraphicsWidget>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include "Document.h"

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
    //Workaround because there is no "repaint" signal in Calligra
    d->updateTimer.setInterval(1000);
    d->updateTimer.setSingleShot(true);
    connect(&d->updateTimer, &QTimer::timeout, [&]() { update(); });
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

    QStyleOptionGraphicsItem option;
    option.exposedRect = QRectF{0, 0, width(), height()};
    option.rect = option.exposedRect.toAlignedRect();
    d->canvas->paint(painter, &option);
}

Document* View::document() const
{
    return d->document;
}

void View::setDocument(Document* newDocument)
{
    if(newDocument != d->document) {
        d->document = newDocument;
        connect(d->document, &Document::stateChanged, [&]() { d->updateCanvas(); });

        d->updateCanvas();
        emit documentChanged();
    }
}

void View::Private::updateCanvas()
{
    if(document && document->state() == Document::LoadedState) {
        canvas = document->canvas();
        canvas->setGeometry(0, 0, q->width(), q->height());
        updateTimer.start();
    }
}
