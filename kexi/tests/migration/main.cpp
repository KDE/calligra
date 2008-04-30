/* This file is part of the KDE project
   Copyright (C) 2008 Sharan Rao <sharanrao@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <QFileInfo>
#include <QPointer>

#include <kdebug.h>
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kcomponentdata.h>
#include <kiconloader.h>
#include <kaboutdata.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/utils.h>
#include <migration/keximigrate.h>
#include <migration/migratemanager.h>

#include <core/kexiprojectdata.h>

#include <iostream>

//! TODO Make this more generic to test any migration driver

int main(int argc, char** argv)
{
	// first the formalities
	KComponentData *instance = 0;
	QByteArray prgname;
	
	//! TODO use KCmdLineArguments with options
	// first argument should be xbase source directory
	QString xBaseSourceDirectory = QString::fromLatin1( argv[1] );
	
	// second argument should be kexi file ( destination )
	QString destinationDatabase = QString::fromLatin1( argv[2] );
	
	QFileInfo info=QFileInfo(argv[0]);
	prgname = info.baseName().toLatin1();
	
	instance = new KComponentData(prgname);
	
	// write the code for testing migration here
	// Start with a driver manager
	KexiDB::DriverManager manager;
	KexiMigration::MigrateManager migrateManager;
	
	kDebug() << "Creating destination driver..." << endl;
	
	// Get a driver to the destination database
	KexiDB::Driver *destDriver = manager.driver(KexiDB::defaultFileBasedDriverName() //file based
		);
	if (!destDriver || manager.error())
	{
		kDebug() << "Manager error..." << endl;
		manager.debugError();
	}
	
	KexiDB::ConnectionData *cdata;
	QString dbname;
	
	cdata = new KexiDB::ConnectionData();
	
	// set destination file name here.
	//! TODO User should be able to specify this
	cdata->driverName = KexiDB::defaultFileBasedDriverName();
	
	//! TODO User should be able to specify this
	dbname = destinationDatabase;
	cdata->setFileName( dbname );
	kDebug() << "Current file name: " << dbname << endl;
	
	
	QString sourceDriverName = "xbase";
	// get the source migration driver
	KexiMigration::KexiMigrate* sourceDriver = 0;
	sourceDriver = migrateManager.driver( sourceDriverName );
	if(!sourceDriver || migrateManager.error()) {
		kDebug() << "Import migrate driver error..." << endl;
		return -1;
	}
	
	KexiMigration::Data* md = new KexiMigration::Data();
	md->keepData = true;
	//	delete md->destination;
	md->destination = new KexiProjectData(*cdata, dbname);
	
	// Setup XBase connection data
	KexiDB::ConnectionData* conn_data = new KexiDB::ConnectionData();
	conn_data->setFileName(xBaseSourceDirectory);
	
	md->source = conn_data;
	md->sourceName = "";
	
	sourceDriver->setData(md);
	
	if ( !sourceDriver->performImport() ) {
		kDebug()<<"Import failed";
		return -1;
	}
	
	return 0;
}
