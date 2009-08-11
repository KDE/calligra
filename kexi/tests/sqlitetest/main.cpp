#include <kdebug.h>
#include <KAboutData>
#include <KApplication>
#include <KCmdLineArgs>

#include <kexidb/connection.h>
#include <kexidb/drivermanager.h>
#include <kexidb/cursor.h>

int main(int argc, char** argv)
{
    KAboutData aboutData("kexicrash", 0, ki18n("kexicrash"), "0.1");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    KexiDB::DriverManager manager;
    KexiDB::Driver* driver = manager.driver("sqlite3");
    KexiDB::ConnectionData cd;

    QString fname("d.kexi");
    cd.setFileName(fname);

    KexiDB::Connection *connection = driver->createConnection(cd);
    connection->connect();
    connection->useDatabase(fname);

    KexiDB::Cursor *cursor = connection->executeQuery("select * from abc", KexiDB::Cursor::Buffered);
    //cursor->moveFirst();
    if (cursor) {
        cursor->moveNext();
        cursor->moveNext();
        cursor->moveNext();

        cursor->movePrev();
        cursor->moveNext(); // <---- Crashes here
    }
    return 0;
}
