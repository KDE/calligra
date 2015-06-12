#include <iostream>
#include <string>

#include <QFileInfo>
#include <QDebug>

#include <kcomponentdata.h>

#include <KDbDriverManager>
#include <KDbDriver>
#include <KDbConnection>
#include <KDbCursor>
#include <KDbParser>

using namespace std;
QByteArray prgname;

int main(int argc, char **argv)
{
    qDebug() << "main()";
    QFileInfo info = QFileInfo(argv[0]);
    prgname = info.baseName().toLatin1();
    KComponentData componentData(prgname);
    if (argc < 2) {
        return 1;
    }
    QByteArray drv_name(argv[1]);
    QByteArray db_name = QString(argv[2]).toLower().toLatin1();

    KDbDriverManager manager;
    QStringList names = manager.driverNames();
    qDebug() << "DRIVERS: ";
    for (QStringList::ConstIterator it = names.constBegin(); it != names.constEnd() ; ++it)
        qDebug() << *it;
    if (manager.result().isError()) {
        qDebug() << manager.result();
        return 1;
    }

    //get driver
    KDbDriver *driver = manager.driver(drv_name);
    if (!driver || manager.result().isError()) {
        qDebug() << manager.result();
        return 1;
    }

    //connection data that can be later reused
    KDbConnectionData conn_data;
    conn_data.setFileName(db_name);

    KDbConnection *conn = driver->createConnection(conn_data);
    if (!conn || driver->result().isError()) {
        qDebug() << "error: " << driver->result();
        return 1;
    }
    if (!conn->connect()) {
        qDebug() << "error: " << conn->errorMsg();
        return 1;
    }
    if (!conn->useDatabase(db_name)) {
        qDebug() << "error: " << conn->errorMsg();
        return 1;
    }

    KDbParser *parser = new KDbParser(conn);

    std::string cmd;
    while (cmd != "quit") {
        std::cout << "SQL> ";
        getline(std::cin, cmd);
        parser->parse(cmd.c_str());
        switch (parser->operation()) {
        case KDbParser::OP_Error:
            qDebug() << "***********************";
            qDebug() << "* error               *";
            qDebug() << "***********************";
            break;
        case KDbParser::OP_CreateTable: {
            if (parser->table()) {
                qDebug() << "Schema of table: " << parser->table()->name();
                qDebug() << *parser->table();
            }
            break;
        }
        case KDbParser::OP_Select: {
            qDebug() << "Select statement: ";
            KDbQuerySchema *q = parser->query();
            if (q) {
                qDebug() << *q;
            }
            delete q;
            break;
        }
        default:
            qDebug() << "main(): not implemented in main.cpp";
        }
        parser->clear();
    }
    return 0;
}

