
/* This file is part of the KDE project

   (C) Copyright 2008 by Lorenzo Villani <lvillani@binaryhelix.net>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "DataProvider.h"

#include <QPointer>

#include <KDebug>

#include <main/startup/KexiStartup.h>

#include <db/driver.h>
#include <db/connection.h>
#include <db/drivermanager.h>

namespace KexiWebForms {
namespace Model {


#ifdef __GNUC__
#warning Direct calls to gConnections are deprecated from now on!
#else
#pragma WARNING( Direct calls to gConnections are deprecated from now on! )
#endif
KexiDB::Connection* gConnection;

DataProvider* DataProvider::m_instance = 0;

DataProvider* DataProvider::instance() {
    if (!m_instance)
        m_instance = new DataProvider();
    return m_instance;
}

KexiDB::Connection* DataProvider::connection() {
    // FIXME: Ensure that the connection pointer exists!
    return m_connection;
}

bool DataProvider::initDatabase(const QString& fileName) {
    bool status = false;
    m_connData = new KexiDB::ConnectionData();

    QString driverName;
    QString suggestedDriverName;
    KexiStartupData::Import data;

    tristate res = KexiStartupHandler::detectActionForFile(data, driverName, "", fileName);
    kDebug() << "Database file name: " << fileName << " driver name: " << driverName;

    if (true == res) {
        if (driverName == "shortcut") {
            //! @todo Implement
        } else if (driverName == "connection") {
            //! @todo Implement
        } else {
            kDebug() << "This should be a file-based database... now loading it";

            m_driver = m_manager.driver(driverName);
            if (!m_driver || m_manager.error()) {
                m_manager.debugError();
                status = false;
            } else status = true;

            m_connData->setFileName(fileName);

            m_connection = m_driver->createConnection(*m_connData);
#ifdef __GNUC__
#warning This is just for compatibility
#else
#pragma WARNING( This is just for compatibility )
#endif
            gConnection = m_connection; // FIXME: drop this and port all code

            if (!m_connection || m_driver->error()) {
                m_driver->debugError();
                status = false;
            } else status = true;

            if (!m_connection->connect()) {
                m_connection->debugError();
                status = false;
            } else status = true;

            if (!m_connection->useDatabase(fileName)) {
                kError() << m_connection->errorMsg();
                status = false;
            } else {
                status = true;
            }
        }
    }
    return status;
}

}
}
