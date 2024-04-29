/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "LinkArea.h"

using namespace Calligra::Components;

class LinkArea::Private
{
public:
    Private()
        : document{nullptr}
        , controllerZoom(1.f)
        , clickInProgress(false)
        , wiggleFactor(2)
    {
    }

    Calligra::Components::Document *document;
    float controllerZoom;

    bool clickInProgress;
    QPoint clickLocation;
    int wiggleFactor;
};

LinkArea::LinkArea(QQuickItem *parent)
    : QQuickItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
}

LinkArea::~LinkArea()
{
    delete d;
}

Calligra::Components::Document *LinkArea::document() const
{
    return d->document;
}

void LinkArea::setDocument(Calligra::Components::Document *newDocument)
{
    if (newDocument != d->document) {
        d->document = newDocument;
        Q_EMIT documentChanged();
    }
}

void LinkArea::mousePressEvent(QMouseEvent *event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void LinkArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (!d->clickInProgress)
        return;
    d->clickInProgress = false;

    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if (!rect.contains(event->pos())) {
        return;
    }

    QPoint pos = event->pos() / d->controllerZoom;
    QUrl url;
    if (d->document)
        url = d->document->urlAtPoint(pos);

    if (url.isEmpty()) {
        Q_EMIT clicked();
    } else {
        Q_EMIT linkClicked(url);
    }
    event->accept();
}

void LinkArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    Q_EMIT doubleClicked();
}

float LinkArea::controllerZoom() const
{
    return d->controllerZoom;
}

void LinkArea::setControllerZoom(float newZoom)
{
    if (d->controllerZoom != newZoom) {
        d->controllerZoom = newZoom;
        Q_EMIT controllerZoomChanged();
    }
}
