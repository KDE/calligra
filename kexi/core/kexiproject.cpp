/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qfile.h>
#include <qapplication.h>
#include <qdom.h>

#include <kmimetype.h>
#include <kdebug.h>
#include <klocale.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "kexipartmanager.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexiproject.h"
#include "kexi.h"

#include <assert.h>

KexiProject::KexiProject(KexiProjectData *pdata)
 : QObject(), Object()
 , m_data(pdata)
{
//	m_drvManager = new KexiDB::DriverManager();
//	m_connData = new KexiProjectConnectionData();
//js	m_partManager = new KexiPart::Manager(this);

//TODO: partmanager is outside project, so can be initialised just once:

	m_itemListsCache = QIntDict<KexiPart::ItemList>(199);
	m_itemListsCache.setAutoDelete(true);
	Kexi::partManager().lookup();
	
	m_connection = 0;
}

KexiProject::~KexiProject()
{
	closeConnection();
	delete m_data;
	m_data=0;
}

bool
KexiProject::open()
{
	kdDebug() << "KexiProject::open(): " << m_data->databaseName() <<" "<< m_data->connectionData()->driverName  << endl;
	m_error_title = i18n("Could not open project \"%1\"").arg(m_data->databaseName());
	if (!createConnection()) {
		kdDebug() << "KexiProject::open(): !createConnection()" << endl;
		return false;
	}
	if (!m_connection->useDatabase(m_data->databaseName()))
	{
		kdDebug() << "KexiProject::open(): !m_connection->useDatabase() " << m_data->databaseName() <<" "<< m_data->connectionData()->driverName  << endl;
		setError(m_connection);
		closeConnection();
		return false;
	}

	initProject();

	return true;
}

bool 
KexiProject::create()
{
	m_error_title = i18n("Could not create project \"%1\"").arg(m_data->databaseName());
	if (!createConnection())
		return false;
	if (m_connection->databaseExists( m_data->databaseName() )) {
		if (!m_connection->dropDatabase( m_data->databaseName() )) {
			setError(m_connection);
			closeConnection();
			return false;
		}
		kdDebug() << "--- DB '" << m_data->databaseName() << "' dropped ---"<< endl;
	}
	if (!m_connection->createDatabase( m_data->databaseName() )) {
		setError(m_connection);
		closeConnection();
		return false;
	}
	kdDebug() << "--- DB '" << m_data->databaseName() << "' created ---"<< endl;
	// and now: open
	if (!m_connection->useDatabase(m_data->databaseName()))
	{
		kdDebug() << "--- DB '" << m_data->databaseName() << "' USE ERROR ---"<< endl;
		setError(m_connection);
		closeConnection();
		return false;
	}
	kdDebug() << "--- DB '" << m_data->databaseName() << "' used ---"<< endl;
	
	initProject();
	
	return true;
}

bool
KexiProject::createConnection()
{
	closeConnection();//for sanity
	clearError();
	
	KexiDB::Driver *driver = Kexi::driverManager().driver(m_data->connectionData()->driverName);
	if(!driver) {
		setError(&Kexi::driverManager());
		return false;
	}

	m_connection = driver->createConnection(*m_data->connectionData());
	if (!m_connection)
	{
		kdDebug() << "KexiProject::open(): uuups faild " << driver->errorMsg()  << endl;
		setError(driver);
		return false;
	}

	if (!m_connection->connect())
	{
		setError(m_connection);
		kdDebug() << "KexiProject::openConnection() errror connecting: " << m_connection->errorMsg() << endl;
		closeConnection();
		return false;
	}

	return true;
}


void
KexiProject::closeConnection()
{
	if (!m_connection)
		return;
		
	delete m_connection;
	m_connection = 0;
}

void
KexiProject::initProject()
{
//	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	
	Kexi::partManager().checkProject(m_connection);
}

bool
KexiProject::isConnected()
{
	if(m_connection && m_connection->isDatabaseUsed())
		return true;

	return false;
}

KexiPart::ItemList
KexiProject::items(KexiPart::Info *i)
{
	kdDebug() << "KexiProject::items()" << endl;
	if(!i || !isConnected())
		return KexiPart::ItemList();

	//trying in cache...
	KexiPart::ItemList *list = m_itemListsCache[ i->projectPartID() ];
	if (list)
		return *list;
	//retrieve:
	KexiDB::Cursor *cursor = m_connection->executeQuery(
		"SELECT o_id, o_name, o_caption  FROM kexi__objects WHERE o_type = " 
		+ QString::number(i->projectPartID()), KexiDB::Cursor::Buffered);
	if(!cursor)
		return KexiPart::ItemList();

	list = new KexiPart::ItemList();

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		KexiPart::Item it;
		it.setIdentifier(cursor->value(0).toInt());
		it.setMime(i->mime());
		it.setName(cursor->value(1).toString());
		it.setCaption(cursor->value(2).toString());

		list->append(it);
		kdDebug() << "KexiProject::items(): ITEM ADDED == "<<cursor->value(1).toString()<<endl;
	}

	m_connection->deleteCursor(cursor);
	kdDebug() << "KexiProject::items(): end with count " << list->count() << endl;
	m_itemListsCache.insert( i->projectPartID(), list );
	return *list;
}

KexiPart::ItemList
KexiProject::items(const QString &mime)
{
	KexiPart::Info *info = Kexi::partManager().info(mime);
	return items(info);
}

KexiPart::Item
KexiProject::item(const QString &mime, const QString &name)
{
	KexiPart::ItemList &list = items(mime);
	const QString &l_name = name.lower();
	for (KexiPart::ItemList::Iterator it = list.begin(); it!=list.end(); ++ it) {
		if ((*it).name().lower()==l_name)
			return *it;
	}
	return KexiPart::Item();
}

KexiPart::Item
KexiProject::item(KexiPart::Info *i, const QString &name)
{
	KexiPart::ItemList &list = items(i);
	const QString &l_name = name.lower();
	for (KexiPart::ItemList::Iterator it = list.begin(); it!=list.end(); ++ it) {
		if ((*it).name().lower()==l_name)
			return *it;
	}
	return KexiPart::Item();
}

void KexiProject::setError(int code, const QString &msg )
{
	Object::setError(code, msg);
	if (Object::error())
		emit error(m_error_title, this);
}

void KexiProject::setError( const QString &msg )
{
	Object::setError(msg);
	if (Object::error())
		emit error(m_error_title, this);
}

void KexiProject::setError( KexiDB::Object *obj )
{
	if (!obj)
		return;
	Object::setError(obj);
	if (Object::error())
		emit error(m_error_title, obj);
}

#include "kexiproject.moc"

