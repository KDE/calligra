#include <iostream>
#include <string>

#include <qfileinfo.h>
//Added by qt3to4:
#include <Q3CString>

#include <kdebug.h>
#include <kcomponentdata.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>

using namespace std;
Q3CString prgname;

int main(int argc, char **argv)
{
	kDebug() << "main()" << endl;
	QFileInfo info=QFileInfo(argv[0]);
	prgname = info.baseName().latin1();
	KComponentData componentData( prgname );
	if (argc<2) {
		return 1;
	}
	Q3CString drv_name(argv[1]);
	Q3CString db_name = QString(argv[2]).lower().latin1();

	KexiDB::DriverManager manager; // = KexiDB::DriverManager::self();
	QStringList names = manager.driverNames();
	kDebug() << "DRIVERS: " << endl;
	for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
		kDebug() << *it << endl;
	if (manager.error()) {
		kDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//get driver
	KexiDB::Driver *driver = manager.driver(drv_name);
	if (!driver || manager.error()) {
		kDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//connection data that can be later reused
	KexiDB::ConnectionData conn_data;
	conn_data.setFileName(db_name);

	KexiDB::Connection *conn = driver->createConnection(conn_data);
	if (!conn || driver->error()) {
		kDebug() << "error: " << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kDebug() << "error: " << conn->errorMsg() << endl;
		return 1;
	}
	if (!conn->useDatabase( db_name )) {
		kDebug() << "error: " << conn->errorMsg() << endl;
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
				kDebug() << "***********************" << endl;
				kDebug() << "* error               *" << endl;
				kDebug() << "***********************" << endl;
				break;
			case KexiDB::Parser::OP_CreateTable:
			{
				kDebug() << "Schema of table: " << parser->table()->name() << endl;
				parser->table()->debug();
				break;
			}
			case KexiDB::Parser::OP_Select:
			{
				kDebug() << "Select statement: " << endl;
				KexiDB::QuerySchema *q = parser->query();
				q->debug();
				delete q;
				break;
			}
			default:
				kDebug() << "main(): not implemented in main.cpp" << endl;

				
		}
		parser->clear();
	}
	return 0;
}

