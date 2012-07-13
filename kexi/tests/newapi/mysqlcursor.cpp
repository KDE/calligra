
#include <kdebug.h>
#include <kcomponentdata.h>

#include <db/drivermanager.h>
#include <db/driver.h>
#include <db/connection.h>
#include <db/cursor.h>

int main(int argc, char * argv[])
{
    KComponentData componentData("newapi");
    KexiDB::DriverManager manager;
    QStringList names = manager.driverNames();
    kDebug() << "DRIVERS: ";
    for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
        kDebug() << *it;
    if (manager.error()) {
        kDebug() << manager.errorMsg();
        return 1;
    }

    //get driver
    KexiDB::Driver *driver = manager.driver("mySQL");
    if (manager.error()) {
        kDebug() << manager.errorMsg();
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
        kDebug() << driver->errorMsg();
        return 1;
    }
    if (!conn->connect()) {
        kDebug() << conn->errorMsg();
        return 1;
    }
    if (!conn->useDatabase("test")) {
        kDebug() << "use db:" << conn->errorMsg();
        return 1;
    }

    kDebug() << "Creating first cursor";
    KexiDB::Cursor *c = conn->executeQuery("select * from Applications");
    if (!c) kDebug() << conn->errorMsg();
    kDebug() << "Creating second cursor";
    KexiDB::Cursor *c2 = conn->executeQuery("select * from Applications");
    if (!c2) kDebug() << conn->errorMsg();

    QStringList l = conn->databaseNames();
    if (l.isEmpty()) kDebug() << conn->errorMsg();
    kDebug() << "Databases:";
    for (QStringList::ConstIterator it = l.constBegin(); it != l.constEnd() ; ++it)
        kDebug() << *it;

    if (c) {
        while (c->moveNext()) {
            kDebug() << "Cursor: Value(0)" << c->value(0).toString();
            kDebug() << "Cursor: Value(1)" << c->value(1).toString();
        }
        kDebug() << "Cursor error:" << c->errorMsg();
    }
    if (c2) {
        while (c2->moveNext()) {
            kDebug() << "Cursor2: Value(0)" << c2->value(0).toString();
            kDebug() << "Cursor2: Value(1)" << c2->value(1).toString();
        }
    }
    if (c) {
        kDebug() << "Cursor::prev";
        while (c->movePrev()) {
            kDebug() << "Cursor: Value(0)" << c->value(0).toString();
            kDebug() << "Cursor: Value(1)" << c->value(1).toString();

        }
        kDebug() << "up/down";
        c->moveNext();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString();
        kDebug() << "Cursor: Value(1)" << c->value(1).toString();
        c->moveNext();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString();
        kDebug() << "Cursor: Value(1)" << c->value(1).toString();
        c->movePrev();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString();
        kDebug() << "Cursor: Value(1)" << c->value(1).toString();
        c->movePrev();
        kDebug() << "Cursor: Value(0)" << c->value(0).toString();
        kDebug() << "Cursor: Value(1)" << c->value(1).toString();

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
        kDebug() << conn->errorMsg();
        return 1;
    }
    debug("before del");
    delete conn;
    debug("after del");
#endif
    return 0;
}
