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

#include "palettemodel.h"

#include <KoColorSet.h>
#include <KoResourceServerAdapter.h>
#include <KoResourceServerProvider.h>
#include <ui/kis_resource_server_provider.h>

class PaletteModel::Private {
public:
    Private(QObject* q) {
        KoResourceServer<KoColorSet>* rServer = KoResourceServerProvider::instance()->paletteServer();
        serverAdaptor = new KoResourceServerAdapter<KoColorSet>(rServer, q);
        serverAdaptor->connectToResourceServer();
    }
    KoResourceServerAdapter<KoColorSet>* serverAdaptor;
};

PaletteModel::PaletteModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private(this))
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TextRole] = "text";
    setRoleNames(roles);
}

PaletteModel::~PaletteModel()
{
    delete d;
}

int PaletteModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    return d->serverAdaptor->resources().count();
}

QVariant PaletteModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    if(index.isValid())
    {
        switch(role)
        {
        case ImageRole:
            result = ":/images/help-about.png";
            break;
        case TextRole:
            result = d->serverAdaptor->resources().at(index.row())->name();
            break;
        default:
            break;
        }
    }
    return result;
}

QVariant PaletteModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    QVariant result;
    return result;
}

#include "palettemodel.moc"
