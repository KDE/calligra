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

#include <sys/types.h>
#include <unistd.h>

#include <qdom.h>
#include <qdir.h>
#include <qfile.h>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <koStore.h>

#include<kexiDB/kexidb.h>
#include<kexiDB/kexidberror.h>
#include "kexidbconnection.h"

KexiDBConnection::KexiDBConnection()
{
	m_connected = false;
	m_type = KexiDB::NoDB;
}

KexiDBConnection::KexiDBConnection(const QString &engine, const QString &host, const QString &dbname,
 const QString &user, const QString &pass, const QString &socket, const QString &port)
{
	m_connected = false;
	m_type = KexiDB::RemoteDB;
	m_engine = engine;
	m_host = host;
	m_dbname = dbname;
	m_user = user;
	m_socket = socket;
	m_port = port;
}

KexiDBConnection::KexiDBConnection(const QString &engine, const QString &file)
{
	m_connected = false;
	//FIXME: don't use directory db only :)
	//even if we only got cql by now, or who knows?
	m_type = KexiDB::LocalDirectoryDB;
	m_engine = engine;
}

KexiDB*
KexiDBConnection::connectDB(KexiDB *parent, KoStore *store)
{
	kdDebug() << "KexiDBConnection::connectDB()" << endl;

	KexiDB *addDB = parent->add(m_engine);
	if(!addDB)
	{
		KMessageBox::detailedError(0, i18n("Error in databaseconnection"), i18n("kexi couldn't ditermine engine-type"), i18n("Database Connection"));
		return 0;
	}

	kdDebug() << "KexiDBConnection::connectDB(): engine loaded" << endl;

	switch(m_type)
	{
		case KexiDB::RemoteDB:
		{
			try
			{
				if(addDB->connect(m_host, m_user, m_pass, m_socket, m_port, m_dbname, false))
				{
					return addDB;
				}

				kdDebug() << "KexiDBConnection::connectDB(): remote = " << addDB << endl;
			}
			catch(KexiDBError err)
			{
				KMessageBox::detailedError(0, i18n("Error in databaseconnection"), err.message(), i18n("Database Connection"));
				return 0;
			}

			return addDB;
		}
		case KexiDB::LocalDirectoryDB:
		{
			QString tmpfile(KGlobal::dirs()->resourceDirs("tmp").first() + "kexi-" + QString::number(getpid()));
			QDir tempdir(tmpfile);
			tempdir.mkdir(tmpfile);
			m_tmpname = tmpfile;

			try
			{
				addDB->load(tmpfile);
			}
			catch(KexiDBError err)
			{
				err.toUser(0);
				return false;
			}

			return addDB;
		}
		default:
		{
			KMessageBox::detailedError(0, i18n("Error in databaseconnection"), i18n("kexi couldn't ditermine engine-type"), i18n("Database Connection"));
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
//	QDomElement
//	QDomElement savePassElement = rootElement.namedItem("savePassword").toElement();

	KexiDB::DBType type = KexiDB::NoDB;
	if(locationElement.text() == "")
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

	switch(type)
	{
		case KexiDB::RemoteDB:
		{
			KexiDBConnection *conn;
			conn = new KexiDBConnection(engineElement.text(), hostElement.text(),
			 nameElement.text(), userElement.text(), passElement.text(), 0);
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
}

void
KexiDBConnection::flush(KoStore *store)
{
	if(!m_tmpname.isEmpty() && m_type == KexiDB::LocalDirectoryDB)
	{
		QDir tmpdir(m_tmpname);
		QStringList index = tmpdir.entryList(QDir::All);

		for(QStringList::Iterator it = index.begin(); it != index.end(); ++it)
		{
			if((*it) == "." || (*it) == "..")
			{
				index.remove(it);
			}
			else
			{
				QFile f(m_tmpname + "/" + (*it));
				if(f.open(IO_ReadOnly))
				{
					store->open("/dir/" + (*it));
					char *buffer = new char[f.size() * 2 + 2];
					QDataStream sstream(&f);
					sstream << buffer;
					QDataStream tstream(store->device());
					tstream >> buffer;
					store->close();
					f.close();
				}
			}
		}

		QString pindex = index.join("\n");
		store->open("/db-index");
		store->write(pindex.utf8());
		store->close();
	}
}

void
KexiDBConnection::clean()
{
	kdDebug() << "KexiDBConnection::clean()" << endl;
	if(!m_tmpname.isEmpty() && m_type == KexiDB::LocalDirectoryDB)
	{
		KIO::NetAccess::del(KURL(m_tmpname));
		kdDebug() << "KexiDBConnection::clean(): cleaned" << endl;
	}
}

KexiDBConnection::~KexiDBConnection()
{
}
