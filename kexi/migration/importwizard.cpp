/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "importwizard.h"
#include "keximigrate.h"
#include "migratemanager.h"

#include <qhbox.h>
#include <qlabel.h>
#include <qvbox.h>
#include <qlayout.h>

#include <kcombobox.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <klineedit.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <KexiConnSelector.h>
#include <KexiProjectSelector.h>
#include <KexiOpenExistingFile.h>
#include <KexiDBTitlePage.h>

using namespace KexiMigration;

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

    conndata = new KexiDB::ConnectionData();
    conndata->connName = "Local MySQL connection";
    conndata->driverName = "mysql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    Kexi::connset().addConnectionData(conndata);
    
    //============================================================

    setMinimumSize(400, 300);
    introPage = new QVBox(this);
    setupintro();
    this->addPage(introPage, i18n("Introduction"));
    srcTypePage = new QVBox(this);
    setupsrcType();
    this->addPage(srcTypePage, i18n("Select Source Database Type"));
    srcConnPage = new QVBox(this);
    setupsrcconn();
    this->addPage(srcConnPage, i18n("Select Source Connection"));
    srcdbPage = new QVBox(this);
    setupsrcdb();
    this->addPage(srcdbPage, i18n("Select Source Database"));
    dstTypePage = new QVBox(this);
    setupdstType();
    this->addPage(dstTypePage, i18n("Select Destination Database Type"));
    setupdstTitle();
    this->addPage(dstTitlePage, i18n("Select Destination Project's Caption"));
    dstPage = new QVBox(this);
    setupdst();
    this->addPage(dstPage, i18n("Select Destination Database"));
    finishPage = new QHBox(this);
    setupfinish();
    this->addPage(finishPage, i18n("Finished"));

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
    
    MigrateManager manager;

    QStringList names = manager.migrateDriverNames();

    srcTypeCombo->insertStringList(names);
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
void importWizard::setupdstTitle()
{
	dstTitlePage = new KexiDBTitlePage(this, "KexiDBTitlePage");
	dstTitlePage->label->setText(i18n("Destination project's caption:"));
	dstNewDBName = dstTitlePage->le_caption;
}

//===========================================================
//
void importWizard::setupdst()
{
    QVBox *dstControls = new QVBox(dstPage);

    dstConn = new KexiConnSelectorWidget(Kexi::connset(), dstControls, "DstConnSelector");
	connect(dstConn->m_fileDlg,SIGNAL(accepted()),this,SLOT(next()));
	dstConn->showAdvancedConn();
	connect(dstConn,SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
		this,SLOT(next()));

    dstConn->hideHelpers();
		dstConn->showSimpleConn();
	//anyway, db files will be _saved_
		dstConn->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
    dstConn->m_file->btn_advanced->hide();
    dstConn->m_file->label->hide();
    dstConn->m_file->lbl->hide();
//    dstConn->m_file->spacer7->hide();
    

//js    dstNewDBName = new KLineEdit(dstControls);
//    dstNewDBName->setText(i18n("Enter new database name here"));
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
    
    if ((srcTypeCombo->currentText() != "PostgreSQL") & srcTypeCombo->currentText() != "MySQL")
    {
        problem = true;
        finishtxt = i18n("Source type was not PostgreSQL Database.");
    }
//    if ((dstNewDBName->text() == "Enter new database name here" || dstNewDBName->text().isEmpty()))
    if (dstNewDBName->text().isEmpty())
    {
        problem = true;
        finishtxt = finishtxt + i18n("\nNo new database name was entered.");
    }

    if (problem)
    {
        finishtxt = i18n("The following problems were found with the data you entered:") + "\n\n"+ finishtxt;
        finishtxt = finishtxt + "\n\n" + i18n("Please go back and correct these errors.");
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
    
    kdDebug() << "Creating managers..." << endl;
    //Start with a driver manager
    KexiDB::DriverManager manager;
    MigrateManager mmanager;
    
    kdDebug() << "Creating destination driver..." << endl;
    //get a driver to the destination database
    KexiDB::Driver *driver = manager.driver(dstTypeCombo->currentText());
    
    
    //Check for errors
    if (!driver || manager.error())
    {
      	kdDebug() << "Manager error..." << endl;
        manager.debugError();
    }
    
    if (dstConn->selectedConnectionData())
    {
        //server-based project
      	kdDebug() << "Server destination..." << endl;
        cdata = dstConn->selectedConnectionData();
        dbname = dstNewDBName->text();
        
    }
    else if (dstTypeCombo->currentText().lower() == KexiDB::Driver::defaultFileBasedDriverName()) 
    {
        //file-based project
      	kdDebug() << "File Destination..." << endl;
        cdata = new KexiDB::ConnectionData;
        cdata->connName = dstNewDBName->text();
        cdata->driverName = KexiDB::Driver::defaultFileBasedDriverName();
        cdata->setFileName( dstConn->selectedFileName() );
	
	kdDebug() << "Current file name: " << dstConn->selectedFileName() << endl;
	
	dbname = dstConn->selectedFileName();
    }
    else
    {
        //TODO This needs a better message
        KMessageBox::error(this, "No connection data was available and you didnt select an SQLite destination", "Error");
        return;
    }

    
    kdDebug() << "Creating connection to destination..." << endl;
    //Create connections to the kexi database
    kexi_conn = driver->createConnection(*cdata);

    kdDebug() << "Creating source driver..." << endl;
    import = mmanager.migrateDriver(srcTypeCombo->currentText());
    
    kdDebug() << "Setting import data.." << endl;
    import->setData(srcConn->selectedConnectionData(), srcdbname->selectedProjectData()->databaseName(), kexi_conn, dbname, false);

    kdDebug() << "Performing import..." << endl;
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
        if (srcTypeCombo->currentText() == "PostgreSQL")
        {
            srcConn->showAdvancedConn();
        }
        else if (srcTypeCombo->currentText() == "MySQL") {
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
        if ((srcTypeCombo->currentText() == "PostgreSQL") || (srcTypeCombo->currentText() == "MySQL"))
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
    else if (currentPage() == dstTitlePage) {
         dstNewDBName->setText( srcdbname->selectedProjectData()->databaseName() );
    }
    else if (currentPage() == dstPage)
    {
        dstPage->hide();
        if (dstTypeCombo->currentText() == "PostgreSQL" || dstTypeCombo->currentText() == "MySQL")
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

#include "importwizard.moc"
