/*
    This file is part of the KDE project
    Copyright (C) 2010 Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "KexiFieldListModel.h"
#include "KexiFieldListModelItem.h"
#include <KLocalizedString>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/utils.h>
#include <kdebug.h>
#include <drivers/xbase/xbaseexport.h>
#include <QMimeData>

class KexiFieldListModel::Private
{
public:
    Private();
    KexiDB::TableOrQuerySchema* schema;
    KexiFieldListOptions options;
    KexiFieldListModelItem *allColumnsItem;
    QList<KexiFieldListModelItem*> items;
};

KexiFieldListModel::Private::Private() : schema(0), allColumnsItem(0)
{

}

KexiFieldListModel::KexiFieldListModel(QObject* parent, KexiFieldListOptions options): QAbstractTableModel(parent)
                                      , d(new Private())
{
    d->options = options;
}

KexiFieldListModel::~KexiFieldListModel()
{
    delete d;
}

void KexiFieldListModel::setSchema(KexiDB::TableOrQuerySchema* schema)
{
    if (schema && d->schema == schema)
        return;

    delete d->schema;
    d->schema = schema;
    if (!d->schema)
        return;

    KexiFieldListModelItem *item = 0;
    KexiDB::QueryColumnInfo::Vector columns = d->schema->columns(true /*unique*/);
    const int count = columns.count();
    for (int i = -2; i < count; i++) {
        KexiDB::QueryColumnInfo *colinfo = 0;
        if (i == -2) {
            if (!(d->options & ShowEmptyItem))
                continue;
            item = new KexiFieldListModelItem(QString(), QString(), false);
        }else if (i == -1) {
            if (!(d->options & ShowAsterisk))
                continue;
            item = new KexiFieldListModelItem("*", "", false);
            d->allColumnsItem = item;
        } else {
            colinfo = columns[i];
            item = new KexiFieldListModelItem(colinfo->aliasOrName(), colinfo->field->typeName(), (colinfo->field->isPrimaryKey() || colinfo->field->isUniqueKey()));
            item->setCaption(colinfo->captionOrAliasOrName());
        }
        d->items.append(item);
        kDebug() << item->data(0);
    }
}

QVariant KexiFieldListModel::data(const QModelIndex& index, int role) const
{
    KexiFieldListModelItem *item = 0;

    if (index.isValid() && index.row() < d->items.count()) {
        item = d->items[index.row()];
    }

    if (item) {
        if (role == Qt::DisplayRole) {
            return item->data(index.column());
        } else if (role == Qt::DecorationRole) {
            return item->icon();
        } else if (role == Qt::UserRole) {
            kDebug() << item->caption();
            return item->caption();
        }
    }
    return QVariant();
}

int KexiFieldListModel::columnCount(const QModelIndex& /*parent*/) const
{
    return (d->options & ShowDataTypes) ? 2 : 1;
}

int KexiFieldListModel::rowCount(const QModelIndex& /*parent*/) const
{
    return d->items.count();
}

QVariant KexiFieldListModel::headerData(int /*section*/, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return i18n("Field Name");
    else
        return QVariant();
}

QStringList KexiFieldListModel::mimeTypes() const
{
    QStringList types;
    types << "kexi/fields";
    return types;
}

QMimeData* KexiFieldListModel::mimeData(const QModelIndexList& indexes) const
{
    if (!d->schema) {
        return new QMimeData();
    }

    QString sourceMimeType;
    QString sourceName;
    QStringList fields;

    QMimeData *mimedata = new QMimeData();
    QByteArray fielddata;
    QDataStream stream1(&fielddata, QIODevice::WriteOnly);

    if (d->schema->table()) {
        sourceMimeType = "kexi/table";
    } else if (d->schema->query()) {
        sourceMimeType = "kexi/query";
    }

    sourceName = d->schema->name();

    foreach (QModelIndex idx, indexes) {
        fields << data(idx, Qt::DisplayRole).toString();
    }

    stream1 << sourceMimeType << sourceName << fields;
    mimedata->setData("kexi/fields", fielddata);

    return mimedata;
}

Qt::ItemFlags KexiFieldListModel::flags(const QModelIndex& index) const
{
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);

    if (index.isValid())
        return d->items[index.row()]->flags()| defaultFlags;
    else
        return defaultFlags;
}
