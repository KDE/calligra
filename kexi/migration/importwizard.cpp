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
#include <main/startup/KexiOpenExistingFile.h>
#include <kmessagebox.h>

#include <kpushbutton.h>
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
    connect(this, SIGNAL(helpClicked()), this, SLOT(helpClicked()));
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
    lblIntro->setText(i18n("This wizard will guide you through the\n"
                      "process of converting an existing data\n"
                      "set into a kexi database"));
}

//===========================================================
//
void importWizard::setupsrcType()
{
    QVBox *srcTypeControls = new QVBox(srcTypePage);
    srcTypeCombo = new KComboBox(srcTypeControls);
    srcTypeCombo->insertItem("PostgreSQL Database", 0);
    srcTypeCombo->insertItem("Text File", 1);
}

//===========================================================
//
void importWizard::setupsrcconn()
{
   QVBox *srcconnControls = new QVBox(srcConnPage);

    srcConn = new KexiConnSelectorWidget(Kexi::connset(), srcconnControls, "SrcConnSelector");

    srcConn->hideHelpers();

}

//===========================================================
//
void importWizard::setupsrcdb()
{
    srcdbControls = new QVBox(srcdbPage);

    srcdbname = NULL;
}

//===========================================================
//
void importWizard::setupdstType()
{
    KexiDB::DriverManager manager;

    QStringList names = manager.driverNames();

    QVBox *dstTypeControls = new QVBox(dstTypePage);

    dstTypeCombo = new KComboBox(dstTypeControls);
    dstTypeCombo->insertStringList(names);
}

//===========================================================
//
void importWizard::setupdst()
{
    QVBox *dstControls = new QVBox(dstPage);

    dstConn = new KexiConnSelectorWidget(Kexi::connset(), dstControls, "DstConnSelector");

    dstConn->hideHelpers();
    //dstConn->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
    dstConn->m_file->btn_advanced->hide();
    dstConn->m_file->label->hide();
    dstConn->m_file->lbl->hide();
//    dstConn->m_file->spacer7->hide();
    

    dstNewDBName = new KLineEdit(dstControls);
    dstNewDBName->setText(i18n("Enter new database name here"));
}

//===========================================================
//
void importWizard::setupfinish()
{
    finishPage->hide();
    QLabel *lblDone = new QLabel(finishPage);
    lblfinishTxt = new QLabel(finishPage);

    lblDone->setText(i18n("Finished!\n"
                     "All required information has now\n"
                     "been gathered.  Click Finish below\n"
                     "to start the import process\n\n"
                     "NOTE:  You may be asked for extra\n"
                     "information such as field types if\n"
                     "the import module cannot automatically\n"
                     "determine this for you"));
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
        finishtxt = i18n("Source type was not PostgreSQL Database.");
    }
    if ((dstNewDBName->text() == "Enter new database name here" || dstNewDBName->text() == "") 
		&& dstTypeCombo->currentText().lower() == KexiDB::Driver::defaultFileBasedDriverName())
    {
        problem = true;
        finishtxt = finishtxt + i18n("\nNo new database name was entered.");
    }

    if (problem)
    {
        finishtxt = i18n("The following problems were found with the data you entered:\n\n") + finishtxt;
        finishtxt = finishtxt + i18n("\n\nPlease go back and correct these errors.");
    }
    else
    {
        finishtxt = i18n("No problems were found with the data you entered.");
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
    KexiDB::ConnectionData *cdata;
    QString dbname;
    
    //Start with a driver manager
    KexiDB::DriverManager manager;

    //get a driver to the destination database
    KexiDB::Driver *driver = manager.driver(dstTypeCombo->currentText());

    //Check for errors
    if (!driver || manager.error())
    {
        manager.debugError();
    }
    
    if (!dstConn->selectedConnectionData() == 0)
    {
        //server-based project
        cdata = dstConn->selectedConnectionData();
        dbname = dstNewDBName->text();
        
    }
    else if (dstTypeCombo->currentText().lower() == KexiDB::Driver::defaultFileBasedDriverName()) 
    {
        //file-based project
        cdata = new KexiDB::ConnectionData;
        cdata->connName = dstNewDBName->text();
        cdata->driverName = KexiDB::Driver::defaultFileBasedDriverName();
        cdata->setFileName( dstConn->m_fileDlg->currentFileName() );
        dbname = dstConn->m_fileDlg->currentFileName();
    }
    else
    {
        //TODO This needs a better message
        KMessageBox::error(this, "No connection data was available and you didnt select an SQLite destination", "Error");
        return;
    }

    
    //Create connections to the kexi database
    kexi_conn = driver->createConnection(*cdata);

    //import = new pqxxMigrate(srcConn->selectedConnectionData(), srcdbname->selectedProjectData()->databaseName(), kexi_conn, dbname, false);
    
    if (import->performImport())
    {
        KMessageBox::information(this, i18n("Import Succeeded."), i18n("Success"));
    }
    else
    {
        KMessageBox::error(this, i18n("Import failed because: "), i18n("Failure"));
    }
}

//===========================================================
//
void importWizard::nextClicked(const QString & p)
{
    if (currentPage() == introPage)
    {
    }
    else if (currentPage() == srcTypePage)
    {
    }
    else if (currentPage() == srcConnPage)
    {
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
    }
    else if (currentPage() == dstPage)
    {
        dstPage->hide();
        if (dstTypeCombo->currentText() == "PostgreSQL")
        {
            dstConn->showAdvancedConn();
        }
        else
        {
            dstConn->showSimpleConn();
            dstConn->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
        }
        dstPage->show();
    }
    else if (currentPage() == finishPage)
    {
        KMessageBox::information(this, dstConn->m_fileDlg->currentFileName(), "Current File Name");
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
    introPage = new QVBox(this);
    srcTypePage = new QVBox(this);
    srcConnPage = new QVBox(this);
    srcdbPage = new QVBox(this);
    dstTypePage = new QVBox(this);
    dstPage = new QVBox(this);
    finishPage = new QHBox(this);
    this->addPage(introPage, i18n("Introduction"));
    this->addPage(srcTypePage, i18n("Source Database Type"));
    this->addPage(srcConnPage, i18n("Source Connection"));
    this->addPage(srcdbPage, i18n("Source Database"));
    this->addPage(dstTypePage, i18n("Destination Database Type"));
    this->addPage(dstPage, i18n("Destination Database"));
    this->addPage(finishPage, i18n("Finished"));
}

void importWizard::helpClicked()
{
    if (currentPage() == introPage)
    {
        KMessageBox::information(this, "No help availbale for this page", "Help");
    }
    else if (currentPage() == srcTypePage)
    {
        KMessageBox::information(this, "Here you can choose the type of data to import data from", "Help");
    }
    else if (currentPage() == srcConnPage)
    {
        KMessageBox::information(this, "Here you can choose the location to import data from", "Help");
    }
    else if (currentPage() == srcdbPage)
    {
        KMessageBox::information(this, "Here you can choose the actual database to import data from", "Help");
    }
    else if (currentPage() == dstTypePage)
    {
        KMessageBox::information(this, "Here you can choose the location to save the data", "Help");
    }
    else if (currentPage() == dstPage)
    {
        KMessageBox::information(this, "Here you can choose the location to save the data in and the new database name", "Help");
    }
    else if (currentPage() == finishPage)
    {
        KMessageBox::information(this, "No help availbale for this page", "Help");
    }
}
};
#include "importwizard.moc"
