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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <sys/types.h>
#include <unistd.h>

#include <QDomDocument>
#include <QDir>
#include <QFile>
#include <QRegExp>

#include <kglobal.h>
#include <kstandarddirs.h>
#include <kdebug.h>
#include <kio/netaccess.h>
#include <kurl.h>
#include <klocale.h>
#include <kmessagebox.h>

#include <db/connectiondata.h>
#include <db/drivermanager.h>
#include "kexiprojectconnectiondata.h"

KexiProjectConnectionData::KexiProjectConnectionData(): KexiDB::ConnectionData()
{
}

KexiProjectConnectionData::KexiProjectConnectionData(const QString& driverName, const QString& databaseName, const QString &host,
        unsigned short int rport, const QString& user, const QString &pass, const QString& file): KexiDB::ConnectionData()
{
    m_driverName = driverName;
    m_databaseName = databaseName;
    hostName = host;
    port = rport;
    userName = user;
    password = pass;
    setFileName(file);
}

KexiProjectConnectionData::KexiProjectConnectionData(const QString &driverName, const QString &fileName)
        : KexiDB::ConnectionData()
{
    m_driverName = driverName;
    setFileName(fileName);
}

const QString &
KexiProjectConnectionData::generateTmpName()
{
    return QString();
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

    KexiProjectConnectionData *tmp = new KexiProjectConnectionData(
        engineElement.text(), nameElement.text(), hostElement.text(), portElement.text().toInt(),
        userElement.text(), passElement.text(), "");

    return tmp;
}

void    KexiProjectConnectionData::setDriverName(const QString &driverName)
{
    m_driverName = driverName;
}

void KexiProjectConnectionData::setDatabaseName(const QString &databaseName)
{
    m_databaseName = databaseName;
}

QString KexiProjectConnectionData::driverName() const
{
    return m_driverName;
}

QString KexiProjectConnectionData::databaseName() const
{
    return m_databaseName;
}


void
KexiProjectConnectionData::writeInfo(QDomDocument &domDoc)
{
    QDomElement connectionElement = domDoc.createElement("KexiDBConnection");
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

    QDomText tPass = domDoc.createTextNode(password);
    passElement.appendChild(tPass);

}



KexiProjectConnectionData::~KexiProjectConnectionData()
{
}
