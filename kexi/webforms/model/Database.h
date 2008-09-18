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

#ifndef KEXIWEBFORMS_MODEL_DATABASE_H
#define KEXIWEBFORMS_MODEL_DATABASE_H

#include <QPair>
#include <QHash>

#include <kexidb/global.h>
#include <kexidb/field.h>


namespace KexiDB {
class TableSchema;
}
class QString;

namespace KexiWebForms { // begin namespace KexiWebForms
namespace Model {        // begin namespace Model

/*!
 * @brief Useful functions to access database contents
 *
 * The Database namespace contains some useful functions to access the
 * contents of the Kexi database.
 */
namespace Database {     // begin namespace Database

/*!
 * Get all table names
 * @param KexiDB::ObjectTypes specify for which type of object we should retrieve names
 * @return QHash with captions as keys and object names as values
 */
QHash<QString, QString> getNames(KexiDB::ObjectTypes);

/*!
 * Get the schema of a table
 * @param QString& the table name
 * @return a pointer to KexiDB::TableSchema
 */
KexiDB::TableSchema* getSchema(const QString&);

/*!
 * Get the schema of a table and specific field value
 * @param QString& the table name
 * @param QString& the primary key name
 * @param uint the primary key (numeric) value
 */
QPair< KexiDB::TableSchema, QList<QVariant> > getSchema(const QString&, const QString&, const uint);

/*!
 * Despite its name, this method is useful when creating new rows, too
 * Create/Update a row in a given table
 * Data is automatically percent-encoded
 * @param QString& the table name
 * @param QHash<QString, QVariant> a Hash with Name/Value pairs
 * @param create create a new row, instead of trying to update it
 */
bool updateRow(const QString&, const QHash<QString, QVariant>, bool create = false, int pkeyValue = -1);

/*!
 * Deletes a row from the database
 * @param KexiDB::TableSchema* the table schema
 * @param QString& primary key name
 * @param QString& primary key value
 */
bool deleteRow(const QString&, const QString&, const QString&);

/*!
 * Read the contents of a table
 * @param QString& the table name
 * @return QPair whose first value is a KexiDB::TableSchema and the second is a QMap \
 *   with record number as key and a QList<QString> as value (representing data in the row)
 */
QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > readTable(const QString&);

/*!
 * Run query and show result
 * @param QString& the query name
 * @return QPair whose first value is a KexiDB::TableSchema representing the master table \
 *  with record number as key and a QList<QString> as value (representing data in the row)
 */
QPair< KexiDB::TableSchema, QMap<uint, QList<QString> > > readQuery(const QString&);

} // end namespace Database
} // end namespace Model
} // end namespace KexiWebForms

#endif /* KEXIWEBFORMS_MODEL_DATABASE_H */
