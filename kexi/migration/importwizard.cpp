/* This file is part of the KDE project
   Copyright (C) 2004 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005 Martin Ellis <martin.ellis@kdemail.net>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "importwizard.h"
#include "keximigrate.h"
#include "migratemanager.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qvbuttongroup.h>
#include <qradiobutton.h>

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
#include <kexiutils/utils.h>
#include <kexidbdrivercombobox.h>
#include <kexiguimsghandler.h>

using namespace KexiMigration;

//===========================================================
//
ImportWizard::ImportWizard(QWidget *parent, QVariant* result)
 : KWizard(parent)
 , m_result(result)
{
    setCaption(i18n("Database Importing"));
    finishButton()->setText(i18n("&Import"));
    if (m_result)
        *m_result = QVariant();
    m_prjSet = 0;
    fileBasedDstWasPresented = false;
    setupFileBasedSrcNeeded = true;
#if 0
    //=========================Temporary===================================
    Kexi::connset().clear();
    KexiDB::ConnectionData *conndata;
#ifdef KEXI_CUSTOM_HARDCODED_CONNDATA
#include <custom_connectiondata.h>
#endif

    conndata = new KexiDB::ConnectionData();
    conndata->caption = "Local pgsql connection";
    conndata->driverName = "postgresql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    conndata->port = 5432;
    Kexi::connset().addConnectionData(conndata);

    conndata = new KexiDB::ConnectionData();
    conndata->caption = "SSH Postgresql Connection";
    conndata->driverName = "postgresql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    conndata->userName = "piggz"; //-- temporary e.g."jarek"
    conndata->password = "zggip";
    conndata->port = 3333;

    Kexi::connset().addConnectionData(conndata);

    conndata = new KexiDB::ConnectionData();
    conndata->caption = "Local MySQL connection";
    conndata->driverName = "mysql";
    conndata->hostName = "localhost"; // -- default //"host.net";
    Kexi::connset().addConnectionData(conndata);
#endif

    //============================================================

    setMinimumSize(400, 400);
    introPage = new QWidget(this);
    setupintro();
    this->addPage(introPage, i18n("Introduction"));
    srcTypePage = new QWidget(this);
    setupsrcType();
    this->addPage(srcTypePage, i18n("Select Source Database Type"));
    srcConnPage = new QWidget(this);
    setupsrcconn();
    this->addPage(srcConnPage, i18n("Select Source Database Connection"));
    srcdbPage = new QWidget(this);
    setupsrcdb();
    this->addPage(srcdbPage, i18n("Select Source Database"));
    dstTypePage = new QWidget(this);
    setupdstType();
    this->addPage(dstTypePage, i18n("Select Destination Database Type"));
    setupdstTitle();
    this->addPage(dstTitlePage, i18n("Select Destination Project's Caption"));
    dstPage = new QWidget(this);
    setupdst();
    this->addPage(dstPage, i18n("Select Destination Database"));
    importTypePage = new QWidget(this);
    setupImportType();
    this->addPage(importTypePage, i18n("Select Type Of Import"));
    finishPage = new QWidget(this);
    setupfinish();
    this->addPage(finishPage, i18n("Finished"));

    connect(this, SIGNAL(selected(const QString &)), this, SLOT(pageSelected(const QString &)));
    connect(this, SIGNAL(helpClicked()), this, SLOT(helpClicked()));
}

//===========================================================
//
ImportWizard::~ImportWizard()
{
	delete m_prjSet;
}

//===========================================================
//
void ImportWizard::setupintro()
{
    QVBoxLayout *vbox = new QVBoxLayout(introPage, KDialog::marginHint());
    
    QLabel *lblIntro = new QLabel(introPage);
    lblIntro->setAlignment( Qt::AlignTop | Qt::AlignLeft | Qt::WordBreak );
    lblIntro->setText(i18n("This wizard will guide you through the process of converting an existing data set into a Kexi database."));
    vbox->addWidget( lblIntro );
}

//===========================================================
//
void ImportWizard::setupsrcType()
{
/*! @todo Would be good if KexiDBDriverComboBox worked for migration drivers */
    QVBoxLayout *vbox = new QVBoxLayout(srcTypePage, KDialog::marginHint());
//    vbox->addStretch(1);

    QHBoxLayout *hbox = new QHBoxLayout(vbox);
    hbox->addWidget(new QLabel(i18n("Source database type:")+" ", srcTypePage));

    srcTypeCombo = new KComboBox(srcTypePage);
    hbox->addWidget(srcTypeCombo);
    hbox->addStretch(1);
    vbox->addStretch(1);

    MigrateManager manager;

    QStringList names = manager.migrateDriverNames();

    srcTypeCombo->insertStringList(names);
}

//===========================================================
//
void ImportWizard::setupsrcconn()
{
   QVBoxLayout *vbox = new QVBoxLayout(srcConnPage, KDialog::marginHint());

   srcConn = new KexiConnSelectorWidget(Kexi::connset(), srcConnPage, "SrcConnSelector");

   srcConn->hideHelpers();
   vbox->addWidget(srcConn);
}

//===========================================================
//
void ImportWizard::setupsrcdb()
{
//    QVBoxLayout *vbox = new QVBoxLayout(srcdbPage, KDialog::marginHint());
//    Q_UNUSED(vbox);  // arriveSrcDBPage creates widgets on that page
    srcdbname = NULL;
}

//===========================================================
//
void ImportWizard::setupdstType()
{
    KexiDB::DriverManager manager;
    KexiDB::Driver::InfoMap drvs = manager.driversInfo();

    QVBoxLayout *vbox = new QVBoxLayout(dstTypePage, KDialog::marginHint());
//    vbox->addStretch(1);

    QHBoxLayout *hbox = new QHBoxLayout(vbox);
    hbox->addWidget(new QLabel(i18n("Destination database type:")+" ", dstTypePage));
    dstTypeCombo = new KexiDBDriverComboBox(drvs, true, dstTypePage);

    hbox->addWidget( dstTypeCombo );
    hbox->addStretch(1);
    vbox->addStretch(1);

//! @todo hardcoded: find a way to preselect default engine item
    dstTypeCombo->setCurrentText("SQLite3");
}

//===========================================================
//
void ImportWizard::setupdstTitle()
{
	dstTitlePage = new KexiDBTitlePage(this, "KexiDBTitlePage");
	dstTitlePage->label->setText(i18n("Destination project's caption:"));
	dstTitlePage->layout()->setMargin( KDialog::marginHint() );
	dstTitlePage->updateGeometry();
	dstNewDBName = dstTitlePage->le_caption;
}

//===========================================================
//
void ImportWizard::setupdst()
{
    QVBoxLayout *vbox = new QVBoxLayout(dstPage, KDialog::marginHint());

    dstConn = new KexiConnSelectorWidget(Kexi::connset(), dstPage, "DstConnSelector");
    //me: Can't connect dstconn->m_fileDlg here, it doesn't exist yet
    //connect(this, SLOT(next()), dstConn->m_fileDlg, SIGNAL(accepted()));

    vbox->addWidget( dstConn );
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
void ImportWizard::setupImportType()
{
    QVBoxLayout *vbox = new QVBoxLayout(importTypePage, KDialog::marginHint());
    importTypeButtonGroup = new QVButtonGroup(importTypePage);
    importTypeButtonGroup->setLineWidth(0);
    vbox->addWidget( importTypeButtonGroup );

    (void)new QRadioButton(i18n("Structure and data"), importTypeButtonGroup);
    (void)new QRadioButton(i18n("Structure only"), importTypeButtonGroup);

    importTypeButtonGroup->setExclusive( true );
    importTypeButtonGroup->setButton( 0 );
}
//===========================================================
//
void ImportWizard::setupfinish()
{
    finishPage->hide();
    QVBoxLayout *hbox = new QVBoxLayout(finishPage, KDialog::marginHint());
    QLabel *lblDone = new QLabel(finishPage);
    
    
    lblDone->setAlignment( Qt::AlignTop | Qt::AlignLeft | Qt::WordBreak );
    lblfinishTxt = new QLabel(finishPage);
    
    
    lblfinishTxt->setAlignment( Qt::AlignTop | Qt::AlignLeft | Qt::WordBreak );

    lblDone->setText(i18n(
                     "All required information has now "
                     "been gathered. Click \"Import\" button to start importing."
                     /*"Note: You may be asked for extra "
                     "information such as field types if "
                     "the wizard could not automatically "
                     "determine this for you."*/));
    progress = new KProgress(100, finishPage);
    progress->hide();

    hbox->addWidget( lblDone );
    hbox->addWidget( lblfinishTxt );
    hbox->addWidget( progress );

    finishPage->show();

//! @todo add "Open imported project" checkbox
}

//===========================================================
//
bool ImportWizard::checkUserInput()
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
        finishtxt = i18n("Following problems were found with the data you entered:") +
                    "\n\n" +
                    finishtxt +
                    "\n\n" +
                    i18n("Please click 'Back' button and correct these errors.");
    }
//    else
//    {
//it was weird        finishtxt = i18n("No problems were found with the data you entered.");
//    }
    lblfinishTxt->setText(finishtxt);

    return !problem;
}

void ImportWizard::arriveSrcConnPage()
{
  srcConnPage->hide();

  checkIfSrcTypeFileBased(srcTypeCombo->currentText());
  if (fileBasedSrc) {
    srcConn->showSimpleConn();
    /*! @todo KexiStartupFileDialog needs "open file" and "open server" modes
    in addition to just "open" */
    if (setupFileBasedSrcNeeded) {
        setupFileBasedSrcNeeded = false;
        QStringList additionalMimeTypes;
        if (srcTypeCombo->currentText().contains("Access")) {
//! @todo tmp: hardcoded!
            additionalMimeTypes << "application/x-msaccess";
        }
        srcConn->m_fileDlg->setMode(KexiStartupFileDialog::Opening, additionalMimeTypes);
        if (srcTypeCombo->currentText().contains("Access")) {
//! @todo tmp: hardcoded!
#ifdef Q_WS_WIN
            srcConn->m_fileDlg->setSelectedFilter("*.mdb");
#else
            srcConn->m_fileDlg->setFilter("*.mdb");
#endif
        }
        srcConn->m_file->label->hide();
        srcConn->m_file->btn_advanced->hide();
        srcConn->m_file->label->parentWidget()->hide();
    }
  } else {
    srcConn->showAdvancedConn();
  }
  /*! @todo Support different file extensions based on MigrationDriver */
  srcConnPage->show();
}

void ImportWizard::arriveSrcDBPage()
{
  if (fileBasedSrc) {
    //! @todo Back button doesn't work after selecting a file to import
//moved    showPage(dstTypePage);
  }
  else {
    if (!srcdbname)
    {
      srcdbPage->hide();
      kdDebug() << "Looks like we need a project selector widget!" << endl;

      KexiDB::ConnectionData* condata = srcConn->selectedConnectionData();
      if(condata) {
          m_prjSet = new KexiProjectSet(*condata);
					QVBoxLayout *vbox = new QVBoxLayout(srcdbPage, KDialog::marginHint());
          srcdbname = new KexiProjectSelectorWidget(srcdbPage,
              "KexiMigrationProjectSelector", m_prjSet);
					vbox->addWidget( srcdbname );
					srcdbname->label->setText(i18n("Select source database to you wish to import:"));
      }

      srcdbPage->show();
    }
  }
}

void ImportWizard::arriveDstTitlePage()
{
  if(fileBasedSrc) {
    QString suggestedDBName( QFileInfo(srcConn->selectedFileName()).fileName() );
    const QFileInfo fi( suggestedDBName );
    suggestedDBName = suggestedDBName.left(suggestedDBName.length() 
        - (fi.extension().length() ? (fi.extension().length()+1) : 0));
    dstNewDBName->setText( suggestedDBName );
  } else {
    if (!srcdbname || !srcdbname->selectedProjectData()) {
      back(); //todo!
      return;
    }
    dstNewDBName->setText( srcdbname->selectedProjectData()->databaseName() );
  }
}

void ImportWizard::arriveDstPage()
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

void ImportWizard::arriveFinishPage() {
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

void ImportWizard::checkIfSrcTypeFileBased(const QString& srcType) {
  //! @todo Use MigrateManager to get src type property
  if ((srcType == "PostgreSQL") || (srcType == "MySQL")) {
    fileBasedSrc = false;
  } else {
    fileBasedSrc = true;
  }
}

void ImportWizard::checkIfDstTypeFileBased(const QString& dstType) {
  //! @todo Use DriverManager to get dst type property
  if ((dstType == "PostgreSQL") || (dstType == "MySQL")) {
    fileBasedDst = false;
  } else {
    fileBasedDst = true;
  }
}


void ImportWizard::progressUpdated(int percent) {
  progress->setProgress(percent);
  KApplication::kApplication()->processEvents();
}

//===========================================================
//
void ImportWizard::accept()
{
    backButton()->setEnabled(false);
    finishButton()->setEnabled(false);
//    cancelButton()->setEnabled(false);
    acceptImport();
    backButton()->setEnabled(true);
    finishButton()->setEnabled(true);
//    cancelButton()->setEnabled(true);
}

void ImportWizard::acceptImport()
{
    KexiUtils::WaitCursor wait;
    
    kdDebug() << "Creating managers..." << endl;
    // Start with a driver manager
    KexiDB::DriverManager manager;

    kdDebug() << "Creating destination driver..." << endl;
    // Get a driver to the destination database
    KexiDB::Driver *driver = manager.driver(dstTypeCombo->currentText());

    //Check for errors
    if (!driver || manager.error())
    {
        kdDebug() << "Manager error..." << endl;
        manager.debugError();
        return;
    }

    KexiDB::ConnectionData *cdata;
    bool cdataOwned = false;
    QString dbname;
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
        cdata = new KexiDB::ConnectionData();
        cdataOwned = true;
        cdata->caption = dstNewDBName->text();
        cdata->driverName = KexiDB::Driver::defaultFileBasedDriverName();
        dbname = dstConn->selectedFileName();
        cdata->setFileName( dbname );
        kdDebug() << "Current file name: " << dbname << endl;
    }
    else
    {
        //TODO This needs a better message
        KMessageBox::error(this, i18n("No connection data is available. You did not select an SQLite destination filename."));
        return;
    }

    kdDebug() << "Creating connection to destination..." << endl;
/*moved to KexiMigrate
    //Create connections to the kexi database
    KexiDB::Connection *kexi_conn = driver->createConnection(*cdata);
    if(!kexi_conn || driver->error()) {
        kdDebug() << "Creating destination connection error..." << endl;
        KMessageBox::error(this, driver->errorMsg());
        delete kexi_conn;
        return;
    }
*/
    kdDebug() << "Creating source driver..." << endl;
    MigrateManager migrateManager;

    KexiMigrate* import = migrateManager.migrateDriver(srcTypeCombo->currentText());
    if(!import || migrateManager.error()) {
        kdDebug() << "Import migrate driver error..." << endl;
        KMessageBox::error(this, migrateManager.errorMsg());
//        delete kexi_conn;
        return;
    }

    if(import->progressSupported()) {
      progress->show();
      progress->updateGeometry();
      connect(import, SIGNAL(progressPercent(int)),
              this, SLOT(progressUpdated(int)));
			progressUpdated(0);
    }

    kdDebug() << "Setting import data.." << endl;
    bool keepData;
    if (importTypeButtonGroup->selectedId() == 0)
    {
        kdDebug() << "Structure and data selected" << endl;
        keepData = true;
    }
    else if (importTypeButtonGroup->selectedId() == 1)
    {
        kdDebug() << "structure only selected" << endl;
        keepData = false;
    }
    else
    {
        kdDebug() << "Neither radio button is selected (not possible?) presume keep data" << endl;
        keepData = true;
    }
    
    KexiMigration::Data* md = new KexiMigration::Data();
		delete md->destination;
    md->destination = new KexiProjectData(*cdata, dbname);
    if(fileBasedSrc) {
      KexiDB::ConnectionData* conn_data = new KexiDB::ConnectionData();
      conn_data->setFileName(srcConn->selectedFileName());
      md->source = conn_data;
      md->sourceName = "";
//      md->destName = dbname;
//      md->keepData = keepData;
//      import->setData(md);
    }
    else 
    {
      md->source = srcConn->selectedConnectionData();
      md->sourceName = srcdbname->selectedProjectData()->databaseName();
//      md->destName = dbname;
//      md->keepData = keepData;
//      import->setData(md);
//! @todo Aah, this is so C-like. Move to performImport().
    }
    md->keepData = keepData;
    import->setData(md);
    kdDebug() << "Performing import..." << endl;
    KexiUtils::removeWaitCursor();
    Kexi::ObjectStatus result;
    if (import->performImport(&result)) {
        KWizard::accept(); //tmp, before adding "final page"
        KMessageBox::information(this, i18n("Import Succeeded."), i18n("Success"));
        if (m_result) {
            if (fileBasedDst) {
//! @todo also pass result when using server connection as target
                *m_result = dbname;
            }
        }
    }
    else
    {
//??        KWizard::reject(); //tmp, before adding "final page"
        progress->setProgress(0);
        progress->hide();
        KexiGUIMessageHandler handler;
        handler.showErrorMessage(i18n("Import failed."), &result);
//        KMessageBox::error(this, i18n("Import failed."), i18n("Failure"));
    }
//    delete kexi_conn;
}

void ImportWizard::reject()
{
    KWizard::reject();
}


//===========================================================
//
void ImportWizard::next()
{
    if (currentPage() == srcConnPage) {
      if (fileBasedSrc
          && /*! @todo use KURL? */!QFileInfo(srcConn->selectedFileName()).isFile()) {

        KMessageBox::sorry(this,i18n("Select source database filename."));
        return;
      }

      if ( (! fileBasedSrc) && (! srcConn->selectedConnectionData()) ) {
        KMessageBox::sorry(this,i18n("Select source database."));
        return;
      }
    }

    setAppropriate( srcdbPage, !fileBasedSrc ); //skip srcdbPage
    KWizard::next();
}

void ImportWizard::back()
{
    setAppropriate( srcdbPage, !fileBasedSrc ); //skip srcdbPage
    KWizard::back();
}

void ImportWizard::pageSelected(const QString &)
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

void ImportWizard::helpClicked()
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
