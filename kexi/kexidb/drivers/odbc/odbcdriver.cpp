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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

//QT Includes
#include <qfile.h>
#include <qdir.h>
#include <qstring.h>
#include <qcstring.h>

//KDE Includes
#include <kgenericfactory.h>
#include <kdebug.h>

//Kexi Includes
#include <kexidb/connection.h>
#include <kexidb/drivermanager.h>
#include <kexidb/connection.h>
#include <kexidb/driver_p.h>

//ODBC Includes
#include "odbcdriver.h"
#include "odbcconnection.h"

//K_EXPORT_COMPONENT_FACTORY(kexidb_odbcdriver, KGenericFactory<KexiDB::ODBCDriver>( "kexidb_odbcdriver" ))

using namespace KexiDB;

KEXIDB_DRIVER_INFO( ODBCDriver, odbc, "ODBC" );

ODBCDriver::ODBCDriver( QObject *parent, const char *name, const QStringList &args )
	: Driver( parent, name, args )
{
	m_isFileDriver = false;
	m_isDBOpenedAfterCreate = true;
	m_features = SingleTransactions | CursorForward;

	m_typeNames[ Field::Byte ] = "Byte";
	m_typeNames[ Field::ShortInteger ] = "ShortInteger";
	m_typeNames[ Field::Integer ] = "Integer";
	m_typeNames[ Field::BigInteger ] = "BigInteger";
	m_typeNames[ Field::Boolean ] = "Boolean";
	m_typeNames[ Field::Date ] = "Date";
	m_typeNames[ Field::DateTime ] = "DateTime";
	m_typeNames[ Field::Time ] = "Time";
	m_typeNames[ Field::Float ] = "Float";
	m_typeNames[ Field::Double ] = "Double";
	m_typeNames[ Field::Text ] = "Text";
	m_typeNames[ Field::LongText ] = "CLOB";
	m_typeNames[ Field::BLOB ] = "BLOB";
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

QCString ODBCDriver::escapeString( const QCString& str ) const
{
	return str;
}

#include "odbcdriver.moc"

