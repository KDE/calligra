#include <QDebug>
#include <QApplication>

#include <KAboutData>

#include <KDbConnection>
#include <KDbDriverManager>
#include <KDbCursor>

int main(int argc, char** argv)
{
    KAboutData aboutData("kexicrash", 0, kxi18n("kexicrash"), "0.1");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    KDbDriverManager manager;
    KDbDriver* driver = manager.driver("sqlite3");
    KDbConnectionData cd;

    QString fname("d.kexi");
    cd.setFileName(fname);

    KDbConnection *connection = driver->createConnection(cd);
    connection->connect();
    connection->useDatabase(fname);

    KDbCursor *cursor = connection->executeQuery("select * from abc", KDbCursor::Buffered);
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
