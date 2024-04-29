/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#include "ImageDataItem.h"

#include <QPixmap>
#include <QQuickWindow>
#include <QSGSimpleTextureNode>

using namespace Calligra::Components;

class ImageDataItem::Private
{
public:
    Private()
        : imageChanged(false)
    {
    }

    QImage data;
    bool imageChanged;
};

ImageDataItem::ImageDataItem(QQuickItem *parent)
    : QQuickItem{parent}
    , d{new Private}
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

void ImageDataItem::setData(const QImage &newValue)
{
    if (newValue != d->data) {
        d->data = newValue;
        setImplicitWidth(d->data.width());
        setImplicitHeight(d->data.height());
        d->imageChanged = true;
        update();
        Q_EMIT dataChanged();
    }
}

QSGNode *ImageDataItem::updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *)
{
    if (d->data.isNull()) {
        return node;
    }

    float w = widthValid() ? width() : d->data.width();
    float h = heightValid() ? height() : d->data.height();

    auto texNode = static_cast<QSGSimpleTextureNode *>(node);
    if (!texNode) {
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
