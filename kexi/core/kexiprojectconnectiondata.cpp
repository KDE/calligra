/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>

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

#include <sys/types.h>
#include <unistd.h>

#include <qdom.h>
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <koStore.h>

#include <kexidb/connectiondata.h>
#include <kexidb/drivermanager.h>
#include "kexiprojectconnectiondata.h"

KexiProjectConnectionData::KexiProjectConnectionData(): KexiDB::ConnectionData()
{
}

KexiProjectConnectionData::KexiProjectConnectionData(const QString& driverName, const QString& databaseName, const QString &hostName, 
		unsigned short int port, const QString& userName, const QString &password, const QString& fileName):KexiDB::ConnectionData()
{
	m_driverName=driverName;
	m_databaseName=databaseName;
	this->hostName=hostName;
	this->port=port;
	this->userName=userName;
	this->password=password;
	setFileName(fileName);
}

KexiProjectConnectionData::KexiProjectConnectionData(const QString &driverName, const QString &fileName)
 : KexiDB::ConnectionData()
{
	m_driverName=driverName;
	setFileName(fileName);
}

KexiProjectConnectionData*
KexiProjectConnectionData::loadInfo(QDomElement &rootElement)
{
	QDomElement engineElement = rootElement.namedItem("engine").toElement();
	QDomElement hostElement = rootElement.namedItem("host").toElement();
	QDomElement portElement = rootElement.namedItem("port").toElement();
	QDomElement nameElement = rootElement.namedItem("name").toElement();
	QDomElement userElement = rootElement.namedItem("user").toElement();
	QDomElement passElement = rootElement.namedItem("password").toElement();
	QDomElement persElement = rootElement.namedItem("persistant").toElement();
	QDomElement encodingElement = rootElement.namedItem("encoding").toElement();

	KexiProjectConnectionData *tmp=new KexiProjectConnectionData(
		engineElement.text(), nameElement.text(),hostElement.text(),portElement.text().toInt(),
		userElement.text(),passElement.text(),"");	

	return tmp;
}

void    KexiProjectConnectionData::setDriverName(const QString &driverName) {
	m_driverName=driverName;
}

void KexiProjectConnectionData::setDatabaseName(const QString &databaseName) {
	m_databaseName=databaseName;
}

QString KexiProjectConnectionData::driverName() const {
	return m_driverName;
}

QString KexiProjectConnectionData::databaseName() const {
	return m_databaseName;
}


#if 0
void
KexiProjectConnectionData::writeInfo(KexiDB* destDB,int priority)
{
	bool firstEntry=false;
	if (priority==0) {
		/* try to create the needed table */
		KexiDBTable kdbt("kexi_databaselist");
		kdbt.addField(KexiDBField("id",KexiDBField::SQLInteger,
			KexiDBField::CCUnique | KexiDBField::CCNotNull |
			KexiDBField::CCPrimaryKey));
		kdbt.addField(KexiDBField("type",KexiDBField::SQLVarchar,
			KexiDBField::CCNotNull,20));
		kdbt.addField(KexiDBField("engine",KexiDBField::SQLVarchar,
			KexiDBField::CCNotNull,30));
		kdbt.addField(KexiDBField("host",KexiDBField::SQLVarchar,
			KexiDBField::CCNone,255));
		kdbt.addField(KexiDBField("kexiuser",KexiDBField::SQLVarchar,
			KexiDBField::CCNone,20));
		kdbt.addField(KexiDBField("kexipassword",KexiDBField::SQLVarchar,
			KexiDBField::CCNone,20));

		//firstEntry=!destDB->createTable(kdbt);
		/* perhaps error checking should be done here */
		//what a good idea :)
	}
	if (firstEntry) {
	} else {
	/*  here checks for insert/update are needed */
	}
}
#endif


void
KexiProjectConnectionData::writeInfo(QDomDocument &domDoc)
{
	QDomElement connectionElement = domDoc.createElement("connectionSettings");
	domDoc.documentElement().appendChild(connectionElement);

//DB ENGINE
	QDomElement engineElement = domDoc.createElement("engine");
	connectionElement.appendChild(engineElement);

	QDomText tEngine = domDoc.createTextNode(m_driverName);
	engineElement.appendChild(tEngine);

//HOST
	QDomElement hostElement = domDoc.createElement("host");
	connectionElement.appendChild(hostElement);

	QDomText tHost = domDoc.createTextNode(hostName);
	hostElement.appendChild(tHost);

//DATABASE NAME
	QDomElement nameElement = domDoc.createElement("name");
	connectionElement.appendChild(nameElement);

	QDomText tName = domDoc.createTextNode(m_databaseName);
	nameElement.appendChild(tName);

//USER
	QDomElement userElement = domDoc.createElement("user");
	connectionElement.appendChild(userElement);

	QDomText tUser = domDoc.createTextNode(userName);
	userElement.appendChild(tUser);

//PASSWORD STUFF
	QDomElement passElement = domDoc.createElement("password");
	connectionElement.appendChild(passElement);

	QDomText tPass=domDoc.createTextNode(password);
	passElement.appendChild(tPass);

}

void
KexiProjectConnectionData::flush(KoStore *store)
{
#ifndef Q_WS_WIN
#warning fixme
#endif
#if 0
	kdDebug() << "KexiDBConnection::flush()" << endl;
	if(!m_tmpname.isEmpty() && m_type == KexiDB::LocalDirectoryDB && store && !m_persistant)
	{
		QStringList index = store->addLocalDirectory(m_tmpname, "/db");
		QString dindex = index.join("\n");
		QByteArray rawIndex = dindex.utf8();
		rawIndex.resize(rawIndex.size() - 1);
		store->open("/db-index");
		store->write(rawIndex);
		store->close();
		//kdDebug() << "added data: " << index << endl;
	}
#endif
}


void
KexiProjectConnectionData::provide(KoStore *store)
{
#ifndef Q_WS_WIN
#warning fixme
#endif
#if 0
	kdDebug() << "KexiDBConnection::provide()" << endl;
	if(!store)
	 return;
//
	if(!m_tmpname.isEmpty() && m_type == KexiDB::LocalDirectoryDB && !m_persistant)
	{
		kdDebug() << "KexiDBConnection::provide(): right env" << endl;
		if(!store->open("/db-index"))
			return;

		QByteArray rawIndex = store->read(store->size());
		store->close();
		QStringList index = QStringList::split("\n", QString(rawIndex));
		for(QStringList::Iterator it = index.begin(); it != index.end(); ++it)
		{
			QString currentDest = *it;
//js:	QT3.0 compat:		currentDest = currentDest.replace("/db", "");
			currentDest = currentDest.replace(QRegExp("/db"), "");
			currentDest = m_tmpname + currentDest;
//
			kdDebug() << "KexiDBConnection::provide(): extracting " << *it << ":" << currentDest << endl;
			if(!store->extractFile(*it, currentDest))
			{
				kdDebug() << "KexiDBConnection::provide(): extract faild" << endl;
			}
//			else
//			{
//				kdDebug() << "KexiDBConnection::provide(): extract succeeded" << endl;
//			}
		}

	}
#endif
}

void
KexiProjectConnectionData::clean()
{
#ifndef Q_WS_WIN
#warning fixme
#endif
#if 0
	kdDebug() << "KexiDBConnection::clean()" << endl;
	if(!m_tmpname.isEmpty() && m_type == KexiDB::LocalDirectoryDB && !m_persistant)
	{
		KIO::NetAccess::del(KURL(m_tmpname));
		kdDebug() << "KexiDBConnection::clean(): cleaned" << endl;
	}
#endif
}

const QString &
KexiProjectConnectionData::generateTmpName()
{
	return QString::null;
}

KexiProjectConnectionData::~KexiProjectConnectionData()
{
}
