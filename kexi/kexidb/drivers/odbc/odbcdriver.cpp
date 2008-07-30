/* This file is part of the KDE project
   Copyright (C) 2004 Matt Rogers <matt.rogers@kdemail.net>

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

//QT Includes
#include <qfile.h>
#include <qdir.h>

//KDE Includes
#include <kdebug.h>

//Kexi Includes
#include <kexidb/connection.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver_p.h>

//ODBC Includes
#include "odbcdriver.h"
#include "odbcconnection.h"

using namespace KexiDB;

KEXIDB_DRIVER_INFO( ODBCDriver, odbc )

ODBCDriver::ODBCDriver( QObject *parent, const char *name, const QStringList &args )
  : Driver( parent, name, args )
{
  d->isFileDriver = false;
  d->isDBOpenedAfterCreate = true;
  d->features = SingleTransactions | CursorForward;

  //predefined properties
  d->properties["client_library_version"] = "";//TODO
  d->properties["default_server_encoding"] = ""; //TODO

  d->typeNames[ Field::Byte ] = "Byte";
  d->typeNames[ Field::ShortInteger ] = "ShortInteger";
  d->typeNames[ Field::Integer ] = "Integer";
  d->typeNames[ Field::BigInteger ] = "BigInteger";
  d->typeNames[ Field::Boolean ] = "Boolean";
  d->typeNames[ Field::Date ] = "Date";
  d->typeNames[ Field::DateTime ] = "DateTime";
  d->typeNames[ Field::Time ] = "Time";
  d->typeNames[ Field::Float ] = "Float";
  d->typeNames[ Field::Double ] = "Double";
  d->typeNames[ Field::Text ] = "Text";
  d->typeNames[ Field::LongText ] = "CLOB";
  d->typeNames[ Field::BLOB ] = "BLOB";
}

ODBCDriver::~ODBCDriver()
{
}

KexiDB::Connection* ODBCDriver::drv_createConnection( ConnectionData &conn_data )
{
  Q_UNUSED( conn_data );
  return 0L;
  //return new ODBCConnection( this, conn_data );
}

bool ODBCDriver::isSystemDatabaseName( const QString& name ) const
{
  Q_UNUSED( name );
  return false;
}

bool ODBCDriver::isSystemObjectName( const QString& name )
{
  Q_UNUSED( name );
  return false;
}

bool ODBCDriver::isSystemFieldName( const QString& name ) const
{
  Q_UNUSED( name );
  return false;
}

QString ODBCDriver::escapeString( const QString& str ) const
{
  return str;
}

QByteArray ODBCDriver::escapeString( const QByteArray& str ) const
{
  return str;
}

#include "odbcdriver.moc"

