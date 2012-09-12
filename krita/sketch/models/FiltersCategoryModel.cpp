/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  Dan Leinir Turthra Jensen <admin@leinir.dk>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#include "FiltersCategoryModel.h"

class FiltersCategoryModel::Private
{
public:
    Private() {}
};

FiltersCategoryModel::FiltersCategoryModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{

}

FiltersCategoryModel::~FiltersCategoryModel()
{
    delete d;
}

QVariant FiltersCategoryModel::data(const QModelIndex& index, int role) const
{
    QVariant data;
    if(index.isValid())
    {
        switch(role)
        {
            default:
                break;
        }
    }
    return data;
}

int FiltersCategoryModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
        return 0;
    return 0;
}

QObject* FiltersCategoryModel::filterModel() const
{
    return 0;
}

#include "FiltersCategoryModel.moc"
