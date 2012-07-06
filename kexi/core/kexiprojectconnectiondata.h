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

#ifndef KEXIPROJECTCONNECTIONDATA_H
#define KEXIPROJECTCONNECTIONDATA_H

#include <db/connectiondata.h>

class QDomElement;
class QDomDocument;
/**
 * This class aims to provide
 * methods to store/load database settings
 * especially for file based engines. Extends KexiDB::ConnectionData with
 * additional information (selected driver name and database name)
 * that allows fully-automatic reconnect eg. on next application startup.
 */

class KEXICORE_EXPORT KexiProjectConnectionData: public KexiDB::ConnectionData
{
public:

    KexiProjectConnectionData();

    KexiProjectConnectionData(const QString& driverName, const QString& databaseName, const QString &hostName, unsigned short int port,
                              const QString& userName, const QString &password, const QString& fileName);

    /**
     * connect to a embedded database
     */
    KexiProjectConnectionData(const QString &driverName, const QString &fileName = QString());

    ~KexiProjectConnectionData();

    static const QString &generateTmpName();

    static KexiProjectConnectionData* loadInfo(QDomElement &e);
    void writeInfo(QDomDocument &doc);

    void    setDriverName(const QString &driverName);
    void    setDatabaseName(const QString &databaseName);

    QString driverName() const;
    QString databaseName() const;

private:
    QString  m_driverName;
    QString  m_databaseName;

};

#endif
