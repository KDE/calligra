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

#include "driver.h"
#include "error.h"
#include "drivermanager.h"
#include "connection.h"
#include "connectiondata.h"

#include <qfileinfo.h>

#include <klocale.h>

#include <assert.h>

using namespace KexiDB;

//! used when we do not have Driver instance yet
QValueVector<QString> dflt_typeNames;


Driver::Driver( QObject *parent, const char *name, const QStringList & )
	: QObject( parent, name )
	, Object()
	, m_isFileDriver(false)
	, m_isDBOpenedAfterCreate(false)
	, m_features(NoFeatures)
{
	m_connections.setAutoDelete(true);
	//reasonable size
	m_connections.resize(101);
}

Driver::~Driver()
{
	qDebug("Driver::~Driver()");
//	Connection *conn;
	m_connections.clear();
/*	for ( conn = m_connections.first(); conn ; conn = m_connections.next() ) {
		conn->disconnect();
		conn->m_driver = 0; //don't let the connection touch our driver now
		m_connections.remove();
		delete conn;
	}*/
	qDebug("Driver::~Driver() ok");
}

const QPtrList<Connection> Driver::connectionsList() 
{
	QPtrList<Connection> clist;
	QPtrDictIterator<Connection> it( m_connections );
	for( ; it.current(); ++it )
		clist.append( &(*it) );
	return clist; 
}

Connection *Driver::createConnection( ConnectionData &conn_data )
{
	clearError();
	if (m_isFileDriver) {
		if (conn_data.fileName().isEmpty()) {
			setError(ERR_MISSING_DB_LOCATION, i18n("File name expected for file-based database driver.") );
			return 0;
		}
	}
//	Connection *conn = new Connection( this, conn_data );
	Connection *conn = drv_createConnection( conn_data );
	m_connections.insert( conn, conn );
	return conn;
}

Connection* Driver::removeConnection( Connection *conn )
{
	clearError();
	return m_connections.take( conn );
}

QString Driver::defaultSQLTypeName(int id_t)
{
	if (dflt_typeNames.isEmpty()) {
		dflt_typeNames.resize(Field::LastType + 1);
		dflt_typeNames[Field::Byte]="Byte";
		dflt_typeNames[Field::ShortInteger]="ShortInteger";
		dflt_typeNames[Field::Integer]="Integer";
		dflt_typeNames[Field::BigInteger]="BigInteger";
		dflt_typeNames[Field::Boolean]="Boolean";
		dflt_typeNames[Field::Date]="Date";
		dflt_typeNames[Field::DateTime]="DateTime";
		dflt_typeNames[Field::Time]="Time";
		dflt_typeNames[Field::Float]="Float";
		dflt_typeNames[Field::Double]="Double";
		dflt_typeNames[Field::Text]="Text";
		dflt_typeNames[Field::LongText]="Text";
		dflt_typeNames[Field::BLOB]="BLOB";
	}
	return dflt_typeNames[id_t];
}

bool Driver::isSystemObjectName( const QString& /*n*/ )
{
	return false;
}

bool Driver::isSystemFieldName( const QString& /*n*/ )
{
	return false;
}
