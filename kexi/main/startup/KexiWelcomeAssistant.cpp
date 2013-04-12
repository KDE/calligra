/* This file is part of the KDE project
   Copyright (C) 2011-2012 Jarosław Staniek <staniek@kde.org>

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
#include <kurlcombobox.h>
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
    connect(this, SIGNAL(openProject(KexiProjectData,QString,bool*)),
            assistant, SIGNAL(openProject(KexiProjectData,QString,bool*)));
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
        bool opened = false;
        emit openProject(*pdata, m_assistant->projects()->shortcutPath(*pdata), &opened);
        if (opened) { // update
            pdata->setLastOpened(QDateTime::currentDateTime());
            m_recentProjects->update();
        }
    }
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
    }
    
    KexiMainWelcomePage* mainWelcomePage() {
        return page<KexiMainWelcomePage>(&m_mainWelcomePage, q);
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
    
    QAction* messageWidgetActionNo;
    QAction* messageWidgetActionTryAgain;
    QPointer<KexiContextMessageWidget> messageWidget;

    KexiRecentProjects* projects;
    
    KexiWelcomeAssistant *q;
};

// ----

KexiWelcomeAssistant::KexiWelcomeAssistant(
    KexiRecentProjects* projects, QWidget* parent)
 : KexiAssistantWidget(parent)
 , d(new Private(this))
{
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

void KexiWelcomeAssistant::previousPageRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
}

void KexiWelcomeAssistant::nextPageRequested(KexiAssistantPage* page)
{
    if (page == d->m_mainWelcomePage) {
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

void KexiWelcomeAssistant::cancelRequested(KexiAssistantPage* page)
{
    Q_UNUSED(page);
    //TODO?
}

void KexiWelcomeAssistant::showErrorMessage(
    const QString &title, const QString &details)
{
    Q_UNUSED(title);
    Q_UNUSED(details);
}

void KexiWelcomeAssistant::showErrorMessage(
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
            koIcon("view-refresh"), i18n("Try Again"), this);
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

void KexiWelcomeAssistant::tryAgainActionTriggered()
{
//    d->m_projectConnectionSelectionPage->next();
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

#include "KexiWelcomeAssistant.moc"
