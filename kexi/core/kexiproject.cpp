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
//#include "kexiprojectconnectiondata.h"
#include "kexiproject.h"
#include "kexi.h"

KexiProject::KexiProject(KexiProjectData *pdata)
 : QObject(), Object()
 , m_data(pdata)
{
//	m_drvManager = new KexiDB::DriverManager();
//	m_connData = new KexiProjectConnectionData();
	m_partManager = new KexiPart::Manager(this);

//*******************
//(JS): shouldn't partmanager be outside project, so can be initialised just once?
//*******************
	m_partManager->lookup();
	m_connection = 0;
}

KexiProject::~KexiProject()
{
	delete m_data;
	m_data=0;
}

#if 0
void
KexiProject::parseCmdLineOptions()
{
	//FIXME: allow multiple files in different proceses
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count() > 0 && open(QFile::decodeName(args->arg(0))))
	{
		kdDebug() << "KexiProject::opening was nice..." << endl;
	}
	else
	{
		KexiStartupDlg *dlg = new KexiStartupDlg(m_view);
		int res = dlg->exec();
		switch(res)
		{
			case KexiStartupDlg::Cancel:
				qApp->quit();
			case KexiStartupDlg::OpenExisting:
				open(dlg->fileName());
				break;
			default:
				return;
		}
	}
}
#endif

bool
KexiProject::open()
{
	clearError();
	if (Kexi::driverManager.error()) {//get initial errors
		setError(&Kexi::driverManager);
		return false;
	}
	
	KexiDB::Driver *driver = Kexi::driverManager.driver(m_data->connectionData()->driverName);
	if(!driver) {
		setError(&Kexi::driverManager);
		return false;
	}

	m_connection = driver->createConnection(*m_data->connectionData());
	if(!m_connection) 
	{
		kdDebug() << "KexiProject::open(): uuups faild " << driver->errorMsg()  << endl;
		setError(driver);
		return false;
	}

	if (!m_connection->connect())
	{
		setError(m_connection);
		kdDebug() << "KexiProject::openConnection() errror connecting: " << m_connection->errorMsg() << endl;
		delete m_connection;
		m_connection = 0;
		return false;
	}

	return open(m_connection);
}

bool 
KexiProject::open(KexiDB::Connection* conn)
{
	if (conn!=m_connection) {
		delete m_connection;
		m_connection = 0;
	}
	if (!conn->useDatabase(m_data->databaseName()))
	{
		setError(conn);
//		m_error = i18n("Couldn't open database '%1'").arg(m_connData->databaseName());
		return false;
	}

	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	
	m_connection = conn;
	m_partManager->checkProject(m_connection);
	
	return true;
}

#if 0
bool
KexiProject::open(const QString &file)
{
	QFile f(file);
	if(!f.exists())
		return false;

	KMimeType::Ptr ptr = KMimeType::findByFileContent(file);
	kdDebug() << "KexiProject::open(): found mime is: " << ptr.data()->name() << endl;
	QString drivername = Kexi::driverManager.lookupByMime(ptr.data()->name());
	kdDebug() << "KexiProject::open(): driver name: " << drivername << endl;
	if(!drivername.isNull())
	{
		kdDebug() << "KexiProject::open(): file is a living databse of engine " << drivername << endl;
		m_connData->setDriverName(drivername);
		m_connData->setDatabaseName(file);
		m_connData->setFileName(file);
	}
	else
	{
		kdDebug() << "KexiProject::open(): could be a xml :)" << endl;
		delete m_connData;
		if(!f.open(IO_ReadOnly))
		{
			m_error = i18n("Couldn't open connection file");
			return false;
		}
		QString errMsg;
		int errRow;
		int errCol;

		QDomDocument doc;
		

		if(!doc.setContent(&f, &errMsg, &errRow, &errCol))
		{
			m_error = i18n("Couldn't parse connection file: %1\nRow: %1, Col: %1").arg(errMsg).arg(errRow).arg(errCol);
			return false;
		}

		QDomElement root = doc.elementsByTagName("KexiDBConnection").item(0).toElement();

		m_connData = KexiProjectConnectionData::loadInfo(root);
		kdDebug() << "KexiProject::open(): hope stuff is up and running (" << m_connData << ")" << endl;
	}

//	f.close();
	if(!openConnection(m_connData) || !m_connection)
	{
		m_error = i18n("Couldn't connect");
		return false;
	}

	if(!m_connection->useDatabase(m_connData->dbFileName()))
	{
		m_error = i18n("Couldn't open database '%1'").arg(m_connData->databaseName());
//		/*m_error = */m_connection->debugError();
		return false;
	}

	emit dbAvailable();
	kdDebug() << "KexiProject::open(): checking project parts..." << endl;
	m_partManager->checkProject(m_connection);
	return true;
}

bool
KexiProject::openConnection(KexiProjectConnectionData *connection)
{
	KexiDB::Driver *driver = m_drvManager->driver(connection->driverName().latin1());
	if(!driver)
		return false;

	KexiDB::Connection *conn = driver->createConnection(*connection);
	if(!conn)
	{
		kdDebug() << "KexiProject::openConnection(): uuups faild " << driver->errorMsg()  << endl;
		return false;
	}

	if(driver->error())
	{
		m_error = i18n("Error while connecting to db %1").arg(driver->errorMsg());
		return false;
	}

	if(!conn->connect())
	{
		m_error = i18n("Error while connecting to db %1").arg(driver->errorMsg());
		kdDebug() << "KexiProject::openConnection() errror connecting: " << driver->errorMsg() << endl;
		return false;
	}

	m_connection = conn;
	return true;
}
#endif

bool
KexiProject::isConnected()
{
	if(m_connection && m_connection->isDatabaseUsed())
		return true;

	return false;
}

ItemList
KexiProject::items(KexiPart::Info *i)
{
	kdDebug() << "KexiProject::items()" << endl;
	ItemList list;
	if(!isConnected())
		return list;

	KexiDB::Cursor *cursor = m_connection->executeQuery("SELECT o_id, o_name, o_caption  FROM kexi__objects WHERE o_type = " + QString::number(i->projectPartID()), KexiDB::Cursor::Buffered);
	if(!cursor)
		return list;

	for(cursor->moveFirst(); !cursor->eof(); cursor->moveNext())
	{
		KexiPart::Item it;
		it.setIdentifier(cursor->value(0).toInt());
		it.setMime(i->mime());
		it.setName(cursor->value(1).toString());
		it.setCaption(cursor->value(2).toString());

		list.append(it);
	}

	m_connection->deleteCursor(cursor);
	kdDebug() << "KexiProject::items(): end with cout " << list.count() << endl;
	return list;
}

#include "kexiproject.moc"

