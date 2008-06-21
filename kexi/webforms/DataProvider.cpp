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

#include <core/kexidbshortcutfile.h>
#include <core/kexiproject.h>
#include <core/kexiprojectdata.h>
#include <core/kexistartupdata.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/indexschema.h>

namespace KexiWebForms {

    KexiDB::Connection* gConnection;
    
    // FIXME: Move creation of ConnectionData outside this function
    bool initDatabase(const QString& fileName) {
        bool status = false;
        KexiDB::Driver* driver;
        KexiDB::DriverManager manager;
        KexiDB::ConnectionData* connData = new KexiDB::ConnectionData();
        KexiProjectData* projectData;


        QString driverName;
        QString suggestedDriverName;
        KexiStartupData::Import data;

        tristate res = KexiStartupHandler::detectActionForFile(data, driverName, "", fileName);
        kDebug() << "Database file name: " << fileName << " driver name: " << driverName << endl;

        if (true == res) {
            if (driverName == "shortcut") {
                //
                // TODO: Finish implementing this stuff
                //
                kDebug() << "Loading Kexi shortcut file..." << endl;
                KexiDBShortcutFile shortcut(fileName);
                projectData = new KexiProjectData();
                if (!shortcut.loadProjectData(*projectData)) {
                    delete projectData; projectData = NULL;
                    return false;
                }
            } else if (driverName == "connection") {
                //
                // FIXME: This piece of code does NOT work, ouch!
                //
                kDebug() << "Loading connection file..." << endl;
                KexiDBConnShortcutFile connFile(fileName);
                if (!connFile.loadConnectionData(*connData)) {
                    kDebug() << "Failed to load connection data from .kexic file..." << endl;
                    return false;
                } else {
                    kDebug() << "Data loaded successfully, create project object..." << endl;
                    // FIXME: as noticed by jstaniek I should ask the user which
                    // database has to be opened
                    /* commented out until i find a better solution to display project data */
                    //projectData = new KexiProjectData(*connData);
                }
            } else {
                kDebug() << "This should be a file-based database... now loading it" << endl;

                driver = manager.driver(driverName);
                if (!driver || manager.error()) {
                    manager.debugError();
                    status = false;
                } else status = true;

                connData->setFileName(fileName);

                gConnection = driver->createConnection(*connData);

                if (!gConnection || driver->error()) {
                    driver->debugError();
                    status = false;
                } else status = true;

                if (!gConnection->connect()){
                    gConnection->debugError();
                    status = false;
                } else status = true;

                if (!gConnection->useDatabase(fileName)) {
                    kError() << gConnection->errorMsg() << endl;
                    status = false;
                } else {
                    status = true;
                }

                projectData = new KexiProjectData(*connData);
            }
        }
        return status;
    }

}
