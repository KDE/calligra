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

#ifndef CALLIGRA_COMPONENTS_IMAGEDATAITEM_H
#define CALLIGRA_COMPONENTS_IMAGEDATAITEM_H

#include <QPixmap>
#include <QQuickItem>

namespace Calligra {
namespace Components {

/**
 * \brief This provides a simple item that can be used to render Pixmaps.
 *
 */

class ImageDataItem : public QQuickItem
{
    Q_OBJECT

    /**
     * \property data
     * \brief The data this item needs to display.
     *
     * \default null
     * \get data() const
     * \set setData()
     * \notify dataChanged()
     */
    Q_PROPERTY(QImage data READ data WRITE setData NOTIFY dataChanged)

public:
    explicit ImageDataItem(QQuickItem* parent = 0);
    ~ImageDataItem() override;
    
    /**
     * Getter for property #data.
     */
    QImage data() const;
    /**
     * Setter for property #data.
     */
    void setData(const QImage& newValue);

Q_SIGNALS:
    void dataChanged();

protected:
    QSGNode* updatePaintNode(QSGNode* node, QQuickItem::UpdatePaintNodeData*) override;

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_PIXMAPITEM_H
