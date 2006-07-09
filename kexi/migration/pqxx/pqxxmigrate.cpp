/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
 
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
#include <kdebug.h>
#include <qstringlist.h>

//I maybe shouldnt use stl?
#include <string>
#include <vector>

#include <kexidb/cursor.h>
#include <kexiutils/identifier.h>

using namespace KexiDB;
using namespace KexiMigration;

/*
This is the implementation for the pqxx specific import routines
Thi is currently pre alpha and in no way is it meant
to compile, let alone work.  This is meant as an example of
what the system might be and is a work in progress
*/

KEXIMIGRATE_DRIVER_INFO( PqxxMigrate, pqxx )

//==================================================================================
//Constructor
/*PqxxMigrate::PqxxMigrate()
 : KexiMigrate(parent, name, args)
{
    m_res=0;
    m_trans=0;
    m_conn=0;
}*/

PqxxMigrate::PqxxMigrate(QObject *parent, const char *name, const QStringList &args)
 : KexiMigrate(parent, name, args)
{
    m_res=0;
    m_trans=0;
    m_conn=0;
}
//==================================================================================
//Destructor
PqxxMigrate::~PqxxMigrate()
{
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
    if (query("select * from \"" + originalName + "\" limit 1"))
    {
        //Loop round the fields
        for (uint i = 0; i < (uint)m_res->columns(); i++)
        {
            QString fldName(m_res->column_name(i));
            KexiDB::Field::Type fldType = type(m_res->column_type(i), fldName);
            QString fldID( KexiUtils::string2Identifier(fldName) );
            const pqxx::oid toid = tableOid(originalName);
            if (toid==0)
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

           kdDebug() << "Added field [" << f->name() << "] type [" << f->typeName() 
           	<< "]" << endl;
        }
        return true;
    }
    else
    {
        return false;
    }
}

//==================================================================================
//get a list of tables and put into the supplied string list
bool PqxxMigrate::drv_tableNames(QStringList& tableNames)
{
    //pg_ = standard postgresql tables, pga_ = tables added by pgaccess, sql_ = probably information schemas, kexi__ = existing kexi tables
    if (query("SELECT relname FROM pg_class WHERE ((relkind = 'r') AND ((relname !~ '^pg_') AND (relname !~ '^pga_') AND (relname !~ '^sql_') AND (relname !~ '^kexi__')))"))
    {
        for (pqxx::result::const_iterator c = m_res->begin(); c != m_res->end(); ++c)
        {
            // Copy the result into the return list
            tableNames << QString::fromLatin1 (c[0].c_str());
        }
        return true;
    }
    else
    {
        return false;
    }
}

//==================================================================================
//Convert a postgresql type to a kexi type
KexiDB::Field::Type PqxxMigrate::type(int t, const QString& fname)
{
    switch(t)
    {
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
    //case BYTEAOID:
        //    return KexiDB::Field::Type::SQLVarBinary;
    case BPCHAROID:
        return KexiDB::Field::Text;
    case VARCHAROID:
        return KexiDB::Field::Text;
    case TEXTOID:
        return KexiDB::Field::LongText;
        //TODO: Binary Types (BLOB)
    }

    //Ask the user what to do with this field
    return userType(fname);
}

//==================================================================================
//Connect to the db backend
bool PqxxMigrate::drv_connect()
{
    kdDebug() << "drv_connect: " << m_migrateData->sourceName << endl;

    QString conninfo;
    QString socket;

    //Setup local/remote connection
    if (m_migrateData->source->hostName.isEmpty())
    {
        if (m_migrateData->source->fileName().isEmpty())
        {
            socket="/tmp/.s.PGSQL.5432";
        }
        else
        {
            socket=m_migrateData->source->fileName();
        }
    }
    else
    {
        conninfo = "host='" + m_migrateData->source->hostName + "'";
    }

    //Build up the connection string
    if (m_migrateData->source->port == 0)
        m_migrateData->source->port = 5432;

    conninfo += QString::fromLatin1(" port='%1'").arg(m_migrateData->source->port);

    conninfo += QString::fromLatin1(" dbname='%1'").arg(m_migrateData->sourceName);

    if (!m_migrateData->source->userName.isNull())
        conninfo += QString::fromLatin1(" user='%1'").arg(m_migrateData->source->userName);

    if (!m_migrateData->source->password.isNull())
        conninfo += QString::fromLatin1(" password='%1'").arg(m_migrateData->source->password);

    try
    {
        m_conn = new pqxx::connection( conninfo.latin1() );
        return true;
    }
    catch(const std::exception &e)
    {
        kdDebug() << "PqxxMigrate::drv_connect:exception - " << e.what() << endl;
    }
    catch(...)
    {
        kdDebug() << "PqxxMigrate::drv_connect:exception(...)??" << endl;
    }
    return false;
}

//==================================================================================
//Connect to the db backend
bool PqxxMigrate::drv_disconnect()
{
    if (m_conn)
    {
        m_conn->disconnect();
        delete m_conn;
		m_conn = 0;
    }
    return true;
}
//==================================================================================
//Perform a query on the database and store result in m_res
bool PqxxMigrate::query (const QString& statement)
{
    kdDebug() << "query: " << statement.latin1() << endl;

    Q_ASSERT (m_conn);

    // Clear the last result information...
    clearResultInfo ();

    try
    {
        //Create a transaction
        m_trans = new pqxx::nontransaction(*m_conn);
        //Create a result opject through the transaction
        m_res = new pqxx::result(m_trans->exec(statement.latin1()));
        //Commit the transaction
        m_trans->commit();
        //If all went well then return true, errors picked up by the catch block
        return true;
    }
    catch (const std::exception &e)
    {
        //If an error ocurred then put the error description into _dbError
        kdDebug() << "pqxxImport::query:exception - " << e.what() << endl;
        return false;
    }
    catch(...)
    {
        kdDebug() << "PqxxMigrate::query:exception(...)??" << endl;
    }
    return true;
}

//=========================================================================
//Clears the current result
void PqxxMigrate::clearResultInfo ()
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
    if (table == otable)
    {
        kdDebug() << "Returning table OID from cache..." << endl;
        return toid;
    }
    else
    {
        otable = table;
    }

    try
    {
        statement = "SELECT relfilenode FROM pg_class WHERE (relname = '";
        statement += table;
        statement += "')";

        tran = new pqxx::nontransaction(*m_conn, "find_t_oid");
        tmpres = new pqxx::result(tran->exec(statement.latin1()));

        tran->commit();
        if (tmpres->size() > 0)
        {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(toid);
        }
        else
        {
            toid = 0;
        }
    }
    catch(const std::exception &e)
    {
        kdDebug() << "pqxxSqlDB::tableOid:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::tableOid:failed statement - " << statement << endl;
        toid = 0;
    }
    catch(...)
    {
        kdDebug() << "PqxxMigrate::tableOid:exception(...)??" << endl;
    }
    delete tmpres;
	tmpres = 0;
    
	delete tran;
	tran = 0;

    kdDebug() << "OID for table [" << table << "] is [" << toid << "]" << endl;
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

    try
    {
        statement = QString("SELECT indkey FROM pg_index WHERE ((indisprimary = true) AND (indrelid = %1))").arg(table_uid);

        tran = new pqxx::nontransaction(*m_conn, "find_pkey");
        tmpres = new pqxx::result(tran->exec(statement.latin1()));

        tran->commit();
        if (tmpres->size() > 0)
        {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(keyf);
            if (keyf-1 == col) //-1 because pg counts from 1 and we count from 0
            {
                pkey = true;
                kdDebug() << "Field is pkey" << endl;
            }
            else
            {
                pkey = false;
                kdDebug() << "Field is NOT pkey" << endl;
            }
        }
        else
        {
            pkey = false;
            kdDebug() << "Field is NOT pkey" << endl;
        }
    }
    catch(const std::exception &e)
    {
        kdDebug() << "pqxxSqlDB::primaryKey:exception - " << e.what() << endl;
        kdDebug() << "pqxxSqlDB::primaryKey:failed statement - " << statement << endl;
        pkey = false;
    }
    delete tmpres;
	tmpres = 0;
    
	delete tran;
	tran = 0;

    return pkey;
}

//=========================================================================
/*! Copy PostgreSQL table to KexiDB database */
bool PqxxMigrate::drv_copyTable(const QString& srcTable, KexiDB::Connection *destConn,
	KexiDB::TableSchema* dstTable)
{
    std::vector<std::string> R;

    pqxx::work T(*m_conn, "PqxxMigrate::drv_copyTable");

    pqxx::tablereader stream(T, (srcTable.latin1()));

    //Loop round each row, reading into a vector of strings
    for (int n=0; (stream >> R); ++n)
    {
        QValueList<QVariant> vals = QValueList<QVariant>();
        for (std::vector<std::string>::const_iterator i = R.begin(); i != R.end(); ++i)
        {
             QVariant var = QVariant((*i).c_str());
             vals << var;
        }

        destConn->insertRecord(*dstTable, vals);
        R.clear();
    }

    //This doesnt work in <libpqxx 2.2
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

    try
    {
        statement = QString("SELECT indkey FROM pg_index WHERE ((indisunique = true) AND (indrelid = %1))").arg(table_uid);

        tran = new pqxx::nontransaction(*m_conn, "find_ukey");
        tmpres = new pqxx::result(tran->exec(statement.latin1()));

        tran->commit();
        if (tmpres->size() > 0)
        {
            //We have a key field for this table, lets check if its this column
            tmpres->at(0).at(0).to(keyf);
            if (keyf-1 == col) //-1 because pg counts from 1 and we count from 0
            {
                ukey = true;
                kdDebug() << "Field is unique" << endl;
            }
            else
            {
                ukey = false;
                kdDebug() << "Field is NOT unique" << endl;
            }
        }
        else
        {
            ukey = false;
            kdDebug() << "Field is NOT unique" << endl;
        }
    }
    catch(const std::exception &e)
    {
        kdDebug() << "uniqueKey:exception - " << e.what() << endl;
        kdDebug() << "uniqueKey:failed statement - " << statement << endl;
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
