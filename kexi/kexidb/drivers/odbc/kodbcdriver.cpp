/* This file is part of the KDE project
   Copyright (C) 2009 Sharan Rao <sharanrao@gmail.com>

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

#include "kodbcdriver.h"
#include "odbcdriver.h"
#include "kexidb/connection.h"
#include <db/field.h>
#include <db/driver_p.h>
#include <db/utils.h>

#include <QVariant>
#include <QFile>

#include <sql.h>
#include <sqltypes.h>

#include <kdebug.h>

using namespace KexiDB;

K_EXPORT_KEXIDB_DRIVER(KODBCDriver, "odbc")

class KODBCDriver::KODBCDriverPrivate {
public:
    QMap<ConnectionData, ODBCDriver*> connDataDriverMap;

};

/*!
 * Constructor sets database features and
 * maps the types in KexiDB::Field::Type to the ODBC types.
 *
 */
KODBCDriver::KODBCDriver(QObject *parent, const QVariantList &args) :
        Driver(parent, args), k_d( new KODBCDriverPrivate )
{
// KexiDBDrvDbg << "KODBCDriver::KODBCDriver()";

    // ah what crap. do we need all this ?

    d->isFileDriver = false;
    d->features = IgnoreTransactions | CursorForward;

    beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY = false;
    beh->USING_DATABASE_REQUIRED_TO_CONNECT = false;
    beh->QUOTATION_MARKS_FOR_IDENTIFIER = '\"';
//    initDriverSpecificKeywords(keywords);

    d->typeNames[Field::Byte] = "SQL_CHAR";
    d->typeNames[Field::ShortInteger] = "SQL_SMALLINT";
    d->typeNames[Field::Integer] = "SQL_INTEGER";
    d->typeNames[Field::BigInteger] = "SQL_BIGINT";
    d->typeNames[Field::Boolean] = "SQL_BINARY";
    d->typeNames[Field::Date] = "SQL_TYPE_DATE";
    d->typeNames[Field::DateTime] = "SQL_TYPE_TIMESTAMP";
    d->typeNames[Field::Time] = "SQL_TYPE_TIME";
    d->typeNames[Field::Float] = "SQL_FLOAT";
    d->typeNames[Field::Double] = "SQL_DOUBLE";
    d->typeNames[Field::Text] = "SQL_VARCHAR";
    d->typeNames[Field::LongText] = "SQL_LONGVARCHAR";
    d->typeNames[Field::BLOB] = "SQL_VARBINARY";
}

KODBCDriver::~KODBCDriver()
{
    delete k_d;
}

KexiDB::Connection*
KODBCDriver::drv_createConnection(ConnectionData &conn_data)
{
    ODBCDriver* odbcDriver = 0;
    if ( ( odbcDriver = getODBCDriver( conn_data ) ) == 0 )
        odbcDriver = new ODBCDriver( this );
    return odbcDriver->createConnection( conn_data );
}

bool KODBCDriver::isSystemDatabaseName(const QString& /* n */ ) const
{
    // TODO. somehow query the data source to find out.
    return false;
}

bool KODBCDriver::isSystemObjectName(const QString& /* name */ ) const
{
    return false;
}

bool KODBCDriver::drv_isSystemFieldName(const QString&) const
{
    return false;
}

QString KODBCDriver::escapeString(const QString& str) const
{
    return str;
}

QString KODBCDriver::escapeBLOB(const QByteArray& array) const
{
    return array;
}

QByteArray KODBCDriver::escapeString(const QByteArray& str) const
{
    return str;
}

/*! Add back-ticks to an identifier, and replace any back-ticks within
 * the name with single quotes.
 */
QString KODBCDriver::drv_escapeIdentifier(const QString& str) const
{
    return str;
}

QByteArray KODBCDriver::drv_escapeIdentifier(const QByteArray& str) const
{
    return str;
}

ODBCDriver* KODBCDriver::getODBCDriver( const ConnectionData& connData ) {
    QMap<ConnectionData, ODBCDriver*>::iterator it = k_d->connDataDriverMap.find( connData );
    if ( it == k_d->connDataDriverMap.end() )
        return 0;
    return it.value();
}

bool operator<( const ConnectionData& cd1,  const ConnectionData& cd2 ) {
    return cd1.id < cd2.id;
}

#include "kodbcdriver.moc"

