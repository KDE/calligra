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

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>

namespace KexiWebForms {
namespace Model {

KexiDB::Connection* gConnection;


DataProvider::DataProvider() {
    if (!m_instance)
        m_istance = new DataProvider();
    return m_istance;
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

            m_driver = manager.driver(driverName);
            if (!driver || manager.error()) {
                m_manager.debugError();
                status = false;
            } else status = true;

            m_connData->setFileName(fileName);

            m_connection = m_driver->createConnection(*connData);

            if (!m_connection || driver->error()) {
                m_driver->debugError();
                status = false;
            } else status = true;

            if (!m_connection->connect()) {
                m_connection->debugError();
                status = false;
            } else status = true;

            if (!gConnection->useDatabase(fileName)) {
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
