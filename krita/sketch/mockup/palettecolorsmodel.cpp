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

#include <KoColorSet.h>

class PaletteColorsModel::Private {
public:
    Private()
        : colorSet(0)
    {}

    KoColorSet* colorSet;
};

PaletteColorsModel::PaletteColorsModel(QObject *parent)
    : QAbstractListModel(parent)
    , d(new Private)
{
    QHash<int, QByteArray> roles;
    roles[ImageRole] = "image";
    roles[TextRole] = "text";
    setRoleNames(roles);
}

PaletteColorsModel::~PaletteColorsModel()
{
    delete d;
}

int PaletteColorsModel::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return 0;
    if(!d->colorSet)
        return 0;
    return d->colorSet->nColors();
}

QVariant PaletteColorsModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    QColor color;
    if(index.isValid() && d->colorSet)
    {
        switch(role)
        {
        case ImageRole:
            color = d->colorSet->getColor(index.row()).color.toQColor();
            result = QString("image://color/%1,%2,%3,%4").arg(color.redF()).arg(color.greenF()).arg(color.blueF()).arg(color.alphaF());
            break;
        case TextRole:
            result = d->colorSet->getColor(index.row()).name;
            break;
        default:
            break;
        }
    }
    return result;
}

QVariant PaletteColorsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(orientation);
    QVariant result;
    if(section == 0)
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
            break;
        }
    }
    return result;
}

void PaletteColorsModel::setColorSet(QObject *newColorSet)
{
    d->colorSet = qobject_cast<KoColorSet*>(newColorSet);
    reset();
    emit colorSetChanged();
}

QObject* PaletteColorsModel::colorSet() const
{
    return d->colorSet;
}

#include "palettecolorsmodel.moc"
