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

#include <kexidb/driver.h>
#include <kexidb/driver_p.h>
#include "error.h"
#include "drivermanager.h"
#include "connection.h"
#include "connectiondata.h"

#include <qfileinfo.h>

#include <klocale.h>
#include <kdebug.h>

#include <assert.h>

using namespace KexiDB;

/*! used when we do not have Driver instance yet,
 or when we cannot get one */
QValueVector<QString> dflt_typeNames;


DriverBehaviour::DriverBehaviour()
	: UNSIGNED_TYPE_KEYWORD("UNSIGNED")
	, AUTO_INCREMENT_FIELD_OPTION("AUTO_INCREMENT")
	, SPECIAL_AUTO_INCREMENT_DEF(false)
	, USING_DATABASE_REQUIRED_TO_CONNECT(true)
{
}


Driver::Driver( QObject *parent, const char *name, const QStringList & )
	: QObject( parent, name )
	, Object()
	, m_isFileDriver(false)
	, m_isDBOpenedAfterCreate(false)
	, m_features(NoFeatures)
	, beh( new DriverBehaviour() )
	, d(0) //because unsused
{
	m_connections.setAutoDelete(false);
	//TODO: reasonable size
	m_connections.resize(101);
	m_typeNames.resize(Field::LastType + 1);
}

Driver::~Driver()
{
	KexiDBDbg << "Driver::~Driver()" << endl;
//	Connection *conn;
	QPtrDictIterator<Connection> it( m_connections );
	Connection *conn;
	while ( (conn = it.toFirst()) ) {
		delete conn;
	}
//	m_connections.clear();
	delete beh;
/*	for ( conn = m_connections.first(); conn ; conn = m_connections.next() ) {
		conn->disconnect();
		conn->m_driver = 0; //don't let the connection touch our driver now
		m_connections.remove();
		delete conn;
	}*/
	KexiDBDbg << "Driver::~Driver() ok" << endl;
}

bool Driver::isValid()
{
	QString not_init = i18n("not initialized for \"%1\" driver.").arg(name());
	if (beh->ROW_ID_FIELD_NAME.isEmpty()) {
		setError(ERR_INVALID_DRIVER_IMPL, QString("DriverBehaviour::ROW_ID_FIELD_NAME ") + not_init);
		return false;
	}
	return true;
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
	conn_data.driverName = name();
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

bool Driver::isSystemObjectName( const QString& n )
{
	return n.startsWith("kexi__");
}

bool Driver::isSystemFieldName( const QString& /*n*/ )
{
	return false;
}
