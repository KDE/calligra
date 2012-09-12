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

#include "FiltersCategoryModel.h"
#include "FiltersModel.h"
#include <filter/kis_filter_registry.h>
#include <filter/kis_filter.h>

class FiltersCategoryModel::Private
{
public:
    Private() {}

    QList<FiltersModel*> categories;
};

bool categoryLessThan(const FiltersModel* s1, const FiltersModel* s2)
{
    return s1->categoryName.toLower() < s2->categoryName.toLower();
}

FiltersCategoryModel::FiltersCategoryModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QList<KisFilterSP> filters = KisFilterRegistry::instance()->values();
    QList<QString> tmpCategoryIDs;
    foreach(const KisFilterSP filter, filters) {
        Q_ASSERT(filter);
        if (!tmpCategoryIDs.contains(filter->menuCategory().id())) {
            FiltersModel* cat = new FiltersModel(this);
            cat->categoryId = filter->menuCategory().id();
            cat->categoryName = filter->menuCategory().name();
            d->categories << cat;
            tmpCategoryIDs << filter->menuCategory().id();
        }
        /*Private::Filter filt;
        filt.id = filter->id();
        filt.name = filter->name();
        filt.filter = filter;
        d->categories[ filter->menuCategory().id()].filters.append(filt);*/
    }
    qSort(d->categories.begin(), d->categories.end(), categoryLessThan);

    QHash<int, QByteArray> roles;
    roles[TextRole] = "text";
    setRoleNames(roles);
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
            case TextRole:
                data = d->categories[index.row()]->categoryName;
                break;
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
    return d->categories.count();
}

QObject* FiltersCategoryModel::filterModel() const
{
    return 0;
}

#include "FiltersCategoryModel.moc"
