/* This file is part of the KDE project
   Copyright (C) 2003-2010 Jarosław Staniek <staniek@kde.org>

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

#include <KexiIcon.h>
#include <core/kexiproject.h>

#include <KDbDriverManager>
#include <KDbDriver>
#include <KDbConnection>
#include <KDbCursor>
#include <KDbField>
#include <KDbTableSchema>
#include <KDbQuerySchema>
#include <KDbIndexSchema>
#include <KDbParser>

#include <kcmdlineargs.h>
#include <kapplication.h>
#include <kcomponentdata.h>
#include <kiconloader.h>
#include <kaboutdata.h>

#include <QFileInfo>
#include <QPointer>
#include <QDebug>

#include <iostream>

using namespace std;

QByteArray prgname;
QString db_name;
QString drv_name;
QString test_name;
int cursor_options = 0;
bool db_name_required = true;

QPointer<KexiProject> project;
QPointer<KexiDB::Connection> conn;
QPointer<KexiDB::Driver> driver;
KApplication *app = 0;
KComponentData *instance = 0;

#include "dbcreation_test.h"
#include "cursors_test.h"
#include "schema_test.h"
#include <db/tests/tables_test.h>
#ifndef NO_GUI
# include "tableview_test.h"
#endif
#include "parser_test.h"
#include "dr_prop_test.h"

void exitRoutine()
{
    if (project)
        project->closeConnection();
    delete project;
}

#define RETURN(code) \
    exitRoutine(); \
    qDebug()<< test_name << " TEST: " << (code==0?"PASSED":"ERROR"); \
    return code

int main(int argc, char** argv)
{
    int minargs = 2;
    bool gui = false;
    /* if (argc < minargs) {
        usage();
        RETURN(0);
      }*/
    QFileInfo info = QFileInfo(argv[0]);
    prgname = info.baseName().toLatin1();

    KAboutData aboutData(prgname, 0, kxi18n("KexiDBTest"),
                         KEXI_VERSION_STRING, KLocalizedString(), KAboutData::License_GPL,
                         kxi18n("(c) 2003-2010, Kexi Team\n"
                               "(c) 2003-2006, OpenOffice Software.\n"),
                         KLocalizedString(),
                         "http://www.calligra.org/kexi",
                         "submit@bugs.kde.org");
    KCmdLineArgs::init(argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add("test <test_name>", kxi18n("Available tests:\n"
                                          "- cursors: test for cursors behaviour\n"
                                          "- schema: test for db schema retrieving\n"
                                          "- dbcreation: test for new db creation\n"
                                          "- tables: test for tables creation and data\n"
                                          "   inserting\n"
#ifndef NO_GUI
                                          "- tableview: test for KexiDataTableView data-aware\n"
                                          "   widget\n"
#endif
                                          "- parser: test for parsing sql statements,\n"
                                          "   returns debug string for a given\n"
                                          "   sql statement or error message\n"
                                          "- dr_prop: shows properties of selected driver"));
    options.add("buffered-cursors", kxi18n("Optional switch: turns cursors used in any tests\n"
                                          " to be buffered"));
    options.add("query-params <params>", kxi18n("Query parameters separated\n"
                "by '|' character that will be passed to query\n"
                "statement to replace [...] placeholders."));
    options.add("", kxi18n(" Notes:<nl/>"
                          "1. 'dr_prop' requires <placeholder>db_name</placeholder> argument.<nl/>"
                          "2. 'parser' test requires <placeholder>db_name</placeholder>,<nl/>"
                          " <placeholder>driver_name</placeholder> and <placeholder>sql_statement</placeholder> arguments<nl/>"
                          "3. All other tests require <placeholder>db_name</placeholder><nl/>"
                          " and <placeholder>driver_name</placeholder> arguments.\n"
                          "4. 'tables' test automatically runs 'dbcreation'<nl/>"
                          " test. (<placeholder>new_db_name</placeholder> is removed if already exists).<nl/>"
                          "5. <placeholder>db_name</placeholder> must be a valid kexi database<nl/>"
                          " e.g. created with 'tables' test."));
    options.add("+driver_name", kxi18n("Driver name"));
    options.add("+[db_name]", kxi18n("Database name"));
    options.add("+[sql_statement]", kxi18n("Optional SQL statement (for parser test)"));
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    QStringList tests;
    tests << "cursors" << "schema" << "dbcreation" << "tables"
          << "tableview" << "parser" << "dr_prop";
    if (!args->isSet("test")) {
        qDebug() << "No test specified. Use --help.";
        RETURN(1);
    }
    test_name = args->getOption("test");
    if (!tests.contains(test_name)) {
        qDebug() << QString("No such test \"%1\". Use --help.").arg(test_name);
        RETURN(1);
    }

    if (test_name == "tableview") {
        gui = true;
    } else if (test_name == "parser") {
        minargs = 3;
    } else if (test_name == "dr_prop") {
        minargs = 1;
        db_name_required = false;
    }
    if ((int)args->count() < minargs) {
        qDebug() << QString("Not enough args (%1 required). Use --help.").arg(minargs);
        RETURN(1);
    }

    if (gui) {
        app = new KApplication(true);
        app->setWindowIcon(koIcon("table"));
        instance = new KComponentData(KGlobal::mainComponent());
        KIconLoader::global()->addAppDir("kexi");
    } else {
        instance = new KComponentData(prgname);
    }

    drv_name = args->arg(0);

    KexiDB::DriverManager manager;
    QStringList names = manager.driverNames();
    qDebug() << "DRIVERS: ";
    for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
        qDebug() << *it;
    if (manager.error() || names.isEmpty()) {
        manager.debugError();
        RETURN(1);
    }

    //get driver
    const KexiDB::Driver::Info drv_info = manager.driverInfo(drv_name);
    driver = manager.driver(drv_name);
    if (drv_info.name.isEmpty() || manager.error()) {
        manager.debugError();
        RETURN(1);
    }
    qDebug() << "MIME type for '" << drv_info.name << "': " << drv_info.fileDBMimeType;

    //open connection
    if (args->count() >= 2)
        db_name = args->arg(1);

    if (db_name_required && db_name.isEmpty()) {
        qDebug() << prgname << ": database name?";
        RETURN(1);
    }
    if (!db_name.isEmpty()) {
        //additional switches:
        if (args->isSet("buffered-cursors")) {
            cursor_options |= KexiDB::Cursor::Buffered;
        }
        KexiProjectData project_data;
        project_data.setDatabaseName(db_name);
        if (drv_info.fileBased) {
            project_data.connectionData()->setFileName(db_name);
        }
        project_data.connectionData()->driverName = drv_name;
        project = new KexiProject(project_data);
        bool incompatibleWithKexi = false;
        tristate res;
        if (test_name == "dbcreation" || test_name == "tables")
            res = project->create(true /*force overwrite*/);
        else
            res = project->open(&incompatibleWithKexi);
        if (res != true) {
            if (incompatibleWithKexi)
                qDebug() << "incompatibleWithKexi";
            project->debugError();
            RETURN(1);
        }
        conn = project->dbConnection();
/*        conn = driver->createConnection(conn_data);
        if (!conn || driver->error()) {
            driver->debugError();
            RETURN(1);
        }
        if (!conn->connect()) {
            conn->debugError();
            RETURN(1);
        }*/
    }

    //start test:
    int r = 0;
    if (test_name == "cursors")
        r = cursorsTest();
    else if (test_name == "schema")
        r = schemaTest();
    else if (test_name == "dbcreation")
        r = dbCreationTest();
    else if (test_name == "tables")
        r = tablesTest();
#ifndef NO_GUI
    else if (test_name == "tableview")
        r = tableViewTest();
#endif
    else if (test_name == "parser") {
        QStringList params;
        if (args->isSet("query-params"))
            params = args->getOption("query-params").split('|');
        r = parserTest(QString(args->arg(2)), params);
    } else if (test_name == "dr_prop")
        r = drPropTest();
    else {
        qWarning() << "No such test: " << test_name;
//  usage();
        RETURN(1);
    }

    if (app && r == 0)
        app->exec();

    if (r)
        qDebug() << "RECENT SQL STATEMENT: " << conn->recentSQLString();

    if (project) {
        if (!project->closeConnection())
            r = 1;
        delete project;
    }
//    if (conn && !conn->disconnect())
//        r = 1;

// qDebug() << "!!! KexiDB::Transaction::globalcount == " << KexiDB::Transaction::globalCount();
// qDebug() << "!!! KexiDB::TransactionData::globalcount == " << KexiDB::TransactionData::globalCount();

    delete app;

    RETURN(r);
}
