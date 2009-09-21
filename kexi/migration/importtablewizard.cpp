/*
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "importtablewizard.h"
#include "migratemanager.h"
#include "keximigrate.h"
#include "keximigratedata.h"

#include <QSet>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStringList>
#include <QProgressBar>

#include <KPushButton>
#include <KDebug>

#include <kexidb/drivermanager.h>
#include <kexidb/driver.h>
#include <kexidb/connectiondata.h>
#include <kexidb/utils.h>
#include <core/kexidbconnectionset.h>
#include <core/kexi.h>
#include <KexiConnSelector.h>
#include <KexiProjectSelector.h>
#include <KexiNewProjectWizard.h>
#include <KexiDBTitlePage.h>
#include <kexiutils/utils.h>
#include <kexidbdrivercombobox.h>
#include <kexitextmsghandler.h>
#include <widget/kexicharencodingcombobox.h>
#include <widget/kexiprjtypeselector.h>
#include <main/startup/KexiStartupFileWidget.h>

using namespace KexiMigration;

ImportTableWizard::ImportTableWizard ( KexiDB::Connection* curDB, QWidget* parent, Qt::WFlags flags ) : KAssistantDialog ( parent, flags ) {
    m_currentDatabase = curDB;
    m_migrateDriver = 0;
    m_migrateManager = new MigrateManager();
    m_importComplete = false;
    
    setupIntroPage();
    setupSrcConn();
    setupTableSelectPage();
    setupImportingPage();
    setupFinishPage();
    
    connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)), this, SLOT(slot_currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)));
    
}


ImportTableWizard::~ImportTableWizard() {

}

void ImportTableWizard::back() {
    KAssistantDialog::back();
}

void ImportTableWizard::next() {
    if (currentPage() == m_importingPageItem) {
        if (m_importComplete) {
            KAssistantDialog::next();
        }
        else
        {
            doImport();
        }
    }
    else {
        KAssistantDialog::next();
    }
}

void ImportTableWizard::accept() {
    QDialog::accept();
}

void ImportTableWizard::reject() {
    QDialog::reject();
}

//===========================================================
//
void ImportTableWizard::setupIntroPage()
{
    m_introPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout();
    
    m_introPageWidget->setLayout(vbox);
    
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    
    QLabel *lblIntro = new QLabel(m_introPageWidget);
    lblIntro->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lblIntro->setWordWrap(true);
    QString msg;

    msg = i18n("Table importing wizard allows you to import a table from an existing database into the current Kexi database.");
 
    lblIntro->setText(msg + "\n\n"
    + i18n("Click \"Next\" button to continue or \"Cancel\" button to exit this wizard."));
    vbox->addWidget(lblIntro);
    
    m_introPageItem = new KPageWidgetItem(m_introPageWidget, i18n("Welcome to the Table Importing Wizard"));
    addPage(m_introPageItem);
}

void ImportTableWizard::setupSrcConn()
{
    m_srcConnPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_srcConnPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    
    m_srcConnSel = new KexiConnSelectorWidget(Kexi::connset(),
                                           "kfiledialog:///ProjectMigrationSourceDir",
                                           KAbstractFileWidget::Opening, m_srcConnPageWidget);
                                           
    m_srcConnSel->hideConnectonIcon();
    m_srcConnSel->showSimpleConn();

    QSet<QString> excludedFilters;
    //! @todo remove when support for kexi files as source prj is added in migration
    excludedFilters += KexiDB::defaultFileBasedDriverMimeType();
    excludedFilters += "application/x-kexiproject-shortcut";
    excludedFilters += "application/x-kexi-connectiondata";
    m_srcConnSel->fileWidget->setExcludedFilters(excludedFilters);

    kDebug() << m_migrateManager->supportedMimeTypes();
    m_srcConnSel->fileWidget->setAdditionalFilters(QSet<QString>::fromList(m_migrateManager->supportedMimeTypes()));
    
    // m_srcConn->hideHelpers();
    vbox->addWidget(m_srcConnSel);

    m_srcConnPageItem = new KPageWidgetItem(m_srcConnPageWidget, i18n("Select Location for Source Database"));
    addPage(m_srcConnPageItem);
}

void ImportTableWizard::setupTableSelectPage() {
    m_tablesPageWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(m_tablesPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);

    m_tableListWidget = new QListWidget(this);
    m_tableListWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
    
    vbox->addWidget(m_tableListWidget);
    
    m_tablesPageItem = new KPageWidgetItem(m_tablesPageWidget, i18n("Select the Tables to Import"));
    addPage(m_tablesPageItem);
}

//===========================================================
//
void ImportTableWizard::setupImportingPage()
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
    m_importOptionsButton = new KPushButton(KIcon("configure"), i18n("Advanced Options"), options_widget);
    connect(m_importOptionsButton, SIGNAL(clicked()),this, SLOT(slotOptionsButtonClicked()));
    options_vbox->addWidget(m_importOptionsButton);
    options_vbox->addStretch(1);

    vbox->addWidget(m_progressBar);
    vbox->addStretch(2);
    m_importingPageWidget->show();

    m_importingPageItem = new KPageWidgetItem(m_importingPageWidget, i18n("Importing"));
    addPage(m_importingPageItem);
}

void ImportTableWizard::setupFinishPage()
{
    m_finishPageWidget = new QWidget(this);
    m_finishPageWidget->hide();
    QVBoxLayout *vbox = new QVBoxLayout(m_finishPageWidget);
    KexiUtils::setStandardMarginsAndSpacing(vbox);
    m_finishLbl = new QLabel(m_finishPageWidget);
    m_finishLbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_finishLbl->setWordWrap(true);
    
    vbox->addWidget(m_finishLbl);
    vbox->addStretch(1);

    m_finishPageItem = new KPageWidgetItem(m_finishPageWidget, i18n("Success"));
    addPage(m_finishPageItem);
}

void ImportTableWizard::slot_currentPageChanged(KPageWidgetItem* curPage,KPageWidgetItem* prevPage)
{
    if (curPage == m_introPageItem) {
    }
    else if (curPage == m_srcConnPageItem) {
        arriveSrcConnPage();
    } else if (curPage == m_tablesPageItem) {
        arriveTableSelectPage();
    } else if (curPage == m_importingPageItem) {
        arriveImportingPage();
    } else if (curPage == m_finishPageItem) {
        arriveFinishPage();
    }
}

void ImportTableWizard::arriveSrcConnPage()
{

}

void ImportTableWizard::arriveTableSelectPage()
{
    if (m_migrateDriver) {
        delete m_migrateDriver;
        m_migrateDriver = 0;
    }
    Kexi::ObjectStatus result;
    KexiUtils::WaitCursor wait;
    m_migrateDriver = prepareImport(result);

    if (m_migrateDriver) {
        if (!m_migrateDriver->connectSource())
            return;

        QStringList tableNames;
        if (m_migrateDriver->tableNames(tableNames)) {
            m_tableListWidget->addItems(tableNames);
        }
    }
    KexiUtils::removeWaitCursor();
    
}

void ImportTableWizard::arriveImportingPage()
{
    m_importingPageWidget->hide();
#if 0
    if (checkUserInput()) {
        //setNextEnabled(m_importingPageWidget, true);
        enableButton(KDialog::User2, true);
    } else {
        //setNextEnabled(m_importingPageWidget, false);
        enableButton(KDialog::User2, false);
    }
    #endif

    QString txt;

    txt = i18n("All required information has now "
    "been gathered. Click \"Next\" button to start importing.\n\n"
    "Depending on size of the tables this may take some time.\n\n"
    "You have chosen to import the following tables:\n");

    foreach(QListWidgetItem *table, m_tableListWidget->selectedItems()) {
        txt += "\t" + table->text() + "\n";
    }
    
    m_lblImportingTxt->setText(txt);
    
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


void ImportTableWizard::arriveFinishPage()
{

}

bool ImportTableWizard::fileBasedSrcSelected() const
{
    return m_srcConnSel->selectedConnectionType() == KexiConnSelectorWidget::FileBased;
}

KexiMigrate* ImportTableWizard::prepareImport(Kexi::ObjectStatus& result)
{
    // Find a source (migration) driver name
    QString sourceDriverName;
    
    sourceDriverName = driverNameForSelectedSource();
    if (sourceDriverName.isEmpty())
        result.setStatus(i18n("No appropriate migration driver found."),
                            m_migrateManager->possibleProblemsInfoMsg());

    
    // Get a source (migration) driver
    KexiMigrate* sourceDriver = 0;
    if (!result.error()) {
        sourceDriver = m_migrateManager->driver(sourceDriverName);
        if (!sourceDriver || m_migrateManager->error()) {
            kDebug() << "Import migrate driver error...";
            result.setStatus(m_migrateManager);
        }
    }
    
    // Set up source (migration) data required for connection
    if (sourceDriver && !result.error()) {
        #if 0
        // Setup progress feedback for the GUI
        if (sourceDriver->progressSupported()) {
            m_progressBar->updateGeometry();
            disconnect(sourceDriver, SIGNAL(progressPercent(int)),
                       this, SLOT(progressUpdated(int)));
                       connect(sourceDriver, SIGNAL(progressPercent(int)),
                               this, SLOT(progressUpdated(int)));
                               progressUpdated(0);
        }
        #endif
        
        bool keepData = true;

        #if 0
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
        #endif
        
        KexiMigration::Data* md = new KexiMigration::Data();
        
        if (fileBasedSrcSelected()) {
            KexiDB::ConnectionData* conn_data = new KexiDB::ConnectionData();
            conn_data->setFileName(m_srcConnSel->selectedFileName());
            md->source = conn_data;
            md->sourceName = "";
        } else {
            md->source = m_srcConnSel->selectedConnectionData();
            //md->sourceName = m_srcDBName->selectedProjectData()->databaseName();
            
        }
        md->keepData = keepData;
        sourceDriver->setData(md);
        
        return sourceDriver;
    }
    return 0;
}

//===========================================================
//
QString ImportTableWizard::driverNameForSelectedSource()
{
    if (fileBasedSrcSelected()) {
        KMimeType::Ptr ptr = KMimeType::findByFileContent(m_srcConnSel->selectedFileName());
        if (!ptr
            || ptr.data()->name() == "application/octet-stream"
            || ptr.data()->name() == "text/plain") {
            //try by URL:
            ptr = KMimeType::findByUrl(m_srcConnSel->selectedFileName());
        }
    return ptr ? m_migrateManager->driverForMimeType(ptr.data()->name()) : QString();
    }

return m_srcConnSel->selectedConnectionData() ? m_srcConnSel->selectedConnectionData()->driverName : QString();
}

bool ImportTableWizard::doImport()
{
    QString tableName;
    KexiDB::TableSchema ts;
    
    foreach(QListWidgetItem *table, m_tableListWidget->selectedItems()) {
        tableName = table->text();
        if (m_migrateDriver->readTableSchema(tableName, ts)) {
            m_currentDatabase->createTable(&ts, true);
        }

    }

    m_importComplete = true;
    
}
