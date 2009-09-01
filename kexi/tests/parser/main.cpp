#include <iostream>
#include <string>

#include <qfileinfo.h>

#include <kdebug.h>
#include <kcomponentdata.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connection.h>
#include <kexidb/cursor.h>
#include <kexidb/parser/parser.h>

using namespace std;
QByteArray prgname;

int main(int argc, char **argv)
{
    kDebug() << "main()";
    QFileInfo info = QFileInfo(argv[0]);
    prgname = info.baseName().toLatin1();
    KComponentData componentData(prgname);
    if (argc < 2) {
        return 1;
    }
    QByteArray drv_name(argv[1]);
    QByteArray db_name = QString(argv[2]).toLower().toLatin1();

    KexiDB::DriverManager manager; // = KexiDB::DriverManager::self();
    QStringList names = manager.driverNames();
    kDebug() << "DRIVERS: ";
    for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
        kDebug() << *it;
    if (manager.error()) {
        kDebug() << manager.errorMsg();
        return 1;
    }

    //get driver
    KexiDB::Driver *driver = manager.driver(drv_name);
    if (!driver || manager.error()) {
        kDebug() << manager.errorMsg();
        return 1;
    }

    //connection data that can be later reused
    KexiDB::ConnectionData conn_data;
    conn_data.setFileName(db_name);

    KexiDB::Connection *conn = driver->createConnection(conn_data);
    if (!conn || driver->error()) {
        kDebug() << "error: " << driver->errorMsg();
        return 1;
    }
    if (!conn->connect()) {
        kDebug() << "error: " << conn->errorMsg();
        return 1;
    }
    if (!conn->useDatabase(db_name)) {
        kDebug() << "error: " << conn->errorMsg();
        return 1;
    }

    KexiDB::Parser *parser = new KexiDB::Parser(conn);

    std::string cmd;
    while (cmd != "quit") {
        std::cout << "SQL> ";
        getline(std::cin, cmd);
        parser->parse(cmd.c_str());
        switch (parser->operation()) {
        case KexiDB::Parser::OP_Error:
            kDebug() << "***********************";
            kDebug() << "* error               *";
            kDebug() << "***********************";
            break;
        case KexiDB::Parser::OP_CreateTable: {
            kDebug() << "Schema of table: " << parser->table()->name();
            parser->table()->debug();
            break;
        }
        case KexiDB::Parser::OP_Select: {
            kDebug() << "Select statement: ";
            KexiDB::QuerySchema *q = parser->query();
            q->debug();
            delete q;
            break;
        }
        default:
            kDebug() << "main(): not implemented in main.cpp";


        }
        parser->clear();
    }
    return 0;
}

