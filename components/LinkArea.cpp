/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 * Copyright (C) 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "LinkArea.h"

using namespace Calligra::Components;

class LinkArea::Private
{
public:
    Private()
        : document{ nullptr }
        , controllerZoom(1.f)
        , clickInProgress(false)
        , wiggleFactor(2)
    { }

    Calligra::Components::Document* document;
    float controllerZoom;

    bool clickInProgress;
    QPoint clickLocation;
    int wiggleFactor;
};

LinkArea::LinkArea(QQuickItem* parent)
    : QQuickItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MidButton);
}

LinkArea::~LinkArea()
{
    delete d;
}

Calligra::Components::Document* LinkArea::document() const
{
    return d->document;
}

void LinkArea::setDocument(Calligra::Components::Document* newDocument)
{
    if( newDocument != d->document )
    {
        d->document = newDocument;
        emit documentChanged();
    }
}

void LinkArea::mousePressEvent(QMouseEvent* event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void LinkArea::mouseReleaseEvent(QMouseEvent* event)
{
    if(!d->clickInProgress)
        return;
    d->clickInProgress = false;

    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if(!rect.contains(event->pos())) {
        return;
    }

    QPoint pos = event->pos() / d->controllerZoom;
    QUrl url;
    if( d->document )
        url = d->document->urlAtPoint( pos );

    if(url.isEmpty()) {
        emit clicked();
    }
    else {
        emit linkClicked(url);
    }
    event->accept();
}

void LinkArea::mouseDoubleClickEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    emit doubleClicked();
}

float LinkArea::controllerZoom() const
{
    return d->controllerZoom;
}

void LinkArea::setControllerZoom(float newZoom)
{
    if(d->controllerZoom != newZoom)
    {
        d->controllerZoom = newZoom;
        emit controllerZoomChanged();
    }
}
