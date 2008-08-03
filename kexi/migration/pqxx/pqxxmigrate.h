/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2006 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef PQXXIMPORT_H
#define PQXXIMPORT_H

#include <migration/keximigrate.h>

//Kexi Includes
#include <kexidb/field.h>
#include <kexidb/connection.h>

#include <pqxx/pqxx>

namespace KexiMigration
{

class PqxxMigrate : public KexiMigrate
{
    Q_OBJECT
    KEXIMIGRATION_DRIVER

public:
    PqxxMigrate(QObject *parent, const QStringList &args = QStringList());
    virtual ~PqxxMigrate();

protected:
    //Driver specific function to return table names
    virtual bool drv_tableNames(QStringList& tablenames);

    //Driver specific implementation to read a table schema
    virtual bool drv_readTableSchema(
        const QString& originalName, KexiDB::TableSchema& tableSchema);

    //Driver specific connection implementation
    virtual bool drv_connect();
    virtual bool drv_disconnect();

    virtual tristate drv_queryStringListFromSQL(
        const QString& sqlStatement, uint columnNumber, QStringList& stringList,
        int numRecords = -1);

    /*! Fetches single record from result obtained
    by running \a sqlStatement.
    \a firstRecord should be first initialized to true, so the method can run
    the query at first call and then set it will set \a firstRecord to false,
    so subsequent calls will only fetch records.
    On success the result is stored in \a data and true is returned,
    \a data is resized to appropriate size. cancelled is returned on EOF. */
//! @todo SQL-dependent!
    virtual tristate drv_fetchRecordFromSQL(const QString& sqlStatement,
                                            KexiDB::RecordData& data, bool &firstRecord);

    virtual bool drv_copyTable(const QString& srcTable,
                               KexiDB::Connection *destConn, KexiDB::TableSchema* dstTable);

private:
    //lowlevel functions/objects
    //database connection
    pqxx::connection* m_conn;

    //transaction
    pqxx::nontransaction* m_trans;

    //lowlevel result
    pqxx::result* m_res;

    //! Used in drv_fetchRecordFromSQL
    pqxx::result::const_iterator m_fetchRecordFromSQL_iter;

    //perform a query on the database
    bool query(const QString& statement);

    //Clear the result info
    void clearResultInfo();

    pqxx::oid tableOid(const QString& tablename);

    //Convert the pqxx type to a kexi type
    KexiDB::Field::Type type(int t, const QString& fname);

    //Find out the field constraints
    //Return whether or not the field is a pkey
    bool primaryKey(pqxx::oid table, int col) const;

    //Return whether or not the field is unique
    bool uniqueKey(pqxx::oid table, int col) const;

    //Return whether or not the field is a foreign key
    bool foreignKey(pqxx::oid table, int col) const;

    //Return whether or not the field is not null
    bool notNull(pqxx::oid table, int col) const;

    //Return whether or not the field is not empty
    bool notEmpty(pqxx::oid table, int col) const;

    //Return whether or not the field is auto incrementing
    bool autoInc(pqxx::oid table, int col) const;
};
}

#endif
