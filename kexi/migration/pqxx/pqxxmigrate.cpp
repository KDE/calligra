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

#include "pqxxmigrate.h"
#include "pg_type.h"

#include <qstring.h>
//Added by qt3to4:
#include <Q3ValueList>
#include <kdebug.h>
#include <qstringlist.h>

//I maybe should not use stl?
#include <string>
#include <vector>

#include <kexidb/cursor.h>
#include <kexidb/utils.h>
#include <kexidb/drivermanager.h>
#include <kexiutils/identifier.h>
#include <kexidb/drivers/pqxx/pqxxcursor.h> //for pgsqlCStrToVariant()

using namespace KexiDB;
using namespace KexiMigration;

/*
This is the implementation for the pqxx specific import routines
Thi is currently pre alpha and in no way is it meant
to compile, let alone work.  This is meant as an example of
what the system might be and is a work in progress
*/

KEXIMIGRATE_DRIVER_INFO(PqxxMigrate, pqxx)

//==================================================================================
//Constructor
/*PqxxMigrate::PqxxMigrate()
 : KexiMigrate(parent, name, args)
{
    m_res=0;
    m_trans=0;
    m_conn=0;
}*/

PqxxMigrate::PqxxMigrate(QObject *parent, const QStringList &args)
        : KexiMigrate(parent, args)
{
    m_res = 0;
    m_trans = 0;
    m_conn = 0;
    KexiDB::DriverManager manager;
    m_kexiDBDriver = manager.driver("pqxx");
}
//==================================================================================
//Destructor
PqxxMigrate::~PqxxMigrate()
{
    clearResultInfo();
}

//==================================================================================
//This is probably going to be quite complex...need to get the types for all columns
//any any other attributes required by kexi
//helped by reading the 'tables' test program
bool PqxxMigrate::drv_readTableSchema(
    const QString& originalName, KexiDB::TableSchema& tableSchema)
{
//    m_table = new KexiDB::TableSchema(table);

    //TODO IDEA: ask for user input for captions
//moved    m_table->setCaption(table + " table");

    //Perform a query on the table to get some data
    if (!query("select * from \"" + originalName + "\" limit 1"))
        return false;
    //Loop round the fields
    for (uint i = 0; i < (uint)m_res->columns(); i++) {
        QString fldName(m_res->column_name(i));
        KexiDB::Field::Type fldType = type(m_res->column_type(i), fldName);
        QString fldID(KexiUtils::string2Identifier(fldName));
        const pqxx::oid toid = tableOid(originalName);
        if (toid == 0)
            return false;
        KexiDB::Field *f = new KexiDB::Field(fldID, fldType);
        f->setCaption(fldName);
        f->setPrimaryKey(primaryKey(toid, i));
        f->setUniqueKey(uniqueKey(toid, i));
        f->setAutoIncrement(autoInc(toid, i));//This should be safe for all field types
        tableSchema.addField(f);

        // Do this for var/char types
        //m_f->setLength(m_res->at(0)[i].size());

        // Do this for numeric type
        /*m_f->setScale(0);
        m_f->setPrecision(0);*/

        kDebug() << "Added field [" << f->name() << "] type [" << f->typeName()
        << "]" << endl;
    }
    return true;
}

//==================================================================================
//get a list of tables and put into the supplied string list
bool PqxxMigrate::drv_tableNames(QStringList& tableNames)
{
    /*
      //pg_ = standard postgresql tables, pga_ = tables added by pgaccess, sql_ = probably information schemas, kexi__ = existing kexi tables
      if (query("SELECT relname FROM pg_class WHERE ((relkind = 'r') AND ((relname !~ '^pg_') AND (relname !~ '^pga_') AND (relname !~ '^sql_') AND (relname !~ '^kexi__')))"))
    */
    if (!query("SELECT relname FROM pg_class WHERE ((relkind = 'r') AND ((relname !~ '^pg_') AND (relname !~ '^pga_') AND (relname !~ '^sql_')))"))
        return false;

    for (pqxx::result::const_iterator c = m_res->begin(); c != m_res->end(); ++c) {
        // Copy the result into the return list
        tableNames << QString::fromLatin1(c[0].c_str());
    }
    return true;
}

//==================================================================================
//Convert a postgresql type to a kexi type
KexiDB::Field::Type PqxxMigrate::type(int t, const QString& fname)
{
    switch (t) {
    case UNKNOWNOID:
        return KexiDB::Field::InvalidType;
    case BOOLOID:
        return KexiDB::Field::Boolean;
    case INT2OID:
        return KexiDB::Field::ShortInteger;
    case INT4OID:
        return KexiDB::Field::Integer;
    case INT8OID:
        return KexiDB::Field::BigInteger;
    case FLOAT4OID:
        return KexiDB::Field::Float;
    case FLOAT8OID:
        return KexiDB::Field::Double;
    case NUMERICOID:
        return KexiDB::Field::Double;
    case DATEOID:
        return KexiDB::Field::Date;
    case TIMEOID:
        return KexiDB::Field::Time;
    case TIMESTAMPOID:
        return KexiDB::Field::DateTime;
    case BYTEAOID:
        return KexiDB::Field::BLOB;
    case BPCHAROID:
        return KexiDB::Field::Text;
    case VARCHAROID:
        return KexiDB::Field::Text;
    case TEXTOID:
        return KexiDB::Field::LongText;
    }

    //Ask the user what to do with this field
    return userType(fname);
}

//==================================================================================
//Connect to the db backend
bool PqxxMigrate::drv_connect()
{
    kDebug() << "drv_connect: " << m_migrateData->sourceName << endl;

    QString conninfo;
    QString socket;

    //Setup local/remote connection
    if (m_migrateData->source->hostName.isEmpty()) {
        if (m_migrateData->source->fileName().isEmpty()) {
            socket = "/tmp/.s.PGSQL.5432";
        } else {
            socket = m_migrateData->source->fileName();
        }
    } else {
        conninfo = "host='" + m_migrateData->source->hostName + "'";
    }

    //Build up the connection string
    if (m_migrateData->source->port == 0)
        m_migrateData->source->port = 5432;

    conninfo += QString::fromLatin1(" port='%1'").arg(m_migrateData->source->port);
    conninfo += QString::fromLatin1(" dbname='%1'").arg(m_migrateData->sourceName);

    if (!m_migrateData->source->userName.isEmpty())
        conninfo += QString::fromLatin1(" user='%1'").arg(m_migrateData->source->userName);

    if (!m_migrateData->source->password.isEmpty())
        conninfo += QString::fromLatin1(" password='%1'").arg(m_migrateData->source->password);

    try {
        m_conn = new pqxx::connection(conninfo.toLatin1().constData());
        return true;
    } catch (const std::exception &e) {
        //kDebug() << "PqxxMigrate::drv_connect:exception - " << e.what() << endl;
    } catch (...) {
        kDebug() << "PqxxMigrate::drv_connect:exception(...)??" << endl;
    }
    return false;
}

//==================================================================================
//Connect to the db backend
bool PqxxMigrate::drv_disconnect()
{
    if (m_conn) {
        m_conn->disconnect();
        delete m_conn;
        m_conn = 0;
    }
    return true;
}
//==================================================================================
//Perform a query on the database and store result in m_res
bool PqxxMigrate::query(const QString& statement)
{
    kDebug() << "query: " << statement.toLatin1() << endl;
    if (!m_conn)
        return false;

    // Clear the last result information...
    clearResultInfo();

    try {
        //Create a transaction
        m_trans = new pqxx::nontransaction(*m_conn);
        //Create a result opject through the transaction
        m_res = new pqxx::result(m_trans->exec(statement.toLatin1().constData()));
        //Commit the transaction
        m_trans->commit();
        //If all went well then return true, errors picked up by the catch block
        return true;
    } catch (const std::exception &e) {
        //If an error ocurred then put the error description into _dbError
        kDebug() << "pqxxImport::query:exception - " << e.what() << endl;
        return false;
    } catch (...) {
        kDebug() << "PqxxMigrate::query:exception(...)??" << endl;
    }
    return true;
}

//=========================================================================
//Clears the current result
void PqxxMigrate::clearResultInfo()
{
    delete m_res;
    m_res = 0;

    delete m_trans;
    m_trans = 0;
}

//=========================================================================
//Return the OID for a table
pqxx::oid PqxxMigrate::tableOid(const QString& table)
{
    QString statement;
    static QString otable;
    static pqxx::oid toid;

    pqxx::nontransaction* tran = 0;
    pqxx::result* tmpres = 0;

    //Some simple result caching
    if (table == otable) {
        kDebug() << "Returning table OID from cache..." << endl;
        return toid;
    } else {
        otable = table;
    }

    try {
        statement = "SELECT relfilenode FROM pg_class WHERE (relname = '";
        statement += table;
        statement += "')";

        tran = new pqxx::nontransaction(*m_conn, "find_t_oid");
        tmpres = new pqxx::result(tran->exec(statement.toLatin1().constData()));

        tran->commit();
        if (tmpres->size() > 0) {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(toid);
        } else {
            toid = 0;
        }
    } catch (const std::exception &e) {
        kDebug() << "pqxxSqlDB::tableOid:exception - " << e.what() << endl;
        kDebug() << "pqxxSqlDB::tableOid:failed statement - " << statement << endl;
        toid = 0;
    } catch (...) {
        kDebug() << "PqxxMigrate::tableOid:exception(...)??" << endl;
    }
    delete tmpres;
    tmpres = 0;

    delete tran;
    tran = 0;

    kDebug() << "OID for table [" << table << "] is [" << toid << "]" << endl;
    return toid;
}

//=========================================================================
//Return whether or not the curent field is a primary key
//TODO: Add result caching for speed
bool PqxxMigrate::primaryKey(pqxx::oid table_uid, int col) const
{
    QString statement;
    bool pkey;
    int keyf;

    pqxx::nontransaction* tran = 0;
    pqxx::result* tmpres = 0;

    try {
        statement = QString::fromLatin1(
                        "SELECT indkey FROM pg_index WHERE ((indisprimary = true) AND (indrelid = %1))")
                    .arg(table_uid);

        tran = new pqxx::nontransaction(*m_conn, "find_pkey");
        tmpres = new pqxx::result(tran->exec(statement.toLatin1().constData()));

        tran->commit();
        if (tmpres->size() > 0) {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(keyf);
            if (keyf - 1 == col) {//-1 because pg counts from 1 and we count from 0
                pkey = true;
                kDebug() << "Field is pkey" << endl;
            } else {
                pkey = false;
                kDebug() << "Field is NOT pkey" << endl;
            }
        } else {
            pkey = false;
            kDebug() << "Field is NOT pkey" << endl;
        }
    } catch (const std::exception &e) {
        kDebug() << "pqxxSqlDB::primaryKey:exception - " << e.what() << endl;
        kDebug() << "pqxxSqlDB::primaryKey:failed statement - " << statement << endl;
        pkey = false;
    }
    delete tmpres;
    tmpres = 0;

    delete tran;
    tran = 0;
    return pkey;
}

//=========================================================================
/*! Fetches single string at column \a columnNumber from result obtained
 by running \a sqlStatement.
 On success the result is stored in \a string and true is returned.
 \return cancelled if there are no records available. */
tristate PqxxMigrate::drv_queryStringListFromSQL(
    const QString& sqlStatement, uint columnNumber, QStringList& stringList, int numRecords)
{
    std::string result;
    int i = 0;
    if (query(sqlStatement)) {
        for (pqxx::result::const_iterator it = m_res->begin();
                it != m_res->end() && (numRecords == -1 || i < numRecords); ++it, i++) {
            if (it.size() > 0 && it.size() > columnNumber) {
                it.at(columnNumber).to(result);
                stringList.append(QString::fromUtf8(result.c_str()));
            } else {
                clearResultInfo();
                return cancelled;
            }
        }
    } else
        return false;
    clearResultInfo();
    /*    delete tmpres;
        tmpres = 0;

        delete tran;
        tran = 0;*/

    if (i < numRecords)
        return cancelled;

    return true;
    /*
    if (d->executeSQL(sqlStatement)) {
      MYSQL_RES *res = mysql_use_result(d->mysql);
      if (res != NULL) {
        MYSQL_ROW row = mysql_fetch_row(res);
        if (!row) {
          tristate r = mysql_errno(d->mysql) ? false : cancelled;
          mysql_free_result(res);
          return r;
        }
        uint numFields = mysql_num_fields(res);
        if (columnNumber > (numFields-1)) {
          kWarning() << "PqxxMigrate::drv_querySingleStringFromSQL("<<sqlStatement
            << "): columnNumber too large ("
            << columnNumber << "), expected 0.." << numFields << endl;
          mysql_free_result(res);
          return false;
        }
        unsigned long *lengths = mysql_fetch_lengths(res);
        if (!lengths) {
          mysql_free_result(res);
          return false;
        }
        string = QString::fromLatin1(row[columnNumber], lengths[columnNumber]);
        mysql_free_result(res);
      } else {
        kDebug() << "PqxxMigrate::drv_querySingleStringFromSQL(): null result" << endl;
      }
      return true;
    } else {
      return false;
    }*/
}

tristate PqxxMigrate::drv_fetchRecordFromSQL(const QString& sqlStatement,
        KexiDB::RecordData& data, bool &firstRecord)
{
    if (firstRecord || !m_res) {
        if (m_res)
            clearResultInfo();
        if (!query(sqlStatement))
            return false;
        m_fetchRecordFromSQL_iter = m_res->begin();
        firstRecord = false;
    } else
        ++m_fetchRecordFromSQL_iter;

    if (m_fetchRecordFromSQL_iter == m_res->end()) {
        clearResultInfo();
        return cancelled;
    }

    std::string result;
    const int numFields = m_fetchRecordFromSQL_iter.size();
    data.resize(numFields);
    for (int i = 0; i < numFields; i++)
        data[i] = KexiDB::pgsqlCStrToVariant(m_fetchRecordFromSQL_iter.at(i));
    return true;
}

//=========================================================================
/*! Copy PostgreSQL table to KexiDB database */
bool PqxxMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
                                KexiDB::TableSchema* dstTable)
{
    std::vector<std::string> R;

    pqxx::work T(*m_conn, "PqxxMigrate::drv_copyTable");

    pqxx::tablereader stream(T, (srcTable.toLatin1().constData()));

    //Loop round each row, reading into a vector of strings
    const KexiDB::QueryColumnInfo::Vector fieldsExpanded(dstTable->query()->fieldsExpanded());
    for (int n = 0; (stream >> R); ++n) {
        QList<QVariant> vals;
        std::vector<std::string>::const_iterator i, end(R.end());
        int index = 0;
        for (i = R.begin(); i != end; ++i, index++) {
            if (fieldsExpanded.at(index)->field->type() == KexiDB::Field::BLOB
                    || fieldsExpanded.at(index)->field->type() == KexiDB::Field::LongText) {
                vals.append(KexiDB::pgsqlByteaToByteArray((*i).c_str(), (*i).size()));
            } else if (fieldsExpanded.at(index)->field->type() == KexiDB::Field::Boolean) {
                vals.append(QString((*i).c_str()).toLower() == "t" ? QVariant(true) : QVariant(false));
            } else
                vals.append(KexiDB::cstringToVariant((*i).c_str(),
                                                     fieldsExpanded.at(index)->field, (*i).size()));
        }
        if (!destConn->insertRecord(*dstTable, vals))
            return false;
        updateProgress();
        R.clear();
    }

    //This does not work in <libpqxx 2.2
    //stream.complete();

    return true;
}

//=========================================================================
//Return whether or not the curent field is a primary key
//TODO: Add result caching for speed
bool PqxxMigrate::uniqueKey(pqxx::oid table_uid, int col) const
{
    QString statement;
    bool ukey;
    int keyf;

    pqxx::nontransaction* tran = 0;
    pqxx::result* tmpres = 0;

    try {
        statement = QString::fromLatin1(
                        "SELECT indkey FROM pg_index WHERE ((indisunique = true) AND (indrelid = %1))")
                    .arg(table_uid);

        tran = new pqxx::nontransaction(*m_conn, "find_ukey");
        tmpres = new pqxx::result(tran->exec(statement.toLatin1().constData()));

        tran->commit();
        if (tmpres->size() > 0) {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(keyf);
            if (keyf - 1 == col) {//-1 because pg counts from 1 and we count from 0
                ukey = true;
                kDebug() << "Field is unique" << endl;
            } else {
                ukey = false;
                kDebug() << "Field is NOT unique" << endl;
            }
        } else {
            ukey = false;
            kDebug() << "Field is NOT unique" << endl;
        }
    } catch (const std::exception &e) {
        kDebug() << "uniqueKey:exception - " << e.what() << endl;
        kDebug() << "uniqueKey:failed statement - " << statement << endl;
        ukey = false;
    }

    delete tmpres;
    tmpres = 0;

    delete tran;
    tran = 0;
    return ukey;
}

//==================================================================================
//TODO::Implement
bool PqxxMigrate::autoInc(pqxx::oid /*table_uid*/, int /*col*/) const
{
    return false;
}

//==================================================================================
//TODO::Implement
bool PqxxMigrate::notNull(pqxx::oid /*table_uid*/, int /*col*/) const
{
    return false;
}

//==================================================================================
//TODO::Implement
bool PqxxMigrate::notEmpty(pqxx::oid /*table_uid*/, int /*col*/) const
{
    return false;
}

//==================================================================================
//Return a list of database names
/*bool PqxxMigrate::drv_getDatabasesList( QStringList &list )
{
    KexiDBDrvDbg << "pqxxSqlConnection::drv_getDatabaseList" << endl;

    if (executeSQL("SELECT datname FROM pg_database WHERE datallowconn = TRUE"))
    {
        std::string N;
        for (pqxx::result::const_iterator c = m_res->begin(); c != m_res->end(); ++c)
        {
            // Read value of column 0 into a string N
            c[0].to(N);
            // Copy the result into the return list
            list << QString::fromLatin1 (N.c_str());
      KexiDBDrvDbg << N.c_str() << endl;
        }
        return true;
    }

    return false;
}*/


#include "pqxxmigrate.moc"
