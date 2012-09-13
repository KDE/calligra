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

#include "FiltersModel.h"
#include <filter/kis_filter.h>

class FiltersModel::Private
{
public:
    Private() {};
    QList<KisFilterSP> filters;
};

FiltersModel::FiltersModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    setRoleNames(roles);
}

FiltersModel::~FiltersModel()
{
    delete d;
}

QVariant FiltersModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if(index.isValid())
    {
        switch(role)
        {
            case TextRole:
                data = d->filters[index.row()]->name();
                break;
            default:
                break;
        }
    }
    return data;
}

int FiltersModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return d->filters.count();
}

void FiltersModel::addFilter(KisFilterSP filter)
{
    if(!filter.isNull())
    {
        int newRow = d->filters.count();
        beginInsertRows(QModelIndex(), newRow, newRow);
        d->filters << filter;
        endInsertRows();
    }
}

#include "FiltersModel.moc"
