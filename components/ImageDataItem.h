/*
 * This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2013 Arjen Hiemstra <ahiemstra@heimr.nl>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 *
 */

#ifndef CALLIGRA_COMPONENTS_IMAGEDATAITEM_H
#define CALLIGRA_COMPONENTS_IMAGEDATAITEM_H

#include <QPixmap>
#include <QQuickItem>

namespace Calligra
{
namespace Components
{

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
    explicit ImageDataItem(QQuickItem *parent = nullptr);
    ~ImageDataItem() override;

    /**
     * Getter for property #data.
     */
    QImage data() const;
    /**
     * Setter for property #data.
     */
    void setData(const QImage &newValue);

Q_SIGNALS:
    void dataChanged();

protected:
    QSGNode *updatePaintNode(QSGNode *node, QQuickItem::UpdatePaintNodeData *) override;

private:
    class Private;
    Private *const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_PIXMAPITEM_H
