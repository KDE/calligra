/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/connection.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver_p.h>
#include <kexidb/connection.h>

#include "sqlitedriver.h"
#include "sqliteconnection.h"

#include <qfile.h>
#include <qdir.h>

#include <kgenericfactory.h>
#include <kdebug.h>

//K_EXPORT_COMPONENT_FACTORY(kexidb_sqlitedriver, KGenericFactory<KexiDB::SQLiteDriver>( "kexidb_sqlitedriver" ));

using namespace KexiDB;

KEXIDB_DRIVER_INFO( SQLiteDriver, sqlite, "sqlite" );

//! driver specific private data
class KexiDB::SQLiteDriverPrivate 
{
	public:
		SQLiteDriverPrivate()
		{
		}
};

//PgSqlDB::PgSqlDB(QObject *parent, const char *name, const QStringList &) 
SQLiteDriver::SQLiteDriver( QObject *parent, const char *name, const QStringList &args )
	: Driver( parent, name, args )
	,d( new SQLiteDriverPrivate() )
{
	m_isFileDriver = true;
	m_isDBOpenedAfterCreate = true;
	m_features = SingleTransactions | CursorForward;
	
	//special method for autoincrement definition
	beh->SPECIAL_AUTO_INCREMENT_DEF = true;
	beh->AUTO_INCREMENT_FIELD_OPTION = "INTEGER PRIMARY KEY";
	beh->ROW_ID_FIELD_NAME = "OID";
	beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY=true;

	m_typeNames[Field::Byte]="Byte";
	m_typeNames[Field::ShortInteger]="ShortInteger";
	m_typeNames[Field::Integer]="Integer";
	m_typeNames[Field::BigInteger]="BigInteger";
	m_typeNames[Field::Boolean]="Boolean";
	m_typeNames[Field::Date]="Date";
	m_typeNames[Field::DateTime]="DateTime";
	m_typeNames[Field::Time]="Time";
	m_typeNames[Field::Float]="Float";
	m_typeNames[Field::Double]="Double";
	m_typeNames[Field::Text]="Text";
	m_typeNames[Field::LongText]="CLOB";
	m_typeNames[Field::BLOB]="BLOB";
}

SQLiteDriver::~SQLiteDriver()
{
	delete d;
}


KexiDB::Connection* 
SQLiteDriver::drv_createConnection( ConnectionData &conn_data )
{
	return new SQLiteConnection( this, conn_data );
}

bool SQLiteDriver::isSystemObjectName( const QString& n ) const
{
	return Driver::isSystemObjectName(n) || n.lower().startsWith("sqlite_");
}

bool SQLiteDriver::isSystemFieldName( const QString& n ) const
{
	return n.lower()=="_rowid_"
		|| n.lower()=="rowid"
		|| n.lower()=="oid";
}

#include "sqlitedriver.moc"

