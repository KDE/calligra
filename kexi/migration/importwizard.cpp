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
#include <qlayout.h>
#include <kcombobox.h>
#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <main/startup/KexiConnSelector.h>
#include <main/startup/KexiProjectSelector.h>

#include <kdebug.h>
#include <klineedit.h>

namespace KexiMigration
{

//===========================================================
//
importWizard::importWizard(QWidget *parent, const char *name)
        : KWizard(parent, name)
{
    setMinimumSize(400, 300);
    createBlankPages();
    setupintro();
    setupsrcType();
    setupsrcconn();
    setupsrcdb();
    setupdstType();
    setupdst();
    setupfinish();
    connect(this, SIGNAL(selected(const QString &)), this, SLOT(nextClicked(const QString &)));
    connect(this, SIGNAL(accept()), this, SLOT(doImport()));
}

//===========================================================
//
importWizard::~importWizard()
{}

//===========================================================
//
void importWizard::setupintro()
{  
    QLabel *lblIntro = new QLabel(intro);
    lblIntro->setText("This wizard will guide you through the\n"
                      "process of converting an existing data\n"
                      "set into a kexi database");
}

//===========================================================
//
void importWizard::setupsrcType()
{
    QLabel *lblSource = new QLabel(srcType);
    lblSource->setText("Here you can choose the type\n"
                       "of data to import data from");

    QVBox *srcTypeControls = new QVBox(srcType);
    srcTypeCombo = new KComboBox(srcTypeControls);
    srcTypeCombo->insertItem("PostgreSQL Database", 0);
    srcTypeCombo->insertItem("Text File", 1);
}

//===========================================================
//
void importWizard::setupsrcconn()
{
    
    QLabel *lblSource = new QLabel(srcconn);
    lblSource->setText("Here you can choose the location\n"
                       "to import data from");

    QVBox *srcconnControls = new QVBox(srcconn);

    srcConn = new KexiConnSelectorWidget(Kexi::connset(), srcconnControls, "ConnSelector");
    
    srcConn->hideHelpers();
    
}

//===========================================================
//
void importWizard::setupsrcdb()
{
    
    QLabel *lblSourceDb = new QLabel(srcdb);
    lblSourceDb->setText("Here you can choose the actual\n"
                       "database to import data from");

    QVBox *srcdbControls = new QVBox(srcdb);
    
}

//===========================================================
//
void importWizard::setupdstType()
{
    KexiDB::DriverManager manager;

    QStringList names = manager.driverNames();
    QLabel *lblDest = new QLabel(dstType);
    lblDest->setText("Here you can choose the location\n"
                     "to save the data");

    QVBox *dstTypeControls = new QVBox(dstType);

    dstTypeCombo = new KComboBox(dstTypeControls);
    dstTypeCombo->insertStringList(names);
}

//===========================================================
//
void importWizard::setupdst()
{
    QLabel *lblDest = new QLabel(dst);
    lblDest->setText("Here you can choose the location\n"
                     "to save the data in and the new\n"
                     "database name");

    QVBox *dstControls = new QVBox(dst);

    dstConn = new KexiConnSelectorWidget(Kexi::connset(), dstControls, "ConnSelector");
    
    dstConn->hideHelpers();
    
    dstNewDBName = new KLineEdit(dstControls);
    dstNewDBName->setText("Enter new database name here");
}

//===========================================================
//
void importWizard::setupfinish()
{
    finish->hide();
    QLabel *lblDone = new QLabel(finish);
    lblDone->setText("Finished!\n"
                     "All required information has now\n"
                     "been gathered.  Click Finish below\n"
                     "to start the import process\n\n"
                     "NOTE:  You may be asked for extra\n"
                     "information such as field types if\n"
                     "the import module cannot automatically\n"
                     "determine this for you");
    finish->show();
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
    kexi_conn = driver->createConnection(*(dstConn->selectedConnectionData()));

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

void importWizard::nextClicked(const QString & p)
{
    kdDebug() << "Next Button Clicked " << endl;
    if (currentPage() == intro)
    {
        kdDebug() << "Current page is introduction" << endl;
    }
    else if (currentPage() == srcType)
    {    
        kdDebug() << "Current page is source type" << endl;
    }
    else if (currentPage() == srcconn)
    {    
        kdDebug() << "Current page is source connection" << endl;
    }
    else if (currentPage() == srcdb)
    {    
        kdDebug() << "Current page is source database" << endl;
    }
    else if (currentPage() == dstType)
    {    
        kdDebug() << "Current page is destination type" << endl;
    }
    else if (currentPage() == dst)
    {    
        kdDebug() << "Current page is destination" << endl;
        dst->hide();
        if (dstTypeCombo->currentText() == "PostgreSQL")
        {
            dstConn->showAdvancedConn();
        }
        dst->show();
    }
    else if (currentPage() == finish)
    {    
        kdDebug() << "Current page is finished" << endl;
        //TODO Add checks that all data is entered
        setFinishEnabled(finish, true);
    }
}

void importWizard::createBlankPages()
{
intro = new QHBox(this);
srcType = new QHBox(this);
srcconn = new QHBox(this);
srcdb = new QHBox(this);
dstType = new QHBox(this);
dst = new QHBox(this);
finish = new QHBox(this);
this->addPage(intro, "Introduction");
this->addPage(srcType, "Source Database Type");
this->addPage(srcconn, "Source Connection");
this->addPage(srcdb, "Source Database");
this->addPage(dstType, "Destination Database Type"); 
this->addPage(dst, "Destination Database");
this->addPage(finish, "Finished");
}
};
#include "importwizard.moc"
