/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include "ImageDataItem.h"

#include <QPixmap>
#include <QSGSimpleTextureNode>
#include <QQuickWindow>

using namespace Calligra::Components;

class ImageDataItem::Private
{
public:
    Private() : imageChanged(false)
    { }

    QImage data;
    bool imageChanged;
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
        setImplicitWidth(d->data.width());
        setImplicitHeight(d->data.height());
        d->imageChanged = true;
        update();
        emit dataChanged();
    }
}

QSGNode* ImageDataItem::updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*)
{
    if(d->data.isNull()) {
        return node;
    }

    float w = widthValid() ? width() : d->data.width();
    float h = heightValid() ? height() : d->data.height();

    auto texNode = static_cast<QSGSimpleTextureNode*>(node);
    if(!texNode) {
        texNode = new QSGSimpleTextureNode{};
    }
    texNode->setRect(0, 0, w, h);

    if (!texNode->texture() || d->imageChanged) {
        delete texNode->texture();
        auto texture = window()->createTextureFromImage(d->data);
        texNode->setTexture(texture);
        d->imageChanged = false;
    }

    return texNode;
}
