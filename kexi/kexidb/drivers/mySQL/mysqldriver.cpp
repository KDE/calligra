/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
Daniel Molkentin <molkentin@kde.org>
Copyright (C) 2003   Joseph Wenninger<jowenn@kde.org>

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

#ifdef Q_WS_WIN
# include <mysql/config-win.h>
#endif
#include <mysql.h>
#include <mysql_version.h>
#define BOOL bool

#include <qvariant.h>
#include <qfile.h>
#include <qdict.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "mysqldriver.h"
#include "mysqlconnection.h"
#include <kexidb/field.h>
#include <kexidb/driver_p.h>

using namespace KexiDB;

KEXIDB_DRIVER_INFO( MySqlDriver, mysql );

/* TODO: Implement buffered/unbuffered, rather than buffer everything.
   Each MYSQL connection can only handle at most one unbuffered cursor,
   so MySqlConnection should keep count?
 */

/*! 
 * Constructor sets database features and
 * maps the types in KexiDB::Field::Type to the MySQL types.
 *
 * See: http://dev.mysql.com/doc/mysql/en/Column_types.html
 */
MySqlDriver::MySqlDriver(QObject *parent, const char *name, const QStringList &args) : Driver(parent, name,args)
{
	KexiDBDrvDbg << "MySqlDriver::MySqlDriver()" << endl;

	d->isFileDriver=false;
	d->features=IgnoreTransactions | CursorForward;

	beh->ROW_ID_FIELD_NAME="LAST_INSERT_ID()";
	beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE=true;
	beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY=false;
	beh->USING_DATABASE_REQUIRED_TO_CONNECT=false;
	beh->QUOTATION_MARKS_FOR_IDENTIFIER='`';
	beh->SQL_KEYWORDS = keywords;
	initSQLKeywords(331);
	
	//predefined properties
#if MYSQL_VERSION_ID < 40000
	d->properties["client_library_version"] = MYSQL_SERVER_VERSION; //nothing better
	d->properties["default_server_encoding"] = MYSQL_CHARSET; //nothing better
#elif MYSQL_VERSION_ID < 50000
//OK?	d->properties["client_library_version"] = mysql_get_client_version();
#endif

	d->typeNames[Field::Byte]="TINYINT";
	d->typeNames[Field::ShortInteger]="SMALLINT";
	d->typeNames[Field::Integer]="INT";
	d->typeNames[Field::BigInteger]="BIGINT";
	// Can use BOOLEAN here, but BOOL has been in MySQL longer
	d->typeNames[Field::Boolean]="BOOL";
	d->typeNames[Field::Date]="DATE";
	d->typeNames[Field::DateTime]="DATETIME";
	d->typeNames[Field::Time]="TIME";
	d->typeNames[Field::Float]="FLOAT";
	d->typeNames[Field::Double]="DOUBLE";
	d->typeNames[Field::Text]="VARCHAR";
	d->typeNames[Field::LongText]="LONGTEXT";
	d->typeNames[Field::BLOB]="BLOB";
}

MySqlDriver::~MySqlDriver()
{
}

KexiDB::Connection*
MySqlDriver::drv_createConnection( ConnectionData &conn_data )
{
	return new MySqlConnection( this, conn_data );
}

bool MySqlDriver::isSystemDatabaseName(const QString &n) const
{
	return n.lower()=="mysql";
}

bool MySqlDriver::isSystemFieldName( const QString&) const {
	return false;
}

QString MySqlDriver::escapeString(const QString& str) const
{
	return QString("'")+QString(str).replace( '\'', "''" ) + "'";
}

QCString MySqlDriver::escapeString(const QCString& str) const
{
	return QCString("'")+QCString(str).replace( '\'', "''" )+"'";
}

/*! Add back-ticks to an identifier, and replace any back-ticks within
 * the name with single quotes.
 */
QString MySqlDriver::drv_escapeIdentifier( const QString& str) const {
	return QString(str).replace('`', "'");
}

QCString MySqlDriver::drv_escapeIdentifier( const QCString& str) const {
	return QCString(str).replace('`', "'");
}

#include "mysqldriver.moc"

