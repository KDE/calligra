/*
 * This file is part of the KDE project
 *
 * (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QPair>
#include <QHash>
#include <QString>
#include <QtAlgorithms>
#include <QTextDocument>

#include <KDebug>

#include <kexidb/utils.h>
#include <kexidb/field.h>
#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>

#include "DataProvider.h"

#include "Database.h"

namespace KexiWebForms { // begin namespace KexiWebForms
namespace Model {        // begin namespace Model
namespace Database {     // begin namespace Database

QHash<QString, QString> getNames(KexiDB::ObjectType objectType) {
    QList<int> objectIds(gConnection->objectIds(objectType));
    QHash<QString, QString> objectNamesForCaptions;

    foreach(const int id, objectIds) {
        KexiDB::SchemaData schema;
        tristate res = gConnection->loadObjectSchemaData(id, schema);
        if (res != true)
            continue;
        objectNamesForCaptions.insertMulti(
            schema.captionOrName(), schema.name());  //insertMulti() because there can be many objects with the same caption
    }
    return objectNamesForCaptions;
}


KexiDB::TableSchema* getSchema(const QString& table) {
    return gConnection->tableSchema(table);
}


QPair< KexiDB::TableSchema, QList<QVariant> > getSchema(const QString& table, const QString& pkey, const uint pkeyValue) {
    KexiDB::TableSchema tableSchema(*getSchema(table));
    KexiDB::QuerySchema* query = 0;
    KexiDB::Cursor* cursor = 0;
    query = new KexiDB::QuerySchema(tableSchema);
    query->addToWhereExpression(tableSchema.field(pkey), QVariant(pkeyValue));
    cursor = gConnection->executeQuery(*query);
    cursor->moveNext(); // we just hope that everything goes well (aka FIXME!)

    QList<QVariant> values;
    for (uint i = 0; i < tableSchema.fieldCount(); i++) {
        values.append(cursor->value(i));
    }

    if (cursor) {
        cursor->close();
        gConnection->deleteCursor(cursor);
    }

    return QPair< KexiDB::TableSchema, QList<QVariant> >(tableSchema, values);
}

bool updateRow(const QString& table, const QHash<QString, QVariant> data, bool create, int pkeyValue) {
    KexiDB::TableSchema tableSchema(*gConnection->tableSchema(table));
    KexiDB::QuerySchema query(tableSchema);
    KexiDB::Cursor* cursor = gConnection->prepareQuery(query);

    KexiDB::RecordData recordData(tableSchema.fieldCount());
    if (!create && (pkeyValue != -1)) {
        QVector<int> pkeyFields(query.pkeyFieldsOrder());
        KexiDB::Field* primaryKeyField = tableSchema.primaryKey()->field(0);
        for (int i = 0; i < pkeyFields.count(); ++i) {
            int fieldId = pkeyFields.at(i);
            if (primaryKeyField == query.field(fieldId)) {
                recordData.insert(fieldId, pkeyValue);
                break;
            }
        }
    }

    KexiDB::RowEditBuffer editBuffer(true);

    QStringList fieldNames(data.keys());

    foreach(const QString& name, fieldNames) {
        QVariant currentValue(data.value(name));
        // FIXME: Regression, we don't encode data...
        if (!(tableSchema.field(name)->isAutoIncrement() && (currentValue.toString() == ""))) {
            kDebug() << "Inserting " << name << "=" << currentValue.toString();
            editBuffer.insert(*query.columnInfo(name), currentValue);
        }
    }

    bool result = false;
    if (create)
        result = cursor->insertRow(recordData, editBuffer);
    else
        result = cursor->updateRow(recordData, editBuffer);

    if (cursor) {
        cursor->close();
        gConnection->deleteCursor(cursor);
    }
    return result;
}

bool deleteRow(const QString& table, const QString& pkeyName, const QString& pkeyValue) {
    return KexiDB::deleteRow(*gConnection, gConnection->tableSchema(table), pkeyName, pkeyValue);
}

QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > readTable(const QString& tableName) {
    KexiDB::TableSchema table(*getSchema(tableName));
    KexiDB::QuerySchema query(table);
    KexiDB::Cursor* cursor = gConnection->executeQuery(query);

    QMap<uint, QList<QString> > tableContents;
    if (cursor) {
        uint record = 1;
        uint pkeyVal = 0;
        while (cursor->moveNext()) {
            for (uint i = 0; i < query.fieldCount(); i++) {
                if (query.field(i) == table.primaryKey()->field(0)) {
                    pkeyVal = cursor->value(i).toUInt();
                }

                if (query.field(i)->type() == KexiDB::Field::BLOB) {
                    tableContents[record].append(QString("<img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/>")
                                                 .arg(tableName).arg(query.field(i)->name()).arg(table.primaryKey()->field(0)->name())
                                                 .arg(QVariant(pkeyVal).toString()));
                } else {
                    tableContents[record].append(Qt::escape(cursor->value(i).toString()));
                }
            }
            record++;
        }

        cursor->close();
        gConnection->deleteCursor(cursor);
    }

    return QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > >(table, tableContents);
}

QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > readQuery(const QString& queryName) {
    KexiDB::QuerySchema* query = gConnection->querySchema(queryName);
    KexiDB::TableSchema* table = 0;
    KexiDB::QueryColumnInfo::Vector expandedFields = query->fieldsExpanded();
    QMap<uint, QList<QString> > queryContents;
    if (query) {
        table = query->masterTable();
        if (table) {
            KexiDB::Cursor* cursor = gConnection->executeQuery(*query);
            if (cursor) {
                uint record = 1;
                uint pkeyVal = 0;
                while (cursor->moveNext()) {
                    for (uint i = 0; i < uint(expandedFields.size()); i++) {
                        if (query->field(i) == table->primaryKey()->field(0)) {
                            pkeyVal = cursor->value(i).toUInt();
                        }

                        /*if (query->field(i)->type() == KexiDB::Field::BLOB) {
                            queryContents[record].append(QString("<img src=\"/blob/%1/%2/%3/%4\" alt=\"Image\"/>")
                                                         .arg(table->name()).arg(query->field(i)->name())
                                                         .arg(table->primaryKey()->field(0)->name())
                                                         .arg(QString(pkeyVal)));
                        } else {*/
                        queryContents[record].append(Qt::escape(cursor->value(i).toString()));
                        //}
                    }
                    record++;
                }
            }
        }
    }
    return QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > >(*table, queryContents);
}


} // end namespace Database
} // end namespace Model
} // end namespace KexiWebForms
