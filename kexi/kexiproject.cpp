/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
					  Daniel Molkentin <molkentin@kde.org>

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

#include <qcstring.h>
#include <qdom.h>

#include "kexiapplication.h"
#include "kexiproject.h"
#include "kexidoc.h"

#include <kdebug.h>

KexiProject::KexiProject(QObject* parent) : QObject(parent)
{
	//hope that changes soon too
	m_db = new KexiDB(this);
	m_formManager=new KexiFormManager();
}

KexiProject::~KexiProject()
{
}

bool
KexiProject::saveProject()
{
	QDomDocument domDoc("KexiProject");
	domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
	
	QDomElement projectElement = domDoc.createElement("KexiProject");
	domDoc.appendChild(projectElement);
	
	QDomElement engineElement = domDoc.createElement("engine");
	projectElement.appendChild(engineElement);
	
	QDomText tEngine = domDoc.createTextNode(m_cred.driver);
	engineElement.appendChild(tEngine);
	
	QDomElement hostElement = domDoc.createElement("host");
	projectElement.appendChild(hostElement);
	
	QDomText tHost = domDoc.createTextNode(m_cred.host);
	hostElement.appendChild(tHost);
	
	QDomElement nameElement = domDoc.createElement("name");
	projectElement.appendChild(nameElement);
	
	QDomText tName = domDoc.createTextNode(m_cred.database);
	nameElement.appendChild(tName);
	
	QDomElement userElement = domDoc.createElement("user");
	projectElement.appendChild(userElement);
	
	QDomText tUser = domDoc.createTextNode(m_cred.user);
	userElement.appendChild(tUser);
	
	QDomElement passElement = domDoc.createElement("password");
	projectElement.appendChild(passElement);
	
	QDomText tPass = domDoc.createTextNode(m_cred.password);
	passElement.appendChild(tPass);


	QByteArray data = domDoc.toCString();
	data.resize(data.size()-1);

	if(kexi->store()->store("/project.xml", data))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool
KexiProject::loadProject()
{
	QByteArray data = kexi->store()->readData("/project.xml");

	QDomDocument inBuf;
	
	//error reporting
	QString errorMsg;
	int errorLine;
	int errorCol;
	
	bool parsed = inBuf.setContent(data, &errorMsg, &errorLine, &errorCol);
	if(!parsed)
	{
		kdDebug() << "coudn't parse:" << endl;
		kdDebug() << "error: " << errorMsg << " line: " << errorLine << " col: " << errorCol << endl; 
		return false;
	}

		
	QDomElement projectData = inBuf.namedItem("KexiProject").toElement();

	QDomElement engineElement = projectData.namedItem("engine").toElement();
	QDomElement hostElement = projectData.namedItem("host").toElement();
	QDomElement nameElement = projectData.namedItem("name").toElement();
	QDomElement userElement = projectData.namedItem("user").toElement();
	QDomElement passElement = projectData.namedItem("password").toElement();

	Credentials parsedCred;
	parsedCred.driver   = engineElement.text();
	parsedCred.host     = hostElement.text();
	parsedCred.database = nameElement.text();
	parsedCred.user     = userElement.text();
	parsedCred.password = passElement.text();
	
	initDbConnection(parsedCred);
}

bool 
KexiProject::initDbConnection(const Credentials &cred)
{
	kdDebug() << "KexiProject::initDbConnection()" << endl;
	
	kdDebug() << "KexiProject::initDbConnection(): engine:" << cred.driver << endl;
	kdDebug() << "KexiProject::initDbConnection(): host:" << cred.host << endl;
	kdDebug() << "KexiProject::initDbConnection(): user:" << cred.user << endl;
	kdDebug() << "KexiProject::initDbConnection(): database:" << cred.database << endl;

	
	if(m_db->driverName() != cred.driver)
	{
		kdDebug() << "KexiProject::initDBConnection(): abroating" << endl;
		initHostConnection(cred);
	}

	kdDebug() << "KexiProject::initDBConnection(): using simple method\n  because current driver is: " << m_db->driverName() << endl;
	if(m_db->connect(cred.host, cred.user, cred.password, cred.database))
	{
		m_cred = cred;
		kdDebug() << "KexiProject::initDbConnection(): loading succeeded" << endl;
		kexi->mainWindow()->slotProjectModified();
		return true;
	}
	else
	{
		kdDebug() << "KexiProject::initDbConnection(): connection failed: #need to implement" /*m_db->lastError().databaseText() */ << endl;
		m_cred = cred;
		return false;
	}
}

bool 
KexiProject::initHostConnection(const Credentials &cred)
{
	kdDebug() << "KexiProject::initHostConnection" << endl;
	KexiDB *addDB = m_db->add(cred.driver);
	if(addDB)
	{
		m_db = addDB;
	}
	else
	{
		return false;
	}

	if(!m_db->connect(cred.host, cred.user, cred.password))
	{
		m_cred = cred;
		return false;
	}
	else
	{
		m_cred = cred;
		return true;
	}
}

#include "kexiproject.moc"
