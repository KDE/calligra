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

#include<kexiDB/kexidb.h>
#include <kexiDB/kexidbinterfacemanager.h>
#include <kexiDB/kexidberror.h>
#include "kexidbconnection.h"

KexiDBConnection::KexiDBConnection()
{
	m_connected = false;
	m_type = KexiDB::NoDB;
	m_encoding = KexiDB::Latin1;
}

KexiDBConnection::KexiDBConnection(const QString &engine, const QString &host, const QString &dbname,
 const QString &user, const QString &pass, const QString &socket, const QString &port, bool create)
{
	m_connected = false;
	m_type = KexiDB::RemoteDB;
	m_engine = engine;
	m_host = host;
	m_dbname = dbname;
	m_user = user;
	m_pass = pass;
	m_socket = socket;
	m_port = port;

	m_persistant = false;
	m_createDb = create;
	m_encoding = KexiDB::Latin1;
}

KexiDBConnection::KexiDBConnection(const QString &engine, const QString &file, bool persistant)
{
	m_connected = false;
	//FIXME: don't use directory db only :)
	//even if we only got cql by now, or who knows?
	m_type = KexiDB::LocalDirectoryDB;
	m_engine = engine;
	m_file = file;
	m_persistant = persistant;
	m_encoding = KexiDB::Latin1;
}

KexiDB*
KexiDBConnection::connectDB(KexiDBInterfaceManager *parent, KoStore *store)
{
	KexiDB *addDB = parent->newDBInstance(m_engine);
	if(!addDB)
	{
	kdDebug() << "KexiDBConnection::connectDB()" << endl;

		KMessageBox::detailedError(0, i18n("Error in database connection"),
                                   i18n("kexi couldn't determine engine type"), i18n("Database Connection"));
		return 0;
	}

	kdDebug() << "KexiDBConnection::connectDB(): engine loaded" << endl;

	switch(m_type)
	{
		case KexiDB::RemoteDB:
		{
			if(addDB->connect(m_host, m_user, m_pass, m_socket, m_port, m_dbname, m_createDb))
			{
				kdDebug() << "KexiDBConnection::connectDB(): remote = " << addDB << endl;
				addDB->setEncoding(m_encoding);
				return addDB;
			}
			else
			{
				KMessageBox::detailedError(0, i18n("Error in database connection"),addDB->latestError()->message(), i18n("Database Connection"));
				delete addDB;
				return 0;
			}
		}
		case KexiDB::LocalDirectoryDB:
		{
			QString tmpfile;
			if(!m_persistant)
			{
				kdDebug() << "KexiDBConnection::connectDB(): using tmp-method" << endl;
				tmpfile = (KGlobal::dirs()->resourceDirs("tmp").first() + "kexi-" + QString::number(getpid()));
				QDir tempdir(tmpfile);
				tempdir.mkdir(tmpfile);
				m_tmpname = tmpfile;
			}
			else
			{
				kdDebug() << "KexiDBConnection::connectDB(): persistant-connect" << endl;
				kdDebug() << "KexiDBConnection::connectDB(): pfile"<<m_file << endl;
				tmpfile = m_file;
				QDir dir(tmpfile);
				if(!dir.exists())
					dir.mkdir(tmpfile);
			}

//			if(store)
			provide(store);
			if (addDB->load(tmpfile, m_persistant))
			{
				addDB->setEncoding(m_encoding);
				return addDB;
			}
			else
			{
				addDB->latestError()->toUser(0);
				delete addDB;
				return 0;
			}

		}
		default:
		{
			KMessageBox::detailedError(0, i18n("Error in database connection"), i18n("kexi couldn't determine engine type"),
                                       i18n("Database Connection"));
			return 0;
		}
	}
	return 0;
}

KexiDBConnection*
KexiDBConnection::loadInfo(QDomElement &rootElement)
{
	QDomElement locationElement = rootElement.namedItem("type").toElement();
	QDomElement engineElement = rootElement.namedItem("engine").toElement();
	QDomElement hostElement = rootElement.namedItem("host").toElement();
	QDomElement nameElement = rootElement.namedItem("name").toElement();
	QDomElement userElement = rootElement.namedItem("user").toElement();
	QDomElement passElement = rootElement.namedItem("password").toElement();
	QDomElement persElement = rootElement.namedItem("persistant").toElement();
	QDomElement encodingElement = rootElement.namedItem("encoding").toElement();
//	QDomElement
//	QDomElement savePassElement = rootElement.namedItem("savePassword").toElement();

	KexiDB::DBType type = KexiDB::NoDB;
	if(locationElement.text().isEmpty())
	{
		type = KexiDB::RemoteDB;
	}
	if(locationElement.text() == "RemoteDB")
	{
		type = KexiDB::RemoteDB;
	}
	if(locationElement.text() == "LocalDir")
	{
		type = KexiDB::LocalDirectoryDB;
	}
	if(locationElement.text() == "LocalFile")
	{
		type = KexiDB::LocalFileDB;
	}

	KexiDB::Encoding enc;
	if(encodingElement.text().isEmpty())
	{
		enc = KexiDB::Latin1;
	}
	else if(encodingElement.text() == "Utf8")
	{
		enc = KexiDB::Utf8;
	}
	else if(encodingElement.text() == "Local8Bit")
	{
		enc = KexiDB::Local8Bit;
	}
	else if(encodingElement.text() == "Ascii")
	{
		enc = KexiDB::Ascii;
	}
	else
	{
		enc = KexiDB::Latin1;
	}

	switch(type)
	{
		case KexiDB::RemoteDB:
		{
			KexiDBConnection *conn;
			conn = new KexiDBConnection(engineElement.text(), hostElement.text(),
			 nameElement.text(), userElement.text(), passElement.text(), 0);
			conn->setEncoding(enc);
			return conn;
		}
		case KexiDB::LocalDirectoryDB:
		{
			KexiDBConnection *conn;
			if(persElement.attributeNode("enabled").value() == "1")
			{
				conn = new KexiDBConnection(engineElement.text(), persElement.text(), true);
			}
			else
			{
				conn = new KexiDBConnection(engineElement.text(), 0);
			}

			conn->setEncoding(enc);
			return conn;
		}
		default:
		{
			return 0;
		}
	}

/*	Credentials parsedCred;
	parsedCred.driver   = engineElement.text();
	parsedCred.host     = hostElement.text();
	parsedCred.database = nameElement.text();
	parsedCred.user     = userElement.text();
	parsedCred.password = passElement.text();
*/
}


void
KexiDBConnection::writeInfo(KexiDB* destDB,int priority)
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

		firstEntry=!destDB->createTable(kdbt);
		/* perhaps error checking should be done here */
	}
	if (firstEntry) {
	} else {
	/*  here checks for insert/update are needed */
	}

}


void
KexiDBConnection::writeInfo(QDomDocument &domDoc)
{
	QDomElement connectionElement = domDoc.createElement("connectionSettings");
	domDoc.documentElement().appendChild(connectionElement);

//DB TYPE
	QDomElement typeElement = domDoc.createElement("type");
	connectionElement.appendChild(typeElement);

	QDomText tType = domDoc.createTextNode("");
	switch(m_type)
	{
		case KexiDB::RemoteDB:
		{
			tType = domDoc.createTextNode("RemoteDB");
			break;
		}
		case KexiDB::LocalDirectoryDB:
		{
			tType = domDoc.createTextNode("LocalDir");
			break;
		}
		case KexiDB::LocalFileDB:
		{
			tType = domDoc.createTextNode("LocalFile");
			break;
		}
		default:
		{
			break;
		}
	}
	typeElement.appendChild(tType);

//DB ENGINE
	QDomElement engineElement = domDoc.createElement("engine");
	connectionElement.appendChild(engineElement);

	QDomText tEngine = domDoc.createTextNode(m_engine);
	engineElement.appendChild(tEngine);

//PERSISTANT
	QDomElement persistantElement = domDoc.createElement("persistant");
	persistantElement.setAttribute("enabled", m_persistant?1:0);
	connectionElement.appendChild(persistantElement);

	if(m_persistant)
	{
		QDomText tPers = domDoc.createTextNode(m_file);
		persistantElement.appendChild(tPers);
	}
//HOST
	QDomElement hostElement = domDoc.createElement("host");
	connectionElement.appendChild(hostElement);

	QDomText tHost = domDoc.createTextNode(m_host);
	hostElement.appendChild(tHost);

//DATABASE NAME
	QDomElement nameElement = domDoc.createElement("name");
	connectionElement.appendChild(nameElement);

	QDomText tName = domDoc.createTextNode(m_dbname);
	nameElement.appendChild(tName);

//USER
	QDomElement userElement = domDoc.createElement("user");
	connectionElement.appendChild(userElement);

	QDomText tUser = domDoc.createTextNode(m_user);
	userElement.appendChild(tUser);

//PASSWORD STUFF
	QDomElement passElement = domDoc.createElement("password");
	connectionElement.appendChild(passElement);

	QDomText tPass=domDoc.createTextNode(m_pass);
	passElement.appendChild(tPass);

/*	QDomText tPass=domDoc.createTextNode(m_cred.savePassword?m_cred.password:"");
	passElement.appendChild(tPass);

	QDomElement savePassElement = domDoc.createElement("savePassword");
	connectionElement.appendChild(savePassElement);

	QDomText tSavePass = domDoc.createTextNode(boolToString(m_cred.savePassword));
	savePassElement.appendChild(tSavePass);
*/

//ENCODING
	QDomElement encodingElement = domDoc.createElement("encoding");
	connectionElement.appendChild(encodingElement);

	QString ev;
	switch(m_encoding)
	{
		case KexiDB::Utf8:
			ev = "Utf8";
			break;

		case KexiDB::Local8Bit:
			ev = "Local8Bit";
			break;

		case KexiDB::Ascii:
			ev = "Ascii";
			break;

		default:
			ev = "Latin1";
			break;
	}

	QDomText tEnc = domDoc.createTextNode(ev);
	encodingElement.appendChild(tEnc);
}

void
KexiDBConnection::flush(KoStore *store)
{
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
}
//
void
KexiDBConnection::provide(KoStore *store)
{
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
}

void
KexiDBConnection::clean()
{
	kdDebug() << "KexiDBConnection::clean()" << endl;
	if(!m_tmpname.isEmpty() && m_type == KexiDB::LocalDirectoryDB && !m_persistant)
	{
		KIO::NetAccess::del(KURL(m_tmpname));
		kdDebug() << "KexiDBConnection::clean(): cleaned" << endl;
	}
}

KexiDBConnection::~KexiDBConnection()
{
}
