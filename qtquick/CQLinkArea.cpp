/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "CQLinkArea.h"

#include <QMouseEvent>
#include <QPainter>
#include <qquickitem.h>

struct LinkLayerLink {
    QRectF linkRect;
    QUrl linkTarget;
};

class CQLinkArea::Private
{
public:
    Private()
        : clickInProgress(false)
        , wiggleFactor(4)
    {
    }
    QVariantList links;
    QList<LinkLayerLink> realLinks;

    bool clickInProgress;
    QPointF clickLocation;
    int wiggleFactor;

    QSizeF sourceSize;

    QColor linkColor;
};

CQLinkArea::CQLinkArea(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , d(new Private)
{
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton | Qt::MiddleButton);
    setAcceptTouchEvents(true);
}

CQLinkArea::~CQLinkArea() = default;

void CQLinkArea::paint(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::transparent);
    painter->setBrush(QBrush(d->linkColor));
    for (const LinkLayerLink &link : std::as_const(d->realLinks)) {
        QRectF target((link.linkRect.y() / d->sourceSize.height()) * height(),
                      (link.linkRect.x() / d->sourceSize.width()) * width(),
                      (link.linkRect.height() / d->sourceSize.height()) * height(),
                      (link.linkRect.width() / d->sourceSize.width()) * width());
        painter->drawRect(target);
    }
    painter->restore();
}

QVariantList CQLinkArea::links() const
{
    return d->links;
}

void CQLinkArea::setLinks(const QVariantList &newLinks)
{
    d->links = newLinks;
    // run through the new data and cache a data list with the information
    // so we don't have to interpret the QObjects all the time
    d->realLinks.clear();
    foreach (const QVariant &var, newLinks) {
        QObject *obj = var.value<QObject *>();
        if (!obj) {
            continue;
        }
        LinkLayerLink link;
        link.linkRect = obj->property("linkRect").toRectF().adjusted(-d->wiggleFactor, -d->wiggleFactor, d->wiggleFactor, d->wiggleFactor);
        link.linkTarget = obj->property("linkTarget").toUrl();
        d->realLinks.append(link);
    }
    Q_EMIT linksChanged();
}

QSizeF CQLinkArea::sourceSize() const
{
    return d->sourceSize;
}

void CQLinkArea::setSourceSize(const QSizeF &size)
{
    if (size != d->sourceSize) {
        d->sourceSize = size;
        Q_EMIT sourceSizeChanged();
        update();
    }
}

QColor CQLinkArea::linkColor() const
{
    return d->linkColor;
}

void CQLinkArea::setLinkColor(const QColor &color)
{
    if (color != d->linkColor) {
        d->linkColor = color;
        d->linkColor.setAlphaF(0.25);
        Q_EMIT linkColorChanged();
        update();
    }
}

void CQLinkArea::mousePressEvent(QMouseEvent *event)
{
    d->clickInProgress = true;
    d->clickLocation = event->pos();
}

void CQLinkArea::mouseReleaseEvent(QMouseEvent *event)
{
    d->clickInProgress = false;
    // Don't activate anything if the finger has moved too far
    QRect rect((d->clickLocation - QPointF(d->wiggleFactor, d->wiggleFactor)).toPoint(), QSize(d->wiggleFactor * 2, d->wiggleFactor * 2));
    if (!rect.contains(event->pos())) {
        return;
    }
    QUrl url;
    QPointF inverted(event->pos().y(), event->pos().x());
    foreach (const LinkLayerLink &link, d->realLinks) {
        QRectF scaledTarget((link.linkRect.x() / d->sourceSize.width()) * width(),
                            (link.linkRect.y() / d->sourceSize.height()) * height(),
                            (link.linkRect.width() / d->sourceSize.width()) * width(),
                            (link.linkRect.height() / d->sourceSize.height()) * height());

        if (scaledTarget.contains(inverted)) {
            url = link.linkTarget;
            break;
        }
    }
    if (url.isEmpty()) {
        Q_EMIT clicked();
    } else {
        Q_EMIT linkClicked(url);
    }
}

void CQLinkArea::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    Q_EMIT doubleClicked();
}
