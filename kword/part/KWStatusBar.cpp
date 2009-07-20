/* This file is part of the KDE project
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2008-2009 Thomas Zander <zander@kde.org>
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
#include "KWCanvas.h"

#include <KoToolManager.h>
#include <KoCanvasController.h>
#include <KoZoomController.h>

#include <QLabel>
#include <KSqueezedTextLabel>
#include <KStatusBar>
#include <KLocale>
#include <KActionCollection>
#include <kdebug.h>

const QString i18nModified = i18n("Modified");
const QString i18nSaved = i18n("Saved");
const KLocalizedString i18nPage = ki18n("Page: %1/%2");

#define KWSTATUSBAR "KWStatusBarPointer"

KWStatusBar::KWStatusBar(KStatusBar* statusBar, KWView* view)
    : QObject(view),
    m_statusbar(statusBar),
    m_currentView(0),
    m_document(view->kwdocument()),
    m_controller(0),
    m_currentPageNumber(0)
{
    m_statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    m_pageLabel = new QLabel(m_statusbar);
    m_pageLabel->setFrameShape(QFrame::Panel);
    m_pageLabel->setFrameShadow(QFrame::Sunken);
    const QString s = i18nPage.subs("999").subs("999").toString();
    m_pageLabel->setMinimumWidth(QFontMetrics(m_pageLabel->font()).width(s));
    m_statusbar->addWidget(m_pageLabel);
    connect(m_document, SIGNAL(pageSetupChanged()), this, SLOT(updatePageCount()));

    KAction *action = new KAction(i18n("Page: current/total"), this);
    action->setObjectName("pages_current_total");
    action->setCheckable(true);
    action->setChecked(true);
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), m_pageLabel, SLOT(setVisible(bool)));

    m_modifiedLabel = new QLabel(m_statusbar);
    m_modifiedLabel->setFrameShape(QFrame::Panel);
    m_modifiedLabel->setFrameShadow(QFrame::Sunken);
    QFontMetrics fm(m_modifiedLabel->font());
    m_modifiedLabel->setMinimumWidth(qMax(fm.width(i18nModified), fm.width(i18nSaved)));
    m_statusbar->addWidget(m_modifiedLabel);
    setModified(m_document->isModified());
    connect(m_document, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));

    action = new KAction(i18n("State: saved/modified"), this);
    action->setObjectName("doc_save_state");
    action->setCheckable(true);
    action->setChecked(true);
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), m_modifiedLabel, SLOT(setVisible(bool)));

    m_mousePosLabel = new QLabel(m_statusbar);
    m_mousePosLabel->setFrameShape(QFrame::Panel);
    m_mousePosLabel->setFrameShadow(QFrame::Sunken);
    m_mousePosLabel->setMinimumWidth(QFontMetrics(m_mousePosLabel->font()).width("9999:9999"));
    m_mousePosLabel->setVisible(false);
    m_statusbar->addWidget(m_mousePosLabel);

    action = new KAction(i18n("Mouseposition: X:Y"), this);
    action->setObjectName("mousecursor_pos");
    action->setCheckable(true);
    action->setChecked(false);
    m_statusbar->addAction(action);
    connect(action, SIGNAL(toggled(bool)), m_mousePosLabel, SLOT(setVisible(bool)));

    m_statusLabel = new KSqueezedTextLabel(m_statusbar);
    m_statusbar->addWidget(m_statusLabel, 1);
    connect(m_statusbar, SIGNAL(messageChanged(const QString&)), this, SLOT(setText(const QString&)));

    m_zoomAction = new KAction(i18n("Zoom Controller"), this);
    m_zoomAction->setObjectName("zoom_controller");
    m_zoomAction->setCheckable(true);
    m_zoomAction->setChecked(true);
    m_statusbar->addAction(m_zoomAction);

    updateCurrentTool(0);
    setCurrentCanvas(view->kwcanvas());
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
            .subs(m_document->pageCount()).toString());
    else
        m_pageLabel->setText(i18nPage.toString());
}

void KWStatusBar::resourceChanged(int key, const QVariant &value)
{
    Q_UNUSED(value);
    if (key ==  KWord::CurrentPage)
        updatePageCount();
}

void KWStatusBar::updateCurrentTool(KoCanvasController *canvasController)
{
    // kDebug(32003) << "KWStatusBar::updateCurrentTool";
    if (m_controller) {
        disconnect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)),
                this, SLOT(updateMousePosition(const QPoint&)));
    }
    m_controller = canvasController;
    if (m_controller) {
        KWCanvas *canvas = dynamic_cast<KWCanvas*>(m_controller->canvas());
        if (canvas)
            setCurrentCanvas(canvas);
        connect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this,
                SLOT(updateMousePosition(const QPoint&)));
    } else {
        m_mousePosLabel->setText(QString());
    }
}

void KWStatusBar::setCurrentCanvas(KWCanvas *canvas)
{
    if (canvas == 0) {
        m_currentView = 0;
        return;
    }
    else if (canvas->view() == m_currentView) {
        return;
    }

    if (m_currentView) {
        KWCanvas *const canvas =  m_currentView->kwcanvas();
        Q_ASSERT(canvas);
        KoCanvasResourceProvider *resourceprovider = canvas->resourceProvider();
        Q_ASSERT(resourceprovider);
        disconnect(resourceprovider, SIGNAL(resourceChanged(int, QVariant)),
            this, SLOT(resourceChanged(int, QVariant)));
        QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
        if (zoomWidget) {
            m_statusbar->removeWidget(zoomWidget);
            disconnect(m_zoomAction, SIGNAL(toggled(bool)), zoomWidget, SLOT(setVisible(bool)));
        }
    }

    m_currentView = canvas->view();
    if (m_currentView == 0)
        return;
    QWidget *zoomWidget = m_zoomWidgets.value(m_currentView);
    if (zoomWidget == 0) { // create it
        KoZoomController *zoomController = m_currentView->zoomController();
        if (zoomController) {
            zoomWidget = zoomController->zoomAction()->createWidget(m_statusbar);
            m_zoomWidgets.insert(m_currentView, zoomWidget);
        }
    }
    if (zoomWidget) {
        m_statusbar->addWidget(zoomWidget);
        connect(m_zoomAction, SIGNAL(toggled(bool)), zoomWidget, SLOT(setVisible(bool)));
        zoomWidget->setVisible(m_zoomAction->isChecked());
    }

    KoCanvasResourceProvider *resourceprovider = canvas->resourceProvider();
    Q_ASSERT(resourceprovider);
    connect(resourceprovider, SIGNAL(resourceChanged(int, QVariant)),
        this, SLOT(resourceChanged(int, QVariant)));
    updatePageCount();
}

void KWStatusBar::updateMousePosition(const QPoint &pos)
{
    //kDebug(32003)<<"KWStatusBar::updateMousePosition";
    m_mousePosLabel->setText(QString("%1:%2").arg(pos.x()).arg(pos.y()));
}

//static
void KWStatusBar::addViewControls(KStatusBar *statusBar, KWView *view)
{
    QVariant variant = statusBar->property(KWSTATUSBAR);
    if (variant.isValid()) // already exists!
        return;
    KWStatusBar *decorator = new KWStatusBar(statusBar, view);
    variant.setValue<void*>(decorator);
    statusBar->setProperty(KWSTATUSBAR, variant);
}


