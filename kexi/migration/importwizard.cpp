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
#include <kmessagebox.h>

#include <kdebug.h>
#include <klineedit.h>

namespace KexiMigration
{

//===========================================================
//
importWizard::importWizard(QWidget *parent, const char *name)
        : KWizard(parent, name)
{
    //=========================Temporary===================================
    KexiDB::ConnectionData *conndata;

    conndata = new KexiDB::ConnectionData();
    conndata->connName = "Local pgsql connection";
    conndata->driverName = "postgresql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    conndata->port = 5432;
    Kexi::connset().addConnectionData(conndata);

    conndata = new KexiDB::ConnectionData();
    conndata->connName = "SSH Postgresql Connection";
    conndata->driverName = "postgresql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    conndata->userName = "piggz"; //-- temporary e.g."jarek"
    conndata->password = "zggip";
    conndata->port = 3333;

    Kexi::connset().addConnectionData(conndata);

    //============================================================

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
}

//===========================================================
//
importWizard::~importWizard()
{}

//===========================================================
//
void importWizard::setupintro()
{
    QLabel *lblIntro = new QLabel(introPage);
    lblIntro->setText("This wizard will guide you through the\n"
                      "process of converting an existing data\n"
                      "set into a kexi database");
}

//===========================================================
//
void importWizard::setupsrcType()
{
    QLabel *lblSource = new QLabel(srcTypePage);
    lblSource->setText("Here you can choose the type\n"
                       "of data to import data from");

    QVBox *srcTypeControls = new QVBox(srcTypePage);
    srcTypeCombo = new KComboBox(srcTypeControls);
    srcTypeCombo->insertItem("PostgreSQL Database", 0);
    srcTypeCombo->insertItem("Text File", 1);
}

//===========================================================
//
void importWizard::setupsrcconn()
{

    QLabel *lblSource = new QLabel(srcConnPage);
    lblSource->setText("Here you can choose the location\n"
                       "to import data from");

    QVBox *srcconnControls = new QVBox(srcConnPage);

    srcConn = new KexiConnSelectorWidget(Kexi::connset(), srcconnControls, "ConnSelector");

    srcConn->hideHelpers();

}

//===========================================================
//
void importWizard::setupsrcdb()
{

    QLabel *lblSourceDb = new QLabel(srcdbPage);
    lblSourceDb->setText("Here you can choose the actual\n"
                         "database to import data from");

    srcdbControls = new QVBox(srcdbPage);

    srcdbname = NULL;
}

//===========================================================
//
void importWizard::setupdstType()
{
    KexiDB::DriverManager manager;

    QStringList names = manager.driverNames();
    QLabel *lblDest = new QLabel(dstTypePage);
    lblDest->setText("Here you can choose the location\n"
                     "to save the data");

    QVBox *dstTypeControls = new QVBox(dstTypePage);

    dstTypeCombo = new KComboBox(dstTypeControls);
    dstTypeCombo->insertStringList(names);
}

//===========================================================
//
void importWizard::setupdst()
{
    QLabel *lblDest = new QLabel(dstPage);
    lblDest->setText("Here you can choose the location\n"
                     "to save the data in and the new\n"
                     "database name");

    QVBox *dstControls = new QVBox(dstPage);

    dstConn = new KexiConnSelectorWidget(Kexi::connset(), dstControls, "ConnSelector");

    dstConn->hideHelpers();

    dstNewDBName = new KLineEdit(dstControls);
    dstNewDBName->setText("Enter new database name here");
}

//===========================================================
//
void importWizard::setupfinish()
{
    finishPage->hide();
    QLabel *lblDone = new QLabel(finishPage);
    lblfinishTxt = new QLabel(finishPage);

    lblDone->setText("Finished!\n"
                     "All required information has now\n"
                     "been gathered.  Click Finish below\n"
                     "to start the import process\n\n"
                     "NOTE:  You may be asked for extra\n"
                     "information such as field types if\n"
                     "the import module cannot automatically\n"
                     "determine this for you");
    finishPage->show();
}

//===========================================================
//
bool importWizard::checkUserInput()
{
    QString finishtxt;
    bool problem;

    problem = false;
    
    if (srcTypeCombo->currentText() != "PostgreSQL Database")
    {
        problem = true;
        finishtxt = "Source type was not PostgreSQL Database";
    }
    if (dstNewDBName->text() == "Enter new database name here" || dstNewDBName->text() == "")
    {
        problem = true;
        finishtxt = finishtxt + "\nNo new database name was entered";
    }

    if (problem)
    {
        finishtxt = "I found the following problems with the data you entered:\n\n" + finishtxt;
        finishtxt = finishtxt + "\n\nPlease go back and correct these errors";
    }
    else
    {
        finishtxt = "I did not find any problems with the data you entered";
    }
    lblfinishTxt->setText(finishtxt);
    
    return !problem;
}

//===========================================================
//
void importWizard::accept()
{
    QGuardedPtr<KexiDB::Connection> kexi_conn;
    KexiMigrate* import;

    //Start with a driver manager
    KexiDB::DriverManager manager;

    //get a driver to the destination database
    KexiDB::Driver *driver = manager.driver("Postgresql");

    //Check for errors
    if (!driver || manager.error())
    {
        manager.debugError();
    }

    //Create connections to the kexi database
    kexi_conn = driver->createConnection(*(dstConn->selectedConnectionData()));

    KMessageBox::information(this, "Creating pqxxMigrate Object...", "Busy...");
    import = new pqxxMigrate(srcConn->selectedConnectionData(), srcdbname->selectedProjectData()->databaseName(), kexi_conn, false);
    KMessageBox::information(this, "Performing Import...", "Busy...");
    if (import->performImport())
    {
        KMessageBox::error(this, "", "Import suceeded!");
    }
    else
    {
        KMessageBox::error(this, "", "Import failed!");
    }
}

//===========================================================
//
void importWizard::nextClicked(const QString & p)
{
    if (currentPage() == introPage)
    {
        kdDebug() << "Current page is introduction" << endl;
    }
    else if (currentPage() == srcTypePage)
    {
        kdDebug() << "Current page is source type" << endl;
    }
    else if (currentPage() == srcConnPage)
    {
        kdDebug() << "Current page is source connection" << endl;
        srcConnPage->hide();
        if (srcTypeCombo->currentText() == "PostgreSQL Database")
        {
            srcConn->showAdvancedConn();
        }
/*        else
        {
            KMessageBox::information(this, "Sorry, only data migration form postgresql is possible at the moment.  Please go back and change the option, or press cancel to quit", "Sorry, featuree not available");
        }
*/
        srcConnPage->show();
    }
    else if (currentPage() == srcdbPage)
    {
        kdDebug() << "Current page is source database" << endl;
        if (srcTypeCombo->currentText() == "PostgreSQL Database")
        {
            if (!srcdbname)
            {
                srcdbControls->hide();
                kdDebug() << "Looks like we need a project selector widget!" << endl;
                KexiProjectSet *prj_set = new KexiProjectSet( *srcConn->selectedConnectionData() );
                srcdbname = new KexiProjectSelectorWidget(srcdbControls, "KexiMigrationProjectSelector", prj_set);
                srcdbControls->show();
            }
        }
    }
    else if (currentPage() == dstTypePage)
    {
        kdDebug() << "Current page is destination type" << endl;
    }
    else if (currentPage() == dstPage)
    {
        kdDebug() << "Current page is destination" << endl;
        dstPage->hide();
        if (dstTypeCombo->currentText() == "PostgreSQL")
        {
            dstConn->showAdvancedConn();
        }
        dstPage->show();
    }
    else if (currentPage() == finishPage)
    {
        kdDebug() << "Current page is finished" << endl;
        if (checkUserInput())
        {
            setFinishEnabled(finishPage, true);
        }
        else
        {
            setFinishEnabled(finishPage, false);
        }
    }
}

void importWizard::createBlankPages()
{
    introPage = new QHBox(this);
    srcTypePage = new QHBox(this);
    srcConnPage = new QHBox(this);
    srcdbPage = new QHBox(this);
    dstTypePage = new QHBox(this);
    dstPage = new QHBox(this);
    finishPage = new QHBox(this);
    this->addPage(introPage, "Introduction");
    this->addPage(srcTypePage, "Source Database Type");
    this->addPage(srcConnPage, "Source Connection");
    this->addPage(srcdbPage, "Source Database");
    this->addPage(dstTypePage, "Destination Database Type");
    this->addPage(dstPage, "Destination Database");
    this->addPage(finishPage, "Finished");
}
};
#include "importwizard.moc"
