//
// C++ Implementation: importwizard
//
// Description:
//
//
// Author: Adam Pigg <adam@piggz.co.uk>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "importwizard.h"
#include "keximigrate.h"
#include "pqxxmigrate.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <kcombobox.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <kdebug.h>
#include <klineedit.h>

namespace KexiMigration
{

//===========================================================
//
importWizard::importWizard(QWidget *parent, const char *name)
        : KWizard(parent, name)
{
    setupPage1();
    setupPage2();
    setupPage3();
    setupPage4();
    
    connect(srcCombo, SIGNAL(activated(const QString & string)), this, SLOT(populateSrcDBList(const QString & driverName)));
}

//===========================================================
//
importWizard::~importWizard()
{}

//===========================================================
//
void importWizard::setupPage1()
{
    page1 = new QHBox(this);
    QLabel *lblIntro = new QLabel(page1);
    lblIntro->setText("This wizard will guide you through the\n"
                      "process of converting an existing data\n"
                      "set into a kexi database");

    this->addPage(page1, "Introduction");
}

//===========================================================
//
void importWizard::setupPage2()
{
    page2 = new QHBox(this);
    QLabel *lblSource = new QLabel(page2);
    lblSource->setText("Here you can choose the location\n"
                       "to import data from");

    QVBox *p2Controls = new QVBox(page2);
    srcCombo = new KComboBox(p2Controls);
    srcCombo->insertItem("PostgreSQL Database", 0);
    srcCombo->insertItem("Text File", 1);

    srcDBList = new KListView(p2Controls);
    this->addPage(page2, "Source Data");
}

//===========================================================
//
void importWizard::setupPage3()
{
    KexiDB::DriverManager manager;

    page3 = new QHBox(this);

    QStringList names = manager.driverNames();
    QLabel *lblDest = new QLabel(page3);
    lblDest->setText("Here you can choose the location\n"
                     "to save the data in and the new\n"
                     "database name");

    QVBox *p3Controls = new QVBox(page3);

    dstConnList = new Kexi::KexiMiniConnList(p3Controls);
    dstNewDBName = new KLineEdit(p3Controls);
    
    this->addPage(page3, "Final Location");
}

//===========================================================
//
void importWizard::setupPage4()
{
    page4 = new QHBox(this);
    QLabel *lblDone = new QLabel(page4);
    lblDone->setText("Finished!\n"
                     "All required information has now\n"
                     "been gathered.  Click Finish below\n"
                     "to start the import process\n\n"
                     "NOTE:  You may be asked for extra\n"
                     "information such as field types if\n"
                     "the import module cannot automatically\n"
                     "determine this for you");

    this->addPage(page4, "Finished");
}

//===========================================================
//
void importWizard::doImport()
{
    KexiDB::ConnectionData conn_data_from;
    KexiDB::ConnectionData conn_data_to;
    QGuardedPtr<KexiDB::Connection> kexi_conn;
    KexiMigrate* import;

    //Start with a driver manager
    KexiDB::DriverManager manager;

    //get a driver to the destination database
    KexiDB::Driver *driver = manager.driver("hello");

    //Check for errors
    if (!driver || manager.error())
    {
        manager.debugError();
    }

    //Create connections to the kexi database
    kexi_conn = driver->createConnection(*(dstConnList->selectedConnection()));

    import = new pqxxMigrate(&conn_data_from, "from_db", kexi_conn, false);
    if (import->performImport())
    {
        kdDebug() << "Import Succeeded" << endl;
    }
    else
    {
        kdDebug() << "Import failed!" << endl;
    }
}

void importWizard::populateSrcDBList(const QString& driverName)
{
    kdDebug() << "Populating list of source databases for " << driverName << endl;
}
};
#include "importwizard.moc"
