/* This file is part of the KDE project
   Copyright (C) 2008 Sharan Rao <sharanrao@gmail.com>

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

#include <migration/keximigrate.h>
#include <migration/migratemanager.h>
#include <core/kexiprojectdata.h>

#include <KDbDriverManager>
#include <KDbDriver>
#include <KDbConnection>
#include <KDbCursor>
#include <KDbField>
#include <KDbUtils>

#include <kcomponentdata.h>

#include <QFileInfo>
#include <QDebug>

#include <iostream>

//! TODO Make this more generic to test any migration driver

int main(int argc, char** argv)
{
    Q_UNUSED(argv);
    Q_UNUSED(argc);
    // first the formalities
    QByteArray prgname;

    //! TODO use KCmdLineArguments with options
    // first argument should be xbase source directory
    QString xBaseSourceDirectory = QString::fromLatin1(argv[1]);

    // second argument should be kexi file ( destination )
    QString destinationDatabase = QString::fromLatin1(argv[2]);

    QFileInfo info = QFileInfo(argv[0]);
    prgname = info.baseName().toLatin1();

    //Needed for variosu things like i18n and kconfig and stuff. No need to keep it around or clean it as this is just a test case so nothing long-lived
    new KComponentData(prgname);

    // write the code for testing migration here
    // Start with a driver manager
    KDbDriverManager manager;
    KexiMigration::MigrateManager migrateManager;

    qDebug() << "Creating destination driver...";

    // Get a driver to the destination database
    KDbDriver *destDriver = manager.driver(KDb::defaultFileBasedDriverId());
    if (!destDriver || manager.result().isError()) {
        qDebug() << "Manager error:" << manager.result();
    }

    KDbConnectionData *cdata;
    QString dbname;

    cdata = new KDbConnectionData();

    // set destination file name here.
    //! TODO User should be able to specify this
    cdata->driverName = KDb::defaultFileBasedDriverId();

    //! TODO User should be able to specify this
    dbname = destinationDatabase;
    cdata->setDatabaseName(dbname);
    qDebug() << "Current file name: " << dbname;


    QString sourceDriverName = "xbase";
    // get the source migration driver
    KexiMigration::KexiMigrate* sourceDriver = 0;
    sourceDriver = migrateManager.driver(sourceDriverName);
    if (!sourceDriver || migrateManager.error()) {
        qDebug() << "Import migrate driver error...";
        return -1;
    }

    KexiMigration::Data* md = new KexiMigration::Data();
    md->keepData = true;
    // delete md->destination;
    md->destination = new KexiProjectData(*cdata, dbname);

    // Setup XBase connection data
    KDbConnectionData* conn_data = new KDbConnectionData();
    conn_data->setFileName(xBaseSourceDirectory);

    md->source = conn_data;
    md->sourceName = "";

    sourceDriver->setData(md);

    if (!sourceDriver->performImport()) {
        qDebug() << "Import failed";
        return -1;
    }

    return 0;
}
