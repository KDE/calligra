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
	if (argc<2) {
		return 1;
	}
	QCString drv_name(argv[1]);
	QCString db_name = QString(argv[2]).lower().latin1();

	KexiDB::DriverManager manager; // = KexiDB::DriverManager::self();
	QStringList names = manager.driverNames();
	kdDebug() << "DRIVERS: " << endl;
	for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
		kdDebug() << *it << endl;
	if (manager.error()) {
		kdDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//get driver
	KexiDB::Driver *driver = manager.driver(drv_name);
	if (!driver || manager.error()) {
		kdDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//connection data that can be later reused
	KexiDB::ConnectionData conn_data;
	conn_data.setFileName(db_name);

	KexiDB::Connection *conn = driver->createConnection(conn_data);
	if (!conn || driver->error()) {
		kdDebug() << "error: " << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kdDebug() << "error: " << conn->errorMsg() << endl;
		return 1;
	}
	if (!conn->useDatabase( db_name )) {
		kdDebug() << "error: " << conn->errorMsg() << endl;
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
				KexiDB::QuerySchema *q = parser->query();
				q->debug();
				delete q;
				break;
			}
			default:
				kdDebug() << "main(): not implemented in main.cpp" << endl;

				
		}
		parser->clear();
	}
	return 0;
}

