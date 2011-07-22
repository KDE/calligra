/* This file is part of the KDE project
   Copyright (C) 2004-2009 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2006 Jarosław Staniek <staniek@kde.org>
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
#include "importoptionsdlg.h"

#include <qlabel.h>
#include <qlayout.h>
#include <qradiobutton.h>
#include <qcheckbox.h>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QDir>

#include <kcombobox.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <klineedit.h>
#include <kiconloader.h>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
#include <kexidb/utils.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <KexiConnSelector.h>
#include <KexiProjectSelector.h>
#include <KexiDBTitlePage.h>
#include <kexiutils/utils.h>
#include <kexidbdrivercombobox.h>
#include <kexitextmsghandler.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/kexiprjtypeselector.h>
#include <main/startup/KexiStartupFileWidget.h>


using namespace KexiMigration;

//===========================================================
//
ImportWizard::ImportWizard(QWidget *parent, QMap<QString, QString>* args)
        : KAssistantDialog(parent)
        , m_args(args)
{
    setModal(true);
    setWindowTitle(i18n("Import Database"));
    setWindowIcon(KIcon("document-import-database"));
    m_prjSet = 0;
    m_fileBasedDstWasPresented = false;
    m_setupFileBasedSrcNeeded = true;
    m_importExecuted = false;
    m_srcTypeCombo = 0;

    setMinimumSize(400, 400);
    parseArguments();

    setupIntro();
    setupSrcConn();
    setupSrcDB();
    setupDstType();
    setupDstTitle();
    setupDst();
    setupImportType();
    setupImporting();
    setupFinish();

    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), this, SLOT(slot_currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)));
    connect(this, SIGNAL(helpClicked()), this, SLOT(helpClicked()));

    if (m_predefinedConnectionData) {
        // setup wizard for predefined server source
        m_srcConn->showAdvancedConn();
        setAppropriate(m_srcConnPageItem, false);
        setAppropriate(m_srcDBPageItem, false);
    } else if (!m_predefinedDatabaseName.isEmpty()) {
        // setup wizard for predefined source
        // (used when external project type was opened in Kexi, e.g. mdb file)
//  MigrateManager manager;
//  QString driverName = manager.driverForMimeType( m_predefinedMimeType );
//  m_srcTypeCombo->setCurrentText( driverName );

//  showPage( m_srcConnPage );
        setAppropriate(m_srcConnPageItem, false);
        setAppropriate(m_srcDBPageItem, false);
        m_srcConn->showSimpleConn();
        m_srcConn->setSelectedFileName(m_predefinedDatabaseName);

        #if 0
        //disable all prev pages except "welcome" page
        for (int i = 0; i < indexOf(m_dstTypePage); i++) {
            if (page(i) != m_introPage)
                setAppropriate(page(i), false);
        }
        #endif
    }

    m_sourceDBEncoding = QString::fromLatin1(KGlobal::locale()->encoding()); //default
}

//===========================================================
//
ImportWizard::~ImportWizard()
{
    delete m_prjSet;
}

//===========================================================
//
void ImportWizard::parseArguments()
{
    m_predefinedConnectionData = 0;
    if (!m_args)
        return;
    if (!(*m_args)["databaseName"].isEmpty() && !(*m_args)["mimeType"].isEmpty()) {
        m_predefinedDatabaseName = (*m_args)["databaseName"];
        m_predefinedMimeType = (*m_args)["mimeType"];
        if (m_args->contains("connectionData")) {
            m_predefinedConnectionData = new KexiDB::ConnectionData();
            KexiDB::fromMap(
                KexiUtils::deserializeMap((*m_args)["connectionData"]), *m_predefinedConnectionData
            );
        }
    }
    m_args->clear();
}

QString ImportWizard::selectedSourceFileName() const
{
    if (m_predefinedDatabaseName.isEmpty())
        return m_srcConn->selectedFileName();

    return m_predefinedDatabaseName;
}

//===========================================================
//
void ImportWizard::setupIntro()
{
    m_introPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();

    m_introPageWidget->setLayout(vbox);
    
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    QLabel *lblIntro = new QLabel(m_introPageWidget);
    lblIntro->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblIntro->setWordWrap(true);
    QString msg;
    if (m_predefinedConnectionData) { //predefined import: server source
        msg = i18n("<qt>Database Importing wizard is about to import \"%1\" database "
                   "<nobr>(connection %2)</nobr> into a Kexi database.</qt>",
                   m_predefinedDatabaseName, m_predefinedConnectionData->serverInfoString());
    } else if (!m_predefinedDatabaseName.isEmpty()) { //predefined import: file source
//! @todo this message is currently ok for files only
        KMimeType::Ptr mimeTypePtr = KMimeType::mimeType(m_predefinedMimeType);
        if (mimeTypePtr.isNull())
            KexiDBWarn << QString("'%1' mimetype not installed!").arg(m_predefinedMimeType);
        msg = i18n(
                  "<qt>Database Importing wizard is about to import <nobr>\"%1\"</nobr> file "
                  "of type \"%2\" into a Kexi database.</qt>",
                  QDir::convertSeparators(m_predefinedDatabaseName), mimeTypePtr ? mimeTypePtr->comment() : "???");
    } else {
        msg = i18n("Database Importing wizard allows you to import an existing database "
                   "into a Kexi database.");
    }
    lblIntro->setText(msg + "\n\n"
                      + i18n("Click \"Next\" button to continue or \"Cancel\" button to exit this wizard."));
    vbox->addWidget(lblIntro);

    m_introPageItem = new KPageWidgetItem(m_introPageWidget, i18n("Welcome to the Database Importing Wizard"));
    addPage(m_introPageItem);
}

//===========================================================
//
void ImportWizard::setupSrcConn()
{
    m_srcConnPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_srcConnPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    m_srcConn = new KexiConnSelectorWidget(Kexi::connset(),
                                           "kfiledialog:///ProjectMigrationSourceDir",
                                           KAbstractFileWidget::Opening, m_srcConnPageWidget);

    m_srcConn->hideConnectonIcon();
    m_srcConn->showSimpleConn();

    QSet<QString> excludedFilters;
//! @todo remove when support for kexi files as source prj is added in migration
    excludedFilters += KexiDB::defaultFileBasedDriverMimeType();
    excludedFilters += "application/x-kexiproject-shortcut";
    excludedFilters += "application/x-kexi-connectiondata";
    m_srcConn->fileWidget->setExcludedFilters(excludedFilters);

// m_srcConn->hideHelpers();
    vbox->addWidget(m_srcConn);

    m_srcConnPageItem = new KPageWidgetItem(m_srcConnPageWidget, i18n("Select Location for Source Database"));
    addPage(m_srcConnPageItem);
}

//===========================================================
//
void ImportWizard::setupSrcDB()
{
// arrivesrcdbPage creates widgets on that page
    m_srcDBPageWidget = new QWidget(this);
    m_srcDBName = NULL;

    m_srcDBPageItem = new KPageWidgetItem(m_srcDBPageWidget, i18n("Select Source Database"));
    addPage(m_srcDBPageItem);
}

//===========================================================
//
void ImportWizard::setupDstType()
{
    m_dstTypePageWidget = new QWidget(this);

    KexiDB::DriverManager manager;
    KexiDB::Driver::InfoHash drvs = manager.driversInfo();

    QVBoxLayout *vbox = new QVBoxLayout(m_dstTypePageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    KexiUtils::setStandardMarginsAndSpacing(hbox);
    QLabel *lbl = new QLabel(i18n("Destination database type:") /*+ ' '*/, m_dstTypePageWidget);
    lbl->setAlignment(Qt::AlignAuto | Qt::AlignTop);
    hbox->addWidget(lbl);

    m_dstPrjTypeSelector = new KexiPrjTypeSelector(m_dstTypePageWidget);
    hbox->addWidget(m_dstPrjTypeSelector);
    m_dstPrjTypeSelector->option_file->setText(i18n("Database project stored in a file"));
    m_dstPrjTypeSelector->option_server->setText(i18n("Database project stored on a server"));

    QVBoxLayout *frame_server_vbox = new QVBoxLayout(m_dstPrjTypeSelector->frame_server);
    KexiUtils::setStandardMarginsAndSpacing(frame_server_vbox);
    m_dstServerTypeCombo = new KexiDBDriverComboBox(m_dstPrjTypeSelector->frame_server, drvs,
            KexiDBDriverComboBox::ShowServerDrivers);
    frame_server_vbox->addWidget(m_dstServerTypeCombo);
    hbox->addStretch(1);
    vbox->addStretch(1);
    lbl->setBuddy(m_dstServerTypeCombo);

//! @todo hardcoded: find a way to preselect default engine item
    //m_dstTypeCombo->setCurrentText("SQLite3");

    m_dstTypePageItem = new KPageWidgetItem(m_dstTypePageWidget, i18n("Select Destination Database Type"));
    addPage(m_dstTypePageItem);
}

//===========================================================
//
void ImportWizard::setupDstTitle()
{
    m_dstTitlePageWidget = new KexiDBTitlePage(i18n("Destination project's title:"), this);
    m_dstTitlePageWidget->layout()->setMargin(KDialog::marginHint());
    m_dstTitlePageWidget->updateGeometry();
    m_dstNewDBNameLineEdit = m_dstTitlePageWidget->le_title;

    m_dstTitlePageItem = new KPageWidgetItem(m_dstTitlePageWidget, i18n("Select Destination Database Project's Title"));
    addPage(m_dstTitlePageItem);
}

//===========================================================
//
void ImportWizard::setupDst()
{
    m_dstPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_dstPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    m_dstConn = new KexiConnSelectorWidget(Kexi::connset(),
                                           "kfiledialog:///ProjectMigrationDestinationDir",
                                           KAbstractFileWidget::Saving, m_dstPageWidget);
    m_dstConn->hideHelpers();

    vbox->addWidget(m_dstConn);
    connect(m_dstConn, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(next()));

// m_dstConn->hideHelpers();
    m_dstConn->showSimpleConn();
    //anyway, db files will be _saved_
    m_dstConn->fileWidget->setMode(KexiStartupFileWidget::SavingFileBasedDB);
// m_dstConn->hideHelpers();
// m_dstConn->m_file->btn_advanced->hide();
// m_dstConn->m_file->label->hide();
// m_dstConn->m_file->lbl->hide();
    //m_dstConn->m_file->spacer7->hide();


    //js dstNewDBName = new KLineEdit(dstControls);
    //   dstNewDBName->setText(i18n("Enter new database name here"));

    m_dstPageItem = new KPageWidgetItem(m_dstPageWidget, i18n("Select Location for Destination Database"));
    addPage(m_dstPageItem);
}

//===========================================================
//
void ImportWizard::setupImportType()
{
    m_importTypePageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_importTypePageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    m_importTypeGroupBox = new QGroupBox(m_importTypePageWidget);

    vbox->addWidget(m_importTypeGroupBox);
    QVBoxLayout *importTypeGroupBoxLyr = new QVBoxLayout;

    importTypeGroupBoxLyr->addWidget(
        m_importTypeStructureAndDataCheckBox = new QRadioButton(i18n("Structure and data"), m_importTypeGroupBox));
    m_importTypeStructureAndDataCheckBox->setChecked(true);

    importTypeGroupBoxLyr->addWidget(
        m_importTypeStructureOnlyCheckBox = new QRadioButton(i18n("Structure only"), m_importTypeGroupBox));

    importTypeGroupBoxLyr->addStretch(1);
    m_importTypeGroupBox->setLayout(importTypeGroupBoxLyr);

    m_importTypePageItem = new KPageWidgetItem(m_importTypePageWidget, i18n("Select Type of Import"));
    addPage(m_importTypePageItem);
}

//===========================================================
//
void ImportWizard::setupImporting()
{
    m_importingPageWidget = new QWidget(this);
    m_importingPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(m_importingPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    m_lblImportingTxt = new QLabel(m_importingPageWidget);
    m_lblImportingTxt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_lblImportingTxt->setWordWrap(true);

    m_lblImportingErrTxt = new QLabel(m_importingPageWidget);
    m_lblImportingErrTxt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_lblImportingErrTxt->setWordWrap(true);

    m_progressBar = new QProgressBar(m_importingPageWidget);
    m_progressBar->setRange(0, 100);
    m_progressBar->hide();

    vbox->addWidget(m_lblImportingTxt);
    vbox->addWidget(m_lblImportingErrTxt);
    vbox->addStretch(1);

    QWidget *options_widget = new QWidget(m_importingPageWidget);
    vbox->addWidget(options_widget);
    QVBoxLayout *options_vbox = new QVBoxLayout(options_widget);
    options_vbox->setSpacing(KDialog::spacingHint());
    QHBoxLayout *importOptionsButtonLyr = new QHBoxLayout;
    options_vbox->addLayout(importOptionsButtonLyr);
    m_importOptionsButton = new KPushButton(KIcon("configure"),
                                            i18n("Advanced Options"), options_widget);
    connect(m_importOptionsButton, SIGNAL(clicked()),
            this, SLOT(slotOptionsButtonClicked()));
    importOptionsButtonLyr->addStretch(1);
    importOptionsButtonLyr->addWidget(m_importOptionsButton);
    importOptionsButtonLyr->addStretch(1);
    options_vbox->addStretch(1);

    vbox->addWidget(m_progressBar);
    vbox->addStretch(2);
    m_importingPageWidget->show();

    m_importingPageItem = new KPageWidgetItem(m_importingPageWidget, i18n("Importing"));
    addPage(m_importingPageItem);
}

//===========================================================
//
void ImportWizard::setupFinish()
{
    m_finishPageWidget = new QWidget(this);
    m_finishPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(m_finishPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    m_finishLbl = new QLabel(m_finishPageWidget);
    m_finishLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_finishLbl->setWordWrap(true);

    vbox->addWidget(m_finishLbl);
    m_openImportedProjectCheckBox = new QCheckBox(i18n("Open imported project"),
            m_finishPageWidget);
    m_openImportedProjectCheckBox->setChecked(true);
    vbox->addSpacing(KDialog::spacingHint());
    vbox->addWidget(m_openImportedProjectCheckBox);
    vbox->addStretch(1);

    m_finishPageItem = new KPageWidgetItem(m_finishPageWidget, i18n("Success"));
    addPage(m_finishPageItem);
}

//===========================================================
//
bool ImportWizard::checkUserInput()
{
    QString finishtxt;

    if (m_dstNewDBNameLineEdit->text().isEmpty()) {
        finishtxt = finishtxt + "<br>" + i18n("No new database name was entered.");
    }

    Kexi::ObjectStatus result;
    KexiMigrate* sourceDriver = prepareImport(result);
    if (sourceDriver && sourceDriver->isSourceAndDestinationDataSourceTheSame()) {
        finishtxt = finishtxt + "<br>" + i18n("Source database is the same as destination.");
    }

    if (! finishtxt.isNull()) {
        finishtxt = "<qt>" + i18n("Following problems were found with the data you entered:") +
                    "<br>" + finishtxt + "<br><br>" +
                    i18n("Please click 'Back' button and correct these errors.");
        m_lblImportingErrTxt->setText(finishtxt);
    }

    return finishtxt.isNull();
}

void ImportWizard::arriveSrcConnPage()
{
    m_srcConnPageWidget->hide();

// checkIfSrcTypeFileBased(m_srcTypeCombo->currentText());
// if (fileBasedSrcSelected()) {
//moved  m_srcConn->showSimpleConn();
    /*! @todo KexiStartupFileWidget needs "open file" and "open server" modes
    in addition to just "open" */
    if (m_setupFileBasedSrcNeeded) {
        m_setupFileBasedSrcNeeded = false;
        QSet<QString> additionalMimeTypes;
        /* moved
            if (m_srcTypeCombo->currentText().contains("Access")) {
        //! @todo tmp: hardcoded!
              additionalMimeTypes << "application/vnd.ms-access";
            }*/
        m_srcConn->fileWidget->setMode(KexiStartupFileWidget::Opening);
        m_srcConn->fileWidget->setAdditionalFilters(additionalMimeTypes);

    }

    /*! @todo Support different file extensions based on MigrationDriver */
    m_srcConnPageWidget->show();
}

void ImportWizard::arriveSrcDBPage()
{
    if (fileBasedSrcSelected()) {
        //! @todo Back button doesn't work after selecting a file to import
        //moved showPage(m_dstTypePage);
    } else if (!m_srcDBName) {
        m_srcDBPageWidget->hide();
        kDebug() << "Looks like we need a project selector widget!";

        KexiDB::ConnectionData* condata = m_srcConn->selectedConnectionData();
        if (condata) {
            m_prjSet = new KexiProjectSet(*condata);
            QVBoxLayout *vbox = new QVBoxLayout(m_srcDBPageWidget);
            KexiUtils::setStandardMarginsAndSpacing(vbox);
            m_srcDBName = new KexiProjectSelectorWidget(m_srcDBPageWidget, m_prjSet);
            vbox->addWidget(m_srcDBName);
            m_srcDBName->label()->setText(i18n("Select source database you wish to import:"));
        }
        m_srcDBPageWidget->show();
    }
}

void ImportWizard::arriveDstTitlePage()
{
    if (fileBasedSrcSelected()) {
        const QString fname(selectedSourceFileName());
        QString suggestedDBName(QFileInfo(fname).fileName());
        const QFileInfo fi(suggestedDBName);
        suggestedDBName = suggestedDBName.left(suggestedDBName.length()
                                               - (fi.completeSuffix().isEmpty() ? 0 : (fi.completeSuffix().length() + 1)));
        m_dstNewDBNameLineEdit->setText(suggestedDBName);
    }
    else {
        if (m_predefinedConnectionData) {
            // server source db is predefined
            m_dstNewDBNameLineEdit->setText(m_predefinedDatabaseName);
        } else {
            if (!m_srcDBName || !m_srcDBName->selectedProjectData()) {
                back(); //todo!
                return;
            }
            m_dstNewDBNameLineEdit->setText(m_srcDBName->selectedProjectData()->databaseName());
        }
    }
}

void ImportWizard::arriveDstPage()
{
    m_dstPageWidget->hide();

// checkIfDstTypeFileBased(m_dstTypeCombo->currentText());
    if (fileBasedDstSelected()) {
        m_dstConn->showSimpleConn();
        m_dstConn->fileWidget->setMode(KexiStartupFileWidget::SavingFileBasedDB);
        if (!m_fileBasedDstWasPresented) {
            //without extension - it will be added automatically
            m_dstConn->fileWidget->setLocationText(m_dstNewDBNameLineEdit->text());
        }
        m_fileBasedDstWasPresented = true;
    } else {
        m_dstConn->showAdvancedConn();
    }
    m_dstPageWidget->show();
}

void ImportWizard::arriveImportingPage()
{
    m_importingPageWidget->hide();
    if (checkUserInput()) {
        //setNextEnabled(m_importingPageWidget, true);
        enableButton(KDialog::User2, true);
    } else {
        //setNextEnabled(m_importingPageWidget, false);
        enableButton(KDialog::User2, false);
    }

    m_lblImportingTxt->setText(i18n(
                                   "All required information has now "
                                   "been gathered. Click \"Next\" button to start importing.\n\n"
                                   "Depending on size of the database this may take some time."
                                   /*"Note: You may be asked for extra "
                                   "information such as field types if "
                                   "the wizard could not automatically "
                                   "determine this for you."*/));

//todo

    //temp. hack for MS Access driver only
//! @todo for other databases we will need KexiMigration::Conenction
//! and KexiMigration::Driver classes
    bool showOptions = false;
    if (fileBasedSrcSelected()) {
        Kexi::ObjectStatus result;
        KexiMigrate* sourceDriver = prepareImport(result);
        if (sourceDriver) {
            showOptions = !result.error()
                          && sourceDriver->propertyValue("source_database_has_nonunicode_encoding").toBool();
            KexiMigration::Data *data = sourceDriver->data();
            sourceDriver->setData(0);
            delete data;
        }
    }
    if (showOptions)
        m_importOptionsButton->show();
    else
        m_importOptionsButton->hide();

    m_importingPageWidget->show();
}

void ImportWizard::arriveFinishPage()
{
// backButton()->hide();
// cancelButton()->setEnabled(false);
// m_finishLbl->setText( m_successText.arg(m_dstNewDBNameLineEdit->text()) );
}

bool ImportWizard::fileBasedSrcSelected() const
{
    if (m_predefinedConnectionData)
        return false;

// kDebug() << (m_srcConn->selectedConnectionType()==KexiConnSelectorWidget::FileBased);
    return m_srcConn->selectedConnectionType() == KexiConnSelectorWidget::FileBased;
}

bool ImportWizard::fileBasedDstSelected() const
{

    return m_dstPrjTypeSelector->buttonGroup->selectedId() == 1;

}

void ImportWizard::progressUpdated(int percent)
{
    m_progressBar->setValue(percent);
    KApplication::kApplication()->processEvents();
}

//===========================================================
//
QString ImportWizard::driverNameForSelectedSource()
{
    if (fileBasedSrcSelected()) {
        KMimeType::Ptr ptr = KMimeType::findByFileContent(selectedSourceFileName());
        if (!ptr
                || ptr.data()->name() == "application/octet-stream"
                || ptr.data()->name() == "text/plain") {
            //try by URL:
            ptr = KMimeType::findByUrl(selectedSourceFileName());
        }
        return ptr ? m_migrateManager.driverForMimeType(ptr.data()->name()) : QString();
    }

    //server-based
    if (m_predefinedConnectionData) {
        return m_predefinedConnectionData->driverName;
    }

    return m_srcConn->selectedConnectionData()
           ? m_srcConn->selectedConnectionData()->driverName : QString();
}

//===========================================================
//
void ImportWizard::accept()
{
    if (m_args) {
        if ((!fileBasedDstSelected() && !m_args->contains("destinationConnectionShortcut"))
                || !m_openImportedProjectCheckBox->isChecked()) {
            //do not open dest db if used didn't want it
            //for server connections, destinationConnectionShortcut must be defined
            m_args->remove("destinationDatabaseName");
        }
    }
    KAssistantDialog::accept();
}

KexiMigrate* ImportWizard::prepareImport(Kexi::ObjectStatus& result)
{
    KexiUtils::WaitCursor wait;

    // Start with a driver manager
    KexiDB::DriverManager manager;

    kDebug() << "Creating destination driver...";

    // Get a driver to the destination database
    KexiDB::Driver *destDriver = manager.driver(
                                     m_dstConn->selectedConnectionData() ? m_dstConn->selectedConnectionData()->driverName //server based
                                     : KexiDB::defaultFileBasedDriverName()
                                     // : m_dstTypeCombo->currentText() //file based
                                 );
    if (!destDriver || manager.error()) {
        result.setStatus(&manager);
        kDebug() << "Manager error...";
        manager.debugError();
//  result.setStatus(&manager);
    }

    // Set up destination connection data
    KexiDB::ConnectionData *cdata;
    QString dbname;
    if (!result.error()) {
        if (m_dstConn->selectedConnectionData()) {
            //server-based project
            kDebug() << "Server destination...";
            cdata = m_dstConn->selectedConnectionData();
            dbname = m_dstNewDBNameLineEdit->text();
        } else { // if (m_dstTypeCombo->currentText().toLower() == KexiDB::defaultFileBasedDriverName())
            //file-based project
            kDebug() << "File Destination...";
            cdata = new KexiDB::ConnectionData();
            cdata->caption = m_dstNewDBNameLineEdit->text();
            cdata->driverName = KexiDB::defaultFileBasedDriverName();
            dbname = m_dstConn->selectedFileName();
            cdata->setFileName(dbname);
            kDebug() << "Current file name: " << dbname;
        }
        /*  else
            {
              //TODO This needs a better message
              //KMessageBox::error(this,
              result.setStatus(i18n("No connection data is available. You did not select a destination filename."),"");
              //return false;
            } */
    }

    // Find a source (migration) driver name
    QString sourceDriverName;
    if (!result.error()) {
        sourceDriverName = driverNameForSelectedSource();
        if (sourceDriverName.isEmpty())
            result.setStatus(i18n("No appropriate migration driver found."),
                             m_migrateManager.possibleProblemsInfoMsg());
    }

    // Get a source (migration) driver
    KexiMigrate* sourceDriver = 0;
    if (!result.error()) {
        sourceDriver = m_migrateManager.driver(sourceDriverName);
        if (!sourceDriver || m_migrateManager.error()) {
            kDebug() << "Import migrate driver error...";
            result.setStatus(&m_migrateManager);
        }
    }

    KexiUtils::removeWaitCursor();

    // Set up source (migration) data required for connection
    if (sourceDriver && !result.error()) {
        // Setup progress feedback for the GUI
        if (sourceDriver->progressSupported()) {
            m_progressBar->updateGeometry();
            disconnect(sourceDriver, SIGNAL(progressPercent(int)),
                       this, SLOT(progressUpdated(int)));
            connect(sourceDriver, SIGNAL(progressPercent(int)),
                    this, SLOT(progressUpdated(int)));
            progressUpdated(0);
        }

        bool keepData;
        if (m_importTypeStructureAndDataCheckBox->isChecked()) {
            kDebug() << "Structure and data selected";
            keepData = true;
        } else if (m_importTypeStructureOnlyCheckBox->isChecked()) {
            kDebug() << "structure only selected";
            keepData = false;
        } else {
            kDebug() << "Neither radio button is selected (not possible?) presume keep data";
            keepData = true;
        }

        KexiMigration::Data* md = new KexiMigration::Data();
        // delete md->destination;
        md->destination = new KexiProjectData(*cdata, dbname);
        if (fileBasedSrcSelected()) {
            KexiDB::ConnectionData* conn_data = new KexiDB::ConnectionData();
            conn_data->setFileName(selectedSourceFileName());
            md->source = conn_data;
            md->sourceName.clear();
        } else {
            if (m_predefinedConnectionData)
                md->source = m_predefinedConnectionData;
            else
                md->source = m_srcConn->selectedConnectionData();

            if (!m_predefinedDatabaseName.isEmpty())
                md->sourceName = m_predefinedDatabaseName;
            else
                md->sourceName = m_srcDBName->selectedProjectData()->databaseName();
            //! @todo Aah, this is so C-like. Move to performImport().
        }
        md->keepData = keepData;
        sourceDriver->setData(md);
        return sourceDriver;
    }
    return 0;
}

tristate ImportWizard::import()
{
    m_importExecuted = true;

    Kexi::ObjectStatus result;
    KexiMigrate* sourceDriver = prepareImport(result);

    bool acceptingNeeded = false;

    // Perform import
    if (sourceDriver && !result.error()) {
        if (!m_sourceDBEncoding.isEmpty()) {
            sourceDriver->setPropertyValue("source_database_nonunicode_encoding",
                                           QVariant(m_sourceDBEncoding.toUpper().replace(' ', QString())) // "CP1250", not "cp 1250"
                                          );
        }

        if (!sourceDriver->checkIfDestinationDatabaseOverwritingNeedsAccepting(&result, acceptingNeeded)) {
            kDebug() << "Abort import cause checkIfDestinationDatabaseOverwritingNeedsAccepting "
            "returned false.";
            return false;
        }

        kDebug() << sourceDriver->data()->destination->databaseName();
        kDebug() << "Performing import...";
    }

    if (sourceDriver && !result.error() && acceptingNeeded) {
        // ok, the destination-db already exists...
        if (KMessageBox::Yes != KMessageBox::warningYesNo(this,
                "<qt>" + i18n("Database %1 already exists."
                              "<p>Do you want to replace it with a new one?",
                              sourceDriver->data()->destination->infoString()),
                0, KGuiItem(i18n("&Replace")), KGuiItem(i18n("No")))) {
            return cancelled;
        }
    }

    if (sourceDriver && !result.error() && sourceDriver->progressSupported()) {
        m_progressBar->show();
    }

    if (sourceDriver && !result.error() && sourceDriver->performImport(&result)) {
        if (m_args) {
//    if (fileBasedDstSelected()) {
            m_args->insert("destinationDatabaseName",
                           sourceDriver->data()->destination->databaseName());
//    }
            QString destinationConnectionShortcut(
                Kexi::connset().fileNameForConnectionData(m_dstConn->selectedConnectionData()));
            if (!destinationConnectionShortcut.isEmpty()) {
                m_args->insert("destinationConnectionShortcut", destinationConnectionShortcut);
            }
        }
        m_finishPageItem->setHeader(i18n("Success"));
        return true;
    }

    if (!sourceDriver || result.error()) {
        m_progressBar->setValue(0);
        m_progressBar->hide();

        QString msg, details;
        KexiTextMessageHandler handler(msg, details);
        handler.showErrorMessage(&result);

        kDebug() << msg << "\n" << details;

        m_finishPageItem->setHeader(i18n("Failure"));
        m_finishLbl->setText(
            i18n(
                "<p>Import failed.</p>%1<p>%2</p><p>You can click \"Back\" button and try again.</p>",
                msg, details));
        return false;
    }
// delete kexi_conn;
    return true;
}

void ImportWizard::reject()
{
    KAssistantDialog::reject();
}

//===========================================================
//
void ImportWizard::next()
{
    if (currentPage() == m_srcConnPageItem) {
        if (fileBasedSrcSelected()
                && /*! @todo use KUrl? */!QFileInfo(selectedSourceFileName()).isFile()) {

            KMessageBox::sorry(this, i18n("Select source database filename."));
            return;
        }

        if ((! fileBasedSrcSelected()) && (! m_srcConn->selectedConnectionData())) {
            KMessageBox::sorry(this, i18n("Select source database."));
            return;
        }

        KexiMigrate* import = m_migrateManager.driver(driverNameForSelectedSource());
        if (!import || m_migrateManager.error()) {
            QString dbname;
            if (fileBasedSrcSelected())
                dbname = selectedSourceFileName();
            else
                dbname = m_srcConn->selectedConnectionData()
                         ? m_srcConn->selectedConnectionData()->serverInfoString() : QString();
            KMessageBox::error(this,
                               dbname.isEmpty() ?
                               i18n("Could not import database. This type is not supported.")
                               : i18n("Could not import database \"%1\". This type is not supported.", dbname));
            return;
        }
    } else if (currentPage() == m_dstPageItem) {
        if (m_fileBasedDstWasPresented) {
            if (fileBasedDstSelected() && !m_dstConn->fileWidget->checkSelectedFile())
                return;
        }
    } else if (currentPage() == m_importingPageItem) {
        if (!m_importExecuted) {
            m_importOptionsButton->hide();
            enableButton(KDialog::User2, false);
            enableButton(KDialog::User1, false);
            enableButton(KDialog::User3, false);
            m_lblImportingTxt->setText(i18n("Importing in progress..."));
            tristate res = import();
            if (true == res) {
                m_finishLbl->setText(
                    i18n("Database has been imported into Kexi database project \"%1\".",
                         m_dstNewDBNameLineEdit->text()));
                enableButtonCancel(false);
                enableButton(KDialog::User3, false);
                enableButton(KDialog::User1, true);
                m_openImportedProjectCheckBox->show();
                next();
                return;
            }

            m_progressBar->hide();

            enableButtonCancel(true);
            enableButton(KDialog::User3, true);
            enableButton(KDialog::User1, false);
            
            m_openImportedProjectCheckBox->hide();
            if (!res)
                next();
            else if (~res) {
                arriveImportingPage();
                //   back();
            }
            m_importExecuted = false;
            return;
        }
    }

    setAppropriate(m_srcDBPageItem, !fileBasedSrcSelected() && !m_predefinedConnectionData);   //skip m_srcDBPage
    KAssistantDialog::next();
}

void ImportWizard::back()
{
    setAppropriate(m_srcDBPageItem, !fileBasedSrcSelected() && !m_predefinedConnectionData);   //skip m_srcDBPage
    KAssistantDialog::back();
}

void ImportWizard::slot_currentPageChanged(KPageWidgetItem* curPage,KPageWidgetItem* prevPage)
{
    Q_UNUSED(prevPage);
    if (curPage == m_introPageItem) {
    }
    else if (curPage == m_srcConnPageItem) {
        arriveSrcConnPage();
    } else if (curPage == m_srcDBPageItem) {
        arriveSrcDBPage();
    } else if (curPage == m_dstTypePageItem) {
    } else if (curPage == m_dstTitlePageItem) {
        arriveDstTitlePage();
    } else if (curPage == m_dstPageItem) {
        arriveDstPage();
    } else if (curPage == m_importingPageItem) {
        arriveImportingPage();
    } else if (curPage == m_finishPageItem) {
        arriveFinishPage();
    }
}

void ImportWizard::helpClicked()
{
    if (currentPage() == m_introPageItem) {
        KMessageBox::information(this, i18n("No help is available for this page."), i18n("Help"));
    }
    /* else if (currentPage() == m_srcTypePage)
      {
        KMessageBox::information(this, i18n("Here you can choose the type of data to import data from."), i18n("Help"));
      }*/
    else if (currentPage() == m_srcConnPageItem) {
        KMessageBox::information(this, i18n("Here you can choose the location to import data from."), i18n("Help"));
    } else if (currentPage() == m_srcDBPageItem) {
        KMessageBox::information(this, i18n("Here you can choose the actual database to import data from."), i18n("Help"));
    } else if (currentPage() == m_dstTypePageItem) {
        KMessageBox::information(this, i18n("Here you can choose the location to save the data."), i18n("Help"));
    } else if (currentPage() == m_dstPageItem) {
        KMessageBox::information(this, i18n("Here you can choose the location to save the data in and the new database name."), i18n("Help"));
    } else if (currentPage() == m_finishPageItem || currentPage() == m_importingPageItem) {
        KMessageBox::information(this, i18n("No help is available for this page."), i18n("Help"));
    }
}

void ImportWizard::slotOptionsButtonClicked()
{
    QPointer<OptionsDialog> dlg = new OptionsDialog(selectedSourceFileName(), m_sourceDBEncoding, this);
    if (QDialog::Accepted == dlg->exec()) {
        if (m_sourceDBEncoding != dlg->encodingComboBox()->selectedEncoding()) {
            m_sourceDBEncoding = dlg->encodingComboBox()->selectedEncoding();
        }
    }
    delete dlg;
}

#include "importwizard.moc"
