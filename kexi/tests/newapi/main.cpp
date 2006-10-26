/* This file is part of the KDE project
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

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

#include <qfileinfo.h>
#include <qguardedptr.h>

#include <kdebug.h>
#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kinstance.h>
#include <kiconloader.h>
#include <kaboutdata.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/field.h>
#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/indexschema.h>
#include <kexidb/parser/parser.h>

#include <iostream>

using namespace std;

QCString prgname;
QCString db_name;
QCString drv_name;
QCString test_name;
int cursor_options = 0;
bool db_name_required = true;

KexiDB::ConnectionData conn_data;
QGuardedPtr<KexiDB::Connection> conn;
QGuardedPtr<KexiDB::Driver> driver;
KApplication *app = 0;
KInstance *instance = 0;

static KCmdLineOptions options[] =
{
	{ "test <test_name>",
		"Available tests:\n"
		"- cursors: test for cursors behaviour\n"
		"- schema: test for db schema retrieving\n"
		"- dbcreation: test for new db creation\n"
		"- tables: test for tables creation and data\n"
		"   inserting\n"
		"- tableview: test for KexiDataTableView data-aware\n"
		"   widget\n"
		"- parser: test for parsing sql statements,\n"
		"   returns debug string for a given\n"
		"   sql statement or error message\n"
		"- dr_prop: shows properties of selected driver"
		, 0},
	{ "buffered-cursors",
		"Optional switch :turns cursors used in any tests\n"
		" to be buffered", 0},
	{ "query-params <params>", "Query parameters separated\n"
		"by '|' character that will be passed to query\n"
		"statement to replace [...] placeholders.", 0 },
	{ "", " Notes:\n"
		"1. 'dr_prop' requires <db_name> argument.\n"
		"2. 'parser' test requires <db_name>,\n"
		" <driver_name> and <sql_statement> arguments\n"
		"3. All other tests require <db_name>\n"
		" and <driver_name> arguments.\n"
		"4. 'tables' test automatically runs 'dbcreation'\n"
		" test. (<new_db_name> is removed if already exists.\n"
		"5. <db_name> must be a valid kexi database\n"
		" e.g. created with 'tables' test.", 0},
	{ "+driver_name", "Driver name", 0},
	{ "+[db_name]", "Database name", 0},
	{ "+[sql_statement]", "Optional SQL statement (for parser test)", 0},
	KCmdLineLastOption
};

#include "dbcreation_test.h"
#include "cursors_test.h"
#include "schema_test.h"
#include "tables_test.h"
#include "tableview_test.h"
#include "parser_test.h"
#include "dr_prop_test.h"

#define RETURN(code) \
	kdDebug()<< test_name << " TEST: " << (code==0?"PASSED":"ERROR") << endl; \
	return code

int main(int argc, char** argv)
{
	int minargs = 2;
	bool gui = false;
/*	if (argc < minargs) {
		usage();
		RETURN(0);
	}*/
	QFileInfo info=QFileInfo(argv[0]);
	prgname = info.baseName().latin1();
	
	KCmdLineArgs::init(argc, argv, 
		new KAboutData( prgname, "KexiDBTest",
			"0.1.2", "", KAboutData::License_GPL,
			"(c) 2003-2006, Kexi Team\n"
			"(c) 2003-2006, OpenOffice Polska Ltd.\n",
			"",
			"http://www.koffice.org/kexi",
			"submit@bugs.kde.org"
		)
	);
	KCmdLineArgs::addCmdLineOptions( options );
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	QCStringList tests;
	tests << "cursors" << "schema" << "dbcreation" << "tables" 
		<< "tableview" << "parser" << "dr_prop";
	if (!args->isSet("test")) {
		kdDebug() << "No test specified. Use --help." << endl;
		RETURN(1);
	}
	test_name = args->getOption("test");
	if (!tests.contains(test_name)) {
		kdDebug() << QString("No such test \"%1\". Use --help.").arg(test_name) << endl;
		RETURN(1);
	}
	
	if (test_name=="tableview") {
		gui = true;
	}
	else if (test_name=="parser") {
		minargs = 3;
	}
	else if (test_name=="dr_prop") {
		minargs = 1;
		db_name_required = false;
	}
	if ((int)args->count()<minargs) {
		kdDebug() << QString("Not enough args (%1 required). Use --help.").arg(minargs) << endl;
		RETURN(1);
	}
	
	if (gui) {
		app = new KApplication(true, true);
		instance = app;
		KGlobal::iconLoader()->addAppDir("kexi");
	}
	else {
		instance = new KInstance(prgname);
	}

	drv_name = args->arg(0);
			 
	KexiDB::DriverManager manager;
	QStringList names = manager.driverNames();
	kdDebug() << "DRIVERS: " << endl;
	for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
		kdDebug() << *it << endl;
	if (manager.error() || names.isEmpty()) {
		manager.debugError();
		RETURN(1);
	}

	//get driver
	driver = manager.driver(drv_name);
	if (!driver || manager.error()) {
		manager.debugError();
		RETURN(1);
	}
	kdDebug() << "MIME type for '" << driver->name() << "': " << driver->fileDBDriverMimeType() << endl;

	//open connection
	if (args->count() >= 2)
		db_name = args->arg(1);
	if (db_name_required && db_name.isEmpty()) {
		kdDebug() << prgname << ": database name?" << endl;
		RETURN(1);
	}
	if (!db_name.isEmpty()) {
		//additional switches:
		if (args->isSet("buffered-cursors")) {
			cursor_options |= KexiDB::Cursor::Buffered;
		}
		conn_data.setFileName( db_name );
		conn = driver->createConnection(conn_data);

		if (!conn || driver->error()) {
			driver->debugError();
			RETURN(1);
		}
		if (!conn->connect()) {
			conn->debugError();
			RETURN(1);
		}
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
	else if (test_name == "parser") {
		QStringList params;
		if (args->isSet("query-params"))
			params = QStringList::split("|", args->getOption("query-params"));
		r=parserTest(args->arg(2), params);
	}
	else if (test_name == "dr_prop")
		r=drPropTest();
	else {
		kdWarning() << "No such test: " << test_name << endl;
//		usage();
		RETURN(1);
	}

	if (app && r==0)
		app->exec();

	if (r)
		kdDebug() << "RECENT SQL STATEMENT: " << conn->recentSQLString() << endl;

	if (conn && !conn->disconnect())
		r = 1;
	
//	kdDebug() << "!!! KexiDB::Transaction::globalcount == " << KexiDB::Transaction::globalCount() << endl;
//	kdDebug() << "!!! KexiDB::TransactionData::globalcount == " << KexiDB::TransactionData::globalCount() << endl;

	delete app;

	RETURN(r);
}
