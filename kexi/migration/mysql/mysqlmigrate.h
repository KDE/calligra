/* This file is part of the KDE project
   Copyright (C) 2004 Martin Ellis <m.a.ellis@ncl.ac.uk>
   Copyright (C) 2006 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef MYSQLMIGRATE_H
#define MYSQLMIGRATE_H

#include <migration/keximigrate.h>
#include <migration/keximigrate_p.h>
#include <kexidb/drivers/mysql/mysqlconnection_p.h>

namespace KexiMigration
{

class MySQLMigrate : public KexiMigrate
{
    Q_OBJECT
    KEXIMIGRATION_DRIVER

public:
    MySQLMigrate(QObject *parent, const QVariantList& args = QVariantList());
    virtual ~MySQLMigrate();

    KexiDB::Field::Type type(const QString& table, const MYSQL_FIELD* t);

    KexiDB::Field::Type examineBlobField(const QString& table,
                                         const MYSQL_FIELD* fld);

    QStringList examineEnumField(const QString& table,
                                 const MYSQL_FIELD* fld);

    void getConstraints(int mysqlConstraints, KexiDB::Field* fld);

    void getOptions(int flags, KexiDB::Field* fld);

protected:
    //! Driver specific function to return table names
    virtual bool drv_tableNames(QStringList& tablenames);

    //! Driver specific implementation to read a table schema
    virtual bool drv_readTableSchema(
        const QString& originalName, KexiDB::TableSchema& tableSchema);

    //! Driver specific connection implementation
    virtual bool drv_connect();

    virtual bool drv_disconnect();

    virtual tristate drv_queryStringListFromSQL(
        const QString& sqlStatement, uint columnNumber,
        QStringList& stringList, int numRecords = -1);

    virtual tristate drv_fetchRecordFromSQL(const QString& sqlStatement,
                                            KexiDB::RecordData& data, bool &firstRecord);

    virtual bool drv_copyTable(const QString& srcTable,
                               KexiDB::Connection *destConn, KexiDB::TableSchema* dstTable);

    virtual bool drv_progressSupported() {
        return true;
    }

    virtual bool drv_getTableSize(const QString& table, quint64& size);

//TODO: move this somewhere to low level class (MIGRATION?)
//   virtual bool drv_getTablesList( QStringList &list );
//TODO: move this somewhere to low level class (MIGRATION?)
//   virtual bool drv_containsTable( const QString &tableName );
    
    //Extended API
    //! Position the source dataset at the start of a table
    virtual bool drv_readFromTable(const QString & tableName);
    
    //! Move to the next row
    virtual bool drv_moveNext();
    
    //! Move to the previous row
    virtual bool drv_movePrevious();
    
    //! Move to the next row
    virtual bool drv_moveFirst();
    
    //! Move to the previous row
    virtual bool drv_moveLast();
    
    //! Read the data at the given row/field
    virtual QVariant drv_value(uint i);

private:
    MySqlConnectionInternal * const d;
    MYSQL_RES *m_mysqlres;
    
    long m_rows;
    long m_row;
    MYSQL_ROW m_dataRow;
    
    void getRow();
};
}

#endif
