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

#include "KexiXMLStorage.h"

#include "kexidbconnectionset.h"

#include <qdom.h>
#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>

#include <kglobal.h>
#include <kdebug.h>

KexiProjectData* loadKexiConnectionDataXML(QIODevice *dev, QString &error)
{
	//TODO
	return 0;
}

bool saveKexiConnectionDataListXML( QIODevice *dev, const KexiDBConnectionSet& cset)
{
	QDomDocument doc( "kexiconnectionset" );
	doc.createComment("List of database connections used for Kexi.");
	QDomElement mainel = doc.createElement("kexiconnectionset");
	doc.appendChild(mainel);
	KexiDB::ConnectionData::List clist = cset.list();
	const KexiDB::ConnectionData *cdata = clist.first();
	while (cdata) {
		QDomElement el = doc.createElement("connectiondata");
		el.setAttribute("id",QString::number(cdata->id));
		if (!cdata->connName.isEmpty())
			el.setAttribute("name",cdata->connName);
		if (!cdata->driverName.isEmpty())
			el.setAttribute("engine",cdata->driverName);
		if (!cdata->hostName.isEmpty())
			el.setAttribute("host",cdata->hostName);
		if (cdata->port>0)
			el.setAttribute("port",QString::number(cdata->port));
		if (!cdata->localSocketFileName.isEmpty())
			el.setAttribute("localSocketFileName",cdata->localSocketFileName);
		if (!cdata->userName.isEmpty())
			el.setAttribute("user",cdata->userName);
		if (!cdata->password.isEmpty())
			el.setAttribute("password",cdata->password);
		if (!cdata->dbPath().isEmpty())
			el.setAttribute("dbPath",cdata->dbPath());
		if (!cdata->dbFileName().isEmpty())
			el.setAttribute("dbFileName",cdata->dbFileName());
		mainel.appendChild(el);
		cdata = clist.next();
	}
	return true;
}

bool saveKexiConnectionDataXML(QIODevice *dev, const KexiDB::ConnectionData &data, QString &error)
{
	
	//TODO
	return false;
}


KexiProjectData* loadKexiProjectDataXML(QIODevice *dev, QString &error)
{
	//TODO
	return 0;
}

bool saveKexiProjectDataXML(QIODevice *dev, const KexiProjectData &data, QString &error)
{
	//TODO
	return false;
}


KexiProjectData* loadKexiProjectSetXML(QIODevice *dev, QString &error)
{
	//TODO
	return 0;
}

bool saveKexiProjectSetXML(QIODevice *dev, const KexiProjectData &pset, QString &error)
{
	//TODO
	return false;
}
