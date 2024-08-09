/*
 * This file is part of the KDE project
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "CQThumbnailItem.h"

#include <QPainter>

class CQThumbnailItem::Private
{
public:
    Private()
        : contentWidth(-1)
        , contentHeight(-1)
    {
    }

    QPixmap content;
    QString source;
    int contentWidth;
    int contentHeight;
};

CQThumbnailItem::CQThumbnailItem(QQuickItem *parent)
    : QQuickPaintedItem(parent)
    , d(new Private)
{
}

CQThumbnailItem::~CQThumbnailItem()
{
    delete d;
}

void CQThumbnailItem::paint(QPainter *painter)
{
    if (!d->content.isNull()) {
        QPixmap pixmap = d->content.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int xpos = (width() - pixmap.width()) / 2;
        int ypos = (height() - pixmap.height()) / 2;
        painter->drawPixmap(xpos, ypos, pixmap);
    }
}

QPixmap CQThumbnailItem::content() const
{
    return d->content;
}

void CQThumbnailItem::setContent(const QPixmap &content)
{
    d->content = content;
    Q_EMIT contentChanged();
    update();
}
