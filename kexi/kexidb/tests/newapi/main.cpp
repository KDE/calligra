#include <qfileinfo.h>

#include <kdebug.h>
#include <kinstance.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

QCString prgname;

void usage()
{
	kdDebug() << "usage: " << prgname << " <driver_name> cursors" << endl;
	kdDebug() << prgname << " <driver_name> schema" << endl;
	kdDebug() << prgname << " <driver_name> dbcreation <new_db_name>" << endl;
}

int main(int argc, char** argv)
{
	QFileInfo info=QFileInfo(argv[0]);
	prgname = info.baseName().latin1();
	KInstance instance( prgname );
	if (argc<=2) {
		usage();
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
//	conn_data.host = "myhost";
//	conn_data.password = "mypwd";

	if (test_name == "cursors") {
#include "cursors_test.h"
	}
	else if (test_name == "schema") {
#include "schema_test.h"
	}
	else if (test_name == "dbcreation") {
#include "dbcreation_test.h"
	}
	else {
		kdDebug() << "No such test: " << test_name << endl;
		usage();
		return 1;
	}

	return 0;
}
