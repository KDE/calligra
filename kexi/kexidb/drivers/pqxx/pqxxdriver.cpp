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
#include <kexidb/connection.h>

#include "pqxxdriver.h"
#include "pqxxconnection.h"

//#include <qfile.h>
//#include <qdir.h>

#include <kgenericfactory.h>
#include <kdebug.h>

K_EXPORT_COMPONENT_FACTORY(kexidb_pqxxsqldriver, KGenericFactory<KexiDB::pqxxSqlDriver>( "kexidb_pqxxsqldriver" ));

using namespace KexiDB;

pqxxSqlDriver::pqxxSqlDriver( QObject *parent, const char *name, const QStringList &args )
	: Driver( parent, name, args )
{
	m_driverName = "pqxxSql";
	m_isFileDriver = false;
	m_features = Transactions | CursorForward | CursorBackward;

	m_typeNames.resize(Field::LastType + 1);
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
	m_typeNames[Field::LongText]="Text";
	m_typeNames[Field::BLOB]="BLOB";
}

pqxxSqlDriver::~pqxxSqlDriver()
{
//	delete d;
}


KexiDB::Connection*
pqxxSqlDriver::drv_createConnection( ConnectionData &conn_data )
{
	return new pqxxSqlConnection( this, conn_data );
}

bool pqxxSqlDriver::isSystemObjectName( const QString& n )
{
	return false;
	//return n.lower().startsWith("sqlite_");
}

bool pqxxSqlDriver::isSystemFieldName( const QString& n )
{
	return false;
	//return n.lower()=="_rowid_";
}


#include "pqxxdriver.moc"
