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
#include <kis_view2.h>

bool categoryLessThan(const FiltersModel* s1, const FiltersModel* s2)
{
    return s1->categoryName.toLower() < s2->categoryName.toLower();
}

class FiltersCategoryModel::Private
{
public:
    Private(FiltersCategoryModel* qq)
        : q(qq)
        , currentCategory(-1)
        , view(0)
    {}

    FiltersCategoryModel* q;
    int currentCategory;
    KisView2* view;
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

    void refreshContents()
    {
        q->beginResetModel();
        qDeleteAll(categories);
        categories.clear();
        QList<KisFilterSP> filters = KisFilterRegistry::instance()->values();
        QList<QString> tmpCategoryIDs;
        foreach(const KisFilterSP filter, filters) {
            Q_ASSERT(filter);
            FiltersModel* cat = 0;
            if (!tmpCategoryIDs.contains(filter->menuCategory().id())) {
                cat = new FiltersModel(q);
                cat->categoryId = filter->menuCategory().id();
                cat->categoryName = filter->menuCategory().name();
                cat->setView(view);
                categories << cat;
                tmpCategoryIDs << filter->menuCategory().id();
            }
            else
                cat = categoryByName(filter->menuCategory().id());
            cat->addFilter(filter);
        }
        qSort(categories.begin(), categories.end(), categoryLessThan);
        q->endResetModel();
    }
};

FiltersCategoryModel::FiltersCategoryModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
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
    if(d->currentCategory == -1)
        return 0;
    return d->categories[d->currentCategory];
}

void FiltersCategoryModel::activateItem(int index)
{
    if(index > -1 && index < d->categories.count())
    {
        d->currentCategory = index;
        emit filterModelChanged();
    }
}

QObject* FiltersCategoryModel::view() const
{
    return d->view;
}

void FiltersCategoryModel::setView(QObject* newView)
{
    d->view = qobject_cast<KisView2*>( newView );
    if(d->view)
        d->refreshContents();
    emit viewChanged();
}

#include "FiltersCategoryModel.moc"
