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

#include <KDebug>

#include <kexidb/field.h>
#include <kexidb/cursor.h>
#include <kexidb/connection.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/roweditbuffer.h>

#include "DataProvider.h"

#include "Database.h"

namespace KexiWebForms {
    namespace Model {
        
        QHash<QString, QString> Database::getNames(KexiDB::ObjectTypes objectType) {
            QList<int> objectIds(gConnection->objectIds( objectType ));
            QHash<QString, QString> objectNamesForCaptions;
            
            foreach (const int id, objectIds) {
                KexiDB::SchemaData schema;
                tristate res = gConnection->loadObjectSchemaData( id, schema );
                if (res != true)
                    continue;
                objectNamesForCaptions.insertMulti( 
                    schema.captionOrName(), schema.name() ); //insertMulti() because there can be many objects with the same caption
            }
            return objectNamesForCaptions;
        }

        KexiDB::TableSchema* Database::getSchema(const QString& table) {
            return gConnection->tableSchema(table);
        }
        
        QPair< KexiDB::TableSchema, QList<QVariant> > Database::getSchema(const QString& table, const QString& pkey, const uint pkeyValue) {
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

            return QPair< KexiDB::TableSchema, QList<QVariant> >(tableSchema, values);
        }
        
        bool Database::updateRow(const QString& table, const QHash<QString, QVariant> data, bool create, int pkeyValue) {
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
                if (create) {
                    if (!(tableSchema.field(name)->isAutoIncrement() && (currentValue.toString() == ""))) {
                        kDebug() << "Inserting " << name << "=" << currentValue.toString() << endl;
                        editBuffer.insert(*query.columnInfo(name), currentValue);
                    }
                } else {
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


        // Following stuff is ugly
        bool Database::updateCachedPkeys(const QString& requestedTable) {
            // FIXME: Check for errors
            if (cachedPkeys[requestedTable].isEmpty()) {
                kDebug() << "Cached Pkeys is empty, updating" << endl;
                KexiDB::TableSchema tableSchema(*gConnection->tableSchema(requestedTable));
                KexiDB::QuerySchema idSchema;
                idSchema.addField(tableSchema.primaryKey()->field(0));
                KexiDB::Cursor* cursor = gConnection->executeQuery(idSchema);
                while (cursor->moveNext()) {
                    kDebug() << "Appending " << cursor->value(0).toUInt() << " to cache" << endl;
                    cachedPkeys[requestedTable].append(cursor->value(0).toUInt());
                }
                if (cursor) {
                    cursor->close();
                    gConnection->deleteCursor(cursor);
                }
            }
            return true;
        }

        const QList<uint>& Database::getCachedPkeys(const QString& requestedTable) {
            return cachedPkeys[requestedTable];
        }

        int Database::getCurrentCachePosition(const QString& requestedTable, uint pkeyValueUInt) {
            for (int i = 0; i < cachedPkeys[requestedTable].size(); i++) {
                if (cachedPkeys[requestedTable].at(i) == pkeyValueUInt)
                    return i;
            }
            return 0;
        }

    }
}
