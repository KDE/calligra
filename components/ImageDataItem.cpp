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

#include "ImageDataItem.h"

#include <QtGui/QPixmap>
#include <QtQuick/QSGSimpleTextureNode>
#include <QQuickWindow>

using namespace Calligra::Components;

class ImageDataItem::Private
{
public:
    Private()
    { }

    QImage data;
};

ImageDataItem::ImageDataItem(QQuickItem* parent)
    : QQuickItem{parent}, d{new Private}
{
    setFlag(QQuickItem::ItemHasContents, true);
}

ImageDataItem::~ImageDataItem()
{
    delete d;
}

QImage ImageDataItem::data() const
{
    return d->data;
}

void ImageDataItem::setData(const QImage& newValue)
{
    if(newValue != d->data) {
        d->data = newValue;
        update();
        emit dataChanged();
    }
}

QSGNode* ImageDataItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*)
{
    if(d->data.isNull()) {
        return node;
    }

    auto texNode = static_cast<QSGSimpleTextureNode*>(node);
    if(!texNode) {
        texNode = new QSGSimpleTextureNode{};
    }
    texNode->setRect(0, 0, width(), height());

    auto texture = window()->createTextureFromImage(d->data);
    if(texNode->texture()) {
        delete texNode->texture();
    }
    texNode->setTexture(texture);

    return texNode;
}
