/* This file is part of the KDE project
   Copyright (C) 2004-2009 Adam Pigg <adam@piggz.co.uk>
   Copyright (C) 2004-2014 Jarosław Staniek <staniek@kde.org>
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
#include <QApplication>
#include <QLineEdit>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPushButton>

#include <kmessagebox.h>
#include <kdebug.h>

#include <KexiIcon.h>

#include <db/drivermanager.h>
#include <db/driver.h>
#include <db/connectiondata.h>
#include <db/utils.h>
#include <core/KexiMainWindowIface.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>
#include <kexiutils/identifier.h>
#include <kexidbdrivercombobox.h>
#include <kexitextmsghandler.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/kexiprjtypeselector.h>
#include <widget/KexiFileWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiProjectSelectorWidget.h>
#include <widget/KexiDBTitlePage.h>
#include <widget/KexiDBPasswordDialog.h>
#include <widget/KexiStartupFileHandler.h>

using namespace KexiMigration;

class ImportWizard::Private
{
public:

    Private(QMap<QString, QString>* args_)
      : srcProjectSelector(0)
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

    KexiPrjTypeSelector *dstPrjTypeSelector;

    KexiConnectionSelectorWidget *srcConn, *dstConn;
    QLineEdit *dstNewDBTitleLineEdit;
    QLabel *dstNewDBNameLabel;
    QLineEdit *dstNewDBNameLineEdit;

    QLabel *dstNewDBNameUrlLabel;
    KUrlRequester *dstNewDBNameUrl;
    KexiStartupFileHandler *dstNewDBFileHandler;
    KexiProjectSelectorWidget *srcProjectSelector;

    QLabel *lblImportingTxt, *lblImportingErrTxt, *finishLbl;
    QCheckBox *openImportedProjectCheckBox;
    bool fileBasedDstWasPresented;
    bool setupFileBasedSrcNeeded;
    bool importExecuted; //!< used in import()
    KexiProjectSet* prjSet;
    QProgressBar *progressBar;
    QPushButton* importOptionsButton;
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
    setWindowTitle(i18nc("@title:window", "Import Database"));
    setWindowIcon(KexiIcon(koIconName("database-import")));

    KexiMainWindowIface::global()->setReasonableDialogSize(this);

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
        msg = i18nc("@info",
                    "Database Importing Assistant is about to import <resource>%1</resource> database "
                    "(connection <resource>%2</resource>) into a Kexi project.",
                   d->predefinedDatabaseName, d->predefinedConnectionData->serverInfoString());
    } else if (!d->predefinedDatabaseName.isEmpty()) { //predefined import: file source
//! @todo this message is currently ok for files only
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForName(d->predefinedMimeType);
        if (!mime.isValid()) {
            KexiDBWarn << QString("'%1' mimetype not installed!").arg(d->predefinedMimeType);
        }
        msg = i18nc("@info",
                    "Database Importing Assistant is about to import <filename>%1</filename> file "
                    "of type <resource>%2</resource> into a Kexi project.",
                    QDir::convertSeparators(d->predefinedDatabaseName), mime.isValid() ? mime.comment() : "???");
    } else {
        msg = i18n("Database Importing Assistant allows you to import an existing database "
                   "into a Kexi project.");
    }
    lblIntro->setText(msg + "\n\n"
                      + i18n("Click <interface>Next</interface> button to continue or "
                             "<interface>Cancel</interface> button to exit this assistant."));
    vbox->addWidget(lblIntro);

    d->introPageItem = new KPageWidgetItem(d->introPageWidget,
                                           i18n("Welcome to the Database Importing Assistant"));
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
    lbl->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    hbox->addWidget(lbl);

    d->dstPrjTypeSelector = new KexiPrjTypeSelector(d->dstTypePageWidget);
    hbox->addWidget(d->dstPrjTypeSelector);
    d->dstPrjTypeSelector->option_file->setText(i18n("Database project stored in a file"));
    d->dstPrjTypeSelector->option_server->setText(i18n("Database project stored on a server"));
    hbox->addStretch(1);
    vbox->addStretch(1);

    d->dstTypePageItem = new KPageWidgetItem(d->dstTypePageWidget, i18n("Select Destination Database Type"));
    addPage(d->dstTypePageItem);
}

//===========================================================
//
void ImportWizard::setupDstTitle()
{
    d->dstTitlePageWidget = new KexiDBTitlePage(i18n("Destination project's caption:"), this);
    d->dstTitlePageWidget->layout()->setMargin(KDialog::marginHint());
    d->dstTitlePageWidget->updateGeometry();
    d->dstNewDBTitleLineEdit = d->dstTitlePageWidget->le_title;
    connect(d->dstNewDBTitleLineEdit, SIGNAL(textChanged(QString)),
            this, SLOT(destinationTitleTextChanged(QString)));
    d->dstNewDBNameUrlLabel = d->dstTitlePageWidget->label_requester;
    d->dstNewDBNameUrl = d->dstTitlePageWidget->file_requester;
    d->dstNewDBFileHandler = new KexiStartupFileHandler(
        QUrl("kfiledialog:///ProjectMigrationDestinationDir"),
        KexiStartupFileHandler::SavingFileBasedDB,
        d->dstTitlePageWidget->file_requester);
    d->dstNewDBNameLabel = new QLabel(i18n("Destination project's name:"), d->dstTitlePageWidget);
    d->dstTitlePageWidget->formLayout->setWidget(2, QFormLayout::LabelRole, d->dstNewDBNameLabel);
    d->dstNewDBNameLineEdit = new QLineEdit(d->dstTitlePageWidget);
    d->dstNewDBNameLineEdit->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    KexiUtils::IdentifierValidator *idValidator = new KexiUtils::IdentifierValidator(this);
    idValidator->setLowerCaseForced(true);
    d->dstNewDBNameLineEdit->setValidator(idValidator);
    d->dstTitlePageWidget->formLayout->setWidget(2, QFormLayout::FieldRole, d->dstNewDBNameLineEdit);

    d->dstTitlePageItem = new KPageWidgetItem(d->dstTitlePageWidget, i18n("Enter Destination Database Project's Caption"));
    addPage(d->dstTitlePageItem);
}

void ImportWizard::destinationTitleTextChanged(const QString & text)
{
    Q_UNUSED(text);
    updateDestinationDBFileName();
}

void ImportWizard::updateDestinationDBFileName()
{
    d->dstNewDBFileHandler->updateUrl(d->dstNewDBTitleLineEdit->text());
    d->dstNewDBNameLineEdit->setText(d->dstNewDBTitleLineEdit->text());
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

    d->dstConn->showSimpleConn();
    //anyway, db files will be _saved_
    d->dstConn->fileWidget->setMode(KexiFileWidget::SavingFileBasedDB);
    d->dstPageItem = new KPageWidgetItem(d->dstPageWidget, i18n("Select Location for Destination Database Project"));
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
        d->importTypeStructureAndDataCheckBox = new QRadioButton(
            i18nc("Scope of import", "Structure and data"), d->importTypeGroupBox));
    d->importTypeStructureAndDataCheckBox->setChecked(true);

    importTypeGroupBoxLyr->addWidget(
        d->importTypeStructureOnlyCheckBox = new QRadioButton(
            i18nc("Scope of import", "Structure only"), d->importTypeGroupBox));

    importTypeGroupBoxLyr->addStretch(1);
    d->importTypeGroupBox->setLayout(importTypeGroupBoxLyr);

    d->importTypePageItem = new KPageWidgetItem(d->importTypePageWidget,
                                                i18n("Select Scope of Import"));
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
    d->importOptionsButton = new QPushButton(koIcon("configure"),
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

    if (d->dstNewDBTitleLineEdit->text().isEmpty()) {
        finishtxt = finishtxt + "<br>" + i18n("No new database name was entered.");
    }

    Kexi::ObjectStatus result;
    KexiMigrate* sourceDriver = prepareImport(result);
    if (sourceDriver && sourceDriver->isSourceAndDestinationDataSourceTheSame()) {
        finishtxt = finishtxt + "<br>" + i18n("Source database is the same as destination.");
    }

    if (! finishtxt.isNull()) {
        finishtxt = "<qt>" + i18n("Following issues were found with the data you entered:") +
                    "<br>" + finishtxt + "<br><br>" +
                    i18n("Please click <interface>Back</interface> button and correct these issues.");
        d->lblImportingErrTxt->setText(finishtxt);
    }

    return finishtxt.isNull();
}

void ImportWizard::arriveSrcConnPage()
{
    d->srcConnPageWidget->hide();

    /*! @todo KexiFileWidget needs "open file" and "open server" modes
    in addition to just "open" */
    if (d->setupFileBasedSrcNeeded) {
        d->setupFileBasedSrcNeeded = false;
        QSet<QString> additionalMimeTypes;
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
    }
    else {
        if (!d->srcProjectSelector) {
            QVBoxLayout *vbox = new QVBoxLayout(d->srcDBPageWidget);
            d->srcProjectSelector = new KexiProjectSelectorWidget(d->srcDBPageWidget);
            vbox->addWidget(d->srcProjectSelector);
            KexiUtils::setStandardMarginsAndSpacing(vbox);
            d->srcProjectSelector->label()->setText(i18n("Select source database you wish to import:"));
        }
        d->srcDBPageWidget->hide();
        KexiDB::ConnectionData* condata = d->srcConn->selectedConnectionData();
        Q_ASSERT(condata);
        Q_ASSERT(d->prjSet);
        d->srcProjectSelector->setProjectSet(d->prjSet);
        d->srcDBPageWidget->show();
    }
}

void ImportWizard::arriveDstTitlePage()
{
    d->dstNewDBNameUrlLabel->setVisible(fileBasedDstSelected());
    d->dstNewDBNameUrl->setVisible(fileBasedDstSelected());
    d->dstNewDBNameLabel->setVisible(!fileBasedDstSelected());
    d->dstNewDBNameLineEdit->setVisible(!fileBasedDstSelected());
    if (fileBasedSrcSelected()) {
        const QString fname(selectedSourceFileName());
        QString suggestedDBName(QFileInfo(fname).fileName());
        const QFileInfo fi(suggestedDBName);
        suggestedDBName = suggestedDBName.left(suggestedDBName.length()
                                               - (fi.completeSuffix().isEmpty() ? 0 : (fi.completeSuffix().length() + 1)));
        d->dstNewDBTitleLineEdit->setText(suggestedDBName);
    }
    else {
        if (d->predefinedConnectionData) {
            // server source db is predefined
            d->dstNewDBTitleLineEdit->setText(d->predefinedDatabaseName);
        } else {
            if (!d->srcProjectSelector || !d->srcProjectSelector->selectedProjectData()) {
                back(); //!< @todo
                return;
            }
            d->dstNewDBTitleLineEdit->setText(d->srcProjectSelector->selectedProjectData()->databaseName());
        }
    }
    d->dstNewDBTitleLineEdit->selectAll();
    d->dstNewDBTitleLineEdit->setFocus();
    updateDestinationDBFileName();
}

void ImportWizard::arriveDstPage()
{
    if (fileBasedDstSelected()) {
        d->dstPageWidget->hide();
        KAssistantDialog::next();
        return;
    }
    else {
        d->dstConn->showAdvancedConn();
    }
    d->dstPageWidget->show();
}

void ImportWizard::arriveImportingPage()
{
    d->importingPageWidget->hide();
    if (checkUserInput()) {
        enableButton(KDialog::User2, true);
    } else {
        enableButton(KDialog::User2, false);
    }

    d->lblImportingTxt->setText(i18n(
                                   "<para>All required information has now "
                                   "been gathered. Click <interface>Next</interface> button to start importing.</para>"
                                   "<note>Depending on size of the database this may take some time.</note>"
                                   /*"Note: You may be asked for extra "
                                   "information such as field types if "
                                   "the wizard could not automatically "
                                   "determine this for you."*/));

    //temp. hack for MS Access driver only
//! @todo for other databases we will need KexiMigration::Conenction
//!       and KexiMigration::Driver classes
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
    return d->dstPrjTypeSelector->option_file->isChecked();
}

void ImportWizard::progressUpdated(int percent)
{
    d->progressBar->setValue(percent);
    qApp->processEvents();
}

//===========================================================
//
QString ImportWizard::driverNameForSelectedSource()
{
    if (fileBasedSrcSelected()) {
        QMimeDatabase db;
        QMimeType mime = db.mimeTypeForFile(selectedSourceFileName());
        if (!mime.isValid()
                || mime.name() == "application/octet-stream"
                || mime.name() == "text/plain"
                || mime.name() == "application/zip")
        {
            //try by URL:
            mime = db.mimeTypeForUrl(selectedSourceFileName());
        }
        return mime.isValid() ? d->migrateManager.driverForMimeType(mime.name()) : QString();
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

    //kDebug() << "Creating destination driver...";

    // Get a driver to the destination database
    KexiDB::Driver *destDriver = manager.driver(
                                     d->dstConn->selectedConnectionData() ? d->dstConn->selectedConnectionData()->driverName //server based
                                     : KexiDB::defaultFileBasedDriverName()
                                     // : d->dstTypeCombo->currentText() //file based
                                 );
    if (!destDriver || manager.error()) {
        result.setStatus(&manager);
        kWarning() << "Manager error...";
        manager.debugError();
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
        }
        else {
            //file-based project
            kDebug() << "File Destination...";
            cdata = new KexiDB::ConnectionData();
            cdata->caption = d->dstNewDBTitleLineEdit->text();
            cdata->driverName = KexiDB::defaultFileBasedDriverName();
            dbname = d->dstTitlePageWidget->file_requester->url().toLocalFile();
            cdata->setFileName(dbname);
            kDebug() << "Current file name: " << dbname;
        }
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
                md->sourceName = d->srcProjectSelector->selectedProjectData()->databaseName();
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
                                           QVariant(d->sourceDBEncoding.toUpper().remove(' ')) // "CP1250", not "cp 1250"
                                          );
        }
        if (!sourceDriver->checkIfDestinationDatabaseOverwritingNeedsAccepting(&result, acceptingNeeded)) {
            kDebug() << "Abort import cause checkIfDestinationDatabaseOverwritingNeedsAccepting "
            "returned false.";
            return false;
        }

        //kDebug() << sourceDriver->data()->destination->databaseName();
        //kDebug() << "Performing import...";
    }

    if (sourceDriver && !result.error() && acceptingNeeded) {
        // ok, the destination-db already exists...
        if (KMessageBox::Yes != KMessageBox::warningYesNo(this,
                "<qt>" + i18n("<p>Database %1 already exists.</p>"
                              "<p>Do you want to replace it with a new one?</p>",
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
            d->args->insert("destinationDatabaseName",
                            fileBasedDstSelected() ? sourceDriver->data()->destination->connectionData()->fileName()
                                                   : sourceDriver->data()->destination->databaseName());
            QString destinationConnectionShortcut;
            if (d->dstConn->selectedConnectionData()) {
                destinationConnectionShortcut
                    = Kexi::connset().fileNameForConnectionData(*d->dstConn->selectedConnectionData());
            }
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
                "<para>Import failed.</para><para>%1</para><para>%2</para><para>You can click <interface>Back</interface> button and try again.</para>",
                msg, details));
        return false;
    }
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
                && /*! @todo use QUrl? */!QFileInfo(selectedSourceFileName()).isFile()) {

            KMessageBox::sorry(this, i18n("Select source database filename."));
            return;
        }

        KexiDB::ConnectionData* condata = d->srcConn->selectedConnectionData();
        if (!fileBasedSrcSelected() && !condata) {
            KMessageBox::sorry(this, i18n("Select source database."));
            return;
        }

        KexiMigrate* import = d->migrateManager.driver(driverNameForSelectedSource());
        if (!import || d->migrateManager.error()) {
            QString dbname;
            if (fileBasedSrcSelected())
                dbname = selectedSourceFileName();
            else
                dbname = condata ? condata->serverInfoString() : QString();
            KMessageBox::error(this,
                               dbname.isEmpty() ?
                               i18n("Could not import database. This type is not supported.")
                               : i18n("Could not import database \"%1\". This type is not supported.", dbname));
            return;
        }

        if (!fileBasedSrcSelected()) {
            // make sure we have password if needed
            tristate passwordNeeded = false;
            if (condata->password.isNull()) {
                passwordNeeded = KexiDBPasswordDialog::getPasswordIfNeeded(condata, this);
            }
            bool ok = passwordNeeded != cancelled;
            if (ok) {
                KexiGUIMessageHandler handler;
                d->prjSet = new KexiProjectSet(condata, &handler);
                ok = !d->prjSet->error();
            }
            if (!ok) {
                if (passwordNeeded == true) {
                    condata->password = QString::null; // not clear(), we have to remove password
                }
                delete d->prjSet;
                d->prjSet = 0;
                return;
            }
        }
    } else if (currentPage() == d->dstPageItem) {
//        if (d->fileBasedDstWasPresented) {
//            if (fileBasedDstSelected() && !d->dstConn->fileWidget->checkSelectedFile())
//                return;
//        }
    } else if (currentPage() == d->importTypePageItem) {
        if (!fileBasedDstSelected()) {
            // make sure we have password if needed
            tristate passwordNeeded = false;
            KexiDB::ConnectionData* condata = d->dstConn->selectedConnectionData();
            if (condata->password.isNull()) {
                passwordNeeded = KexiDBPasswordDialog::getPasswordIfNeeded(condata, this);
            }
            bool ok = passwordNeeded != cancelled;
            if (!ok) {
                if (passwordNeeded == true) {
                    condata->password = QString::null; // not clear(), we have to remove password
                }
                return;
            }
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
                    i18n("Database has been imported into Kexi project \"%1\".",
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
            }
            d->importExecuted = false;
            return;
        }
    }

    setAppropriate(d->srcDBPageItem, !fileBasedSrcSelected() && !d->predefinedConnectionData);
    setAppropriate(d->dstPageItem, !fileBasedDstSelected());
    KAssistantDialog::next();
}

void ImportWizard::back()
{
    setAppropriate(d->srcDBPageItem, !fileBasedSrcSelected() && !d->predefinedConnectionData);
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
        if (fileBasedDstSelected()) {
            if (prevPage == d->importTypePageItem) {
                KAssistantDialog::back();
            }
            else {
                KAssistantDialog::next();
            }
        }
        else {
            arriveDstPage();
        }
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

