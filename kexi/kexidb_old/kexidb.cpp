/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Joseph Wenninger <jowenn@kde.org>

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
 
#include <kdebug.h>
 
#include "kexidbinterfacemanager.h"
#include "kexidbrecord.h"
#include "kexidb.h"

KexiDB::KexiDB(QObject *parent, const char *name) : QObject(parent, name)
{
	kdDebug() << "KexiDB::KexiDB()" << endl;
	m_manager = 0;
}

KexiDB*
KexiDB::add(QString driver)
{
	/*! this loads (if needed) the plugin
	    and creates an instance, wich is returned.
	    it should be used in replacement of that one
	    oh, how we love c++
	*/

	kdDebug() << "KexDB::add" << endl;
	
	KexiDB *d = manager()->require(driver);
	if(d)
	{
		kdDebug() << "got driver..." << endl;
		appendManager(manager());
		return d;
	}
	return 0;
}

KexiDBDriver*
KexiDB::driverInfo(QString driver)
{
	return manager()->getDriverInfo(driver);
}

void
KexiDB::appendManager(KexiDBInterfaceManager *m)
{
	if(m)
		m_manager = m;
}

QStringList
KexiDB::getDrivers()
{
	return manager()->getDrivers();
}

KexiDBInterfaceManager*
KexiDB::manager()
{
	if(!m_manager)
		m_manager = new KexiDBInterfaceManager(this, "manager");

	return m_manager;
}

// it's not worth reading behind that line :)

KexiDBRecord*
KexiDB::queryRecord(QString, bool)
{
	return 0;
}


bool
KexiDB::connect(QString, QString, QString, QString, QString)
{
	/*! implemented for linking reasons */
	kdDebug() << "KexiDB::connect(host): sorry you are wrong here" << endl;
	return false;
}

bool
KexiDB::connect(QString, QString, QString, QString, QString, QString, bool)
{
	/*! implemented for linking reasons */
	kdDebug() << "KexiDB::connect(db): sorry you are wrong here" << endl;
	return false;
}

QString
KexiDB::driverName()
{
	return QString::fromLatin1("NONE");
}

bool
KexiDB::load(QString file)
{
	return false;
}

QStringList
KexiDB::tables()
{
	return QStringList();
}

QStringList
KexiDB::databases()
{
	return QStringList();
}

bool
KexiDB::query(QString)
{
	return false;
}

QString
KexiDB::escape(QString &)
{
	return QString::null;
}

bool
KexiDB::alterField(const QString& table, const QString& field, const QString& newFieldName,
 KexiDBField::ColumnType dtype, int length, bool notNull, const QString& defaultVal, bool autoInc)
{
	return false;
}

KexiDBResult*
KexiDB::getResult()
{
	return new KexiDBResult(this);
}

unsigned long
KexiDB::affectedRows()
{
	return 0;
}

KexiDB::~KexiDB()
{
}

#include "kexidb.moc"
