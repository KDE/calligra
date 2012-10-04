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
#include <kis_filter_handler.h>
#include <kis_view2.h>

class FiltersModel::Private
{
public:
    Private()
        : view(0)
    {};
    KisView2* view;
    QList<KisFilterSP> filters;
    QList<KisFilterHandler*> handlers;
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

bool FiltersModel::filterRequiresConfiguration(int index)
{
    if(index > -1 && index < d->filters.count())
    {
        return d->filters[index]->showConfigurationWidget();
    }
    return false;
}

QString FiltersModel::filterID(int index)
{
    if(index > -1 && index < d->filters.count())
    {
        return d->filters[index]->id();
    }
    return QLatin1String("");
}

void FiltersModel::activateFilter(int index)
{
    if(index > -1 && index < d->filters.count())
    {
        // TODO This should of course take configuration if available - see KisFilterHandler::showDialog()
        d->handlers[index]->apply(d->view->activeNode(), d->filters[index]->defaultConfiguration(d->view->activeNode()->original()));
    }
}

void FiltersModel::addFilter(KisFilterSP filter)
{
    if(!filter.isNull())
    {
        int newRow = d->filters.count();
        beginInsertRows(QModelIndex(), newRow, newRow);
        d->filters << filter;
        KisFilterManager* man = 0;
        if(d->view)
            man = d->view->filterManager();
        d->handlers << new KisFilterHandler(man, filter, d->view);
        endInsertRows();
    }
}

QObject* FiltersModel::view() const
{
    return d->view;
}

void FiltersModel::setView(QObject* newView)
{
    d->view = qobject_cast<KisView2*>( newView );
    foreach(KisFilterHandler* handler, d->handlers)
    {
        handler->setView(d->view);
    }
    emit viewChanged();
}

#include "FiltersModel.moc"
