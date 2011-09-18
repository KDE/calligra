/* This file is part of the KDE project
   Copyright (C) 2011 Jarosław Staniek <staniek@kde.org>

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

#include "KexiOpenProjectAssistant.h"
#include "KexiProjectSelector.h"
#include "KexiConnSelector.h"
#include "KexiStartupFileWidget.h"

#include <QTimer>
#include <QVBoxLayout>

#include <KTabWidget>

#include <kexiutils/KexiLinkWidget.h>
#include <kexidb/utils.h>
#include <kexiprojectset.h>
#include <kexiprojectdata.h>
#include <kexi.h>

KexiMainOpenProjectPage::KexiMainOpenProjectPage(QWidget* parent)
 : KexiAssistantPage(i18n("Open Project"),
                  i18n("Select project to open. "
                       "You can choose project stored in file or on database server."),
                  parent)
{
    setNextButtonVisible(true);

    tabWidget = new KTabWidget;
    tabWidget->setElideMode(Qt::ElideNone);
    tabWidget->setAutomaticResizeTabs(true);
    tabWidget->setDocumentMode(true);

    m_fileSelectorWidget = new QWidget;
    tabWidget->addTab(m_fileSelectorWidget, KIcon(KexiDB::defaultFileBasedDriverIcon()),
                      i18n("Projects Stored in File"));
    fileSelector = new KexiConnSelectorWidget(
        Kexi::connset(),
        "kfiledialog:///OpenExistingOrCreateNewProject",
        KAbstractFileWidget::Opening);
    fileSelector->hide(); // delayed opening
    fileSelector->showSimpleConn();
    fileSelector->layout()->setContentsMargins(0, 0, 0, 0);
    fileSelector->hideHelpers();
    fileSelector->hideDescription();
    //connect(fileSelector->fileWidget, SIGNAL(accepted()), this, SLOT(accept()));
    connect(fileSelector->fileWidget, SIGNAL(fileHighlighted()),
            this, SLOT(next()));
                      
    m_connSelectorWidget = new QWidget;
    tabWidget->addTab(m_connSelectorWidget, KIcon(KEXI_ICON_DATABASE_SERVER),
                      i18n("Projects Stored on Database Server"));

    setFocusWidget(tabWidget);
    setContents(tabWidget);
    
    // delayed opening:
    QTimer::singleShot(500, this, SLOT(init()));
}

void KexiMainOpenProjectPage::init()
{
    // file-based:
    QVBoxLayout* fileSelectorLayout = new QVBoxLayout(m_fileSelectorWidget);
    fileSelectorLayout->setContentsMargins(0, KDialog::marginHint() * 2, 0, 0);
    fileSelectorLayout->addWidget(fileSelector);
    fileSelector->show();
                      
    // server-based:
    QVBoxLayout* connSelectorLayout = new QVBoxLayout(m_connSelectorWidget);
    connSelectorLayout->setContentsMargins(0, KDialog::marginHint() * 2, 0, 0);
    QLabel* connSelectorLabel = new QLabel(
        i18n("Select database server's connection with project you wish to open. "
            "<p>Here you may also add, edit or remove connections "
            "from the list."));
    connSelectorLayout->addWidget(connSelectorLabel);
    connSelectorLayout->addSpacing(KDialog::marginHint());
    connSelector = new KexiConnSelectorWidget(
        Kexi::connset(),
        "kfiledialog:///OpenExistingOrCreateNewProject",
        KAbstractFileWidget::Opening);
    connSelectorLayout->addWidget(connSelector);
    
    connSelector->showAdvancedConn();
    connSelector->layout()->setContentsMargins(0, 0, 0, 0);
    connSelector->hideHelpers();
    connSelector->hideDescription();
    connect(connSelector, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(next()));
}

KexiMainOpenProjectPage::~KexiMainOpenProjectPage()
{
}

// ----

KexiProjectDatabaseSelectionPage::KexiProjectDatabaseSelectionPage(
   KexiOpenProjectAssistant* parent)
 : KexiAssistantPage(i18n("Open Project on Database Server"), QString(), parent)
 , m_assistant(parent)
{
    setBackButtonVisible(true);
    setNextButtonVisible(true);
    nextButton()->setLinkText(i18n("Open"));

    projectSelector = new KexiProjectSelectorWidget(
        this, 0,
        true, // showProjectNameColumn
        false // showConnectionColumns
    );
    projectSelector->label()->hide();
    connect(projectSelector, SIGNAL(projectExecuted(KexiProjectData*)),
            m_assistant, SLOT(slotOpenProject(KexiProjectData*)));

    setFocusWidget(projectSelector);
    setContents(projectSelector);
}

KexiProjectDatabaseSelectionPage::~KexiProjectDatabaseSelectionPage()
{
}

bool KexiProjectDatabaseSelectionPage::setConnection(KexiDB::ConnectionData* data)
{
    if (conndataToShow != data) {
        projectSelector->setProjectSet(0);
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
            projectSelector->setProjectSet(m_projectSetToShow);
        }
    }
    if (conndataToShow) {
        setDescription(i18n("Select project on database server <b>%1 (%2)</b> to open.")
                   .arg(conndataToShow->caption)
                   .arg(conndataToShow->serverInfoString(true)));
    }
    return true;
}

// ----

class KexiOpenProjectAssistant::Private
{
public:
    Private(KexiOpenProjectAssistant *qq)
     : q(qq)
    {
    }
    
    ~Private()
    {
    }
    
    KexiMainOpenProjectPage* projectOpenPage() {
        return page<KexiMainOpenProjectPage>(&m_projectOpenPage);
    }
    KexiProjectDatabaseSelectionPage* projectDatabaseSelectionPage() {
        return page<KexiProjectDatabaseSelectionPage>(&m_projectDatabaseSelectionPage, q);
    }
    
    template <class C>
    C* page(QPointer<C>* p, KexiOpenProjectAssistant *parent = 0) {
        if (p->isNull()) {
            *p = new C(parent);
            q->addPage(*p);
        }
        return *p;
    }

    QPointer<KexiMainOpenProjectPage> m_projectOpenPage;
    QPointer<KexiProjectDatabaseSelectionPage> m_projectDatabaseSelectionPage;
    
    KexiOpenProjectAssistant *q;
};

// ----

KexiOpenProjectAssistant::KexiOpenProjectAssistant(QWidget* parent)
 : KexiAssistantWidget(parent)
 , d(new Private(this))
{
    setCurrentPage(d->projectOpenPage());
    setFocusProxy(d->projectOpenPage());
}

KexiOpenProjectAssistant::~KexiOpenProjectAssistant()
{
    delete d;
}
       
void KexiOpenProjectAssistant::previousPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_projectDatabaseSelectionPage) {
        setCurrentPage(d->projectOpenPage());
    }
}

void KexiOpenProjectAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_projectOpenPage) {
        if (d->m_projectOpenPage->tabWidget->currentIndex() == 0) {
            // file-based
            if (!d->m_projectOpenPage->fileSelector->fileWidget->checkSelectedFile())
                return;
            emit openProject(
                d->m_projectOpenPage->fileSelector->fileWidget->highlightedFile());
        }
        else { // server-based
            KexiDB::ConnectionData *cdata
                = d->m_projectOpenPage->connSelector->selectedConnectionData();
            if (cdata) {
                if (d->projectDatabaseSelectionPage()->setConnection(cdata)) {
                    setCurrentPage(d->projectDatabaseSelectionPage());
                }
            }
        }
    }
    else if (page == d->m_projectDatabaseSelectionPage) {
        slotOpenProject(
            d->m_projectDatabaseSelectionPage->projectSelector->selectedProjectData());
    }
}

void KexiOpenProjectAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //TODO?
}

void KexiOpenProjectAssistant::showErrorMessage(
    const QString &title, const QString &details)
{
}

void KexiOpenProjectAssistant::showErrorMessage(
    KexiDB::Object *obj, const QString& msg)
{
#if 0
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
#endif
}

#if 0
void KexiOpenProjectAssistant::tryAgainActionTriggered()
{
    d->m_projectConnectionSelectionPage->next();
}
#endif

void KexiOpenProjectAssistant::slotOpenProject(KexiProjectData* data)
{
    if (data)
        emit openProject(*data);
}

#include "KexiOpenProjectAssistant.moc"
