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

#include <QLabel>
#include <QLayout>
#include <QRadioButton>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QDir>

#include <kcombobox.h>
#include <kmessagebox.h>
#include <kpushbutton.h>
#include <kdebug.h>
#include <klineedit.h>

#include <KoIcon.h>

#include <db/drivermanager.h>
#include <db/driver.h>
#include <db/connectiondata.h>
#include <db/utils.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>
#include <kexidbdrivercombobox.h>
#include <kexitextmsghandler.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/kexiprjtypeselector.h>
#include <widget/KexiFileWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiProjectSelectorWidget.h>
#include <widget/KexiDBTitlePage.h>


using namespace KexiMigration;

class ImportWizard::Private
{
public:

    Private(QMap<QString, QString>* args_)
      : srcTypeCombo(0)
      , fileBasedDstWasPresented(false)
      , setupFileBasedSrcNeeded(true)
      , importExecuted(false)
      , prjSet(0)
      , args(args_)
    {
    }

    ~Private()
    {
        delete prjSet;
    }

    QWidget *introPageWidget, *srcConnPageWidget, *srcDBPageWidget,
      *dstTypePageWidget, *dstPageWidget, *importTypePageWidget,
      *importingPageWidget, *finishPageWidget;

    KPageWidgetItem *introPageItem, *srcConnPageItem, *srcDBPageItem,
      *dstTypePageItem, *dstPageItem, *importTypePageItem,
      *importingPageItem, *finishPageItem;


    QGroupBox *importTypeGroupBox;
    QRadioButton *importTypeStructureAndDataCheckBox;
    QRadioButton *importTypeStructureOnlyCheckBox;
    KexiDBTitlePage* dstTitlePageWidget;
    KPageWidgetItem *dstTitlePageItem;

    KComboBox *srcTypeCombo;
    KexiDBDriverComboBox *dstServerTypeCombo;
    KexiPrjTypeSelector *dstPrjTypeSelector;

    KexiConnectionSelectorWidget *srcConn, *dstConn;
    KLineEdit *dstNewDBNameLineEdit;
    KexiProjectSelectorWidget *srcDBName;

    QLabel *lblImportingTxt, *lblImportingErrTxt, *finishLbl;
    QCheckBox *openImportedProjectCheckBox;
    bool fileBasedDstWasPresented : 1;
    bool setupFileBasedSrcNeeded : 1;
    bool importExecuted  :1; //!< used in import()
    KexiProjectSet* prjSet;
    QProgressBar *progressBar;
    KPushButton* importOptionsButton;
    QMap<QString, QString> *args;
    QString predefinedDatabaseName, predefinedMimeType;
    KexiDB::ConnectionData *predefinedConnectionData;
    MigrateManager migrateManager; //!< object lives here, so status messages can be globally preserved

    //! Encoding for source db. Currently only used for MDB driver.
//! @todo Hardcoded. Move to KexiMigrate driver's impl.
    QString sourceDBEncoding;

};

//===========================================================
//
ImportWizard::ImportWizard(QWidget *parent, QMap<QString, QString>* args)
        : KAssistantDialog(parent)
	, d(new Private(args))
{
    setModal(true);
    setWindowTitle(i18n("Import Database"));
    setWindowIcon(koIcon("document_import_database"));

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

    if (d->predefinedConnectionData) {
        // setup wizard for predefined server source
        d->srcConn->showAdvancedConn();
        setAppropriate(d->srcConnPageItem, false);
        setAppropriate(d->srcDBPageItem, false);
    } else if (!d->predefinedDatabaseName.isEmpty()) {
        // setup wizard for predefined source
        // (used when external project type was opened in Kexi, e.g. mdb file)
//  MigrateManager manager;
//  QString driverName = manager.driverForMimeType( d->predefinedMimeType );
//  d->srcTypeCombo->setCurrentText( driverName );

//  showPage( d->srcConnPage );
        setAppropriate(d->srcConnPageItem, false);
        setAppropriate(d->srcDBPageItem, false);
        d->srcConn->showSimpleConn();
        d->srcConn->setSelectedFileName(d->predefinedDatabaseName);

        #if 0
        //disable all prev pages except "welcome" page
        for (int i = 0; i < indexOf(d->dstTypePage); i++) {
            if (page(i) != d->introPage)
                setAppropriate(page(i), false);
        }
        #endif
    }

    d->sourceDBEncoding = QString::fromLatin1(KGlobal::locale()->encoding()); //default
}

//===========================================================
//
ImportWizard::~ImportWizard()
{
    delete d;
}

//===========================================================
//
void ImportWizard::parseArguments()
{
    d->predefinedConnectionData = 0;
    if (!d->args)
        return;
    if (!(*d->args)["databaseName"].isEmpty() && !(*d->args)["mimeType"].isEmpty()) {
        d->predefinedDatabaseName = (*d->args)["databaseName"];
        d->predefinedMimeType = (*d->args)["mimeType"];
        if (d->args->contains("connectionData")) {
            d->predefinedConnectionData = new KexiDB::ConnectionData();
            KexiDB::fromMap(
                KexiUtils::deserializeMap((*d->args)["connectionData"]), *d->predefinedConnectionData
            );
        }
    }
    d->args->clear();
}

QString ImportWizard::selectedSourceFileName() const
{
    if (d->predefinedDatabaseName.isEmpty())
        return d->srcConn->selectedFileName();

    return d->predefinedDatabaseName;
}

//===========================================================
//
void ImportWizard::setupIntro()
{
    d->introPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();

    d->introPageWidget->setLayout(vbox);

    KexiUtils::setStandardMarginsAndSpacing(vbox);

    QLabel *lblIntro = new QLabel(d->introPageWidget);
    lblIntro->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblIntro->setWordWrap(true);
    QString msg;
    if (d->predefinedConnectionData) { //predefined import: server source
        msg = i18n("<qt>Database Importing wizard is about to import \"%1\" database "
                   "<nobr>(connection %2)</nobr> into a Kexi database.</qt>",
                   d->predefinedDatabaseName, d->predefinedConnectionData->serverInfoString());
    } else if (!d->predefinedDatabaseName.isEmpty()) { //predefined import: file source
//! @todo this message is currently ok for files only
        KMimeType::Ptr mimeTypePtr = KMimeType::mimeType(d->predefinedMimeType);
        if (mimeTypePtr.isNull())
            KexiDBWarn << QString("'%1' mimetype not installed!").arg(d->predefinedMimeType);
        msg = i18n(
                  "<qt>Database Importing wizard is about to import <nobr>\"%1\"</nobr> file "
                  "of type \"%2\" into a Kexi database.</qt>",
                  QDir::convertSeparators(d->predefinedDatabaseName), mimeTypePtr ? mimeTypePtr->comment() : "???");
    } else {
        msg = i18n("Database Importing wizard allows you to import an existing database "
                   "into a Kexi database.");
    }
    lblIntro->setText(msg + "\n\n"
                      + i18n("Click \"Next\" button to continue or \"Cancel\" button to exit this wizard."));
    vbox->addWidget(lblIntro);

    d->introPageItem = new KPageWidgetItem(d->introPageWidget, i18n("Welcome to the Database Importing Wizard"));
    addPage(d->introPageItem);
}

//===========================================================
//
void ImportWizard::setupSrcConn()
{
    d->srcConnPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(d->srcConnPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    d->srcConn = new KexiConnectionSelectorWidget(Kexi::connset(),
                                                 "kfiledialog:///ProjectMigrationSourceDir",
                                                 KAbstractFileWidget::Opening, d->srcConnPageWidget);

    d->srcConn->hideConnectonIcon();
    d->srcConn->showSimpleConn();

    QSet<QString> excludedFilters;
//! @todo remove when support for kexi files as source prj is added in migration
    excludedFilters += KexiDB::defaultFileBasedDriverMimeType();
    excludedFilters += "application/x-kexiproject-shortcut";
    excludedFilters += "application/x-kexi-connectiondata";
    d->srcConn->fileWidget->setExcludedFilters(excludedFilters);

// d->srcConn->hideHelpers();
    vbox->addWidget(d->srcConn);

    d->srcConnPageItem = new KPageWidgetItem(d->srcConnPageWidget, i18n("Select Location for Source Database"));
    addPage(d->srcConnPageItem);
}

//===========================================================
//
void ImportWizard::setupSrcDB()
{
// arrivesrcdbPage creates widgets on that page
    d->srcDBPageWidget = new QWidget(this);
    d->srcDBName = NULL;

    d->srcDBPageItem = new KPageWidgetItem(d->srcDBPageWidget, i18n("Select Source Database"));
    addPage(d->srcDBPageItem);
}

//===========================================================
//
void ImportWizard::setupDstType()
{
    d->dstTypePageWidget = new QWidget(this);

    KexiDB::DriverManager manager;
    KexiDB::Driver::InfoHash drvs = manager.driversInfo();

    QVBoxLayout *vbox = new QVBoxLayout(d->dstTypePageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    QHBoxLayout *hbox = new QHBoxLayout;
    vbox->addLayout(hbox);
    KexiUtils::setStandardMarginsAndSpacing(hbox);
    QLabel *lbl = new QLabel(i18n("Destination database type:") /*+ ' '*/, d->dstTypePageWidget);
    lbl->setAlignment(Qt::AlignAuto | Qt::AlignTop);
    hbox->addWidget(lbl);

    d->dstPrjTypeSelector = new KexiPrjTypeSelector(d->dstTypePageWidget);
    hbox->addWidget(d->dstPrjTypeSelector);
    d->dstPrjTypeSelector->option_file->setText(i18n("Database project stored in a file"));
    d->dstPrjTypeSelector->option_server->setText(i18n("Database project stored on a server"));

    QVBoxLayout *frame_server_vbox = new QVBoxLayout(d->dstPrjTypeSelector->frame_server);
    KexiUtils::setStandardMarginsAndSpacing(frame_server_vbox);
    d->dstServerTypeCombo = new KexiDBDriverComboBox(d->dstPrjTypeSelector->frame_server, drvs,
            KexiDBDriverComboBox::ShowServerDrivers);
    frame_server_vbox->addWidget(d->dstServerTypeCombo);
    hbox->addStretch(1);
    vbox->addStretch(1);
    lbl->setBuddy(d->dstServerTypeCombo);

//! @todo hardcoded: find a way to preselect default engine item
    //d->dstTypeCombo->setCurrentText("SQLite3");

    d->dstTypePageItem = new KPageWidgetItem(d->dstTypePageWidget, i18n("Select Destination Database Type"));
    addPage(d->dstTypePageItem);
}

//===========================================================
//
void ImportWizard::setupDstTitle()
{
    d->dstTitlePageWidget = new KexiDBTitlePage(i18n("Destination project's title:"), this);
    d->dstTitlePageWidget->layout()->setMargin(KDialog::marginHint());
    d->dstTitlePageWidget->updateGeometry();
    d->dstNewDBNameLineEdit = d->dstTitlePageWidget->le_title;

    d->dstTitlePageItem = new KPageWidgetItem(d->dstTitlePageWidget, i18n("Select Destination Database Project's Title"));
    addPage(d->dstTitlePageItem);
}

//===========================================================
//
void ImportWizard::setupDst()
{
    d->dstPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(d->dstPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    d->dstConn = new KexiConnectionSelectorWidget(Kexi::connset(),
                                                 "kfiledialog:///ProjectMigrationDestinationDir",
                                                 KAbstractFileWidget::Saving, d->dstPageWidget);
    d->dstConn->hideHelpers();

    vbox->addWidget(d->dstConn);
    connect(d->dstConn, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(next()));

// d->dstConn->hideHelpers();
    d->dstConn->showSimpleConn();
    //anyway, db files will be _saved_
    d->dstConn->fileWidget->setMode(KexiFileWidget::SavingFileBasedDB);
// d->dstConn->hideHelpers();
// d->dstConn->d->file->btn_advanced->hide();
// d->dstConn->d->file->label->hide();
// d->dstConn->d->file->lbl->hide();
    //d->dstConn->d->file->spacer7->hide();


    //js dstNewDBName = new KLineEdit(dstControls);
    //   dstNewDBName->setText(i18n("Enter new database name here"));

    d->dstPageItem = new KPageWidgetItem(d->dstPageWidget, i18n("Select Location for Destination Database"));
    addPage(d->dstPageItem);
}

//===========================================================
//
void ImportWizard::setupImportType()
{
    d->importTypePageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(d->importTypePageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    d->importTypeGroupBox = new QGroupBox(d->importTypePageWidget);

    vbox->addWidget(d->importTypeGroupBox);
    QVBoxLayout *importTypeGroupBoxLyr = new QVBoxLayout;

    importTypeGroupBoxLyr->addWidget(
        d->importTypeStructureAndDataCheckBox = new QRadioButton(i18n("Structure and data"), d->importTypeGroupBox));
    d->importTypeStructureAndDataCheckBox->setChecked(true);

    importTypeGroupBoxLyr->addWidget(
        d->importTypeStructureOnlyCheckBox = new QRadioButton(i18n("Structure only"), d->importTypeGroupBox));

    importTypeGroupBoxLyr->addStretch(1);
    d->importTypeGroupBox->setLayout(importTypeGroupBoxLyr);

    d->importTypePageItem = new KPageWidgetItem(d->importTypePageWidget, i18n("Select Type of Import"));
    addPage(d->importTypePageItem);
}

//===========================================================
//
void ImportWizard::setupImporting()
{
    d->importingPageWidget = new QWidget(this);
    d->importingPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(d->importingPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    d->lblImportingTxt = new QLabel(d->importingPageWidget);
    d->lblImportingTxt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    d->lblImportingTxt->setWordWrap(true);

    d->lblImportingErrTxt = new QLabel(d->importingPageWidget);
    d->lblImportingErrTxt->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    d->lblImportingErrTxt->setWordWrap(true);

    d->progressBar = new QProgressBar(d->importingPageWidget);
    d->progressBar->setRange(0, 100);
    d->progressBar->hide();

    vbox->addWidget(d->lblImportingTxt);
    vbox->addWidget(d->lblImportingErrTxt);
    vbox->addStretch(1);

    QWidget *options_widget = new QWidget(d->importingPageWidget);
    vbox->addWidget(options_widget);
    QVBoxLayout *options_vbox = new QVBoxLayout(options_widget);
    options_vbox->setSpacing(KDialog::spacingHint());
    QHBoxLayout *importOptionsButtonLyr = new QHBoxLayout;
    options_vbox->addLayout(importOptionsButtonLyr);
    d->importOptionsButton = new KPushButton(koIcon("configure"),
                                            i18n("Advanced Options"), options_widget);
    connect(d->importOptionsButton, SIGNAL(clicked()),
            this, SLOT(slotOptionsButtonClicked()));
    importOptionsButtonLyr->addStretch(1);
    importOptionsButtonLyr->addWidget(d->importOptionsButton);
    importOptionsButtonLyr->addStretch(1);
    options_vbox->addStretch(1);

    vbox->addWidget(d->progressBar);
    vbox->addStretch(2);
    d->importingPageWidget->show();

    d->importingPageItem = new KPageWidgetItem(d->importingPageWidget, i18n("Importing"));
    addPage(d->importingPageItem);
}

//===========================================================
//
void ImportWizard::setupFinish()
{
    d->finishPageWidget = new QWidget(this);
    d->finishPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(d->finishPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    d->finishLbl = new QLabel(d->finishPageWidget);
    d->finishLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    d->finishLbl->setWordWrap(true);

    vbox->addWidget(d->finishLbl);
    d->openImportedProjectCheckBox = new QCheckBox(i18n("Open imported project"),
            d->finishPageWidget);
    d->openImportedProjectCheckBox->setChecked(true);
    vbox->addSpacing(KDialog::spacingHint());
    vbox->addWidget(d->openImportedProjectCheckBox);
    vbox->addStretch(1);

    d->finishPageItem = new KPageWidgetItem(d->finishPageWidget, i18n("Success"));
    addPage(d->finishPageItem);
}

//===========================================================
//
bool ImportWizard::checkUserInput()
{
    QString finishtxt;

    if (d->dstNewDBNameLineEdit->text().isEmpty()) {
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
        d->lblImportingErrTxt->setText(finishtxt);
    }

    return finishtxt.isNull();
}

void ImportWizard::arriveSrcConnPage()
{
    d->srcConnPageWidget->hide();

// checkIfSrcTypeFileBased(d->srcTypeCombo->currentText());
// if (fileBasedSrcSelected()) {
//moved  d->srcConn->showSimpleConn();
    /*! @todo KexiFileWidget needs "open file" and "open server" modes
    in addition to just "open" */
    if (d->setupFileBasedSrcNeeded) {
        d->setupFileBasedSrcNeeded = false;
        QSet<QString> additionalMimeTypes;
        /* moved
            if (d->srcTypeCombo->currentText().contains("Access")) {
        //! @todo tmp: hardcoded!
              additionalMimeTypes << "application/vnd.ms-access";
            }*/
        d->srcConn->fileWidget->setMode(KexiFileWidget::Opening);
        d->srcConn->fileWidget->setAdditionalFilters(additionalMimeTypes);

    }

    /*! @todo Support different file extensions based on MigrationDriver */
    d->srcConnPageWidget->show();
}

void ImportWizard::arriveSrcDBPage()
{
    if (fileBasedSrcSelected()) {
        //! @todo Back button doesn't work after selecting a file to import
        //moved showPage(d->dstTypePage);
    } else if (!d->srcDBName) {
        d->srcDBPageWidget->hide();
        kDebug() << "Looks like we need a project selector widget!";

        KexiDB::ConnectionData* condata = d->srcConn->selectedConnectionData();
        if (condata) {
            d->prjSet = new KexiProjectSet(*condata);
            QVBoxLayout *vbox = new QVBoxLayout(d->srcDBPageWidget);
            KexiUtils::setStandardMarginsAndSpacing(vbox);
            d->srcDBName = new KexiProjectSelectorWidget(d->srcDBPageWidget, d->prjSet);
            vbox->addWidget(d->srcDBName);
            d->srcDBName->label()->setText(i18n("Select source database you wish to import:"));
        }
        d->srcDBPageWidget->show();
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
        d->dstNewDBNameLineEdit->setText(suggestedDBName);
    }
    else {
        if (d->predefinedConnectionData) {
            // server source db is predefined
            d->dstNewDBNameLineEdit->setText(d->predefinedDatabaseName);
        } else {
            if (!d->srcDBName || !d->srcDBName->selectedProjectData()) {
                back(); //todo!
                return;
            }
            d->dstNewDBNameLineEdit->setText(d->srcDBName->selectedProjectData()->databaseName());
        }
    }
}

void ImportWizard::arriveDstPage()
{
    d->dstPageWidget->hide();

// checkIfDstTypeFileBased(d->dstTypeCombo->currentText());
    if (fileBasedDstSelected()) {
        d->dstConn->showSimpleConn();
        d->dstConn->fileWidget->setMode(KexiFileWidget::SavingFileBasedDB);
        if (!d->fileBasedDstWasPresented) {
            //without extension - it will be added automatically
            d->dstConn->fileWidget->setLocationText(d->dstNewDBNameLineEdit->text());
        }
        d->fileBasedDstWasPresented = true;
    } else {
        d->dstConn->showAdvancedConn();
    }
    d->dstPageWidget->show();
}

void ImportWizard::arriveImportingPage()
{
    d->importingPageWidget->hide();
    if (checkUserInput()) {
        //setNextEnabled(d->importingPageWidget, true);
        enableButton(KDialog::User2, true);
    } else {
        //setNextEnabled(d->importingPageWidget, false);
        enableButton(KDialog::User2, false);
    }

    d->lblImportingTxt->setText(i18n(
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
        d->importOptionsButton->show();
    else
        d->importOptionsButton->hide();

    d->importingPageWidget->show();
}

void ImportWizard::arriveFinishPage()
{
// backButton()->hide();
// cancelButton()->setEnabled(false);
// d->finishLbl->setText( d->successText.arg(d->dstNewDBNameLineEdit->text()) );
}

bool ImportWizard::fileBasedSrcSelected() const
{
    if (d->predefinedConnectionData)
        return false;

// kDebug() << (d->srcConn->selectedConnectionType()==KexiConnectionSelectorWidget::FileBased);
    return d->srcConn->selectedConnectionType() == KexiConnectionSelectorWidget::FileBased;
}

bool ImportWizard::fileBasedDstSelected() const
{

    return d->dstPrjTypeSelector->buttonGroup->selectedId() == 1;

}

void ImportWizard::progressUpdated(int percent)
{
    d->progressBar->setValue(percent);
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
        return ptr ? d->migrateManager.driverForMimeType(ptr.data()->name()) : QString();
    }

    //server-based
    if (d->predefinedConnectionData) {
        return d->predefinedConnectionData->driverName;
    }

    return d->srcConn->selectedConnectionData()
           ? d->srcConn->selectedConnectionData()->driverName : QString();
}

//===========================================================
//
void ImportWizard::accept()
{
    if (d->args) {
        if ((!fileBasedDstSelected() && !d->args->contains("destinationConnectionShortcut"))
                || !d->openImportedProjectCheckBox->isChecked()) {
            //do not open dest db if used didn't want it
            //for server connections, destinationConnectionShortcut must be defined
            d->args->remove("destinationDatabaseName");
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
                                     d->dstConn->selectedConnectionData() ? d->dstConn->selectedConnectionData()->driverName //server based
                                     : KexiDB::defaultFileBasedDriverName()
                                     // : d->dstTypeCombo->currentText() //file based
                                 );
    if (!destDriver || manager.error()) {
        result.setStatus(&manager);
        kDebug() << "Manager error...";
        manager.debugError();
//  result.setStatus(&manager);
    }

    // Set up destination connection data
    KexiDB::ConnectionData *cdata = 0;
    QString dbname;
    if (!result.error()) {
        if (d->dstConn->selectedConnectionData()) {
            //server-based project
            kDebug() << "Server destination...";
            cdata = d->dstConn->selectedConnectionData();
            dbname = d->dstNewDBNameLineEdit->text();
        } else { // if (d->dstTypeCombo->currentText().toLower() == KexiDB::defaultFileBasedDriverName())
            //file-based project
            kDebug() << "File Destination...";
            cdata = new KexiDB::ConnectionData();
            cdata->caption = d->dstNewDBNameLineEdit->text();
            cdata->driverName = KexiDB::defaultFileBasedDriverName();
            dbname = d->dstConn->selectedFileName();
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
                             d->migrateManager.possibleProblemsInfoMsg());
    }

    // Get a source (migration) driver
    KexiMigrate* sourceDriver = 0;
    if (!result.error()) {
        sourceDriver = d->migrateManager.driver(sourceDriverName);
        if (!sourceDriver || d->migrateManager.error()) {
            kDebug() << "Import migrate driver error...";
            result.setStatus(&d->migrateManager);
        }
    }

    KexiUtils::removeWaitCursor();

    // Set up source (migration) data required for connection
    if (sourceDriver && !result.error() && cdata) {
        // Setup progress feedback for the GUI
        if (sourceDriver->progressSupported()) {
            d->progressBar->updateGeometry();
            disconnect(sourceDriver, SIGNAL(progressPercent(int)),
                       this, SLOT(progressUpdated(int)));
            connect(sourceDriver, SIGNAL(progressPercent(int)),
                    this, SLOT(progressUpdated(int)));
            progressUpdated(0);
        }

        bool keepData;
        if (d->importTypeStructureAndDataCheckBox->isChecked()) {
            kDebug() << "Structure and data selected";
            keepData = true;
        } else if (d->importTypeStructureOnlyCheckBox->isChecked()) {
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
            if (d->predefinedConnectionData)
                md->source = d->predefinedConnectionData;
            else
                md->source = d->srcConn->selectedConnectionData();

            if (!d->predefinedDatabaseName.isEmpty())
                md->sourceName = d->predefinedDatabaseName;
            else
                md->sourceName = d->srcDBName->selectedProjectData()->databaseName();
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
    d->importExecuted = true;

    Kexi::ObjectStatus result;
    KexiMigrate* sourceDriver = prepareImport(result);

    bool acceptingNeeded = false;

    // Perform import
    if (sourceDriver && !result.error()) {
        if (!d->sourceDBEncoding.isEmpty()) {
            sourceDriver->setPropertyValue("source_database_nonunicode_encoding",
                                           QVariant(d->sourceDBEncoding.toUpper().replace(' ', QString())) // "CP1250", not "cp 1250"
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
        d->progressBar->show();
    }

    if (sourceDriver && !result.error() && sourceDriver->performImport(&result)) {
        if (d->args) {
//    if (fileBasedDstSelected()) {
            d->args->insert("destinationDatabaseName",
                           sourceDriver->data()->destination->databaseName());
//    }
            QString destinationConnectionShortcut(
                Kexi::connset().fileNameForConnectionData(d->dstConn->selectedConnectionData()));
            if (!destinationConnectionShortcut.isEmpty()) {
                d->args->insert("destinationConnectionShortcut", destinationConnectionShortcut);
            }
        }
        d->finishPageItem->setHeader(i18n("Success"));
        return true;
    }

    if (!sourceDriver || result.error()) {
        d->progressBar->setValue(0);
        d->progressBar->hide();

        QString msg, details;
        KexiTextMessageHandler handler(msg, details);
        handler.showErrorMessage(&result);

        kDebug() << msg << "\n" << details;

        d->finishPageItem->setHeader(i18n("Failure"));
        d->finishLbl->setText(
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
    if (currentPage() == d->srcConnPageItem) {
        if (fileBasedSrcSelected()
                && /*! @todo use KUrl? */!QFileInfo(selectedSourceFileName()).isFile()) {

            KMessageBox::sorry(this, i18n("Select source database filename."));
            return;
        }

        if ((! fileBasedSrcSelected()) && (! d->srcConn->selectedConnectionData())) {
            KMessageBox::sorry(this, i18n("Select source database."));
            return;
        }

        KexiMigrate* import = d->migrateManager.driver(driverNameForSelectedSource());
        if (!import || d->migrateManager.error()) {
            QString dbname;
            if (fileBasedSrcSelected())
                dbname = selectedSourceFileName();
            else
                dbname = d->srcConn->selectedConnectionData()
                         ? d->srcConn->selectedConnectionData()->serverInfoString() : QString();
            KMessageBox::error(this,
                               dbname.isEmpty() ?
                               i18n("Could not import database. This type is not supported.")
                               : i18n("Could not import database \"%1\". This type is not supported.", dbname));
            return;
        }
    } else if (currentPage() == d->dstPageItem) {
        if (d->fileBasedDstWasPresented) {
            if (fileBasedDstSelected() && !d->dstConn->fileWidget->checkSelectedFile())
                return;
        }
    } else if (currentPage() == d->importingPageItem) {
        if (!d->importExecuted) {
            d->importOptionsButton->hide();
            enableButton(KDialog::User2, false);
            enableButton(KDialog::User1, false);
            enableButton(KDialog::User3, false);
            d->lblImportingTxt->setText(i18n("Importing in progress..."));
            tristate res = import();
            if (true == res) {
                d->finishLbl->setText(
                    i18n("Database has been imported into Kexi database project \"%1\".",
                         d->dstNewDBNameLineEdit->text()));
                enableButtonCancel(false);
                enableButton(KDialog::User3, false);
                enableButton(KDialog::User1, true);
                d->openImportedProjectCheckBox->show();
                next();
                return;
            }

            d->progressBar->hide();

            enableButtonCancel(true);
            enableButton(KDialog::User3, true);
            enableButton(KDialog::User1, false);

            d->openImportedProjectCheckBox->hide();
            if (!res)
                next();
            else if (~res) {
                arriveImportingPage();
                //   back();
            }
            d->importExecuted = false;
            return;
        }
    }

    setAppropriate(d->srcDBPageItem, !fileBasedSrcSelected() && !d->predefinedConnectionData);   //skip d->srcDBPage
    KAssistantDialog::next();
}

void ImportWizard::back()
{
    setAppropriate(d->srcDBPageItem, !fileBasedSrcSelected() && !d->predefinedConnectionData);   //skip d->srcDBPage
    KAssistantDialog::back();
}

void ImportWizard::slot_currentPageChanged(KPageWidgetItem* curPage,KPageWidgetItem* prevPage)
{
    Q_UNUSED(prevPage);
    if (curPage == d->introPageItem) {
    }
    else if (curPage == d->srcConnPageItem) {
        arriveSrcConnPage();
    } else if (curPage == d->srcDBPageItem) {
        arriveSrcDBPage();
    } else if (curPage == d->dstTypePageItem) {
    } else if (curPage == d->dstTitlePageItem) {
        arriveDstTitlePage();
    } else if (curPage == d->dstPageItem) {
        arriveDstPage();
    } else if (curPage == d->importingPageItem) {
        arriveImportingPage();
    } else if (curPage == d->finishPageItem) {
        arriveFinishPage();
    }
}

void ImportWizard::helpClicked()
{
    if (currentPage() == d->introPageItem) {
        KMessageBox::information(this, i18n("No help is available for this page."), i18n("Help"));
    }
    /* else if (currentPage() == d->srcTypePage)
      {
        KMessageBox::information(this, i18n("Here you can choose the type of data to import data from."), i18n("Help"));
      }*/
    else if (currentPage() == d->srcConnPageItem) {
        KMessageBox::information(this, i18n("Here you can choose the location to import data from."), i18n("Help"));
    } else if (currentPage() == d->srcDBPageItem) {
        KMessageBox::information(this, i18n("Here you can choose the actual database to import data from."), i18n("Help"));
    } else if (currentPage() == d->dstTypePageItem) {
        KMessageBox::information(this, i18n("Here you can choose the location to save the data."), i18n("Help"));
    } else if (currentPage() == d->dstPageItem) {
        KMessageBox::information(this, i18n("Here you can choose the location to save the data in and the new database name."), i18n("Help"));
    } else if (currentPage() == d->finishPageItem || currentPage() == d->importingPageItem) {
        KMessageBox::information(this, i18n("No help is available for this page."), i18n("Help"));
    }
}

void ImportWizard::slotOptionsButtonClicked()
{
    QPointer<OptionsDialog> dlg = new OptionsDialog(selectedSourceFileName(), d->sourceDBEncoding, this);
    if (QDialog::Accepted == dlg->exec()) {
        if (d->sourceDBEncoding != dlg->encodingComboBox()->selectedEncoding()) {
            d->sourceDBEncoding = dlg->encodingComboBox()->selectedEncoding();
        }
    }
    delete dlg;
}

#include "importwizard.moc"
