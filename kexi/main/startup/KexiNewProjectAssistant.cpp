/* This file is part of the KDE project
   Copyright (C) 2003-2011 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiNewProjectAssistant.h"

#include "ui_KexiServerDBNamePage.h"
#include "KexiTemplatesModel.h"
#include "KexiStartupFileHandler.h"

#include <kexi.h>
#include <kexiprojectset.h>
#include <kexiprojectdata.h>
#include <kexiguimsghandler.h>
#include <kexitextmsghandler.h>
#include <kexidb/utils.h>
#include <kexidb/object.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiLinkWidget.h>
#include <widget/KexiFileWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiDBTitlePage.h>
#include <widget/KexiProjectSelectorWidget.h>

#include <kapplication.h>
#include <kiconloader.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kurlcombobox.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kurlcombobox.h>
#include <KTitleWidget>
#include <KCategoryDrawer>
#include <KPushButton>
#include <KAcceleratorManager>
#include <KFileDialog>

#include <qpushbutton.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <QPaintEvent>
#include <QPainter>
#include <QProgressBar>
 
// added because of lack of krecentdirs.h
namespace KRecentDirs
{
    KDE_IMPORT void add(const QString &fileClass, const QString &directory);
};

class KexiServerDBNamePage : public QWidget, public Ui::KexiServerDBNamePage
{
public:
    KexiServerDBNamePage(QWidget* parent = 0);
};

KexiServerDBNamePage::KexiServerDBNamePage(QWidget* parent)
 : QWidget(parent)
{
    setupUi(this);
}

// ----

KexiTemplateSelectionPage::KexiTemplateSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("New Project"),
                  i18n("Kexi will create a new database project. Select blank database or template."),
                  parent)
{
    m_templatesList = new KexiCategorizedView;
    setFocusWidget(m_templatesList);
    m_templatesList->setFrameShape(QFrame::NoFrame);
    m_templatesList->setContentsMargins(0, 0, 0, 0);
    int margin = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, 0)
        + KDialog::marginHint();
    //m_templatesList->setCategorySpacing(5 + margin);
    //not needed in grid:
    m_templatesList->setSpacing(margin);
    m_templatesList->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(m_templatesList, SIGNAL(clicked(QModelIndex)), this, SLOT(slotItemClicked(QModelIndex)));

    KexiTemplateCategoryInfoList templateCategories;
    KexiTemplateCategoryInfo templateCategory;
    templateCategory.name = "blank";
    templateCategory.caption = i18n("Blank Projects");
    
    KexiTemplateInfo info;
    info.name = "blank";
    info.caption = i18n("Blank database");
    info.description = i18n("Database project without any objects");
    info.icon = KIcon(KexiDB::defaultFileBasedDriverIcon()); //"x-office-document");
    templateCategory.addTemplate(info);
    templateCategories.append(templateCategory);
    
    templateCategory = KexiTemplateCategoryInfo();
    templateCategory.name = "office";
    templateCategory.caption = i18n("Office Templates");
    //templateCategory.enabled = false;
    
    info = KexiTemplateInfo();
    info.name = "contacts";
    info.caption = i18n("Contacts");
    info.description = i18n("Database for collecting and managing contacts");
    info.icon = KIcon("view-pim-contacts");
    //info.enabled = false;
    templateCategory.addTemplate(info);
    
    info = KexiTemplateInfo();
    info.name = "movie";
    info.caption = i18n("Movie catalog");
    info.description = i18n("Database for collecting movies");
    info.icon = KIcon("video-x-genenric");
    //info.enabled = false;
    templateCategory.addTemplate(info);
    templateCategories.append(templateCategory);
    
    KexiTemplatesProxyModel* proxyModel = new KexiTemplatesProxyModel(m_templatesList);
    KexiTemplatesModel* model = new KexiTemplatesModel(templateCategories);
    proxyModel->setSourceModel(model);
    m_templatesList->setModel(proxyModel);

    kDebug() << "templatesCategoryDrawer:" << m_templatesList->categoryDrawer();

    setContents(m_templatesList);
}

void KexiTemplateSelectionPage::slotItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    selectedTemplate = index.data(KexiTemplatesModel::NameRole).toString();
    selectedCategory = index.data(KexiTemplatesModel::CategoryRole).toString();
    m_templatesList->clearSelection();

    //! @todo support templates
    if (selectedTemplate == "blank" && selectedCategory == "blank") {
        next();
        return;
    }
    KEXI_UNFINISHED(i18n("Templates"));
}

// ----

KexiProjectStorageTypeSelectionPage::KexiProjectStorageTypeSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("Storage Method"),
                  i18n("Select a storage method which will be used to store the new project."),
                  parent)
 , m_fileTypeSelected(true)
{
    setBackButtonVisible(true);
    QWidget* contents = new QWidget;
    setupUi(contents);
    int dsize = KIconLoader::global()->currentSize(KIconLoader::Desktop);
    btn_file->setIcon(KIcon(KexiDB::defaultFileBasedDriverIcon()));
    btn_file->setIconSize(QSize(dsize, dsize));
    connect(btn_file, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    btn_server->setIcon(KIcon(KEXI_ICON_DATABASE_SERVER));
    btn_server->setIconSize(QSize(dsize, dsize));
    connect(btn_server, SIGNAL(clicked()), this, SLOT(buttonClicked()));
    setFocusWidget(btn_file);

    setContents(contents);
}

KexiProjectStorageTypeSelectionPage::~KexiProjectStorageTypeSelectionPage()
{
}

void KexiProjectStorageTypeSelectionPage::buttonClicked()
{
    m_fileTypeSelected = sender() == btn_file;
    next();
}

// ----

static QString defaultDatabaseName()
{
    return i18n("New database");
}

KexiProjectTitleSelectionPage::KexiProjectTitleSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("Project Title & Filename"),
                  i18n("Enter title for the new project. "
                       "Filename will created automatically based on the title. "
                       "You can change the filename too."),
                  parent)
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    contents = new KexiDBTitlePage(QString());
    contents->formLayout->setSpacing(KDialog::spacingHint());
    contents->le_title->setText(defaultDatabaseName());
    contents->le_title->selectAll();
    connect(contents->le_title, SIGNAL(textChanged(QString)),
            this, SLOT(titleTextChanged(QString)));
    fileHandler = new KexiStartupFileHandler(
        KUrl("kfiledialog:///OpenExistingOrCreateNewProject"),
        KexiStartupFileHandler::SavingFileBasedDB,
        contents->file_requester);
    connect(fileHandler, SIGNAL(askForOverwriting(KexiContextMessage)),
            this, SLOT(askForOverwriting(KexiContextMessage)));

    contents->file_requester->fileDialog()->setCaption(i18n("Save New Project As"));
    updateUrl();

    setContents(contents);
}

KexiProjectTitleSelectionPage::~KexiProjectTitleSelectionPage()
{
    delete fileHandler;
}

void KexiProjectTitleSelectionPage::askForOverwriting(const KexiContextMessage& message)
{
    kDebug() << message.text();
    delete messageWidget;
    messageWidget = new KexiContextMessageWidget(this,
                                                 contents->formLayout,
                                                 contents->file_requester, message);
    messageWidget->setNextFocusWidget(contents->le_title);
}

void KexiProjectTitleSelectionPage::titleTextChanged(const QString & text)
{
    Q_UNUSED(text);
    updateUrl();
//    nextButton()->setEnabled(!text.trimmed().isEmpty());
}

void KexiProjectTitleSelectionPage::updateUrl()
{
    KUrl url = contents->file_requester->url();
    QString fn = KexiUtils::string2FileName(contents->le_title->text());
    if (!fn.isEmpty() && !fn.endsWith(".kexi"))
        fn += ".kexi";
    url.setFileName(fn);
    contents->file_requester->setUrl(url);
}

bool KexiProjectTitleSelectionPage::isAcceptable()
{
    delete messageWidget;
    if (contents->le_title->text().trimmed().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
                                                     contents->le_title,
                                                     i18n("Enter project title."));
        contents->le_title->setText(QString());
        return false;
    }
    KUrl url = contents->file_requester->url();
    if (!url.isValid() || !url.isLocalFile() || url.fileName().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->file_requester,
            i18n("Enter valid project filename. The file should be located on this computer."));
        return false;
    }
    if (!fileHandler->checkSelectedUrl()) {
        return false;
    }
    //urlSelected(url); // to save recent dir
    return true;
}

// ----

KexiProjectCreationPage::KexiProjectCreationPage(QWidget* parent)
 : KexiAssistantPage(i18n("Creating Project"),
                  i18n("Please wait while the project is created."),
                  parent)
{
    QVBoxLayout *vlyr = new QVBoxLayout;
    QHBoxLayout *lyr = new QHBoxLayout;
    vlyr->addLayout(lyr);
    m_progressBar = new QProgressBar;
    m_progressBar->setRange(0, 0);
    lyr->addWidget(m_progressBar);
    lyr->addStretch(1);
//! @todo add cancel
    vlyr->addStretch(1);
    setContents(vlyr);
}

KexiProjectCreationPage::~KexiProjectCreationPage()
{
}

// ----

KexiProjectConnectionSelectionPage::KexiProjectConnectionSelectionPage(QWidget* parent)
 : KexiAssistantPage(i18n("Database Connection"),
                  i18n("Select database server's connection you wish to use to "
                       "create a new Kexi project. "
                       "<p>Here you may also add, edit or remove connections "
                       "from the list."),
                  parent)
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);

    QVBoxLayout *lyr = new QVBoxLayout;
    connSelector = new KexiConnectionSelectorWidget(
        Kexi::connset(),
        "kfiledialog:///OpenExistingOrCreateNewProject",
        KAbstractFileWidget::Saving);
    lyr->addWidget(connSelector);
    connSelector->showAdvancedConn();
    connect(connSelector, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(next()));
    connSelector->layout()->setContentsMargins(0, 0, 0, 0);
    connSelector->hideHelpers();
    connSelector->hideDescription();
    setContents(lyr);
    setFocusWidget(connSelector->connectionsList());
}

KexiProjectConnectionSelectionPage::~KexiProjectConnectionSelectionPage()
{
}

// ----

KexiProjectDatabaseNameSelectionPage::KexiProjectDatabaseNameSelectionPage(
    KexiNewProjectAssistant* parent)
 : KexiAssistantPage(i18n("Project Title & Database Name"),
                  i18n("Enter title for the new project. "
                       "Database name will created automatically based on the title. "
                       "You can change the database name too."),
                  parent)
 , m_assistant(parent)
{
    m_projectDataToOverwrite = 0;
    m_messageWidgetActionYes = 0;
    m_messageWidgetActionNo = new QAction(KStandardGuiItem::no().text(), this);
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    nextButton()->setLinkText(i18n("Create"));

    m_projectSetToShow = 0;
    m_dbNameAutofill = true;
    m_le_dbname_txtchanged_enabled = true;
    contents = new KexiServerDBNamePage;
//! @todo
    m_msgHandler = new KexiGUIMessageHandler(this);

    connect(contents->le_title, SIGNAL(textChanged(QString)),
            this, SLOT(slotTitleChanged(QString)));
    connect(contents->le_dbname, SIGNAL(textChanged(QString)),
            this, SLOT(slotNameChanged(QString)));
    connect(contents->le_title, SIGNAL(returnPressed()),
            this, SLOT(next()));
    connect(contents->le_dbname, SIGNAL(returnPressed()),
            this, SLOT(next()));
    contents->le_title->setText(defaultDatabaseName());
    contents->le_title->selectAll();
    contents->le_dbname->setValidator(new KexiUtils::IdentifierValidator(this));
    m_projectSelector = new KexiProjectSelectorWidget(
        contents->frm_dblist, 0,
        true, // showProjectNameColumn
        false // showConnectionColumns
    );
    m_projectSelector->setFocusPolicy(Qt::NoFocus);
    m_projectSelector->setSelectable(false);
    m_projectSelector->list()->setFrameStyle(QFrame::NoFrame);
    GLUE_WIDGET(m_projectSelector, contents->frm_dblist);
    contents->layout()->setContentsMargins(0, 0, 0, 0);
    m_projectSelector->layout()->setContentsMargins(0, 0, 0, 0);
    
    setContents(contents);
    setFocusWidget(contents->le_title);
}

KexiProjectDatabaseNameSelectionPage::~KexiProjectDatabaseNameSelectionPage()
{
}

bool KexiProjectDatabaseNameSelectionPage::setConnection(KexiDB::ConnectionData* data)
{
    if (conndataToShow != data) {
        m_projectSelector->setProjectSet(0);
        conndataToShow = 0;
        if (data) {
            m_projectSetToShow = new KexiProjectSet(*data, m_assistant);
            if (m_projectSetToShow->error()) {
                delete m_projectSetToShow;
                m_projectSetToShow = 0;
                return false;
            }
            conndataToShow = data;
            //-refresh projects list
            m_projectSelector->setProjectSet(m_projectSetToShow);
        }
    }
    if (conndataToShow) {
        QString selectorLabel = i18n("Existing project databases on <b>%1 (%2)</b> database server:")
                .arg(conndataToShow->caption)
                .arg(conndataToShow->serverInfoString(true));
        m_projectSelector->label()->setText(selectorLabel);
    }
    return true;
}

void KexiProjectDatabaseNameSelectionPage::slotTitleChanged(const QString &capt)
{
    if (contents->le_dbname->text().isEmpty())
        m_dbNameAutofill = true;
    if (m_dbNameAutofill) {
        m_le_dbname_txtchanged_enabled = false;
        QString captionAsId = KexiUtils::string2Identifier(capt);
        contents->le_dbname->setText(captionAsId);
        m_projectDataToOverwrite = 0;
        m_le_dbname_txtchanged_enabled = true;
    }
}

void KexiProjectDatabaseNameSelectionPage::slotNameChanged(const QString &)
{
    if (!m_le_dbname_txtchanged_enabled)
        return;
    m_projectDataToOverwrite = 0;
    m_dbNameAutofill = false;
}

QString KexiProjectDatabaseNameSelectionPage::enteredDbName() const
{
    return contents->le_dbname->text().trimmed();
}

bool KexiProjectDatabaseNameSelectionPage::isAcceptable()
{
    delete messageWidget;
    if (contents->le_title->text().trimmed().isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
                                                     contents->le_title,
                                                     i18n("Enter project title."));
        contents->le_title->setText(QString());
        return false;
    }
    QString dbName(enteredDbName());
    if (dbName.isEmpty()) {
        messageWidget = new KexiContextMessageWidget(contents->formLayout,
            contents->le_dbname,
            i18n("Enter database name."));
        return false;
    }
    if (m_projectSetToShow) {
        KexiProjectData* projectData = m_projectSetToShow->findProject(dbName);
        if (projectData) {
            if (m_projectDataToOverwrite == projectData) {
                delete messageWidget;
                return true;
            }
            KexiContextMessage message(
                i18n("Database with this name already exists. "
                     "Do you want to delete it and create a new one?"));
            if (!m_messageWidgetActionYes) {
                m_messageWidgetActionYes = new QAction(i18n("Delete and Create New"),
                                                            this);
                connect(m_messageWidgetActionYes, SIGNAL(triggered()),
                        this, SLOT(overwriteActionTriggered()));
            }
            m_messageWidgetActionNo->setText(KStandardGuiItem::no().text());
            message.addAction(m_messageWidgetActionYes);
            message.setDefaultAction(m_messageWidgetActionNo);
            message.addAction(m_messageWidgetActionNo);
            messageWidget = new KexiContextMessageWidget(
                this, contents->formLayout,
                contents->le_dbname, message);
            messageWidget->setMessageType(KMessageWidget::Warning);
            messageWidget->setNextFocusWidget(contents->le_title);
            return false;
        }
    }
    return true;
}

void KexiProjectDatabaseNameSelectionPage::overwriteActionTriggered()
{
    m_projectDataToOverwrite = m_projectSetToShow->findProject(enteredDbName());
    next();
}

// ----

class KexiNewProjectAssistant::Private
{
public:
    Private(KexiNewProjectAssistant *qq)
     : q(qq)
    {
    }
    
    ~Private()
    {
    }
    
    KexiTemplateSelectionPage* templateSelectionPage() {
        return page<KexiTemplateSelectionPage>(&m_templateSelectionPage);
    }
    KexiProjectStorageTypeSelectionPage* projectStorageTypeSelectionPage() {
        return page<KexiProjectStorageTypeSelectionPage>(&m_projectStorageTypeSelectionPage);
    }
    KexiProjectTitleSelectionPage* titleSelectionPage() {
        return page<KexiProjectTitleSelectionPage>(&m_titleSelectionPage);
    }
    KexiProjectCreationPage* projectCreationPage() {
        return page<KexiProjectCreationPage>(&m_projectCreationPage);
    }
    KexiProjectConnectionSelectionPage* projectConnectionSelectionPage() {
        return page<KexiProjectConnectionSelectionPage>(&m_projectConnectionSelectionPage);
    }
    KexiProjectDatabaseNameSelectionPage* projectDatabaseNameSelectionPage() {
        return page<KexiProjectDatabaseNameSelectionPage>(&m_projectDatabaseNameSelectionPage, q);
    }
    
    template <class C>
    C* page(QPointer<C>* p, KexiNewProjectAssistant *parent = 0) {
        if (p->isNull()) {
            *p = new C(parent);
            q->addPage(*p);
        }
        return *p;
    }

    QPointer<KexiTemplateSelectionPage> m_templateSelectionPage;
    QPointer<KexiProjectStorageTypeSelectionPage> m_projectStorageTypeSelectionPage;
    QPointer<KexiProjectTitleSelectionPage> m_titleSelectionPage;
    QPointer<KexiProjectCreationPage> m_projectCreationPage;
    QPointer<KexiProjectConnectionSelectionPage> m_projectConnectionSelectionPage;
    QPointer<KexiProjectDatabaseNameSelectionPage> m_projectDatabaseNameSelectionPage;
    
    QAction* messageWidgetActionNo;
    QAction* messageWidgetActionTryAgain;
    QPointer<KexiContextMessageWidget> messageWidget;

    KexiNewProjectAssistant *q;
};

// ----

KexiNewProjectAssistant::KexiNewProjectAssistant(QWidget* parent)
 : KexiAssistantWidget(parent)
 , d(new Private(this))
{
    d->messageWidgetActionNo = 0;
    d->messageWidgetActionTryAgain = 0;
    setCurrentPage(d->templateSelectionPage());
    setFocusProxy(d->templateSelectionPage());
}

KexiNewProjectAssistant::~KexiNewProjectAssistant()
{
    delete d;
}
       
void KexiNewProjectAssistant::previousPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_projectStorageTypeSelectionPage) {
        setCurrentPage(d->templateSelectionPage());
    }
    else if (page == d->m_titleSelectionPage || page == d->m_projectConnectionSelectionPage) {
        setCurrentPage(d->projectStorageTypeSelectionPage());
    }
    else if (page == d->m_projectDatabaseNameSelectionPage) {
        setCurrentPage(d->projectConnectionSelectionPage());
    }
}

void KexiNewProjectAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_templateSelectionPage) {
        setCurrentPage(d->projectStorageTypeSelectionPage());
    }
    else if (page == d->m_projectStorageTypeSelectionPage) {
        if (d->projectStorageTypeSelectionPage()->fileTypeSelected()) {
            setCurrentPage(d->titleSelectionPage());
        }
        else {
            setCurrentPage(d->projectConnectionSelectionPage());
        }
    }
    else if (page == d->m_titleSelectionPage) {
        if (!d->titleSelectionPage()->isAcceptable()) {
            return;
        }
        //file-based project
        KexiDB::ConnectionData cdata;
        cdata.driverName = KexiDB::defaultFileBasedDriverName();
        cdata.setFileName(d->titleSelectionPage()->contents->file_requester->url().toLocalFile());
        createProject(cdata, cdata.fileName(), d->titleSelectionPage()->contents->le_title->text());
    }
    else if (page == d->m_projectConnectionSelectionPage) {
        KexiDB::ConnectionData *cdata
            = d->projectConnectionSelectionPage()->connSelector->selectedConnectionData();
        if (cdata) {
            if (d->projectDatabaseNameSelectionPage()->setConnection(cdata)) {
                setCurrentPage(d->projectDatabaseNameSelectionPage());
            }
        }
    }
    else if (page == d->m_projectDatabaseNameSelectionPage) {
        if (!d->m_projectDatabaseNameSelectionPage->conndataToShow
            || !d->m_projectDatabaseNameSelectionPage->isAcceptable())
        {
            return;
        }
        //server-based project
        createProject(*d->m_projectDatabaseNameSelectionPage->conndataToShow,
                      d->m_projectDatabaseNameSelectionPage->contents->le_dbname->text().trimmed(),
                      d->m_projectDatabaseNameSelectionPage->contents->le_title->text().trimmed());
    }
}

void KexiNewProjectAssistant::createProject(
    const KexiDB::ConnectionData& cdata, const QString& databaseName,
    const QString& caption)
{
    KexiProjectData *new_data = new KexiProjectData(cdata, databaseName, caption);
    setCurrentPage(d->projectCreationPage());
    emit createProject(new_data);
}

void KexiNewProjectAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //TODO?
}

void KexiNewProjectAssistant::showErrorMessage(
    const QString &title, const QString &details)
{
    Q_UNUSED(title);
    Q_UNUSED(details);
}

void KexiNewProjectAssistant::showErrorMessage(
    KexiDB::Object *obj, const QString& msg)
{
    QString _msg, _details;
    if (!obj) {
        showErrorMessage(_msg);
        return;
    }
    //QString _details(details);
    KexiTextMessageHandler textHandler(_msg, _details);
    textHandler.showErrorMessage(obj, msg);
    //KexiDB::getHTMLErrorMesage(obj, _msg, _details);
    //showErrorMessage(_msg, _details);

    KexiContextMessage message(_msg); 
    //! @todo + _details
    if (!d->messageWidgetActionTryAgain) {
        d->messageWidgetActionTryAgain = new QAction(
            KIcon("view-refresh"), i18n("Try Again"), this);
        connect(d->messageWidgetActionTryAgain, SIGNAL(triggered()),
                this, SLOT(tryAgainActionTriggered()));
    }
    if (!d->messageWidgetActionNo) {
        d->messageWidgetActionNo = new QAction(KStandardGuiItem::no().text(), this);
    }
    d->messageWidgetActionNo->setText(KStandardGuiItem::cancel().text());
    message.addAction(d->messageWidgetActionTryAgain);
    message.setDefaultAction(d->messageWidgetActionNo);
    message.addAction(d->messageWidgetActionNo);
    delete d->messageWidget;
    d->messageWidget = new KexiContextMessageWidget(
        this, 0 /*contents->formLayout*/,
        0/*contents->le_dbname*/, message);
    //d->messageWidget->setNextFocusWidget(contents->le_title);
    d->messageWidget->setCalloutPointerDirection(KMessageWidget::Right);
    QWidget *b = currentPage()->nextButton();
    d->messageWidget->setCalloutPointerPosition(
        b->mapToGlobal(QPoint(0, b->height() / 2)));
}

void KexiNewProjectAssistant::tryAgainActionTriggered()
{
    d->m_projectConnectionSelectionPage->next();
}

#include "KexiNewProjectAssistant.moc"
