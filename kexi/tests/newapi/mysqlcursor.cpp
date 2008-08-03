
#include <kdebug.h>
#include <kcomponentdata.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>

int main(int argc, char * argv[])
{
    KComponentData componentData("newapi");
    KexiDB::DriverManager manager;
    QStringList names = manager.driverNames();
    kDebug() << "DRIVERS: " << endl;
    for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
        kDebug() << *it << endl;
    if (manager.error()) {
        kDebug() << manager.errorMsg() << endl;
        return 1;
    }

    //get driver
    KexiDB::Driver *driver = manager.driver("mySQL");
    if (manager.error()) {
        kDebug() << manager.errorMsg() << endl;
        return 1;
    }

    //connection data that can be later reused
    KexiDB::ConnectionData conn_data;

    conn_data.userName = "root";
    if (argc > 1)
        conn_data.password = argv[1];
    else
        conn_data.password = "mysql";
    conn_data.hostName = "localhost";

    KexiDB::Connection *conn = driver->createConnection(conn_data);
    if (driver->error()) {
        kDebug() << driver->errorMsg() << endl;
        return 1;
    }
    if (!conn->connect()) {
        kDebug() << conn->errorMsg() << endl;
        return 1;
    }
    if (!conn->useDatabase("test")) {
        kDebug() << "use db:" << conn->errorMsg() << endl;
        return 1;
    }

    kDebug() << "Creating first cursor" << endl;
    KexiDB::Cursor *c = conn->executeQuery("select * from Applications");
    if (!c) kDebug() << conn->errorMsg() << endl;
    kDebug() << "Creating second cursor" << endl;
    KexiDB::Cursor *c2 = conn->executeQuery("select * from Applications");
    if (!c2) kDebug() << conn->errorMsg() << endl;

    QStringList l = conn->databaseNames();
    if (l.isEmpty()) kDebug() << conn->errorMsg() << endl;
    kDebug() << "Databases:" << endl;
    for (QStringList::ConstIterator it = l.constBegin(); it != l.constEnd() ; ++it)
        kDebug() << *it << endl;

    if (c) {
        while (c->moveNext()) {
            kDebug() << "Cursor: Value(0)" << c->value(0).toString() << endl;
            kDebug() << "Cursor: Value(1)" << c->value(1).toString() << endl;
        }
        kDebug() << "Cursor error:" << c->errorMsg() << endl;
    }
    if (c2) {
        while (c2->moveNext()) {
            kDebug() << "Cursor2: Value(0)" << c2->value(0).toString() << endl;
            kDebug() << "Cursor2: Value(1)" << c2->value(1).toString() << endl;
        }
    }
    if (c) {
        kDebug() << "Cursor::prev" << endl;
        while (c->movePrev()) {
            kDebug() << "Cursor: Value(0)" << c->value(0).toString() << endl;
            kDebug() << "Cursor: Value(1)" << c->value(1).toString() << endl;

        }
        kDebug() << "up/down" << endl;
        c->moveNext();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString() << endl;
        kDebug() << "Cursor: Value(1)" << c->value(1).toString() << endl;
        c->moveNext();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString() << endl;
        kDebug() << "Cursor: Value(1)" << c->value(1).toString() << endl;
        c->movePrev();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString() << endl;
        kDebug() << "Cursor: Value(1)" << c->value(1).toString() << endl;
        c->movePrev();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString() << endl;
        kDebug() << "Cursor: Value(1)" << c->value(1).toString() << endl;

    }
#if 0
    KexiDB::Table *t = conn->tableSchema("persons");
    if (t)
        t->debug();
    t = conn->tableSchema("cars");
    if (t)
        t->debug();

// conn->tableNames();

    if (!conn->disconnect()) {
        kDebug() << conn->errorMsg() << endl;
        return 1;
    }
    debug("before del");
    delete conn;
    debug("after del");
#endif
    return 0;
}
