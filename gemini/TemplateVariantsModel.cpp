/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "TemplateVariantsModel.h"
#include <QColor>
#include <QDebug>

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
}

TemplateVariantsModel::~TemplateVariantsModel()
{
    delete d;
}

QHash<int, QByteArray> TemplateVariantsModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[NameRole] = "text";
    roles[ColorRole] = "color";
    roles[ThumbnailRole] = "thumbnail";
    roles[SwatchRole] = "swatch";
    roles[UrlRole] = "url";
    return roles;
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
    } else {
        entry->thumbnail = "";
    }
}

QModelIndex TemplateVariantsModel::firstIndex()
{
    return createIndex(0, 0);
}
