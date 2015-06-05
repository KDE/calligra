#include <migration/migratemanager.h>
#include <migration/keximigrate.h>
#include <kaboutdata.h>
#include <kapplication.h>
#include <kcmdlineargs.h>

#include <QDebug>
#include <KAboutData>

int main(int argc, char *argv[])
{
    KAboutData aboutData("keximigratetest", 0, kxi18n("Kexi Migrate Test"), "2.0");
    KCmdLineArgs::init(argc, argv, &aboutData);
    KApplication app;

    KexiMigration::MigrateManager mm;

    qDebug() << mm.driverNames();

    //Text File Test
    KexiMigration::KexiMigrate *m = mm.driver("Text");

    KexiDB::ConnectionData cd;
    
    cd.setFileName("/home/piggz/tabdata.txt");
    
    KexiMigration::Data d;
    d.source = &cd;
    
    m->setData(&d);
    
    m->connectSource();
    
    KexiDB::TableSchema ts;
    
    if (!m->readTableSchema("tabdata.txt", ts))
    {
      qDebug() << "Unable to read schema";
      return 0;
    }
    
    if (!m->readFromTable("tabdata.txt"))
    {
      qDebug() << "Unable to read from table";
      return 0;
    }
    
    while(m->moveNext())
    {
        qDebug() << m->value(0) << m->value(1) << m->value(2);
    }
    
    m->movePrevious();
    qDebug() << m->value(0) << m->value(1) << m->value(2);
    
    m->moveNext();
    qDebug() << m->value(0) << m->value(1) << m->value(2);
    
    m->movePrevious();
    qDebug() << m->value(0) << m->value(1) << m->value(2);
    
    m->movePrevious();
    qDebug() << m->value(0) << m->value(1) << m->value(2);
    
    m->movePrevious();
    qDebug() << m->value(0) << m->value(1) << m->value(2);
    
    m->moveNext();
    qDebug() << m->value(0) << m->value(1) << m->value(2);
    
    
    //KSpread file test
  
    KexiMigration::KexiMigrate *k = mm.driver("KSpread");
    cd.setFileName("/home/piggz/Documents/database.fods");
    k->setData(&d);
    
    k->connectSource();
    QStringList tn;
    k->tableNames(tn);
    
    qDebug() << tn;
    KexiDB::TableSchema ts2;
    if (!k->readTableSchema("Names", ts2))
    {
      qDebug() << "Unable to read schema";
      return 0;
    }
    
    k->readFromTable("Names");
    
    while(k->moveNext())
    {
        qDebug() << k->value(0) << k->value(1) << k->value(2);
    }
}
