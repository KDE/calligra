/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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
#include <kexidb/utils.h>

#include "kexiproject.h"
#include "kexipartmanager.h"
#include "kexipartitem.h"
#include "kexipartinfo.h"
#include "kexipart.h"
#include "kexi.h"
#include "keximainwindow.h"

#include <assert.h>

KexiProject::KexiProject(KexiProjectData *pdata)
 : QObject(), Object()
 , m_data(pdata)
{
//	m_drvManager = new KexiDB::DriverManager();
//	m_connData = new KexiProjectConnectionData();
//js	m_partManager = new KexiPart::Manager(this);

//TODO: partmanager is outside project, so can be initialised just once:

	m_itemDictsCache = QIntDict<KexiPart::ItemDict>(199);
	m_itemDictsCache.setAutoDelete(true);
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

	return initProject();
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

	//add some metadata
	KexiDB::Transaction trans = m_connection->beginTransaction();
	KexiDB::TableSchema *t_db = m_connection->tableSchema("kexi__db");
//TODO: put more props. todo - creator, created date, etc. (also to KexiProjectData)
	//caption:
	if (!t_db)
		return false;

	if (!KexiDB::replaceRow(*m_connection, *t_db, "db_property", "project_caption", "db_value", QVariant( m_data->caption() ), KexiDB::Field::Text)
	 || !KexiDB::replaceRow(*m_connection, *t_db, "db_property", "project_desc", "db_value", QVariant( m_data->description() ), KexiDB::Field::Text) )
		return false;

	if (trans.active() && !m_connection->commitTransaction(trans))
		return false;

	return initProject();
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

bool
KexiProject::initProject()
{
//	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	
	Kexi::partManager().checkProject(m_connection);

	//TODO: put more props. todo - creator, created date, etc. (also to KexiProjectData)
	KexiDB::RowData data;
	QString sql = "select db_value from kexi__db where db_property='%1'";
	if (m_connection->querySingleRecord( sql.arg("project_caption"), data) && !data[0].toString().isEmpty())
		m_data->setCaption(data[0].toString());
	if (m_connection->querySingleRecord( sql.arg("project_desc"), data) && !data[0].toString().isEmpty())
		m_data->setDescription(data[0].toString());

	return true;
}

bool
KexiProject::isConnected()
{
	if(m_connection && m_connection->isDatabaseUsed())
		return true;

	return false;
}

KexiPart::ItemDict*
KexiProject::items(KexiPart::Info *i)
{
	kdDebug() << "KexiProject::items()" << endl;
	if(!i || !isConnected())
		return 0;

	//trying in cache...
	KexiPart::ItemDict *dict = m_itemDictsCache[ i->projectPartID() ];
	if (dict)
		return dict;
	//retrieve:
	KexiDB::Cursor *cursor = m_connection->executeQuery(
		"SELECT o_id, o_name, o_caption  FROM kexi__objects WHERE o_type = " 
		+ QString::number(i->projectPartID()), KexiDB::Cursor::Buffered);
	kdDebug() << "KexiProject::items(): cursor handle is:" << cursor << endl;
	if(!cursor)
		return 0;

	dict = new KexiPart::ItemDict(101);
	dict->setAutoDelete(true);

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		KexiPart::Item *it = new KexiPart::Item();
		it->setIdentifier(cursor->value(0).toInt());
		it->setMime(i->mime());
		it->setName(cursor->value(1).toString());
		it->setCaption(cursor->value(2).toString());

		dict->insert(it->identifier(), it);
		kdDebug() << "KexiProject::items(): ITEM ADDED == "<<cursor->value(1).toString()<<endl;
	}

	m_connection->deleteCursor(cursor);
	kdDebug() << "KexiProject::items(): end with count " << dict->count() << endl;
	m_itemDictsCache.insert( i->projectPartID(), dict );
	return dict;
}

KexiPart::ItemDict*
KexiProject::items(const QString &mime)
{
	KexiPart::Info *info = Kexi::partManager().info(mime);
	return items(info);
}

KexiPart::Item*
KexiProject::item(const QString &mime, const QString &name)
{
	KexiPart::ItemDict *dict = items(mime);
	if (!dict)
		return 0;
	const QString l_name = name.lower();
	for (KexiPart::ItemDictIterator it( *dict ); it.current(); ++it) {
		if (it.current()->name().lower()==l_name)
			return it.current();
	}
	return 0;
}

KexiPart::Item*
KexiProject::item(KexiPart::Info *i, const QString &name)
{
	KexiPart::ItemDict *dict = items(i);
	if (!dict)
		return 0;
	const QString l_name = name.lower();
	for (KexiPart::ItemDictIterator it( *dict ); it.current(); ++it) {
		if (it.current()->name().lower()==l_name)
			return it.current();
	}
	return 0;
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

KexiDialogBase* KexiProject::openObject(KexiMainWindow *wnd, const KexiPart::Item& item, int viewMode)
{
	KexiPart::Part *part = Kexi::partManager().part(item.mime());
	if (!part) {
//js TODO:		setError(&Kexi::partManager());
		return 0;
	}
	KexiDialogBase *dlg  = part->openInstance(wnd, item, viewMode);
	if (!dlg) {
		//js TODO check for errors
		return 0;
	}
	return dlg;
}

KexiDialogBase* KexiProject::openObject(KexiMainWindow *wnd, const QString &mime, const QString& name, 
	int viewMode)
{
	KexiPart::Item *it = item(mime, name);
	return it ? openObject(wnd, *it, viewMode) : 0;
}

bool KexiProject::removeObject(KexiMainWindow *wnd, const KexiPart::Item& item)
{
	KexiPart::Part *part = Kexi::partManager().part(item.mime());
	if (!part) {
//js TODO:		setError(&Kexi::partManager());
		return false;
	}
	if (!part->remove(wnd, item)) {
		//js TODO check for errors
		return false;
	}
	emit itemRemoved(item);
	//now: remove this item from cache
	if (part->info()) {
		KexiPart::ItemDict *dict = m_itemDictsCache[ part->info()->projectPartID() ];
		if (dict) {
			dict->remove( item.identifier() );
		}
	}
	return true;
}

#include "kexiproject.moc"

