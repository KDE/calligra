/***************************************************************************
 *   Copyright (C) 2004 by Adam Pigg                                       *
 *   adam@piggz.co.uk                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "keximigrate.h"
#include <kdebug.h>
#include <kinputdialog.h>
#include <kapplication.h>

using namespace KexiDB;
using namespace KexiMigration;
/*
This is the implementation for the pqxx specific import routines
This is currently pre alpha and in no way is it meant
to compile, let alone work.  This is meant as an example of
what the system might be and is a work in progress
*/

//==================================================================================
//Constructor
KexiMigrate::KexiMigrate(KexiDB::ConnectionData* externalConnectionData, QString dbname, KexiDB::Connection* kexiConnection, QString newdbname, bool keep_data)
{
	m_externalData = externalConnectionData;
	m_kexiDB = kexiConnection;
	m_keepData = keep_data;
	m_dbName = dbname;
        m_todbname = newdbname;
}

//==================================================================================
//Destructor
KexiMigrate::~KexiMigrate()
{}

//==================================================================================
//Perform Import operation
bool KexiMigrate::performImport()
{
	QStringList tables;
	bool failure = false;
	
	//Step 1 - connect
	if (drv_connect())
	{
		//Step 2 - get table names
		if (tableNames(tables))
		{
			if (tables.size() > 0)
			{
				//There were some tables
				for (uint i = 0; i < tables.size(); i++)
				{
					//Step 3 - Read table schemas
					if(readTableSchema(tables[i], i))
					{
						//yeah, got a table
						//Add it to list of tables which we will create if all goes well
						v_tableSchemas.push_back(m_table);
					}
					else
					{
						failure = true;
					}
				}
			}
			else
			{
				kdDebug() << "There were no tables to import" << endl;
				failure = true;
			}
			drv_disconnect();
			//Only create a database if all tables were successfully read
			if(failure)
			{
				return false;
			}
			else
			{
				//Create new database as we have all required info ;)
				return createDatabase(m_todbname);
				return true;
			}
		}
		else
		{
			kdDebug() << "Couldnt get list of tables" << endl;
			return false;
		}
	}
else
{
	kdDebug() << "Couldnt connect to database server" << endl;
	return false;
}
}

//==================================================================================
//Create the final database
bool KexiMigrate::createDatabase(const QString& dbname)
{
	bool failure = false;
	
	kdDebug() << "Creating database [" << dbname << "]" << endl;
	if(m_kexiDB->connect())
	{
		if(m_kexiDB->createDatabase(dbname))
		{
			if (m_kexiDB->useDatabase(dbname))
			{
				//Right, were connected..create the tables
				for(uint i = 0; i < v_tableSchemas.size(); i++)
				{
					if(!m_kexiDB->createTable(v_tableSchemas[i]))
					{	
						kdDebug() << "Failed to create a table" << endl;
						m_kexiDB->debugError();
						failure = true;
					}
				}
				return not failure;
			}
			else
			{
				kdDebug() << "Couldnt use newly created database" << endl;
				return false;
			}
		}
		else
		{
			kdDebug() << "Couldnt create database at destination" << endl;
			return false;
		}
	}
	else
	{
		kdDebug() << "Couldnt connect to destination database" << endl;
		return false;
	}
}

//==================================================================================
//Get the table names
bool KexiMigrate::tableNames(QStringList & tn)
{
	kdDebug() << "Reading list of tables..." << endl;
	return drv_tableNames(tn);
}

//==================================================================================
//Get the table names
bool KexiMigrate::readTableSchema(const QString& t, int i)
{
	kdDebug() << "Reading table schema for [" << t << "]" << endl;
	return drv_readTableSchema(t);
}

//==================================================================================
//Prompt the user to choose a field type
KexiDB::Field::Type KexiMigrate::userType()
{
KInputDialog *dlg;
QStringList  types;
QString res;

types << "Byte";
types << "Short Integer";
types << "Integer";
types << "Big Integer";
types << "Boolean";
types << "Date";
types << "Date Time";
types << "Time";
types << "Float";
types << "Double";
types << "Text";
types << "Long Text";
types << "Binary Large Object";

res = dlg->getItem("Choose Field Type", "Field Type", types, 0, false);

if (res == types[0])
	return KexiDB::Field::Byte;
else if (res == types[1])
	return KexiDB::Field::ShortInteger;
else if (res == types[2])
	return KexiDB::Field::Integer;
else if (res == types[3])
	return KexiDB::Field::BigInteger;
else if (res == types[4])
	return KexiDB::Field::Boolean;
else if (res == types[5])
	return KexiDB::Field::Date;
else if (res == types[6])
	return KexiDB::Field::DateTime;
else if (res == types[7])
	return KexiDB::Field::Time;
else if (res == types[8])
	return KexiDB::Field::Float;
else if (res == types[9])
	return KexiDB::Field::Double;
else if (res == types[10])
	return KexiDB::Field::Text;
else if (res == types[11])
	return KexiDB::Field::LongText;
else if (res == types[12])
	return KexiDB::Field::BLOB;
else
	return KexiDB::Field::Text;

}