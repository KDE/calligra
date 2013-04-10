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

#include "CQThumbnailItem.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

class CQThumbnailItem::Private
{
public:
    Private() : contentWidth(-1), contentHeight(-1) { }

    QPixmap content;
    QString source;
    int contentWidth;
    int contentHeight;
};

CQThumbnailItem::CQThumbnailItem(QDeclarativeItem* parent)
    : QDeclarativeItem(parent), d(new Private)
{
    setFlag(QGraphicsItem::QGraphicsItem::ItemHasNoContents, false);
}

CQThumbnailItem::~CQThumbnailItem()
{
    delete d;
}

void CQThumbnailItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* )
{
    if(!d->content.isNull()) {
        painter->setClipRect(option->rect);
        painter->drawPixmap(0, 0, width(), height(), d->content);
    }
}

QPixmap CQThumbnailItem::content() const
{
    return d->content;
}

void CQThumbnailItem::setContent(const QPixmap& content)
{
    d->content = content;
    emit contentChanged();
    update();
}
