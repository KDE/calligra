#include <iostream>
#include <string>

#include <qfileinfo.h>

#include <kdebug.h>
#include <kinstance.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>

using namespace std;
QCString prgname;

int main(int argc, char **argv)
{
	kdDebug() << "main()" << endl;
	QFileInfo info=QFileInfo(argv[0]);
	prgname = info.baseName().latin1();
	KInstance instance( prgname );
	if (argc<=1) {
		return 0;
	}
	QCString drv_name(argv[1]);
	QCString test_name = QString(argv[2]).lower().latin1();

	KexiDB::DriverManager manager; // = KexiDB::DriverManager::self();
	QStringList names = manager.driverNames();
	kdDebug() << "DRIVERS: " << endl;
	for (QStringList::iterator it = names.begin(); it != names.end() ; ++it)
		kdDebug() << *it << endl;
	if (manager.error()) {
		kdDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//get driver
	KexiDB::Driver *driver = manager.driver(drv_name);
	if (manager.error()) {
		kdDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//connection data that can be later reused
	KexiDB::ConnectionData conn_data;
	conn_data.setFileName("db");

	KexiDB::Connection *conn = driver->createConnection(conn_data);
	if (driver->error()) {
		kdDebug() << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	if (!conn->useDatabase( "db" )) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}

	KexiDB::Parser *parser = new KexiDB::Parser(conn);

	std::string cmd;
	while(cmd != "quit")
	{
		std::cout << "SQL> ";
		getline(std::cin, cmd);
		parser->parse(cmd.c_str());
		switch(parser->operation())
		{
			case KexiDB::Parser::OP_Error:
				kdDebug() << "***********************" << endl;
				kdDebug() << "* error               *" << endl;
				kdDebug() << "***********************" << endl;
				break;
			case KexiDB::Parser::OP_CreateTable:
			{
				kdDebug() << "Schema of table: " << parser->table()->name() << endl;
				parser->table()->debug();
				break;
			}
			case KexiDB::Parser::OP_Select:
			{
				kdDebug() << "Select statement: " << endl;
				parser->select()->debug();
				break;
			}
			default:
				kdDebug() << "main(): not implemented in main.cpp" << endl;

				
		}
		parser->clear();
	}
	return 0;
}

