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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "pqxxmigrate.h"
#include "pg_type.h"

#include <qstring.h>
#include <kdebug.h>

#include "./kexidb/cursor.h"

using namespace KexiDB;
using namespace KexiMigration;

/*
This is the implementation for the pqxx specific import routines
Thi is currently pre alpha and in no way is it meant
to compile, let alone work.  This is meant as an example of
what the system might be and is a work in progress
*/

//==================================================================================
//Constructor
pqxxMigrate::pqxxMigrate(KexiDB::ConnectionData* externalConnectionData, QString dbname, KexiDB::Connection* kexiConnection, bool keep_data)
		:KexiMigrate(externalConnectionData, dbname, kexiConnection, keep_data)
{
m_res=0;
m_trans=0;
m_conn=0;
}

//==================================================================================
//Destructor
pqxxMigrate::~pqxxMigrate()
{}

//==================================================================================
//This is probably going to be quite complex...need to get the types for all columns
//any any other attributes required by kexi
//helped by reading the 'tables' test program
bool pqxxMigrate::drv_readTableSchema(const QString table)
{
	m_table = new KexiDB::TableSchema(table);

	//TODO IDEA: ask for user input for captions
	m_table->setCaption(table + " table");

	//Perform a query on the table to get some data
	if (query("select * from " + table + " limit 1"))
	{
		//Loop round the fields
		for (int i = 0; i < m_res->columns(); i++)
		{
			m_table->addField( m_f = new KexiDB::Field(m_res->column_name(i), type(m_res->column_type(i))));
			m_f->setCaption(m_res->column_name(i));
			m_f->setPrimaryKey(primaryKey(tableOid(table), i));
			m_f->setUniqueKey(uniqueKey(tableOid(table), i));
			//f->setLength(m_res->at(0)[i].size());
			kdDebug() << "Added field [" << m_res->column_name(i) << "] type [" << type(m_res->column_type(i)) << "]" << endl;
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
bool pqxxMigrate::drv_tableNames(QStringList& tableNames)
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
KexiDB::Field::Type pqxxMigrate::type(int t)
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
		//case NUMERICOID:
		//    return KexiDB::Field::Type::SQLNumeric;
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
	return userType();
}

//==================================================================================
//Connect to the db backend
bool pqxxMigrate::drv_connect()
{
	kdDebug() << "drv_connect: " << m_dbName << endl;

	QString conninfo;
	QString socket;

	//Setup local/remote connection
	if (m_externalData->hostName.isEmpty())
	{
		if (m_externalData->fileName().isEmpty())
		{
			socket="/tmp/.s.PGSQL.5432";
		}
		else
		{
			socket=m_externalData->fileName();
		}
	}
	else
	{
		conninfo = "host='" + m_externalData->hostName + "'";
	}

	//Build up the connection string
	if (m_externalData->port == 0)
		m_externalData->port = 5432;

	conninfo += QString::fromLatin1(" port='%1'").arg(m_externalData->port);

	conninfo += QString::fromLatin1(" dbname='%1'").arg(m_dbName);

	if (!m_externalData->userName.isNull())
		conninfo += QString::fromLatin1(" user='%1'").arg(m_externalData->userName);

	if (!m_externalData->password.isNull())
		conninfo += QString::fromLatin1(" password='%1'").arg(m_externalData->password);

	try
	{
		m_conn = new pqxx::connection( conninfo.latin1() );
		return true;
	}
	catch(const std::exception &e)
	{
		kdDebug() << "pqxxImport::drv_connect:exception - " << e.what() << endl;
	}
	catch(...)
	{
		kdDebug() << "pqxxMigrate::drv_connect:exception(...)??" << endl;
	}
	return false;
}

//==================================================================================
//Connect to the db backend
bool pqxxMigrate::drv_disconnect()
{
    if (m_conn)
    {
        m_conn->disconnect();
        delete m_conn;
    }
}
//==================================================================================
//Perform a query on the database and store result in m_res
bool pqxxMigrate::query (const QString& statement)
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
		kdDebug() << "pqxxMigrate::query:exception(...)??" << endl;
	}
}

//=========================================================================
//Clears the current result
void pqxxMigrate::clearResultInfo ()
{
	if (m_res)
	{
		delete m_res;
		m_res = 0;
	}
	if (m_trans)
	{
		delete m_trans;
		m_trans = 0;
	}
}

//=========================================================================
//Return the OID for a table
pqxx::oid pqxxMigrate::tableOid(const QString& table)
{
	QString statement;
	static QString otable;
	static pqxx::oid toid;

	pqxx::nontransaction* tran;
	pqxx::result* tmpres;
	
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
		kdDebug() << "pqxxMigrate::tableOid:exception(...)??" << endl;
	}
	if (tmpres)
		delete tmpres;
	if (tran)
		delete tran;

	kdDebug() << "OID for table [" << table << "] is [" << toid << "]" << endl;
	return toid;
}

//=========================================================================
//Return whether or not the curent field is a primary key
//TODO: Add result caching for speed
bool pqxxMigrate::primaryKey(pqxx::oid table_uid, int col) const
{
	QString statement;
	bool pkey;
	int keyf;

	pqxx::nontransaction* tran;
	pqxx::result* tmpres;

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
	if (tmpres)
		delete tmpres;
	if (tran)
		delete tran;

	return pkey;
}

//=========================================================================
//Return whether or not the curent field is a primary key
//TODO: Add result caching for speed
bool pqxxMigrate::uniqueKey(pqxx::oid table_uid, int col) const
{
	QString statement;
	bool ukey;
	int keyf;

	pqxx::nontransaction* tran;
	pqxx::result* tmpres;

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
	if (tmpres)
		delete tmpres;
	if (tran)
		delete tran;

	return ukey;
}

//==================================================================================
//Return a list of database names
/*bool pqxxMigrate::drv_getDatabasesList( QStringList &list )
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