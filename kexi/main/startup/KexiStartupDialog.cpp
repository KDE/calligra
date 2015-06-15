/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#include "KexiStartupDialog.h"
#include "kexi.h"
#include <widget/KexiProjectSelectorWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <widget/KexiFileWidget.h>
#include <kexiutils/utils.h>
#include <KexiIcon.h>

#include <KDbUtils>

#include <KComponentData>
#include <KConfig>
#include <KSharedConfig>
#include <KLocalizedString>

#include <QDebug>
#include <QCheckBox>
#include <QObject>
#include <QLabel>
#include <QKeyEvent>
#include <QEvent>
#include <QListView>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QVBoxLayout>

//! @internal
class KexiStartupDialog::Private
{
public:
    Private()
            : pageTemplates(0), pageOpenExisting(0)
            , templPageWidgetItem_BlankDatabase(0)
            , templPageWidgetItem_ImportExisting(0)
            , templPageWidgetItem_CreateFromTemplate(0)
    {
        result = -1;
        kexi_sqlite_icon = Kexi::defaultFileBasedDriverIcon();
        chkDoNotShow = 0;
        openExistingConnWidget = 0;
        templatesWidget = 0;
        templatesWidget_IconListView = 0;
    }
    ~Private() {
    }

    int dialogType, dialogOptions;

    KPageWidgetItem *pageTemplates, *pageOpenExisting;

    // subpages within "templates" page
    KPageWidgetItem *templPageWidgetItem_BlankDatabase,
    *templPageWidgetItem_ImportExisting, *templPageWidgetItem_CreateFromTemplate;
    QCheckBox *chkDoNotShow;

    //widgets for template tab:
    KPageWidget* templatesWidget;
    QListView *templatesWidget_IconListView;//helper

    int result;

    QIcon kexi_sqlite_icon;

    //! used for "open existing"
    KexiDBConnectionSet *connSet;
    KexiFileWidget *openExistingFileWidget; //! embedded file widget
    KexiConnectionSelectorWidget *openExistingConnWidget;
    KDbConnectionData* selectedExistingConnection; //! helper for returning selected connection

    //! true if the dialog contain single page, not tabs
    bool singlePage;
};

static QString captionForDialogType(int type)
{
    if (type == KexiStartupDialog::Templates)
        return xi18n("Create Project");
    else if (type == KexiStartupDialog::OpenExisting)
        return xi18n("Open Existing Project");

    return xi18n("Choose Project");
}

/*================================================================*/

KexiStartupDialog::KexiStartupDialog(
    int dialogType, int dialogOptions,
    KexiDBConnectionSet& connSet,
    QWidget *parent)
        : KPageDialog(parent)
        , d(new Private())
{
    d->connSet = &connSet;
    d->dialogType = dialogType;
    d->dialogOptions = dialogOptions;
    d->singlePage = dialogType == KexiStartupDialog::Templates
                    || dialogType == KexiStartupDialog::OpenExisting;
    setFaceType(d->singlePage ? Plain : Tabbed);
    setWindowTitle(captionForDialogType(dialogType));

    // buttons
    QPushButton *okButton = buttonBox()->button(QDialogButtonBox::Ok);
    okButton->setDefault(true);
    okButton->setShortcut(Qt::CTRL | Qt::Key_Return);

    if (dialogType == OpenExisting) {//this dialog has "open" tab only!
        setWindowIcon(koIcon("document-open"));
    } else {
        setWindowIcon(d->kexi_sqlite_icon);
    }

    setSizeGripEnabled(true);
    KPageWidgetItem *firstPage = 0;
    if (d->dialogType & Templates) {
        setupPageTemplates();
        //d->pageTemplatesID = id++;
        d->templatesWidget->setFocus();
        if (!firstPage)
            firstPage = d->pageTemplates;
    }
    if (d->dialogType & OpenExisting) {
        setupPageOpenExisting();
        if (d->singlePage)
            d->openExistingConnWidget->setFocus();
        if (!firstPage)
            firstPage = d->pageOpenExisting;
    }

    if (!d->singlePage) {
        connect(this, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
                this, SLOT(slotCurrentPageChanged(KPageWidgetItem*,KPageWidgetItem*)));
        d->templatesWidget->setFocus();
    }
    connect(okButton, SIGNAL(clicked()), this, SLOT(slotOk()));
    setCurrentPage(firstPage);
    updateDialogOKButton(firstPage);
    adjustSize();
}

KexiStartupDialog::~KexiStartupDialog()
{
    delete d;
}

void KexiStartupDialog::showEvent(QShowEvent *e)
{
    KPageDialog::showEvent(e);
    //just some cleanup
    d->result = -1;

    QDialog::centerOnScreen(this);
}

int KexiStartupDialog::result() const
{
    return d->result;
}

void KexiStartupDialog::done(int r)
{
    if (d->result != -1) //already done!
        return;

// qDebug() << r;
// updateSelectedTemplateKeyInfo();

    if (r == QDialog::Rejected) {
        d->result = CancelResult;
    } else {
        KPageWidgetItem *currentPageWidgetItem = currentPage();

        if (currentPageWidgetItem == d->pageTemplates) {
            KPageWidgetItem *currenTemplatesPageWidgetItem = d->templatesWidget->currentPage();
            if (currenTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase)
                d->result = CreateBlankResult;
#ifdef KEXI_PROJECT_TEMPLATES
            else if (currenTemplatesPageWidgetItem == d->templPageWidgetItem_CreateFromTemplate)
                d->result = CreateFromTemplateResult;
#endif
            else if (currenTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting)
                d->result = ImportResult;
        } else if (currentPageWidgetItem == d->pageOpenExisting) {
            // return file or connection:
            if (d->openExistingConnWidget->selectedConnectionType()
                    == KexiConnectionSelectorWidget::FileBased) {
                if (!d->openExistingFileWidget->checkSelectedFile())
                    return;
                d->openExistingFileWidget->accept();
                d->selectedExistingConnection = 0;
            } else {
                d->selectedExistingConnection
                = d->openExistingConnWidget->selectedConnectionData();
            }
            d->result = OpenExistingResult;
        } else
            return;
    }

    //save settings
    KConfigGroup group = KSharedConfig::openConfig()->group("Startup");
    if (d->openExistingConnWidget)
        group.writeEntry("OpenExistingType",
                         (d->openExistingConnWidget->selectedConnectionType() == KexiConnectionSelectorWidget::FileBased)
                         ? "File" : "Server");
    if (d->chkDoNotShow)
        group.writeEntry("ShowStartupDialog", !d->chkDoNotShow->isChecked());

    group.sync();

    KPageDialog::done(r);
}

void KexiStartupDialog::reject()
{
    KPageDialog::reject();
}

void KexiStartupDialog::setupPageTemplates()
{
    QFrame *pageTemplatesFrame = new QFrame(this);
    d->pageTemplates = addPage(pageTemplatesFrame, xi18n("Create Project"));
    QVBoxLayout *lyr = new QVBoxLayout(pageTemplatesFrame);
    lyr->setSpacing(KexiUtils::spacingHint());
    lyr->setMargin(0);

    d->templatesWidget = new KPageWidget(pageTemplatesFrame);
    d->templatesWidget->setObjectName("templatesWidget");
    d->templatesWidget->setFaceType(KPageWidget::List);
    {
        d->templatesWidget_IconListView = KexiUtils::findFirstChild<QListView*>(d->templatesWidget, "QListView");
        if (d->templatesWidget_IconListView)
            d->templatesWidget_IconListView->installEventFilter(this);
    }
    lyr->addWidget(d->templatesWidget);
    connect(d->templatesWidget, SIGNAL(currentPageChanged(KPageWidgetItem*,KPageWidgetItem*)),
            this, SLOT(slotCurrentTemplatesubpageChanged(KPageWidgetItem*,KPageWidgetItem*)));

    if (d->dialogOptions & CheckBoxDoNotShowAgain) {
        d->chkDoNotShow = new QCheckBox(xi18n("Do not show me this dialog again"), pageTemplatesFrame);
        d->chkDoNotShow->setObjectName("chkDoNotShow");
        lyr->addWidget(d->chkDoNotShow);
    }

    //template groups:
    QFrame *templPageWidget = 0;
    QVBoxLayout *tmplyr;

    //- page "blank db"
    QString clickMsg("\n\n" + xi18n("Click <interface>OK</interface> button to proceed."));
    templPageWidget = new QFrame(d->templatesWidget);
    d->templPageWidgetItem_BlankDatabase = d->templatesWidget->addPage(templPageWidget,
                                           xi18n("Blank Database"));
    d->templPageWidgetItem_BlankDatabase->setHeader(xi18n("New Blank Database Project"));
    d->templPageWidgetItem_BlankDatabase->setIcon(koIcon("x-office-document"));
    tmplyr = new QVBoxLayout(templPageWidget);
    tmplyr->setSpacing(KexiUtils::spacingHint());
    QLabel *lbl_blank = new QLabel(
        xi18n("Kexi will create a new blank database project.") + clickMsg, templPageWidget);
    lbl_blank->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    lbl_blank->setWordWrap(true);
    lbl_blank->setMargin(0);
    tmplyr->addWidget(lbl_blank);
    tmplyr->addStretch(1);

    //- page "import db"
    templPageWidget = new QFrame(d->templatesWidget);
    d->templPageWidgetItem_ImportExisting = d->templatesWidget->addPage(templPageWidget,
                                            xi18n("Import Existing Database"));
    d->templPageWidgetItem_ImportExisting->setHeader(
        xi18n("Import Existing Database as New Database Project"));
    d->templPageWidgetItem_ImportExisting->setIcon(KexiIcon(koIconName("database-import")));
    tmplyr = new QVBoxLayout(templPageWidget);
    tmplyr->setSpacing(KexiUtils::spacingHint());
    QLabel *lbl_import = new QLabel(
        xi18n("Kexi will import the structure and data of an existing database "
             "as a new database project.") + clickMsg, templPageWidget);
    lbl_import->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    lbl_import->setWordWrap(true);
    lbl_import->setMargin(0);
    tmplyr->addWidget(lbl_import);
    tmplyr->addStretch(1);
}

void KexiStartupDialog::slotCurrentPageChanged(KPageWidgetItem* current,
        KPageWidgetItem* before)
{
    Q_UNUSED(before);
    updateDialogOKButton(current);
}

void KexiStartupDialog::slotCurrentTemplatesubpageChanged(KPageWidgetItem* current,
        KPageWidgetItem* before)
{
    Q_UNUSED(before);
    if (current == d->templPageWidgetItem_BlankDatabase) {//blank
    } else if (current == d->templPageWidgetItem_ImportExisting) {
    }
#ifdef KEXI_PROJECT_TEMPLATES
    else if (current == d->templPageWidgetItem_CreateFromTemplate) {
        //! @todo d->viewTemplates->populate();
    }
#endif
    updateDialogOKButton(d->pageTemplates);
}

void KexiStartupDialog::updateDialogOKButton(KPageWidgetItem *pageWidgetItem)
{
    if (!pageWidgetItem) {
        pageWidgetItem = currentPage();
        if (!pageWidgetItem)
            return;
    }
    bool enable = true;
    if (pageWidgetItem == d->pageTemplates) {
        //int t_id = d->templatesWidget->activePageIndex();
        KPageWidgetItem *currenTemplatesPageWidgetItem = d->templatesWidget->currentPage();
#ifdef KEXI_PROJECT_TEMPLATES
        enable =
            currenTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase
            || currenTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting
            || (currenTemplatesPageWidgetItem == d->templPageWidgetItem_CreateFromTemplate
                /*! @todo && !d->viewTemplates->selectedFileName().isEmpty()*/);
#else
        enable = currenTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase
                 || currenTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting;
#endif
    } else if (pageWidgetItem == d->pageOpenExisting) {
        qDebug() << "d->openExistingFileWidget->highlightedFile(): " << d->openExistingFileWidget->highlightedFile();
        enable =
            (d->openExistingConnWidget->selectedConnectionType() == KexiConnectionSelectorWidget::FileBased)
            ? !d->openExistingFileWidget->highlightedFile().isEmpty()
            : (bool)d->openExistingConnWidget->selectedConnectionData();
//qDebug() << d->openExistingFileWidget->selectedFile() << "--------------";
    }
    QPushButton *okButton = buttonBox()->button(QDialogButtonBox::Ok);
    okButton->setEnabled(enable);
}

void KexiStartupDialog::setupPageOpenExisting()
{
    QWidget *pageOpenExistingWidget = new QFrame(this);
    d->pageOpenExisting = addPage(pageOpenExistingWidget, xi18n("Open Existing Project"));

    QVBoxLayout *lyr = new QVBoxLayout(pageOpenExistingWidget);
    lyr->setSpacing(KexiUtils::spacingHint());
    lyr->setMargin(0);

    d->openExistingConnWidget = new KexiConnectionSelectorWidget(d->connSet,
            "kfiledialog:///OpenExistingOrCreateNewProject", KFileWidget::Opening,
            pageOpenExistingWidget);
    d->openExistingConnWidget->setObjectName("KexiConnectionSelectorWidget");
    d->openExistingConnWidget->hideConnectonIcon();
    lyr->addWidget(d->openExistingConnWidget);
    KConfigGroup group = KSharedConfig::openConfig()->group("Startup");
    if (group.readEntry("OpenExistingType", "File") == "File")
        d->openExistingConnWidget->showSimpleConn();
    else {
        d->openExistingConnWidget->showSimpleConn();
        d->openExistingConnWidget->showAdvancedConn();
    }
    d->openExistingFileWidget = d->openExistingConnWidget->fileWidget;
    connect(d->openExistingFileWidget, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d->openExistingFileWidget, SIGNAL(fileHighlighted()),
            this, SLOT(existingFileHighlighted()));
    connect(d->openExistingConnWidget, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(connectionItemForOpenExistingExecuted(ConnectionDataLVItem*)));
    connect(d->openExistingConnWidget, SIGNAL(connectionItemHighlighted(ConnectionDataLVItem*)),
            this, SLOT(connectionItemForOpenExistingHighlighted(ConnectionDataLVItem*)));
}

void KexiStartupDialog::connectionItemForOpenExistingExecuted(ConnectionDataLVItem *item)
{
    if (!item)
        return;
    accept();
}

void KexiStartupDialog::connectionItemForOpenExistingHighlighted(ConnectionDataLVItem *item)
{
    QPushButton *okButton = buttonBox()->button(QDialogButtonBox::Ok);
    okButton->setEnabled(item);
}

void KexiStartupDialog::slotOk()
{
// qDebug();
}

void KexiStartupDialog::showSimpleConnForOpenExisting()
{
// qDebug() << "simple";
    d->openExistingConnWidget->showSimpleConn();
}

void KexiStartupDialog::showAdvancedConnForOpenExisting()
{
// qDebug() << "adv";
    d->openExistingConnWidget->showAdvancedConn();
}

QString KexiStartupDialog::selectedFileName() const
{
    if (d->result == OpenExistingResult)
        return d->openExistingFileWidget->highlightedFile();
#ifdef KEXI_PROJECT_TEMPLATES
    /*! @todo
    else if (d->result == CreateFromTemplateResult && d->viewTemplates)
        return d->viewTemplates->selectedFileName();*/
#endif
    else
        return QString();
}

KDbConnectionData* KexiStartupDialog::selectedExistingConnection() const
{
    return d->selectedExistingConnection;
}

void KexiStartupDialog::existingFileHighlighted()
{
    //qDebug();
    updateDialogOKButton(0);
}

KexiProjectData* KexiStartupDialog::selectedProjectData() const
{
    return 0;
}

//! used for accepting templates dialog with just return key press
bool KexiStartupDialog::eventFilter(QObject *o, QEvent *e)
{
    if (o == d->templatesWidget_IconListView && d->templatesWidget_IconListView) {
        bool tryAcept = false;
        if (   e->type() == QEvent::KeyPress
            && (   static_cast<QKeyEvent*>(e)->key() == Qt::Key_Enter
                || static_cast<QKeyEvent*>(e)->key() == Qt::Key_Return)
           )
        {
            tryAcept = true;
        }
        else if (e->type() == QEvent::MouseButtonDblClick) {
            tryAcept = true;
        }

        if (tryAcept) {
            KPageWidgetItem *currentTemplatesPageWidgetItem = d->templatesWidget->currentPage();
            if (   currentTemplatesPageWidgetItem == d->templPageWidgetItem_BlankDatabase
                || currentTemplatesPageWidgetItem == d->templPageWidgetItem_ImportExisting)
            {
                accept();
            }
        }
    }
    return KPageDialog::eventFilter(o, e);
}

void KexiStartupDialog::templateSelected(const QString& fileName)
{
    if (!fileName.isEmpty())
        accept();
}

#ifdef KEXI_PROJECT_TEMPLATES
KexiProjectData::AutoOpenObjects KexiStartupDialog::autoopenObjects() const
{
    /*! @todo if (d->result != CreateFromTemplateResult || !d->viewTemplates)
                  KexiProjectData::AutoOpenObjects();

              return d->viewTemplates->autoopenObjectsForSelectedTemplate();
    */
    return KexiProjectData::AutoOpenObjects();
}
#endif

