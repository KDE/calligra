/* This file is part of the KDE project
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
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

#include <QPoint>
#include <QLabel>
#include <QTextDocument>
#include <QTextCursor>
#include <QFontMetrics>
#include <ksqueezedtextlabel.h>
#include <kstatusbar.h>
#include <klocale.h>

#include <KoMainWindow.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoCanvasController.h>
#include <KoTextSelectionHandler.h>
//#include <KoViewConverter.h>

#include "KWView.h"
#include "KWDocument.h"
#include "KWCanvas.h"
#include "frames/KWTextFrameSet.h"

const QString i18nModified = i18n("Modified");
const QString i18nSaved = i18n("Saved");

KWStatusBar::KWStatusBar(KStatusBar* statusBar, KWView* view)
    : QObject(view)
    , m_statusbar(statusBar)
    , m_view(view)
    , m_controller(0)
    , m_modifiedLabel(0)
    , m_pageLabel(0)
    , m_mousePosLabel(0)
{
    //KoMainWindow* mainwin = view->shell();
    //m_statusLabel = mainwin ? mainwin->statusBarLabel() : 0;

    KWDocument* kwdoc = m_view->kwdocument();
    Q_ASSERT(kwdoc);

    m_modifiedLabel = new QLabel(m_statusbar);
    m_modifiedLabel->setFrameShape(QFrame::Panel);
    m_modifiedLabel->setFrameShadow(QFrame::Sunken);
    QFontMetrics fm(m_modifiedLabel->font());
    m_modifiedLabel->setMinimumWidth( qMax(fm.width(i18nModified), fm.width(i18nSaved)) );
    m_modifiedLabel->setToolTip(i18n("Document state, modified or saved."));
    m_statusbar->addWidget(m_modifiedLabel);
    slotModifiedChanged(kwdoc->isModified());
    connect(kwdoc, SIGNAL(modified(bool)), this, SLOT(slotModifiedChanged(bool)));

    m_pageLabel = new QLabel(m_statusbar);
    m_pageLabel->setFrameShape(QFrame::Panel);
    m_pageLabel->setFrameShadow(QFrame::Sunken);
    m_pageLabel->setMinimumWidth( QFontMetrics(m_pageLabel->font()).width(i18n("999/999")) );
    m_pageLabel->setToolTip(i18n("Pages, current/total"));
    m_statusbar->addWidget(m_pageLabel);
    slotPagesChanged();
    connect(kwdoc, SIGNAL(pageSetupChanged()), this, SLOT(slotPagesChanged()));

    KWTextFrameSet* mainFrameSet = kwdoc->mainFrameSet();
    QTextDocument* doc = mainFrameSet ? mainFrameSet->document() : 0;
    if( doc ) {
        m_mousePosLabel = new QLabel(m_statusbar);
        m_mousePosLabel->setFrameShape(QFrame::Panel);
        m_mousePosLabel->setFrameShadow(QFrame::Sunken);
        m_mousePosLabel->setMinimumWidth( QFontMetrics(m_mousePosLabel->font()).width("9999:9999") );
        m_mousePosLabel->setToolTip(i18n("Position of the mouse, X:Y"));
        m_statusbar->addWidget(m_mousePosLabel);
        slotCursorPositionChanged(QTextCursor());
        connect(doc, SIGNAL(cursorPositionChanged(QTextCursor)), this, SLOT(slotCursorPositionChanged(QTextCursor)));
    }

    /*
    KWCanvas* canvas =  m_view->kwcanvas();
    m_toolproxy = canvas ? canvas->toolProxy() : 0;
    if( m_toolproxy ) {
        m_selectionLabel = new QLabel(m_statusbar);
        m_selectionLabel->setFrameShape(QFrame::Panel);
        m_selectionLabel->setFrameShadow(QFrame::Sunken);
        m_statusbar->addWidget(m_selectionLabel);
        slotSelectionChanged(false);
        connect(m_toolproxy, SIGNAL(selectionChanged(bool)), this, SLOT(slotSelectionChanged(bool)));
    }
    */

    //m_statusLabel = new KSqueezedTextLabel(m_statusbar);
    //m_statusbar->addWidget(m_statusLabel,1);

    slotChangedTool();
    connect(KoToolManager::instance(), SIGNAL(changedTool(const KoCanvasController*,int)), this, SLOT(slotChangedTool()));
}

KWStatusBar::~KWStatusBar()
{
    delete m_modifiedLabel;
    delete m_pageLabel;
    delete m_mousePosLabel;
}

void KWStatusBar::slotModifiedChanged(bool modified)
{
    m_modifiedLabel->setText(modified ? i18nModified : i18nSaved);
}

void KWStatusBar::slotPagesChanged()
{
    kDebug()<<"===> KWStatusBar::slotPagesChanged"<<endl;
    KWDocument* kwdoc = m_view->kwdocument();
    Q_ASSERT(kwdoc);
    //TODO how to fetch the current page?
    m_pageLabel->setText( QString("%1/%2").arg(1).arg(kwdoc->pageCount()) );
}

void KWStatusBar::slotCursorPositionChanged(const QTextCursor& cursor)
{
    kDebug()<<"===> KWStatusBar::slotCursorPositionChanged"<<endl;
    //m_mousePosLabel->setText( QString("%1").arg(cursor.isNull() ? 0 : cursor.position()) );
}

/*
void KWStatusBar::slotSelectionChanged(bool hasSelection)
{
    kDebug()<<"===> KWStatusBar::slotSelectionChanged"<<endl;
    QString pos = "0";
    if( hasSelection ) {
        KoToolSelection* selection = m_toolproxy->selection();
        KoTextSelectionHandler* textselection = dynamic_cast<KoTextSelectionHandler*>(selection);
        if( textselection ) {
            QTextCursor cursor = textselection->caret();
            pos = QString("%1").arg( textselection->selectedText().length() );
        }
    }
    m_selectionLabel->setText(pos);
}
*/

void KWStatusBar::slotChangedTool()
{
    kDebug()<<"===> KWStatusBar::slotChangedTool"<<endl;
    if(m_controller) {
        disconnect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this, SLOT(slotMousePositionChanged(const QPoint&)));
    }
    m_controller = KoToolManager::instance()->activeCanvasController();
    if(m_controller) {
        connect(m_controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this, SLOT(slotMousePositionChanged(const QPoint&)));
    }
    else {
        slotMousePositionChanged(QPoint());
    }
}

void KWStatusBar::slotMousePositionChanged(const QPoint& pos)
{
    //kDebug()<<"===> KWStatusBar::slotMousePositionChanged"<<endl;
    m_mousePosLabel->setText( QString("%1:%2").arg(pos.x()).arg(pos.y()) );
}

#include "KWStatusBar.moc"
