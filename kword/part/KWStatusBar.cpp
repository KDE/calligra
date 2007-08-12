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

 /// \internal d-pointer class.
class KWStatusBar::Private
{
    public:
        KStatusBar * statusbar;
        KWView* view;
        KoToolProxy* toolproxy;
        KoCanvasController* controller;

        QLabel* modifiedLabel;
        QLabel* pageLabel;
        QLabel* mousePosLabel;

        Private(KStatusBar* sb, KWView* v)
            : statusbar(sb)
            , view(v)
            , controller(0)
            , modifiedLabel(0)
            , pageLabel(0)
            , mousePosLabel(0)
        {
        }
        ~Private() {
            //if(modifiedLabel) statusbar->removeWidget(modifiedLabel);
            delete modifiedLabel;
            //if(pageLabel) statusbar->removeWidget(pageLabel);
            delete pageLabel;
            //if(mousePosLabel) statusbar->removeWidget(mousePosLabel);
            delete mousePosLabel;
        }
};

KWStatusBar::KWStatusBar(KStatusBar* statusBar, KWView* view)
    : QObject(view)
    , d(new Private(statusBar, view))
{
    //KoMainWindow* mainwin = view->shell();
    //d->statusLabel = mainwin ? mainwin->statusBarLabel() : 0;

    d->statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    KWDocument* kwdoc = d->view->kwdocument();
    Q_ASSERT(kwdoc);

    //sebsauer, 2007-08-12, this crashes within QMainWindowLayout::animationFinished
    //This may the same bug that let's KWord crash if "Split View" got called.
    //FIXME check later if it's fixed.
#if 0

    {
        d->modifiedLabel = new QLabel(d->statusbar);
        d->modifiedLabel->setFrameShape(QFrame::Panel);
        d->modifiedLabel->setFrameShadow(QFrame::Sunken);
        QFontMetrics fm(d->modifiedLabel->font());
        d->modifiedLabel->setMinimumWidth( qMax(fm.width(i18nModified), fm.width(i18nSaved)) );
        d->statusbar->addWidget(d->modifiedLabel);
        slotModifiedChanged(kwdoc->isModified());
        connect(kwdoc, SIGNAL(modified(bool)), this, SLOT(slotModifiedChanged(bool)));

        QAction* action = new KAction(i18n("Document saved/modified"), this);
        action->setObjectName("doc_save_state");
        action->setCheckable(true);
        action->setChecked(true);
        d->statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), d->modifiedLabel, SLOT(setVisible(bool)));
    }

    {
        d->pageLabel = new QLabel(d->statusbar);
        d->pageLabel->setFrameShape(QFrame::Panel);
        d->pageLabel->setFrameShadow(QFrame::Sunken);
        d->pageLabel->setMinimumWidth( QFontMetrics(d->pageLabel->font()).width(i18n("999/999")) );
        d->statusbar->addWidget(d->pageLabel);
        slotPagesChanged();
        connect(kwdoc, SIGNAL(pageSetupChanged()), this, SLOT(slotPagesChanged()));

        QAction* action = new KAction(i18n("Pages current/total"), this);
        action->setObjectName("pages_current_total");
        action->setCheckable(true);
        action->setChecked(true);
        d->statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), d->pageLabel, SLOT(setVisible(bool)));
    }

    {
        d->mousePosLabel = new QLabel(d->statusbar);
        d->mousePosLabel->setFrameShape(QFrame::Panel);
        d->mousePosLabel->setFrameShadow(QFrame::Sunken);
        d->mousePosLabel->setMinimumWidth( QFontMetrics(d->mousePosLabel->font()).width("9999:9999") );
        d->statusbar->addWidget(d->mousePosLabel);

        QAction* action = new KAction(i18n("Mousecursor X:Y Position"), this);
        action->setObjectName("mousecursor_pos");
        action->setCheckable(true);
        action->setChecked(true);
        d->statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), d->mousePosLabel, SLOT(setVisible(bool)));
    }

    /*
    KWCanvas* canvas =  d->view->kwcanvas();
    d->toolproxy = canvas ? canvas->toolProxy() : 0;
    if( d->toolproxy ) {
        d->selectionLabel = new QLabel(d->statusbar);
        d->selectionLabel->setFrameShape(QFrame::Panel);
        d->selectionLabel->setFrameShadow(QFrame::Sunken);
        d->statusbar->addWidget(d->selectionLabel);
        slotSelectionChanged(false);
        connect(d->toolproxy, SIGNAL(selectionChanged(bool)), this, SLOT(slotSelectionChanged(bool)));
    }
    */

    //d->statusLabel = new KSqueezedTextLabel(d->statusbar);
    //d->statusbar->addWidget(d->statusLabel,1);

    slotChangedTool();
    connect(KoToolManager::instance(), SIGNAL(changedTool(const KoCanvasController*,int)), this, SLOT(slotChangedTool()));
#endif
}

KWStatusBar::~KWStatusBar()
{
    delete d;
}

void KWStatusBar::slotModifiedChanged(bool modified)
{
    d->modifiedLabel->setText(modified ? i18nModified : i18nSaved);
}

void KWStatusBar::slotPagesChanged()
{
    kDebug()<<"===> KWStatusBar::slotPagesChanged"<<endl;
    KWDocument* kwdoc = d->view->kwdocument();
    Q_ASSERT(kwdoc);
    //TODO how to fetch the current page?
    d->pageLabel->setText( QString("%1/%2").arg(1).arg(kwdoc->pageCount()) );
}

/*
void KWStatusBar::slotSelectionChanged(bool hasSelection)
{
    kDebug()<<"===> KWStatusBar::slotSelectionChanged"<<endl;
    QString pos = "0";
    if( hasSelection ) {
        KoToolSelection* selection = d->toolproxy->selection();
        KoTextSelectionHandler* textselection = dynamic_cast<KoTextSelectionHandler*>(selection);
        if( textselection ) {
            QTextCursor cursor = textselection->caret();
            pos = QString("%1").arg( textselection->selectedText().length() );
        }
    }
    d->selectionLabel->setText(pos);
}
*/

void KWStatusBar::slotChangedTool()
{
    kDebug()<<"===> KWStatusBar::slotChangedTool"<<endl;
    if(d->controller) {
        disconnect(d->controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this, SLOT(slotMousePositionChanged(const QPoint&)));
    }
    d->controller = KoToolManager::instance()->activeCanvasController();
    if(d->controller) {
        connect(d->controller, SIGNAL(canvasMousePositionChanged(const QPoint&)), this, SLOT(slotMousePositionChanged(const QPoint&)));
    }
    else {
        //slotMousePositionChanged(QPoint());
    }
}

void KWStatusBar::slotMousePositionChanged(const QPoint& pos)
{
    kDebug()<<"===> KWStatusBar::slotMousePositionChanged"<<endl;
    d->mousePosLabel->setText( QString("%1:%2").arg(pos.x()).arg(pos.y()) );
}

#include "KWStatusBar.moc"
