
#include <kdebug.h>
#include <kinstance.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

int main(int argc, char *argv[])
{
	KInstance instance("newapi");
	KexiDB::DriverManager* manager = KexiDB::DriverManager::self();
	QStringList names = manager->driversNames();
	kdDebug() << "DRIVERS: " << names << endl;
	if (manager->error()) {
		kdDebug() << manager->errorMsg() << endl;
		return 1;
	}

	//get driver
	KexiDB::Driver *driver = manager->driver("SQLite");
	if (manager->error()) {
		kdDebug() << manager->errorMsg() << endl;
		return 1;
	}

	//connection data that can be later reused
	KexiDB::ConnectionData conn_data;
//	conn_data.host = "myhost";
//	conn_data.password = "mypwd";
#if 0
	conn_data.setFileName( "db" );

	KexiDB::Connection *conn = driver->createConnection(conn_data);
	if (driver->error()) {
		kdDebug() << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	kdDebug() << "DATABASES:" << conn->databaseNames() << endl;
	if (conn->error()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	if (conn->databaseExists( "db" )) {
		debug("dropDatabase()=%d",conn->dropDatabase());
	}
	debug("createDatabase()=%d",conn->createDatabase("db"));

#endif

#if 0
	conn_data.setFileName( "mydb" );

	KexiDB::Connection *conn = driver->createConnection(conn_data);
	if (driver->error()) {
		kdDebug() << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	if (!conn->databaseExists( "mydb" )) {
		if (!conn->createDatabase( "mydb" )) {
			kdDebug() << conn->errorMsg() << endl;
			return 1;
		}
		kdDebug() << "DB created"<< endl;
	}
	if (!conn->useDatabase( "mydb" )) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	KexiDB::Cursor *cursor = conn->executeQuery( "select * from osoby, o where o.imie='1'" );
	debug("executeQuery() = %d",!!cursor);
	if (cursor) {
		debug("Cursor::open() == %d",cursor->open());
		debug("Cursor::moveLast() == %d",cursor->moveLast());
		debug("Cursor::moveFirst() == %d",cursor->moveFirst());

		debug("Cursor::moveNext() == %d",cursor->moveNext());
		debug("Cursor::moveNext() == %d",cursor->moveNext());
		debug("Cursor::moveNext() == %d",cursor->moveNext());
		debug("Cursor::moveNext() == %d",cursor->moveNext());
		debug("Cursor::eof() == %d",cursor->eof());
		conn->deleteCursor(cursor);
//		delete cursor;
	}
#endif

#if 1
	conn_data.setFileName( "db" );

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

	KexiDB::Table *t = conn->tableSchema( "persons" );
	if (t)
		t->debug();
	t = conn->tableSchema( "cars" );
	if (t)
		t->debug();

#endif
//	conn->tableNames();

	if (!conn->disconnect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	delete conn;
	return 0;
}
