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

#ifndef CALLIGRA_COMPONENTS_CONTENTSMODEL_H
#define CALLIGRA_COMPONENTS_CONTENTSMODEL_H

#include <QAbstractListModel>
#include <QImage>

namespace Calligra {
namespace Components {
class Document;

class ContentsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(Calligra::Components::Document* document READ document WRITE setDocument NOTIFY documentChanged)
    /**
     * \property thumbnailSize
     * \brief The size of thumbnails this model creates.
     *
     * \default 128x128
     * \get thumbnailSize() const
     * \set setthumbnailSize()
     * \notify thumbnailSizeChanged()
     */
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)
    Q_PROPERTY(bool useToC READ useToC WRITE setUseToC NOTIFY useToCChanged)

public:
    enum Role {
        TitleRole = Qt::UserRole + 1,
        LevelRole,
        ThumbnailRole,
        ContentIndexRole,
    };
    
    explicit ContentsModel(QObject* parent = 0);
    ~ContentsModel() override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex& parent = QModelIndex{}) const override;

    Document* document() const;
    void setDocument(Document* newDocument);

    /**
     * Getter for property #thumbnailSize.
     */
    QSize thumbnailSize() const;
    /**
     * Setter for property #thumbnailSize.
     */
    void setThumbnailSize(const QSize& newValue);

    void setUseToC(bool newValue);
    bool useToC() const;

    /**
     * Render a thumbnail at a specified width.
     *
     * \param index The index of the item to render a thumbnail of.
     * \param width The width to render the thumbnail at.
     * \return A thumbnail or an empty image when the thumbnail could not
     * be rendered.
     */
    Q_INVOKABLE QImage thumbnail(int index, int width) const;

Q_SIGNALS:
    void documentChanged();
    /**
     * Notify signal for property #thumbnailSize.
     */
    void thumbnailSizeChanged();
    void useToCChanged();

protected:
    QHash<int, QByteArray> roleNames() const override;

private Q_SLOTS:
    void updateImpl();
    void reset();

private:
    class Private;
    Private* const d;
};

} // Namespace Components
} // Namespace Calligra

#endif // CALLIGRA_COMPONENTS_CONTENTSMODEL_H
