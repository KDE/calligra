#include <qfileinfo.h>

#include <kdebug.h>
#include <kinstance.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

void usage(const QCString a)
{
	kdDebug() << "usage: " << a << " <driver_name> <test_name>" << endl;
	kdDebug() << "test names can be: cursors, schema" << endl;
}

int main(int argc, char** argv)
{
	QFileInfo info=QFileInfo(argv[0]);
	KInstance instance( info.baseName().latin1() );
	if (argc<=2) {
		usage(instance.instanceName());
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
	else {
		kdDebug() << "No such test: " << test_name << endl;
		usage(instance.instanceName());
		return 1;
	}

	return 0;
}
