
#include <kdebug.h>
#include <kinstance.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

int main(int /*argc*/, char */*argv[]*/)
{
	KInstance instance("newapi");
	KexiDB::DriverManager manager;
	QStringList names = manager.driverNames();
	kdDebug() << "DRIVERS: " << endl;
	for (QStringList::iterator it = names.begin(); it != names.end() ; ++it)
		kdDebug() << *it << endl;
	if (manager.error()) {
		kdDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//get driver
	KexiDB::Driver *driver = manager.driver("mySQL");
	if (manager.error()) {
		kdDebug() << manager.errorMsg() << endl;
		return 1;
	}

	//connection data that can be later reused
	KexiDB::ConnectionData conn_data;

	conn_data.userName="root";
	conn_data.password="mysql";
	conn_data.hostName="localhost";

	KexiDB::Connection *conn = driver->createConnection(conn_data);
	if (driver->error()) {
		kdDebug() << driver->errorMsg() << endl;
		return 1;
	}
	if (!conn->connect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	if (!conn->useDatabase( "test" )) {
		kdDebug() <<"use db:"<< conn->errorMsg() << endl;
		return 1;
	}

	kdDebug()<<"Creating first cursor"<<endl;
	KexiDB::Cursor *c=conn->executeQuery("select * from Applications");
	if (!c) kdDebug()<<conn->errorMsg()<<endl;
	kdDebug()<<"Creating second cursor"<<endl;
	KexiDB::Cursor *c2=conn->executeQuery("select * from Applications");
	if (!c2) kdDebug()<<conn->errorMsg()<<endl;

	QStringList l=conn->databaseNames();
	if (l.isEmpty()) kdDebug()<<conn->errorMsg()<<endl;
	kdDebug()<<"Databases:"<<l<<endl;

	while (c->moveNext()) {
		kdDebug()<<"Cursor: Value(0)"<<c->value(0).asString()<<endl;
	}
	kdDebug()<<"Cursor error:"<<c->errorMsg()<<endl;
	while (c2->moveNext()) {
		kdDebug()<<"Cursor2: Value(0)"<<c2->value(0).asString()<<endl;
	}

	kdDebug()<<"Cursor::prev"<<endl;
	while (c->movePrev()) {
		kdDebug()<<"Cursor: Value(0)"<<c->value(0).asString()<<endl;
	}
#if 0
	KexiDB::Table *t = conn->tableSchema( "persons" );
	if (t)
		t->debug();
	t = conn->tableSchema( "cars" );
	if (t)
		t->debug();

//	conn->tableNames();

	if (!conn->disconnect()) {
		kdDebug() << conn->errorMsg() << endl;
		return 1;
	}
	debug("before del");
	delete conn;
	debug("after del");
	return 0;
#endif
}
