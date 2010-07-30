/* This file is part of the KDE project
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2008-2010 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA
 */

#include "KWStatusBar.h"
#include "KWView.h"
#include "KWDocument.h"

#include <KoCanvasBase.h>
#include <KoToolManager.h>
#include <KoCanvasControllerWidget.h>
#include <KoZoomController.h>

#include <QLabel>
#include <QTimer>
#include <KSqueezedTextLabel>
#include <KStatusBar>
#include <KLocale>
#include <KActionCollection>
#include <kdebug.h>

const QString i18nModified = i18n("Modified");
const QString i18nSaved = i18n("Saved");
const KLocalizedString i18nPage = ki18n("Page: %1/%2");

#define KWSTATUSBAR "KWStatusBarPointer"

KWStatusBar::KWStatusBar(KStatusBar *statusBar, KWView *view)
    : QObject(statusBar),
    m_statusbar(statusBar),
    m_controller(0),
    m_currentPageNumber(0)
{
    KWDocument *document = view->kwdocument();
    m_statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_pageLabel = new QLabel(m_statusbar);
    m_pageLabel->setFrameShape(QFrame::Panel);
    m_pageLabel->setFrameShadow(QFrame::Sunken);
    const QString s = i18nPage.subs("999").subs("999").toString();
    m_pageLabel->setMinimumWidth(QFontMetrics(m_pageLabel->font()).width(s));
    m_statusbar->addWidget(m_pageLabel);
    m_pageLabel->setVisible(document->config().statusBarShowPage());
    connect(document, SIGNAL(pageSetupChanged()), this, SLOT(updatePageCount()));

    KAction *action = new KAction(i18n("Page: current/total"), this);
    action->setObjectName("pages_current_total");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowPage());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showPage(bool)));

    m_modifiedLabel = new QLabel(m_statusbar);
    m_modifiedLabel->setFrameShape(QFrame::Panel);
    m_modifiedLabel->setFrameShadow(QFrame::Sunken);
    QFontMetrics fm(m_modifiedLabel->font());
    m_modifiedLabel->setMinimumWidth(qMax(fm.width(i18nModified), fm.width(i18nSaved)));
    m_statusbar->addWidget(m_modifiedLabel);
    setModified(document->isModified());
    m_modifiedLabel->setVisible(document->config().statusBarShowModified());
    connect(document, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));

    action = new KAction(i18n("State: saved/modified"), this);
    action->setObjectName("doc_save_state");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowModified());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showModified(bool)));

    m_mousePosLabel = new QLabel(m_statusbar);
    m_mousePosLabel->setFrameShape(QFrame::Panel);
    m_mousePosLabel->setFrameShadow(QFrame::Sunken);
    m_mousePosLabel->setMinimumWidth(QFontMetrics(m_mousePosLabel->font()).width("9999:9999"));
    m_statusbar->addWidget(m_mousePosLabel);
    m_mousePosLabel->setVisible(document->config().statusBarShowMouse());

    action = new KAction(i18n("Mouseposition: X:Y"), this);
    action->setObjectName("mousecursor_pos");
    action->setCheckable(true);
    action->setChecked(document->config().statusBarShowMouse());
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), this, SLOT(showMouse(bool)));

    m_statusLabel = new KSqueezedTextLabel(m_statusbar);
    m_statusbar->addWidget(m_statusLabel, 1);
    connect(m_statusbar, SIGNAL(messageChanged(const QString&)), this, SLOT(setText(const QString&)));
    connect(KoToolManager::instance(), SIGNAL(changedStatusText(const QString&)),
            this, SLOT(setText(const QString&)));

    m_zoomAction = new KAction(i18n("Zoom Controller"), this);
    m_zoomAction->setObjectName("zoom_controller");
    m_zoomAction->setCheckable(true);
    m_zoomAction->setChecked(document->config().statusBarShowZoom());
    m_statusbar->addAction(m_zoomAction);

    updateCurrentTool(0);
    setCurrentView(view);
    connect(KoToolManager::instance(), SIGNAL(changedTool(KoCanvasController*, int)),
            this, SLOT(updateCurrentTool(KoCanvasController*)));
}

KWStatusBar::~KWStatusBar()
{
    // delete these as they are children of the statusBar but we want to delete them when the view dissapears
    delete m_modifiedLabel;
    delete m_pageLabel;
    delete m_mousePosLabel;
    delete m_statusLabel;
    foreach (QWidget *widget, m_zoomWidgets)
        widget->deleteLater();

    m_statusbar->setProperty(KWSTATUSBAR, QVariant());
}

void KWStatusBar::setText(const QString &text)
{
    m_statusLabel->setText(text);
}

void KWStatusBar::setModified(bool modified)
{
    m_modifiedLabel->setText(modified ? i18nModified : i18nSaved);
}

void KWStatusBar::updatePageCount()
{
    if (m_currentView)
        m_pageLabel->setText(i18nPage.subs(m_currentView->currentPage().pageNumber())
            .subs(m_currentView->kwdocument()->pageCount()).toString());
    else
        m_pageLabel->setText(i18nPage.toString());
}

void KWStatusBar::resourceChanged(int key, const QVariant &value)
{
    Q_UNUSED(value);
    if (key ==  KoCanvasResource::CurrentPage)
        updatePageCount();
}

void KWStatusBar::updateCurrentTool(KoCanvasController *canvasController)
{
    KoCanvasControllerWidget *widget = dynamic_cast<KoCanvasControllerWidget*>(canvasController);
    if (!widget) {
        return;
    }
    QWidget *root = m_statusbar->window();
    if (root && !root->isAncestorOf(widget))
        return; // ignore tool changes in other mainWindows

    if (m_controller) {
        disconnect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)),
                this, SLOT(updateMousePosition(const QPoint&)));
    }
    m_controller = canvasController->proxyObject;
    if (canvasController) {
        // find KWView parent of the canvas controller widget
        KWView *view = 0;
        QWidget *parent = widget->parentWidget();
        while (view == 0 && parent != 0) {
            view = dynamic_cast<KWView*>(parent);
            if (!view) {
                parent = parent->parentWidget();
            }
        }
        if (view) {
            setCurrentView(view);
        }
        connect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this,
                SLOT(updateMousePosition(const QPoint&)));
    } else {
        m_mousePosLabel->setText(QString());
    }
}

void KWStatusBar::setCurrentView(KWView *view)
{
    if (view == 0) {
        m_currentView = 0;
        return;
    } else if (view == m_currentView) {
        return;
    }

    if (m_currentView) {
        KoCanvasBase *const canvas =  m_currentView->canvasBase();
        Q_ASSERT(canvas);
        KoResourceManager *resourceManager = canvas->resourceManager();
        Q_ASSERT(resourceManager);
        disconnect(resourceManager, SIGNAL(resourceChanged(int, QVariant)),
            this, SLOT(resourceChanged(int, QVariant)));
        QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
        if (zoomWidget) {
            m_statusbar->removeWidget(zoomWidget);
            disconnect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(showZoom(bool)));
        }
    }

    m_currentView = view;
    if (m_currentView == 0)
        return;
    QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
    if (zoomWidget) {
        m_statusbar->addWidget(zoomWidget);
        connect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(showZoom(bool)));
        zoomWidget->setVisible(m_currentView->kwdocument()->config().statusBarShowZoom());
    } else {
        // do it delayed to avoid a race condition where this code
        // is ran from the constructor of KWView before the zoomController is created.
        QTimer::singleShot(0, this, SLOT(createZoomWidget()));
    }

    KoResourceManager *resourceManager = view->canvasBase()->resourceManager();
    Q_ASSERT(resourceManager);
    connect(resourceManager, SIGNAL(resourceChanged(int, QVariant)),
        this, SLOT(resourceChanged(int, QVariant)));
    updatePageCount();
}

void KWStatusBar::createZoomWidget()
{
    if (m_currentView) {
        KoZoomController *zoomController = m_currentView->zoomController();
        if (zoomController && !m_zoomWidgets.contains(m_currentView)) {
            QWidget *zoomWidget = zoomController->zoomAction()->createWidget(m_statusbar);
            m_zoomWidgets.insert(m_currentView, zoomWidget);
            m_statusbar->addWidget(zoomWidget);
            connect(m_zoomAction, SIGNAL(toggled(bool)), this, SLOT(showZoom(bool)));
            zoomWidget->setVisible(m_currentView->kwdocument()->config().statusBarShowZoom());
        }
    }
}

void KWStatusBar::showPage(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowPage(visible);
    m_pageLabel->setVisible(visible);
}

void KWStatusBar::showModified(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowModified(visible);
    m_modifiedLabel->setVisible(visible);
}

void KWStatusBar::showMouse(bool visible)
{
    Q_ASSERT(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowMouse(visible);
    m_mousePosLabel->setVisible(visible);
}

void KWStatusBar::showZoom(bool visible)
{
    Q_ASSERT(m_currentView);
    QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
    m_currentView->kwdocument()->config().setStatusBarShowZoom(visible);
    zoomWidget->setVisible(visible);
}

void KWStatusBar::updateMousePosition(const QPoint &pos)
{
    //kDebug(32003)<<"KWStatusBar::updateMousePosition";
    m_mousePosLabel->setText(QString("%1:%2").arg(pos.x()).arg(pos.y()));
}

void KWStatusBar::removeView(QObject *object)
{
    KWView *view = static_cast<KWView*>(object);
    QWidget *widget = m_zoomWidgets.value(view);
    if (widget) {
        widget->deleteLater();
        m_zoomWidgets.remove(view);
    }
    if (view == m_currentView)
        m_currentView = 0;
}

//static
void KWStatusBar::addViewControls(KStatusBar *statusBar, KWView *view)
{
    /**
     * Life time of a KWStatusBar is tricky...
     * One main window has one KStatusBar.  But it can be re-used by different
     *  documents and thus by many different KWView instances.
     * So;  open a document in a window creates a KWView. That creates a KWStatusBar
     *      split the view creates a new KWView in the same mainwindow, this reuses
     *      the already existing KWStatusBar
     *      Create a new view (new MainWindow) also creates a new KWStatusBar
     *      Close all your views (deletes all KWViews) but not your Mainwindow will
     *      NOT destroy all KWStatusBar instance.  Note that KStatusBar is not
     *      destructed in that case either.
     */

    QVariant variant = statusBar->property(KWSTATUSBAR);
    if (variant.isValid()) { // already exists!
        KWStatusBar *decorator = static_cast<KWStatusBar*>(variant.value<void*>());
        if (decorator)
            decorator->connect(view, SIGNAL(destroyed(QObject*)), SLOT(removeView(QObject*)));
        return;
    }
    KWStatusBar *decorator = new KWStatusBar(statusBar, view);
    decorator->connect(view, SIGNAL(destroyed(QObject*)), SLOT(removeView(QObject*)));
    variant.setValue<void*>(decorator);
    statusBar->setProperty(KWSTATUSBAR, variant);
}

