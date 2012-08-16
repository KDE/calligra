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

#include "palettecolorsmodel.h"

class PaletteColorsModel::Private {
public:
    Private() {}
};

PaletteColorsModel::PaletteColorsModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
}

PaletteColorsModel::~PaletteColorsModel()
{
    delete d;
}

int PaletteColorsModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return 0;
}

QVariant PaletteColorsModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    return result;
}

QVariant PaletteColorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;
    return result;
}

#include "palettecolorsmodel.moc"
