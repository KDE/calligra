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

#ifndef CQSPREADSHEETLISTMODEL_H
#define CQSPREADSHEETLISTMODEL_H

#include <QAbstractListModel>

class CQSpreadsheetListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* canvas READ canvas WRITE setCanvas NOTIFY canvasChanged)
    Q_PROPERTY(QSize thumbnailSize READ thumbnailSize WRITE setThumbnailSize NOTIFY thumbnailSizeChanged)

public:
    enum RoleNames {
        SheetNameRole = Qt::UserRole,
        ThumbnailRole
    };

    explicit CQSpreadsheetListModel(QObject* parent = 0);
    virtual ~CQSpreadsheetListModel();

    virtual QVariant data(const QModelIndex& index, int role) const;
    virtual int rowCount(const QModelIndex& parent) const;

    QObject* canvas() const;

    QSize thumbnailSize() const;

public Q_SLOTS:
    void setCanvas(QObject* canvas);
    void setThumbnailSize(const QSize& size);

Q_SIGNALS:
    void canvasChanged();
    void thumbnailSizeChanged();

private:
    class Private;
    Private * const d;
};

#endif // CQSPREADSHEETLISTMODEL_H
