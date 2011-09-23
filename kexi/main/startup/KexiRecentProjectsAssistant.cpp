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

#include "KexiRecentProjectsAssistant.h"

#include "KexiRecentProjectsModel.h"

#include <core/kexi.h>
#include <core/KexiRecentProjects.h>
#include <core/kexiprojectdata.h>
#include <core/kexiguimsghandler.h>
#include <core/kexitextmsghandler.h>
#include <kexidb/utils.h>
#include <kexidb/object.h>
#include <kexiutils/identifier.h>
#include <kexiutils/utils.h>
#include <kexiutils/KexiAssistantPage.h>
#include <kexiutils/KexiLinkWidget.h>

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
#include <KCategorizedView>
#include <KTitleWidget>
#include <KPushButton>
#include <KAcceleratorManager>
#include <KFileDialog>
#include <KFileItemDelegate>

#include <QLayout>
#include <QCheckBox>
#include <QPaintEvent>
#include <QPainter>
#include <QProgressBar>
#include <QTimer>
 
KexiMainRecentProjectsPage::KexiMainRecentProjectsPage(
   KexiRecentProjectsAssistant* assistant, QWidget* parent)
 : KexiAssistantPage(i18n("Recent Projects"),
                  i18n("Select one of the recently used projects to open."),
                  parent)
 , m_assistant(assistant)
{
    connect(this, SIGNAL(openProject(KexiProjectData)),
            assistant, SIGNAL(openProject(KexiProjectData)));
    m_recentProjects = new KexiCategorizedView;
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
    setContents(m_recentProjects);

    QTimer::singleShot(100, this, SLOT(loadProjects()));
}

void KexiMainRecentProjectsPage::loadProjects()
{
    m_recentProjectsProxyModel = new KexiRecentProjectsProxyModel(m_recentProjects);
    KexiRecentProjectsModel* model = new KexiRecentProjectsModel(*m_assistant->projects());
    m_recentProjectsProxyModel->setSourceModel(model);
    m_recentProjects->setModel(m_recentProjectsProxyModel);
    m_recentProjectsProxyModel->sort(0, Qt::DescendingOrder);
}

void KexiMainRecentProjectsPage::slotItemClicked(const QModelIndex& index)
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
        emit openProject(*pdata);
//        next();
//        return;
    }
}

// ----

class KexiRecentProjectsAssistant::Private
{
public:
    Private(KexiRecentProjectsAssistant *qq)
     : q(qq)
    {
    }
    
    ~Private()
    {
    }
    
    KexiMainRecentProjectsPage* mainRecentProjectsPage() {
        return page<KexiMainRecentProjectsPage>(&m_mainRecentProjectsPage, q);
    }
    
    template <class C>
    C* page(QPointer<C>* p, KexiRecentProjectsAssistant *parent = 0) {
        if (p->isNull()) {
            *p = new C(parent);
            q->addPage(*p);
        }
        return *p;
    }

    QPointer<KexiMainRecentProjectsPage> m_mainRecentProjectsPage;
    
    QAction* messageWidgetActionNo;
    QAction* messageWidgetActionTryAgain;
    QPointer<KexiContextMessageWidget> messageWidget;

    KexiRecentProjects* projects;
    
    KexiRecentProjectsAssistant *q;
};

// ----

KexiRecentProjectsAssistant::KexiRecentProjectsAssistant(
    KexiRecentProjects* projects, QWidget* parent)
 : KexiAssistantWidget(parent)
 , d(new Private(this))
{
    d->messageWidgetActionNo = 0;
    d->messageWidgetActionTryAgain = 0;
    d->projects = projects;
    setCurrentPage(d->mainRecentProjectsPage());
    setFocusProxy(d->mainRecentProjectsPage());
}

KexiRecentProjectsAssistant::~KexiRecentProjectsAssistant()
{
    delete d;
}

void KexiRecentProjectsAssistant::previousPageRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}

void KexiRecentProjectsAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_mainRecentProjectsPage) {
        /*
        KexiDB::ConnectionData *cdata
            = d->projectConnectionSelectionPage()->connSelector->selectedConnectionData();
        if (cdata) {
            if (d->projectDatabaseNameSelectionPage()->setConnection(cdata)) {
                setCurrentPage(d->projectDatabaseNameSelectionPage());
            }
        }*/
    }
}

void KexiRecentProjectsAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //TODO?
}

void KexiRecentProjectsAssistant::showErrorMessage(
    const QString &title, const QString &details)
{
    Q_UNUSED(title);
    Q_UNUSED(details);
}

void KexiRecentProjectsAssistant::showErrorMessage(
    KexiDB::Object *obj, const QString& msg)
{
    Q_UNUSED(obj);
    Q_UNUSED(msg);
    /*
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
        this, 0, //contents->formLayout
        0, //contents->le_dbname
        message);
    //d->messageWidget->setNextFocusWidget(contents->le_title);
    d->messageWidget->setCalloutPointerDirection(KMessageWidget::Right);
    QWidget *b = currentPage()->nextButton();
    d->messageWidget->setCalloutPointerPosition(
        b->mapToGlobal(QPoint(0, b->height() / 2)));*/
}

void KexiRecentProjectsAssistant::tryAgainActionTriggered()
{
//    d->m_projectConnectionSelectionPage->next();
}

KexiRecentProjects* KexiRecentProjectsAssistant::projects()
{
    return d->projects;
}

#include "KexiRecentProjectsAssistant.moc"
