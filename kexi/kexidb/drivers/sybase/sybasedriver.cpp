/* This file is part of the KDE project
Copyright (C) 2007   Sharan Rao <sharanrao@gmail.com>

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

#include <QVariant>
#include <QFile>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "sybasedriver.h"
#include "sybaseconnection.h"
#include <kexidb/field.h>
#include <kexidb/driver_p.h>
#include <kexidb/utils.h>

using namespace KexiDB;

KEXIDB_DRIVER_INFO( SybaseDriver, sybase )

/*!
 * Constructor sets database features and
 * maps the types in KexiDB::Field::Type to the Sybase types.
 *
 */
SybaseDriver::SybaseDriver(QObject *parent, const QStringList &args) :
	Driver(parent, args)
{

//	KexiDBDrvDbg << "SybaseDriver::SybaseDriver()" << endl;

	d->isFileDriver= false ;

        // Sybase supports Nested Transactions. Ignore for now
 	d->features= IgnoreTransactions | CursorForward;

        // Last value assigned is stored in variable @@IDENTITY
	beh->ROW_ID_FIELD_NAME="@@IDENTITY";

        beh->ROW_ID_FIELD_RETURNS_LAST_AUTOINCREMENTED_VALUE = true ;

        beh->_1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY=false;
        beh->USING_DATABASE_REQUIRED_TO_CONNECT=false;

        // for Sybase ASA this field is "DEFAULT AUTOINCREMENT"
        // for MSSQL and Sybase ASE it's IDENTITY
        beh->AUTO_INCREMENT_FIELD_OPTION="IDENTITY";
        beh->AUTO_INCREMENT_PK_FIELD_OPTION = beh->AUTO_INCREMENT_FIELD_OPTION + " PRIMARY KEY ";

        // confirm
        beh->SELECT_1_SUBQUERY_SUPPORTED = true;

        beh->QUOTATION_MARKS_FOR_IDENTIFIER='"';

        initDriverSpecificKeywords(keywords);


	//predefined properties
	d->properties["client_library_version"] = "";//TODO
	d->properties["default_server_encoding"] = ""; //TODO

        // datatypes
        // integers
	d->typeNames[Field::Byte]="TINYINT";
	d->typeNames[Field::ShortInteger]="SMALLINT";
	d->typeNames[Field::Integer]="INT";
	d->typeNames[Field::BigInteger]="BIGINT";

        // boolean
	d->typeNames[Field::Boolean]="BIT";

        // date and time
	d->typeNames[Field::Date]="DATE";
	d->typeNames[Field::DateTime]="DATETIME";
	d->typeNames[Field::Time]="TIME";

        // floating point
	d->typeNames[Field::Float]="FLOAT";
	d->typeNames[Field::Double]="DOUBLE";

        // strings
	d->typeNames[Field::Text]="VARCHAR";
	d->typeNames[Field::LongText]="TEXT";

        // Large Binary Objects
        d->typeNames[Field::BLOB]="IMAGE";
}

SybaseDriver::~SybaseDriver()
{
}

KexiDB::Connection*
SybaseDriver::drv_createConnection( ConnectionData &conn_data )
{
	return new SybaseConnection( this, conn_data );
}

bool SybaseDriver::isSystemDatabaseName(const QString &n) const
{
    QStringList systemDatabases;
    systemDatabases<<QString::fromLatin1( "master" )
               <<QString::fromLatin1( "model" )
               <<QString::fromLatin1( "sybsystemprocs" )
               <<QString::fromLatin1( "tempdb" )
               <<QString::fromLatin1( "sybsecurity" )
               <<QString::fromLatin1( "sybsystemdb" )
               <<QString::fromLatin1( "pubs2" )
               <<QString::fromLatin1( "pubs3" )
               <<QString::fromLatin1( "dbccdb" );

    QStringList::iterator i = qFind( systemDatabases.begin(),systemDatabases.end(), n.toLower() );
    if ( i!=systemDatabases.end() )
        return true;

    return Driver::isSystemObjectName(n);
}

bool SybaseDriver::drv_isSystemFieldName(const QString&) const {
	return false;
}

QString SybaseDriver::escapeString(const QString& str) const
{
    return QString::fromLatin1("'") +
           QString( str ).replace( "\'", "\\''" ) +
           QString::fromLatin1( "'" );
}

QString SybaseDriver::escapeBLOB(const QByteArray& array) const
{
	return KexiDB::escapeBLOB(array, KexiDB::BLOBEscape0xHex);
}

QByteArray SybaseDriver::escapeString(const QByteArray& str) const
{
    // needs any modification ?
    return QByteArray("'")+QByteArray(str)
		.replace( "\'", "\\''" )
           + QByteArray("'");
}

QString SybaseDriver::drv_escapeIdentifier(const QString& str) const
{
    // verify
    return QString("\"") + QString(str).replace( "\\","\\\\").replace( "\"", "\"\"")
         + QString("\"");
}

QByteArray SybaseDriver::drv_escapeIdentifier(const QByteArray& str) const
{
    // verify
    return QByteArray("\"") + QByteArray(str)
            .replace( "\\", "\\\\" )
            .replace( "\"", "\"\"" )
            + QByteArray( "\"" ) ;
}

QString SybaseDriver::addLimitTo1(const QString& sql, bool add )
{
    // length of "select" is 6
    // eg: before:  select foo from foobar
    //     after:   select TOP 1 foo from foobar
    QString returnString = sql.trimmed().insert( 6, " TOP 1 " );
    return add ? returnString : sql;
}

#include "sybasedriver.moc"

