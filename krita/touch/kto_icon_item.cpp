/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

#include "kto_icon_item.h"
#include <QPainter>

KtoIconItem::KtoIconItem(QDeclarativeItem* parent): QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

KtoIconItem::~KtoIconItem()
{

}

void KtoIconItem::setImage(const QImage& _image)
{
    m_image = _image;
    update();
}

void KtoIconItem::paint(QPainter* _painter, const QStyleOptionGraphicsItem* , QWidget* )
{
    _painter->drawImage(QRect(0, 0, width(), height()), m_image);
}

#include "kto_icon_item.moc"
