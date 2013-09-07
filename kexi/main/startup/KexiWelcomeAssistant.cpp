/* This file is part of the KDE project
   Copyright (C) 2011-2013 Jarosław Staniek <staniek@kde.org>

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

#include "KexiWelcomeAssistant.h"
#include "KexiRecentProjectsModel.h"
#include "KexiWelcomeStatusBar.h"
#include "KexiPasswordPage.h"
#include "KexiMainWindow.h"

#include <core/kexi.h>
#include <core/KexiRecentProjects.h>
#include <core/kexiprojectdata.h>
#include <core/kexiguimsghandler.h>
#include <core/kexitextmsghandler.h>
#include <db/utils.h>
#include <db/object.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiLinkWidget.h>

#include <kapplication.h>
#include <kmimetype.h>
#include <klocale.h>
#include <kdebug.h>
#include <kconfig.h>
#include <kurlcombobox.h>
#include <kmessagebox.h>
#include <klineedit.h>
#include <kcategorizedview.h>
#include <ktitlewidget.h>
#include <kpushbutton.h>
#include <kacceleratormanager.h>
#include <kfiledialog.h>
#include <kfileitemdelegate.h>

#include <QLayout>
#include <QCheckBox>
#include <QPaintEvent>
#include <QPainter>
#include <QProgressBar>
#include <QTimer>
 
KexiMainWelcomePage::KexiMainWelcomePage(
   KexiWelcomeAssistant* assistant, QWidget* parent)
 : KexiAssistantPage(i18n("Welcome to Kexi"),
                  i18n("Select one of the recently used projects to open."),
                  parent)
 , m_assistant(assistant)
{
    QWidget* contents = new QWidget;
    QHBoxLayout* contentsLyr = new QHBoxLayout(contents);
    
    m_recentProjects = new KexiCategorizedView;
    // do not alter background palette
    QPalette pal(m_recentProjects->palette());
    pal.setColor(QPalette::Disabled, QPalette::Base,
                    pal.color(QPalette::Normal, QPalette::Base));
    m_recentProjects->setPalette(pal);
    contentsLyr->addWidget(m_recentProjects, 1);
    //m_recentProjects->setItemDelegate(new KFileItemDelegate(this));
    setFocusWidget(m_recentProjects);
    m_recentProjects->setFrameShape(QFrame::NoFrame);
    m_recentProjects->setContentsMargins(0, 0, 0, 0);
    int margin = style()->pixelMetric(QStyle::PM_MenuPanelWidth, 0, 0)
        + KDialog::marginHint();
    //not needed in grid:
    m_recentProjects->setSpacing(margin);
    m_recentProjects->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    connect(m_recentProjects, SIGNAL(clicked(QModelIndex)), this, SLOT(slotItemClicked(QModelIndex)));
    
    m_statusBar = new KexiWelcomeStatusBar;
    contentsLyr->addWidget(m_statusBar);
    
    setContents(contents);

    QTimer::singleShot(100, this, SLOT(loadProjects()));
}

void KexiMainWelcomePage::loadProjects()
{
    m_recentProjectsProxyModel = new KexiRecentProjectsProxyModel(m_recentProjects);
    KexiRecentProjectsModel* model = new KexiRecentProjectsModel(*m_assistant->projects());
    m_recentProjectsProxyModel->setSourceModel(model);
    m_recentProjects->setModel(m_recentProjectsProxyModel);
    m_recentProjectsProxyModel->sort(0, Qt::DescendingOrder);
}

void KexiMainWelcomePage::slotItemClicked(const QModelIndex& index)
{
    if (!index.isValid())
        return;
    QModelIndex sourceIndex = m_recentProjectsProxyModel->mapToSource(index);
    KexiProjectData *pdata = static_cast<KexiProjectData*>(sourceIndex.internalPointer());
    kDebug() << *pdata;
    //selectedTemplate = index.data(KexiTemplatesModel::NameRole).toString();
    //selectedCategory = index.data(KexiTemplatesModel::CategoryRole).toString();
    //m_templatesList->clearSelection();

    if (pdata) {
        m_assistant->openProjectOrShowPasswordPage(pdata);
    }
}

void KexiMainWelcomePage::updateRecentProjects()
{
    m_recentProjects->update();
}

// ----

class KexiWelcomeAssistant::Private
{
public:
    Private(KexiWelcomeAssistant *qq)
     : q(qq)
    {
    }
    
    ~Private()
    {
        mainWindow->redirectMessagesTo(0);
    }
    
    KexiMainWelcomePage* mainWelcomePage() {
        return page<KexiMainWelcomePage>(&m_mainWelcomePage, q);
    }
    KexiPasswordPage* passwordPage() {
        return page<KexiPasswordPage>(&m_passwordPage, q);
    }

    template <class C>
    C* page(QPointer<C>* p, KexiWelcomeAssistant *parent = 0) {
        if (p->isNull()) {
            *p = new C(parent);
            q->addPage(*p);
        }
        return *p;
    }

    QPointer<KexiMainWelcomePage> m_mainWelcomePage;
    QPointer<KexiPasswordPage> m_passwordPage;

    QAction* messageWidgetActionNo;
    QAction* messageWidgetActionTryAgain;
    QPointer<KexiContextMessageWidget> messageWidget;

    KexiRecentProjects* projects;
    QPointer<KexiProjectData> projectData;

    KexiMainWindow *mainWindow;

    KexiWelcomeAssistant *q;
};

// ----

KexiWelcomeAssistant::KexiWelcomeAssistant(
    KexiRecentProjects* projects, KexiMainWindow* parent)
 : KexiAssistantWidget(parent)
 , d(new Private(this))
{
    d->mainWindow = parent;
    d->mainWindow->redirectMessagesTo(this);
    d->messageWidgetActionNo = 0;
    d->messageWidgetActionTryAgain = 0;
    d->projects = projects;
    setCurrentPage(d->mainWelcomePage());
    setFocusProxy(d->mainWelcomePage());
}

KexiWelcomeAssistant::~KexiWelcomeAssistant()
{
    delete d;
}

void KexiWelcomeAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_passwordPage) {
        if (d->projectData) {
            d->passwordPage()->updateConnectionData(d->projectData->connectionData());
            emitOpenProject(d->projectData);
        }
    }
    else {
        d->projectData = 0;
    }
}

void KexiWelcomeAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //TODO?
}

KexiRecentProjects* KexiWelcomeAssistant::projects()
{
    return d->projects;
}

// void KexiWelcomeAssistant::mousePressEvent(QMouseEvent* e)
// {
//     if (e->buttons() == Qt::LeftButton) {
//         QWidget *w = QApplication::widgetAt(e->globalPos());
//         if (w) {
//             emit widgetClicked(w);
//         }
//     }
//     KexiAssistantWidget::mousePressEvent(e);
// }

void KexiWelcomeAssistant::emitOpenProject(KexiProjectData *data)
{
    bool opened = false;
    emit openProject(*data, projects()->shortcutPath(*data), &opened);
    if (opened) { // update recent projects view
        data->setLastOpened(QDateTime::currentDateTime());
        d->m_mainWelcomePage->updateRecentProjects();
    }
}

void KexiWelcomeAssistant::openProjectOrShowPasswordPage(KexiProjectData *data)
{
    KexiDB::ConnectionData *cdata = data->connectionData();
    if (cdata) {
        if (cdata->passwordNeeded()) {
            d->projectData = data;
            d->passwordPage()->setConnectionData(*cdata);
            d->passwordPage()->showDatabaseName(true);
            d->passwordPage()->setDatabaseNameReadOnly(true);
            d->passwordPage()->setDatabaseName(data->databaseName());
            setCurrentPage(d->passwordPage());
            return;
        }
        else {
            d->projectData = 0;
            emitOpenProject(data);
        }
    }

}

void KexiWelcomeAssistant::tryAgainActionTriggered()
{
    messageWidget()->animatedHide();
    currentPage()->next();
}

void KexiWelcomeAssistant::cancelActionTriggered()
{
    if (currentPage() == d->m_passwordPage) {
        d->passwordPage()->focusWidget()->setFocus();
    }
}

QWidget* KexiWelcomeAssistant::calloutWidget() const
{
    return currentPage()->nextButton();
}

#include "KexiWelcomeAssistant.moc"
