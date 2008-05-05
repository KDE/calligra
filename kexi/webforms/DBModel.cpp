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

#include "DBModel.h"

#include <KDebug>

#include <kexidb/drivermanager.h>

namespace KexiWebForms {

    DBModel::DBModel(QString kexiFile) {
        m_driverManager = new KexiDB::DriverManager();

        kDebug() << "Loading default driver...";
        m_driver = m_driverManager->driver("SQLite3");
        if (!m_driver || m_driverManager->error()) {
            m_driverManager->debugError();
            exit(1);
        }

        kDebug() << "Opening database file " << kexiFile;
        m_connData.setFileName(kexiFile);

        kDebug() << "Opening connection with database...";
        m_connection = m_driver->createConnection(m_connData);
        if (!m_connection || m_driver->error()) {
            m_driver->debugError();
            exit(1);
        }
    }

    QStringList DBModel::getDatabases() {
        return m_connection->databaseNames(true);
    }

    QStringList DBModel::getTables(const QString& dbName) {
    }

}
