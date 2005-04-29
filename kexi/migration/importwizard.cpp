/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <kde@martinellis.co.uk>

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
#include <qvbox.h>

#include <kcombobox.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <klineedit.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
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
    m_prjSet = 0;
    fileBasedDstWasPresented = false;
    //=========================Temporary===================================
    Kexi::connset().clear();
    KexiDB::ConnectionData *conndata;

#ifdef KEXI_CUSTOM_HARDCODED_CONNDATA
#include <custom_connectiondata.h>
#endif

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

    setMinimumSize(400, 400);
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
{
	delete m_prjSet;
}

//===========================================================
//
void importWizard::setupintro()
{
    QLabel *lblIntro = new QLabel(introPage);
    lblIntro->setAlignment( Qt::AlignTop | Qt::AlignLeft | Qt::WordBreak );
    lblIntro->setText(i18n("This wizard will guide you through the process of converting an existing data set into a Kexi database."));
}

//===========================================================
//
void importWizard::setupsrcType()
{
    QHBox *hb = new QHBox(srcTypePage);
    srcTypeCombo = new KComboBox(hb);
    hb->setStretchFactor(new QWidget(hb), 1);
    srcTypePage->setStretchFactor(new QWidget(srcTypePage), 1);

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

    QHBox *hb = new QHBox(dstTypePage);
    dstTypeCombo = new KComboBox(hb);
    hb->setStretchFactor(new QWidget(hb), 1);
    dstTypePage->setStretchFactor(new QWidget(dstTypePage), 1);

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
    //me: Can't connect dstconn->m_fileDlg here, it doesn't exist yet
    //connect(this, SLOT(next()), dstConn->m_fileDlg, SIGNAL(accepted()));

	connect(dstConn,SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
		this,SLOT(next()));

    dstConn->hideHelpers();
    dstConn->showSimpleConn();
    //anyway, db files will be _saved_
    dstConn->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
    dstConn->m_file->btn_advanced->hide();
    dstConn->m_file->label->hide();
    dstConn->m_file->lbl->hide();
    //dstConn->m_file->spacer7->hide();


    //js dstNewDBName = new KLineEdit(dstControls);
    //   dstNewDBName->setText(i18n("Enter new database name here"));
}

//===========================================================
//
void importWizard::setupfinish()
{
    finishPage->hide();
    QVBox *vbox = new QVBox(finishPage);
    QLabel *lblDone = new QLabel(vbox);
    lblDone->setAlignment( Qt::AlignTop | Qt::AlignLeft | Qt::WordBreak );
    lblfinishTxt = new QLabel(vbox);
    lblfinishTxt->setAlignment( Qt::AlignTop | Qt::AlignLeft | Qt::WordBreak );

    lblDone->setText(i18n(
                     "All required information has now "
                     "been gathered. Click Finish below "
                     "to start the import process\n"
                     "Note: You may be asked for extra "
                     "information such as field types if "
                     "the wizard could not automatically "
                     "determine this for you."));
    progress = new KProgress(100, vbox);
    progress->hide();
    finishPage->show();
}

//===========================================================
//
bool importWizard::checkUserInput()
{
    QString finishtxt;
    bool problem;

    problem = false;
//    if ((dstNewDBName->text() == "Enter new database name here" || dstNewDBName->text().isEmpty()))
    if (dstNewDBName->text().isEmpty())
    {
        problem = true;
        finishtxt = finishtxt + "\n" + i18n("No new database name was entered.");
    }

    if (problem)
    {
        finishtxt = i18n("Following problems were found with the data you entered:") + "\n\n"+ finishtxt;
        finishtxt = finishtxt + "\n\n" + i18n("Please click 'Back' button and correct these errors.");
    }
    else
    {
        finishtxt = i18n("No problems were found with the data you entered.");
    }
    lblfinishTxt->setText(finishtxt);
    
    return !problem;
}

void importWizard::arriveSrcConnPage()
{
  srcConnPage->hide();

  checkIfSrcTypeFileBased(srcTypeCombo->currentText());
  if (fileBasedSrc) {
    srcConn->showSimpleConn();
    /*! @todo KexiStartupFileDialog needs "open file" and "open server" modes
    in addition to just "open" */
    srcConn->m_file->label->hide();
    srcConn->m_file->btn_advanced->hide();
    srcConn->m_file->label->parentWidget()->hide();
  } else {
    srcConn->showAdvancedConn();
  }
  /*! @todo Support different file extensions based on MigrationDriver */
  /*! @todo Hide the 'Advanced' button on the connection selector here. */
  srcConnPage->show();
}

void importWizard::arriveSrcDBPage()
{
  if (fileBasedSrc) {
    //! @todo Back button doesn't work after selecting a file to import
    showPage(dstTypePage);
  }
  else {
    if (!srcdbname)
    {
      srcdbControls->hide();
      kdDebug() << "Looks like we need a project selector widget!" << endl;
      m_prjSet = new KexiProjectSet(*(srcConn->selectedConnectionData()));
      srcdbname = new KexiProjectSelectorWidget(srcdbControls,
          "KexiMigrationProjectSelector", m_prjSet);
      srcdbControls->show();
    }
  }
}

void importWizard::arriveDstTitlePage()
{
  if (!srcdbname->selectedProjectData()) {
    back(); //todo!
    return;
  }
  if(fileBasedSrc) {
		if (!srcdbname || !srcdbname->selectedProjectData()) {
			back(); //todo!
			return;
		}
    // @todo Might want to show the filename here instead
    dstNewDBName->setText(i18n("Imported Database"));
  } else {
    if (!srcdbname || !srcdbname->selectedProjectData()) {
      back(); //todo!
      return;
    }
    dstNewDBName->setText( srcdbname->selectedProjectData()->databaseName() );
  }
}

void importWizard::arriveDstPage()
{
  dstPage->hide();

  checkIfDstTypeFileBased(dstTypeCombo->currentText());
  if(fileBasedDst) {
    dstConn->showSimpleConn();
    dstConn->m_fileDlg->setMode( KexiStartupFileDialog::SavingFileBasedDB );
    if (!fileBasedDstWasPresented) {
      //without extension - it will be added automatically
      dstConn->m_fileDlg->setLocationText(dstNewDBName->text());
    }
    fileBasedDstWasPresented = true;
  } else {
    dstConn->showAdvancedConn();
  }
  dstPage->show();
}

void importWizard::arriveFinishPage() {
  checkIfDstTypeFileBased(dstTypeCombo->currentText());
  if (fileBasedDstWasPresented) {
     if (!dstConn->m_fileDlg->checkFileName()) {
       back();
       return;
     }
  }
  if (checkUserInput()) {
    setFinishEnabled(finishPage, true);
  }
  else {
    setFinishEnabled(finishPage, false);
  }
}

void importWizard::checkIfSrcTypeFileBased(const QString& srcType) {
  //! @todo Use MigrateManager to get src type property
  if ((srcType == "PostgreSQL") || (srcType == "MySQL")) {
    fileBasedSrc = false;
  } else {
    fileBasedSrc = true;
  }
}

void importWizard::checkIfDstTypeFileBased(const QString& dstType) {
  //! @todo Use DriverManager to get dst type property
  if ((dstType == "PostgreSQL") || (dstType == "MySQL")) {
    fileBasedDst = false;
  } else {
    fileBasedDst = true;
  }
}


void importWizard::progressUpdated(int percent) {
  progress->setProgress(percent);
  KApplication::kApplication()->processEvents();
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
    // Start with a driver manager
    KexiDB::DriverManager manager;
    MigrateManager mmanager;

    kdDebug() << "Creating destination driver..." << endl;
    // Get a driver to the destination database
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
        dbname = dstConn->selectedFileName();
        cdata->setFileName( dbname );
        kdDebug() << "Current file name: " << dbname << endl;
    }
    else
    {
        //TODO This needs a better message
        KMessageBox::error(this, i18n("No connection data is available. You did not select an SQLite destination filename."), i18n("Error"));
        return;
    }


    kdDebug() << "Creating connection to destination..." << endl;
    //Create connections to the kexi database
    kexi_conn = driver->createConnection(*cdata);

    kdDebug() << "Creating source driver..." << endl;
    import = mmanager.migrateDriver(srcTypeCombo->currentText());

    kdDebug() << "Setting import data.." << endl;
    if(fileBasedSrc) {
      KexiDB::ConnectionData* conn_data = new KexiDB::ConnectionData();
      conn_data->setFileName(srcConn->selectedFileName());
      import->setData(conn_data,
                      "",
                      kexi_conn,
                      dbname,
                      false);
    }
    else {
      import->setData(srcConn->selectedConnectionData(),
                      srcdbname->selectedProjectData()->databaseName(),
                      kexi_conn,
                      dbname,
                      false);
    }
    kdDebug() << "Performing import..." << endl;
    if(import->progressSupported()) {
      progress->show();
      progress->updateGeometry();
      connect(import, SIGNAL(progressPercent(int)),
              this, SLOT(progressUpdated(int)));
    }
    if (import->performImport())
    {
        KWizard::accept(); //tmp, before adding "final page"
        KMessageBox::information(this, i18n("Import Succeeded."), i18n("Success"));
    }
    else
    {
//??        KWizard::reject(); //tmp, before adding "final page"
        KMessageBox::error(this, i18n("Import failed."), i18n("Failure"));
    }
}


//===========================================================
//
void importWizard::nextClicked(const QString & p)
{
    if (currentPage() == introPage) {
    }
    else if (currentPage() == srcTypePage) {
    }
    else if (currentPage() == srcConnPage) {
      arriveSrcConnPage();
    }
    else if (currentPage() == srcdbPage) {
      arriveSrcDBPage();
    }
    else if (currentPage() == dstTypePage) {
    }
    else if (currentPage() == dstTitlePage) {
      arriveDstTitlePage();
    }
    else if (currentPage() == dstPage) {
      arriveDstPage();
    }
    else if (currentPage() == finishPage) {
      arriveFinishPage();
    }
}

void importWizard::helpClicked()
{
    if (currentPage() == introPage)
    {
        KMessageBox::information(this, i18n("No help available for this page"), i18n("Help"));
    }
    else if (currentPage() == srcTypePage)
    {
        KMessageBox::information(this, i18n("Here you can choose the type of data to import data from"), i18n("Help"));
    }
    else if (currentPage() == srcConnPage)
    {
        KMessageBox::information(this, i18n("Here you can choose the location to import data from"), i18n("Help"));
    }
    else if (currentPage() == srcdbPage)
    {
        KMessageBox::information(this, i18n("Here you can choose the actual database to import data from"), i18n("Help"));
    }
    else if (currentPage() == dstTypePage)
    {
        KMessageBox::information(this, i18n("Here you can choose the location to save the data"), i18n("Help"));
    }
    else if (currentPage() == dstPage)
    {
        KMessageBox::information(this, i18n("Here you can choose the location to save the data in and the new database name"), i18n("Help"));
    }
    else if (currentPage() == finishPage)
    {
        KMessageBox::information(this, i18n("No help available for this page"), i18n("Help"));
    }
}

#include "importwizard.moc"
