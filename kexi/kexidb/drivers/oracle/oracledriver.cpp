/* This file is part of the KDE project
   Copyright (C) 2008 Julia Sanchez-Simon <hithwen@gmail.com>
   Copyright (C) 2008 Miguel Angel Aragüez-Rey <fizban87@gmail.com>

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

#include "oracledriver.h"
#include "oracleconnection.h"
#include <kexidb/driver_p.h>
//#include <occiCommon.h>
#include <kdebug.h>
using namespace KexiDB;

K_EXPORT_KEXIDB_DRIVER(OracleDriver, "oracle")

/**
 * Constructor sets database features and maps
 * types to Oracle datatypes.
 */
OracleDriver::OracleDriver( QObject *parent, const QVariantList &args)
	: Driver(parent,args)
{
	//KexiDBDrvDbg << "OracleDriver::Constructor: ";
	// Set database features and properties
	d->isFileDriver = false;
	d->features = SingleTransactions | CursorForward;

// TODO: Fill and fix (fix, fix)
	// This may be completely wrong
	d->typeNames[Field::Integer] = "INTEGER";
	d->typeNames[Field::BigInteger] = "NUMBER";
	d->typeNames[Field::Text] = "VARCHAR2";

	d->typeNames[Field::Byte]="NUMBER(3)"; // Unspecified by Oracle
	d->typeNames[Field::ShortInteger]="INTEGER";
	d->typeNames[Field::Integer]="INTEGER";
	d->typeNames[Field::BigInteger]="INTEGER"; // 
	d->typeNames[Field::Boolean]="NUMBER(1)"; // Unspecified (Unsupported?)
	d->typeNames[Field::Date]="DATE";
	d->typeNames[Field::DateTime]="TIMESTAMP";
	d->typeNames[Field::Time]="DATE";
	d->typeNames[Field::Float]="FLOAT"; //
	d->typeNames[Field::Double]="BINARY_DOUBLE"; // Number?
	d->typeNames[Field::Text]="VARCHAR2"; // Length? not needed
	d->typeNames[Field::LongText]="LONG";
	d->typeNames[Field::BLOB]="BLOB";

	// Custom behaviour
	beh->ROW_ID_FIELD_NAME="ROW_ID";
	// beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE=true; I don't think so
	beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY=false;
	beh->USING_DATABASE_REQUIRED_TO_CONNECT=false;
	beh->QUOTATION_MARKS_FOR_IDENTIFIER='\'';
	beh->UNSIGNED_TYPE_KEYWORD="";
	//Autoincrement
	beh->SPECIAL_AUTO_INCREMENT_DEF = false;
	beh->AUTO_INCREMENT_FIELD_OPTION="";
	beh->AUTO_INCREMENT_PK_FIELD_OPTION="PRIMARY KEY";
	beh->AUTO_INCREMENT_TYPE = "";

	initDriverSpecificKeywords(keywords);
	//beh->SQL_KEYWORDS = keywords;
	//initSQLKeywords(171); // 171 as returned from script
	
	//predefined properties
	d->properties["client_library_version"] = OCCI_MAJOR_VERSION;
	d->properties["default_server_encoding"] = OCCI_MINOR_VERSION;
}

OracleDriver::~OracleDriver()
{}

/**
 * Creates a new connection.
 */
KexiDB::Connection* OracleDriver::drv_createConnection(ConnectionData & conn_data)
{
	return new OracleConnection(this, conn_data);
}

/**
 * System resources are tables and views located in their own
 * schemas (SYSTEM and SYS). There is no special object which
 * cannot be used, although access to some could be denied due
 * to insufficient privileges.
 */
bool OracleDriver::isSystemDatabaseName(const QString& /*n*/) const
{
	return false;
}
/**
 * See KexiDB::OracleDriver::isSystemDatabaseName(const QString &n) const.
 */
bool OracleDriver::drv_isSystemFieldName(const QString&) const {
	return false;
}
QString OracleDriver::valueToSQL(uint ftype, const QVariant & v) const
{
  switch (ftype) {
    case Field::Time:
      return "TO_DATE("+escapeString(v.toString())+", 'HH24:MI:SS')";
    case Field::Date:
      //return "TO_DATE("+escapeString(v.toString())+", 'DD-MM-YYYY')";
    case Field::DateTime:
    //return dateTimeToSQL(v.toDateTime());
      return "TO_DATE("+escapeString(v.toString())+", 'DD-MM-YYYY HH24:MI:SS')";
    default:
      return Driver::valueToSQL(ftype,v);
  }
}
/**
 * Add single quotes at the beginning and the end of the string, and escapes any
 * single quotes found within
 */
QString OracleDriver::escapeString(const QString& str) const
{
 //KexiDBDrvDbg <<str;
  QString res = str;
  if (res[0]!='\''){
	  return QString("\'"+res.replace("'","''")+"\'");
	}else{
	  return QString(res.replace("'","''")).mid(1,res.length()-2);
	}
}

/**
 * See KexiDB::OracleDriver::escapeString(const QString& str) const.
 */
QByteArray OracleDriver::escapeString(const QByteArray& str) const
{
//KexiDBDrvDbg<<str;
  if (str[0]!='\''){
    return QByteArray("\'"+str+"\'");
  }else{
	  return QByteArray(str);
	}
}

/*
 * I don't understand this, but what seems quite clear is that Oracle
 * doesn't accept hexadecimal values as one would expect, with '0x' prefix
 * or similar. SQL allows, however, to get the decimal value of a hexadecimal
 * string (without any prefixes). That is, TO_NUMBER('FF','XXXX') returns 255.
 * Therefore, I think this won't work or won't do it as expected.
 *
 * Note:
 * As I haven't fully understood this function, all the above hexadecimal stuff
 * may be useless.
 */
QString OracleDriver::escapeBLOB(const QByteArray& array) const
{
  KexiDBDrvDbg<<array;
	return QString(array);
	//return KexiDB::escapeBLOB(array, KexiDB::BLOBEscape0xHex);
}

/**
 * Add back-ticks to an identifier, and replace any back-ticks within
 * the name with single quotes.
 */
QString OracleDriver::drv_escapeIdentifier( const QString& str) const {
	return QString(str).replace('`', "'");
}

QByteArray OracleDriver::drv_escapeIdentifier( const QByteArray& str) const {
	return QByteArray(str).replace('`', "'");
}

