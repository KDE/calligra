/* This file is part of the KDE project
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#ifndef PALETTECOLORSMODEL_H
#define PALETTECOLORSMODEL_H

#include <QAbstractListModel>

class PaletteColorsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QObject* colorSet READ colorSet WRITE setColorSet NOTIFY colorSetChanged)
public:
    enum PaletteColorsRoles
    {
        ImageRole = Qt::UserRole + 1,
        TextRole
    };

    explicit PaletteColorsModel(QObject *parent = 0);
    virtual ~PaletteColorsModel();

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

public Q_SLOTS:
    void setColorSet(QObject* newColorSet);
    QObject* colorSet() const;

Q_SIGNALS:
    void colorSetChanged();

private:
    class Private;
    Private* d;
};

#endif // PALETTECOLORSMODEL_H
