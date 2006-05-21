/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <kexidb/connection.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver_p.h>

#include "sqlite.h"
#include "sqlitedriver.h"
#include "sqliteconnection.h"
#include "sqliteconnection_p.h"
#include "sqliteadmin.h"

#include <kdebug.h>
//Added by qt3to4:
#include <Q3CString>

using namespace KexiDB;

#ifdef SQLITE2
KEXIDB_DRIVER_INFO( SQLiteDriver, sqlite2 )
#else
KEXIDB_DRIVER_INFO( SQLiteDriver, sqlite3 )
#endif

//! driver specific private data
//! @internal
class KexiDB::SQLiteDriverPrivate 
{
	public:
		SQLiteDriverPrivate()
		{
		}
};

//PgSqlDB::PgSqlDB(QObject *parent, const char *name, const QStringList &) 
SQLiteDriver::SQLiteDriver( QObject *parent, const QStringList &args )
	: Driver( parent, args )
	,dp( new SQLiteDriverPrivate() )
{
	d->isFileDriver = true;
	d->isDBOpenedAfterCreate = true;
	d->features = SingleTransactions | CursorForward
#ifndef SQLITE2
		| CompactingDatabaseSupported;
#endif
	;
	
	//special method for autoincrement definition
	beh->SPECIAL_AUTO_INCREMENT_DEF = true;
	beh->AUTO_INCREMENT_FIELD_OPTION = ""; //not available
	beh->AUTO_INCREMENT_TYPE = "INTEGER";
	beh->AUTO_INCREMENT_PK_FIELD_OPTION = "PRIMARY KEY";
	beh->AUTO_INCREMENT_REQUIRES_PK = true;
	beh->ROW_ID_FIELD_NAME = "OID";
	beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY=true;
	beh->QUOTATION_MARKS_FOR_IDENTIFIER='"';
	beh->SELECT_1_SUBQUERY_SUPPORTED = true;
	beh->SQL_KEYWORDS = keywords;
	initSQLKeywords(29);

	//predefined properties
	d->properties["client_library_version"] = sqlite_libversion();
	d->properties["default_server_encoding"] = 
#ifdef SQLITE2
		sqlite_libencoding();
#else //SQLITE3
		"UTF8"; //OK?
#endif

	d->typeNames[Field::Byte]="Byte";
	d->typeNames[Field::ShortInteger]="ShortInteger";
	d->typeNames[Field::Integer]="Integer";
	d->typeNames[Field::BigInteger]="BigInteger";
	d->typeNames[Field::Boolean]="Boolean";
	d->typeNames[Field::Date]="Date";
	d->typeNames[Field::DateTime]="DateTime";
	d->typeNames[Field::Time]="Time";
	d->typeNames[Field::Float]="Float";
	d->typeNames[Field::Double]="Double";
	d->typeNames[Field::Text]="Text";
	d->typeNames[Field::LongText]="CLOB";
	d->typeNames[Field::BLOB]="BLOB";
}

SQLiteDriver::~SQLiteDriver()
{
	delete dp;
}


KexiDB::Connection* 
SQLiteDriver::drv_createConnection( ConnectionData &conn_data )
{
	return new SQLiteConnection( this, conn_data );
}

bool SQLiteDriver::isSystemObjectName( const QString& n ) const
{
	return Driver::isSystemObjectName(n) || n.toLower().startsWith("sqlite_");
}

bool SQLiteDriver::drv_isSystemFieldName( const QString& n ) const
{
	QString lcName = n.toLower();
	return ( lcName == "_rowid_" )
		|| ( lcName =="rowid" )
		|| ( lcName == "oid");
}

QString SQLiteDriver::escapeString(const QString& str) const
{
	return QString("'")+QString(str).replace( '\'', "''" ) + "'";
}

Q3CString SQLiteDriver::escapeString(const Q3CString& str) const
{
	return Q3CString("'")+Q3CString(str).replace( '\'', "''" )+"'";
}

QString SQLiteDriver::escapeBLOB(const QByteArray& array) const
{
	return escapeBLOBInternal(array, BLOB_ESCAPING_TYPE_USE_X);
}

QString SQLiteDriver::drv_escapeIdentifier( const QString& str) const
{
	return QString(str).replace( '"', "\"\"" );
}

Q3CString SQLiteDriver::drv_escapeIdentifier( const Q3CString& str) const
{
	return Q3CString(str).replace( '"', "\"\"" );
}

AdminTools* SQLiteDriver::drv_createAdminTools() const
{
#ifdef SQLITE2
	return new AdminTools(); //empty impl.
#else
	return new SQLiteAdminTools();
#endif
}

#include "sqlitedriver.moc"
