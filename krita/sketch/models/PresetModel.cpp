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

#include "PresetModel.h"

#include <KoResourceServerAdapter.h>
#include <ui/kis_resource_server_provider.h>
#include <kis_view2.h>
#include <kis_canvas_resource_provider.h>
#include <image/brushengine/kis_paintop_preset.h>

class PresetModel::Private {
public:
    Private()
        : view(0)
    {
        rserver = KisResourceServerProvider::instance()->paintOpPresetServer();
    }

    KoResourceServer<KisPaintOpPreset> * rserver;
    KisView2* view;
};

PresetModel::PresetModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TextRole] = "text";
    setRoleNames(roles);
}

PresetModel::~PresetModel()
{
    delete d;
}

int PresetModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return d->rserver->resources().count();
}

QVariant PresetModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if (index.isValid())
    {
        switch(role)
        {
        case ImageRole:
            result = QString("image://presetthumb/%1").arg(index.row());
            break;
        case TextRole:
            result = d->rserver->resources().at(index.row())->name();
            break;
        default:
            result = "";
            break;
        }
    }
    return result;
}

QVariant PresetModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    QVariant result;
    if (section == 0)
    {
        switch(role)
        {
        case ImageRole:
            result = QString("Thumbnail");
            break;
        case TextRole:
            result = QString("Name");
            break;
        default:
            result = "";
            break;
        }
    }
    return result;
}

QObject* PresetModel::view() const
{
    return d->view;
}

void PresetModel::setView(QObject* newView)
{
    d->view = qobject_cast<KisView2*>( newView );
    emit viewChanged();
}

void PresetModel::activatePreset(int index)
{
    if ( !d->view )
        return;

    qDebug() << "index" << index;

    QList<KisPaintOpPreset*> resources = d->rserver->resources();
    if (index >= 0 && index < resources.count())  {
        d->view->resourceProvider()->setPaintOpPreset( resources.at( index ) );
    }
}

#include "PresetModel.moc"
