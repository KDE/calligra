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
#include <mysql/mysql.h>
#define BOOL bool

#include <qvariant.h>
#include <qfile.h>
#include <qdict.h>

#include <kgenericfactory.h>
#include <kdebug.h>

#include "mysqldriver.h"
#include "mysqldriver.moc"
#include "mysqlconnection.h"
#include <kexidb/field.h>


K_EXPORT_COMPONENT_FACTORY(kexidb_mysqldriver, KGenericFactory<KexiDB::MySqlDriver>( "kexidb_mysqldriver" ))

using namespace KexiDB;

MySqlDriver::MySqlDriver(QObject *parent, const char *name, const QStringList &list) : Driver(parent, name,list)
{
	KexiDBDrvDbg << "MySqlDB::MySqlDB()" << endl;

	m_driverName = "mySQL";
	m_isFileDriver=false;
	m_features=CursorForward;

        m_typeNames[Field::Byte]="UNSIGNED TINYINT";
        m_typeNames[Field::ShortInteger]="SMALLINT";
        m_typeNames[Field::Integer]="INT";
        m_typeNames[Field::BigInteger]="BIGINT";
        m_typeNames[Field::Boolean]="BOOLEAN";
        m_typeNames[Field::Date]="DATE";
        m_typeNames[Field::DateTime]="DATETIME";
        m_typeNames[Field::Time]="TIME";
        m_typeNames[Field::Float]="FLOAT";
        m_typeNames[Field::Double]="DOUBLE";
        m_typeNames[Field::Text]="TEXT";
        m_typeNames[Field::LongText]="LONGTEXT";
        m_typeNames[Field::BLOB]="BLOB"; 

}

KexiDB::Connection*
MySqlDriver::drv_createConnection( ConnectionData &conn_data )
{
        return new MySqlConnection( this, conn_data );
}


MySqlDriver::~MySqlDriver() {
}
