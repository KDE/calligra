/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kexidb/connectiondata.h>

#include <kexidb/drivermanager.h>

#include <qfileinfo.h>
#include <qdir.h>

#include <klocale.h>

using namespace KexiDB;

class ConnectionData::Private {
public:
	Private() {}
};

/*================================================================*/

ConnectionData::ConnectionData()
: port(0)
, d(new ConnectionData::Private())
{
}

ConnectionData::ConnectionData(const ConnectionData& cd)
: d(new ConnectionData::Private())
{
	*this = cd;
	*d = *cd.d;
}

ConnectionData::~ConnectionData()
{
	delete d;
}

void ConnectionData::setFileName( const QString& fn )
{
	if (m_fileName != fn) {
		QFileInfo file(fn);
		m_fileName = fn;
		m_dbPath = file.dirPath(true);
		m_dbFileName = file.fileName();
	}
}

QString ConnectionData::serverInfoString() const
{
	const QString& i18nFile = i18n("file");
	
	if (!m_dbFileName.isEmpty())
		return i18nFile+": "+m_dbPath+ (m_dbPath.isEmpty() ? "" : m_dbPath + QDir::separator()) + m_dbFileName;
	
	DriverManager man;
	if (!driverName.isEmpty()) {
		Driver::Info info = man.driverInfo(driverName);
		if (!info.name.isEmpty() && info.fileBased)
			return QString("<")+i18nFile+">";
	}
	
	return (userName.isEmpty() ? QString("") : (userName+"@"))
		+ (hostName.isEmpty() ? QString("localhost") : hostName)
		+ (port!=0 ? (QString(":")+QString::number(port)) : QString::null);
}

