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
#include <KoZoomAction.h>

#include <QLabel>
#include <KSqueezedTextLabel>
#include <KStatusBar>
#include <KLocale>
#include <KActionCollection>
#include <kdebug.h>

const QString i18nModified = i18n("Modified");
const QString i18nSaved = i18n("Saved");
const KLocalizedString i18nPage = ki18n("Page: %1/%2");

KWStatusBar::KWStatusBar(KStatusBar* statusBar, KWView* view)
    : QObject(view),
    m_statusbar(statusBar),
    m_view(view),
    m_controller(0),
    m_currentPageNumber(0)
{
    //KoMainWindow* mainwin = view->shell();
    //m_statusLabel = mainwin ? mainwin->statusBarLabel() : 0;

    m_statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    KWCanvas *const canvas =  m_view->kwcanvas();
    Q_ASSERT(canvas);
    KoCanvasResourceProvider *resourceprovider = canvas->resourceProvider();
    Q_ASSERT(resourceprovider);
    connect(resourceprovider, SIGNAL(resourceChanged(int, QVariant)), this, SLOT(resourceChanged(int, QVariant)));

    KWDocument *const kwdoc = m_view->kwdocument();
    Q_ASSERT(kwdoc);

    m_pageLabel = new QLabel(m_statusbar);
    m_pageLabel->setFrameShape(QFrame::Panel);
    m_pageLabel->setFrameShadow(QFrame::Sunken);
    const QString s = i18nPage.subs("999").subs("999").toString();
    m_pageLabel->setMinimumWidth(QFontMetrics(m_pageLabel->font()).width(s));
    m_statusbar->addWidget(m_pageLabel);
    updatePageCount();
    connect(kwdoc, SIGNAL(pageSetupChanged()), this, SLOT(updatePageCount()));

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
    setModified(kwdoc->isModified());
    connect(kwdoc, SIGNAL(modified(bool)), this, SLOT(setModified(bool)));

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

    KActionCollection* collection = m_view->actionCollection();
    KoZoomAction *zoomaction = dynamic_cast<KoZoomAction*>(collection->action("view_zoom"));
    QWidget *zoomWidget = zoomaction ? zoomaction->createWidget(m_statusbar) : 0;
    if (zoomWidget) {
        m_statusbar->addWidget(zoomWidget);

        KAction *action = new KAction(i18n("Zoom Controller"), this);
        action->setObjectName("zoom_controller");
        action->setCheckable(true);
        action->setChecked(true);
        m_statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), zoomWidget, SLOT(setVisible(bool)));
    }

    updateCurrentTool();
    connect(KoToolManager::instance(), SIGNAL(changedTool(const KoCanvasController*, int)),
            this, SLOT(updateCurrentTool()));
}

KWStatusBar::~KWStatusBar()
{
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
    KWDocument *kwdoc = m_view->kwdocument();
    Q_ASSERT(kwdoc);
    m_pageLabel->setText(i18nPage.subs(m_view->currentPage().pageNumber())
            .subs(kwdoc->pageCount()).toString());
}

void KWStatusBar::resourceChanged(int key, const QVariant &value)
{
    Q_UNUSED(value);
    if (key ==  KWord::CurrentPage)
        updatePageCount();
}

void KWStatusBar::updateCurrentTool()
{
    //kDebug(32003) << "KWStatusBar::updateCurrentTool" << endl;
    if (m_controller) {
        disconnect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)),
                this, SLOT(updateMousePosition(const QPoint&)));
    }
    m_controller = KoToolManager::instance()->activeCanvasController();
    if (m_controller) {
        connect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this,
                SLOT(updateMousePosition(const QPoint&)));
    } else {
        m_mousePosLabel->setText(QString());
    }
}

void KWStatusBar::updateMousePosition(const QPoint &pos)
{
    //kDebug(32003)<<"KWStatusBar::updateMousePosition"<<endl;
    m_mousePosLabel->setText(QString("%1:%2").arg(pos.x()).arg(pos.y()));
}

