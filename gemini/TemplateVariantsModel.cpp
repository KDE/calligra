/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include "TemplateVariantsModel.h"
#include <QColor>
#include <QDebug>
#include <KIconLoader>
#include <kicontheme.h>
struct TemplateVariant {
    TemplateVariant()
    {}

    QString name;
    QColor color;
    QString thumbnail;
    QString swatch;
    QString url;
};

class TemplateVariantsModel::Private
{
public:
    Private() {}
    ~Private()
    {
        qDeleteAll(entries);
    }

    QList<TemplateVariant*> entries;
};

TemplateVariantsModel::TemplateVariantsModel(QObject* parent)
    : QAbstractListModel(parent)
    , d(new Private())
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "text";
    roles[ColorRole] = "color";
    roles[ThumbnailRole] = "thumbnail";
    roles[SwatchRole] = "swatch";
    roles[UrlRole] = "url";
    setRoleNames(roles);
}

TemplateVariantsModel::~TemplateVariantsModel()
{
    delete d;
}

QVariant TemplateVariantsModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if(index.isValid() && index.row() > -1 && index.row() < d->entries.count())
    {
        TemplateVariant* entry = d->entries.at(index.row());
        switch(role)
        {
            case NameRole:
                result = entry->name;
                break;
            case ColorRole:
                result = entry->color;
                break;
            case ThumbnailRole:
                result = entry->thumbnail;
                break;
            case SwatchRole:
                result = entry->swatch;
                break;
            case UrlRole:
                result = entry->url;
                break;
            default:
                break;
        }
    }

    return result;
}

int TemplateVariantsModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid() || d->entries.count() == 1)
        return 0;
    return d->entries.count();
}

void TemplateVariantsModel::addVariant(QString name, QString color, QString swatch, QString picture, QString file)
{
    TemplateVariant* entry = new TemplateVariant();
    d->entries.append(entry);

    entry->name = name;
    entry->color = QColor(color);
    entry->swatch = swatch;
    entry->url = file;

    if(picture.at(0) == QChar('/') || picture.at(1) == QChar(':')) {
        entry->thumbnail = picture;
    }
    else {
        entry->thumbnail = KIconLoader::global()->iconPath(picture, KIconLoader::Desktop, true);
    }
}

QModelIndex TemplateVariantsModel::firstIndex()
{
    return createIndex(0, 0);
}
