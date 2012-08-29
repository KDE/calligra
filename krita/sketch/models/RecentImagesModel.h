/* This file is part of the KDE project
 * Copyright (C) 2012 Boudewijn Rempt <boud@kogmbh.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef RECENTIMAGESMODEL_H
#define RECENTIMAGESMODEL_H

#include <QAbstractListModel>


class KUrl;

class RecentImagesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PresetRoles
    {
        ImageRole = Qt::UserRole + 1,
        TextRole,
        UrlRole
    };

    explicit RecentImagesModel(QObject *parent = 0);
    virtual ~RecentImagesModel();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;


public Q_SLOTS:

    void addRecent(const KUrl&url);

private:
    class Private;
    Private* d;
};

#endif // RECENTIMAGESMODEL_H
