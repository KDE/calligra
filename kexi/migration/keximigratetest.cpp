/***************************************************************************
 *   Copyright (C) 2004 by Adam Pigg                                       *
 *   adam@piggz.co.uk                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include "keximigrate.h"
#include "pqxxmigrate.h"
#include "./kexidb/connectiondata.h"
#include "./kexidb/connection.h"
#include "./kexidb/drivermanager.h"
#include "./kexidb/driver.h"
#include <kdebug.h>
#include <kinstance.h>

/*
This is in no way meant to compile let alone work
This is very preliminary and is meant for example only
 
This will be an example program to demonstrate how to import an exisiting db into
a new kexi based db
*/

using namespace std;
using namespace KexiDB;
using namespace KexiMigration;

int main(int argc, char *argv[])
{
	KexiDB::ConnectionData conn_data_from;
	KexiDB::ConnectionData conn_data_to;
	QGuardedPtr<KexiDB::Connection> kexi_conn;
	KexiMigrate::KexiMigrate* import;
	KInstance *instance;

	cout << "===Kexi Import Test Program===" << endl;

	instance = new KInstance("kexiimporttest" );

	//Start with a driver manager
	KexiDB::DriverManager manager;
	
	//get a pqxx driver
	KexiDB::Driver *driver = manager.driver("PostgreSQL");

	//Check for errors
	if (!driver || manager.error())
	{
		manager.debugError();
		return(1);
	}
	
	//Create connections to the kexi database
	kexi_conn = driver->createConnection(conn_data_to);

	import = new pqxxMigrate(&conn_data_from, "from_db", kexi_conn, false);
	if (import->performImport())
	{
		kdDebug() << "Import Succeeded" << endl;
	}
	else
	{
		kdDebug() << "Import failed!" << endl;
	}

	return EXIT_SUCCESS;
}
