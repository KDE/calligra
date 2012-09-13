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
    Private()
        : currentCategory(0)
    {}

    int currentCategory;
    QList<FiltersModel*> categories;
    FiltersModel* categoryByName(const QString& name)
    {
        FiltersModel* category = 0;
        for(int i = 0; i < categories.count(); ++i)
        {
            if(categories.at(i)->categoryId == name)
            {
                category = categories[i];
                break;
            }
        }
        return category;
    }
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
        FiltersModel* cat = 0;
        if (!tmpCategoryIDs.contains(filter->menuCategory().id())) {
            cat = new FiltersModel(this);
            cat->categoryId = filter->menuCategory().id();
            cat->categoryName = filter->menuCategory().name();
            d->categories << cat;
            tmpCategoryIDs << filter->menuCategory().id();
        }
        else
            cat = d->categoryByName(filter->menuCategory().id());
        cat->addFilter(filter);
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
    return d->categories[d->currentCategory];
}

#include "FiltersCategoryModel.moc"
