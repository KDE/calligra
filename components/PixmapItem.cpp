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

#include "PixmapItem.h"

#include <QtGui/QPixmap>

using namespace Calligra::Components;

class PixmapItem::Private
{
public:
    Private()
    { }

    QPixmap contents;
};

PixmapItem::PixmapItem(QQuickItem* parent)
    : QQuickItem{parent}, d{new Private}
{

}

PixmapItem::~PixmapItem()
{
    delete d;
}

QPixmap PixmapItem::contents() const
{
    return d->contents;
}

void PixmapItem::setContents(const QPixmap& content)
{
    d->contents = content;
    emit contentsChanged();
}
