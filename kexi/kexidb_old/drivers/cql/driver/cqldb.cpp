/* This file is part of the KDE project
Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

//#include <qstringlist.h>
#include <kgenericfactory.h>
#include <kdebug.h>

#include "cqldb.h"

K_EXPORT_COMPONENT_FACTORY(kexicqlinterface, KGenericFactory<CqlDB>( "kexicqlinterface" ));

CqlDB::CqlDB(QObject *parent, const char *name, const QStringList &)
 : KexiDB(parent, name)
{
	kdDebug() << "CqlDB::CqlDB()" << endl;

	try
	{
		m_db = new SqlHandle;
	}
	catch(CqlException& ex)
	{
		cerr << ex << endl;
		m_db = 0;
	}
}

QString
CqlDB::driverName()
{
	return QString("cql");
}

bool
CqlDB::load(QString file)
{
	kdDebug() << "CqlDB::load()" << endl;

	if(!m_db)
		return false;

	try
	{
		m_db->connect("PUBLIC");
	}
	catch(CqlException& ex)
	{
		cerr << ex << endl;
	}
}

QStringList
CqlDB::tables()
{
	return QStringList();
}

bool
CqlDB::query(QString)
{
}

CqlDB::~CqlDB()
{
}

#include "cqldb.moc"
