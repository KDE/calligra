/* This file is part of the KDE project
   Copyright (C) 2003 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qfileinfo.h>
#include <qguardedptr.h>

#include <kdebug.h>
#include <kinstance.h>
#include <kapplication.h>
#include <kiconloader.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/indexschema.h>

#include <iostream>

using namespace std;

QCString prgname;
QCString db_name;
QCString drv_name;
QCString test_name;
int cursor_options = 0;

KexiDB::ConnectionData conn_data;
QGuardedPtr<KexiDB::Connection> conn;
KInstance *instance;
KApplication *app;

void usage()
{
	kdWarning() << "usage: " << endl;
	kdWarning() << prgname << " <driver_name> cursors <db_name>" << endl;
	kdWarning() << "  - test for cursors behaviour" << endl;
	kdWarning() << prgname << " <driver_name> schema <db_name>" << endl;
	kdWarning() << "  - test for db schema retrieving" << endl;
	kdWarning() << prgname << " <driver_name> dbcreation <new_db_name>" << endl;
	kdWarning() << "  - test for new db creation" << endl;
	kdWarning() << prgname << " <driver_name> tables <new_db_name>" << endl;
	kdWarning() << "  - test for tables creation and data inserting" << endl;
	kdWarning() << prgname << " <driver_name> tableview <db_name>" << endl;
	kdWarning() << "  - test for KexiDataTableView data-aware widget" << endl;
	kdWarning() << " Notes:\n 'tables' test, automatically runs 'dbcreation' test.\n" 
		" <new_db_name> is removed if already exists.\n"
		" <db_name> must be a valid kexi database e.g. created with 'tables' test." 
		<< endl;
	kdWarning() << " Optional switches, you can append at the end:\n"
		" -buffered-cursors -- turns cursors used in any tests to be buffered"
		<< endl;
}

#include "dbcreation_test.h"
#include "cursors_test.h"
#include "schema_test.h"
#include "tables_test.h"
#include "tableview_test.h"

#define RETURN(code) \
	kdWarning()<< test_name << " TEST: " << (code==0?"PASSED":"ERROR") << endl; \
	return code

int main(int argc, char** argv)
{
	if (argc<=2) {
		usage();
		RETURN(0);
	}
	QFileInfo info=QFileInfo(argv[0]);
	prgname = info.baseName().latin1();
	drv_name = argv[1];
	test_name = QString(argv[2]).lower().latin1();

	if (test_name=="tableview") {
		//GUI test
		app = new KApplication(argc, argv, prgname);
		KGlobal::iconLoader()->addAppDir("kexi");
			instance = app;
	}
	else {
		//CLI test
		instance = new KInstance( prgname );
	}

	KexiDB::DriverManager manager;// = new KexiDB::DriverManager;
	QStringList names = manager.driverNames();
	kdWarning() << "DRIVERS: " << endl;
	for (QStringList::iterator it = names.begin(); it != names.end() ; ++it)
		kdWarning() << *it << endl;
	if (manager.error() || names.isEmpty()) {
		manager.debugError();
		RETURN(1);
	}

	//get driver
	KexiDB::Driver *driver = manager.driver(drv_name);
	if (!driver || manager.error()) {
		manager.debugError();
		RETURN(1);
	}
	kdWarning() << "MIME type for '" << driver->name() << "': " << driver->fileDBDriverMime() << endl;

//connection data
//	conn_data.host = "myhost";
//	conn_data.password = "mypwd";

//open connection
	
	if (argc<=3) {
		kdWarning() << prgname << ": database name?" << endl;
		usage();
		RETURN(1);
	}
	db_name = QCString(argv[3]);
	//additional switches:
	for (int i=4;i<argc;i++) {
		if (QCString(argv[i]).contains("-buffered-cursors",false)) {
			cursor_options |= KexiDB::Cursor::Buffered;
		}
		else {
			kdWarning() << "Unknown switch: '" << QCString(argv[i]) << "'" << endl;
			usage();
			RETURN(1);
		}
	}
	
	conn_data.setFileName( db_name );

	conn = driver->createConnection(conn_data);

	if (driver->error()) {
		driver->debugError();
		RETURN(1);
	}
	if (!conn->connect()) {
		conn->debugError();
		RETURN(1);
	}

//start test:
	int r=0;
	if (test_name == "cursors")
		r=cursorsTest();
	else if (test_name == "schema")
		r=schemaTest();
	else if (test_name == "dbcreation")
		r=dbCreationTest();
	else if (test_name == "tables")
		r=tablesTest();
	else if (test_name == "tableview")
		r=tableViewTest();
	else {
		kdWarning() << "No such test: " << test_name << endl;
		usage();
		RETURN(1);
	}

	if (app && r==0)
		app->exec();

	if (conn) {
	    if (!conn->disconnect())
		r = 1;
	}
	else
	    r = 1;
	
	kdWarning() << "!!! KexiDB::Transaction::globalcount == " << KexiDB::Transaction::globalCount() << endl;
	kdWarning() << "!!! KexiDB::TransactionData::globalcount == " << KexiDB::TransactionData::globalCount() << endl;

//	delete manager;
	delete instance;

	RETURN(r);
}

