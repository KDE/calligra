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
#include <kexidb/drivermanager.h>
#include <kexidb/drivermanager_p.h>
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
	, _1ST_ROW_READ_AHEAD_REQUIRED_TO_KNOW_IF_THE_RESULT_IS_EMPTY(false)
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
	DriverManagerInternal::self()->aboutDelete( this );
//	KexiDBDbg << "Driver::~Driver()" << endl;
	QPtrDictIterator<Connection> it( m_connections );
	Connection *conn;
	while ( (conn = it.toFirst()) ) {
		delete conn;
	}
	delete beh;
//	KexiDBDbg << "Driver::~Driver() ok" << endl;
}

bool Driver::isValid()
{
	clearError();
	if (KexiDB::versionMajor() != versionMajor()) {
		setError(ERR_INCOMPAT_DRIVER_VERSION, 
		i18n("Incompatible database driver's \"%1\" version: found version %2, expected version %3.")
		.arg(name()).arg(versionMajor()).arg(KexiDB::versionMajor()));
		return false;
	}
		
	QString inv_impl = i18n("Invalid database driver's \"%1\" implementation:\n").arg(name());
	QString not_init = i18n("Value of \"%1\" is not initialized for the driver.");
	if (beh->ROW_ID_FIELD_NAME.isEmpty()) {
		setError(ERR_INVALID_DRIVER_IMPL, inv_impl + not_init.arg("DriverBehaviour::ROW_ID_FIELD_NAME"));
		return false;
	}
	
	return true;
}

const QPtrList<Connection> Driver::connectionsList() const
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
	if (!isValid())
		return 0;
	
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

bool Driver::isSystemObjectName( const QString& n ) const
{
	return Driver::isKexiDBSystemObjectName(n);
}

bool Driver::isKexiDBSystemObjectName( const QString& n )
{
	return n.lower().startsWith("kexi__");
}

QString Driver::valueToSQL( uint ftype, const QVariant& v ) const
{
	if (v.isNull())
		return "NULL";
	switch (ftype) {
		case Field::Byte:
		case Field::ShortInteger:
		case Field::Integer:
		case Field::Float:
		case Field::Double:
		case Field::BigInteger:
			return v.toString();
//TODO: here special encoding method needed
		case Field::Boolean:
			return QString::number(v.toInt()); //0 or 1
		case Field::Date:
		case Field::Time:
			return QString("\"")+v.toString()+"\"";
		case Field::DateTime:
			return QString("\"") + v.toDate().toString( Qt::ISODate ) + " " + v.toTime().toString( Qt::ISODate ) +"\"";
		case Field::Text:
		case Field::LongText: {
			QString s = v.toString();
			return escapeString(s); //QString("'")+s.replace( '"', "\\\"" ) + "'"; 
		}
		case Field::BLOB: {
//TODO: here special encoding method needed
			QString s = v.toString();
			return escapeString(s); //QString("'")+v.toString()+"'";
		}
		case Field::InvalidType:
			return "!INVALIDTYPE!";
		default:
			KexiDBDbg << "Driver::valueToSQL(): UNKNOWN!" << endl;
			return QString::null;
	}
	return QString::null;
}


#include "driver.moc"

