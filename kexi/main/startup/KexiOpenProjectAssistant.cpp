/* This file is part of the KDE project
   Copyright (C) 2011-2013 Jarosław Staniek <staniek@kde.org>
   Copyright (C) 2012 Dimitrios T. Tanis <dimitrios.tanis@kdemail.net>

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
#include "KexiStartup.h"
#include "KexiPasswordPage.h"
#include "ui_KexiProjectStorageTypeSelectionPage.h"

#include <widget/KexiProjectSelectorWidget.h>
#include <widget/KexiConnectionSelectorWidget.h>
#include <kexiutils/KexiLinkWidget.h>
#include <db/utils.h>
#include <kexiprojectset.h>
#include <kexiprojectdata.h>
#include <kexi.h>

#include <KoIcon.h>

#include <QVBoxLayout>

KexiMainOpenProjectPage::KexiMainOpenProjectPage(QWidget* parent)
 : KexiAssistantPage(i18n("Open Project"),
                  i18n("Select project to open. "
                       "You can choose project stored in file or on database server."),
                  parent)
 , m_errorMessagePopup(0)
{
    setNextButtonVisible(true);

    connSelector = new KexiConnectionSelectorWidget(
        Kexi::connset(),
        "kfiledialog:///OpenExistingOrCreateNewProject",
        QFileDialog::AcceptOpen);
    connSelector->layout()->setContentsMargins(0, 0, 0, 0);
    connSelector->hideDescription();
    connect(connSelector, SIGNAL(connectionItemExecuted(ConnectionDataLVItem*)),
            this, SLOT(next()));
    connect(connSelector, SIGNAL(fileSelected(bool)), this, SLOT(next()));

    setContents(connSelector);
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
    KexiPasswordPage* passwordPage() {
        return page<KexiPasswordPage>(&m_passwordPage, q);
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
    QPointer<KexiPasswordPage> m_passwordPage;

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

void KexiOpenProjectAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_projectOpenPage) {
        if (!d->m_projectOpenPage->connSelector->selectedFile().isEmpty()) {
            // file-based
            emit openProject(
                d->m_projectOpenPage->connSelector->selectedFile());
        }
        else { // server-based
            KexiDB::ConnectionData *cdata
                = d->m_projectOpenPage->connSelector->selectedConnectionData();
            if (cdata) {
                if (cdata->passwordNeeded()) {
                    d->passwordPage()->setConnectionData(*cdata);
                    setCurrentPage(d->passwordPage());
                    return;
                }
                if (d->projectDatabaseSelectionPage()->setConnection(cdata)) {
                    setCurrentPage(d->projectDatabaseSelectionPage());
                }
            }
        }
    }
    else if (page == d->m_passwordPage) {
        KexiDB::ConnectionData *cdata
            = d->projectOpenPage()->connSelector->selectedConnectionData();
        d->passwordPage()->updateConnectionData(cdata);
        if (cdata && d->projectDatabaseSelectionPage()->setConnection(cdata)) {
            setCurrentPage(d->projectDatabaseSelectionPage());
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

void KexiOpenProjectAssistant::slotOpenProject(KexiProjectData* data)
{
    if (data)
        emit openProject(*data);
}

void KexiOpenProjectAssistant::tryAgainActionTriggered()
{
    messageWidget()->animatedHide();
    currentPage()->next();
}

void KexiOpenProjectAssistant::cancelActionTriggered()
{
    if (currentPage() == d->m_passwordPage) {
        d->passwordPage()->focusWidget()->setFocus();
    }
}

QWidget* KexiOpenProjectAssistant::calloutWidget() const
{
    return currentPage()->nextButton();
}

#include "KexiOpenProjectAssistant.moc"
