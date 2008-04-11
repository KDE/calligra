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
#include <QTextCursor>
#include <QFontMetrics>
#include <QPointer>
#include <ksqueezedtextlabel.h>
#include <kstatusbar.h>
#include <klocale.h>
#include <kactioncollection.h>
#include <kdebug.h>
//#include <klocalizedstring.h>

#include <KoMainWindow.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoCanvasController.h>
#include <KoTextSelectionHandler.h>
#include <KoShapeManager.h>
#include <KoZoomAction.h>

#include "KWView.h"
#include "KWDocument.h"
#include "KWPage.h"
#include "KWCanvas.h"
#include "frames/KWTextFrameSet.h"

const QString i18nModified = i18n("Modified");
const QString i18nSaved = i18n("Saved");
const KLocalizedString i18nPage = ki18n("Page: %1/%2");

 /// \internal d-pointer class.
class KWStatusBar::Private
{
    public:
        KStatusBar * statusbar;
        KWView* view;
        KoToolProxy* toolproxy;
        QPointer<KoCanvasController> controller;
        int currentPageNumber;

        QLabel* modifiedLabel;
        QLabel* pageLabel;
        QLabel* mousePosLabel;
        KSqueezedTextLabel* statusLabel;
        QWidget* zoomWidget;

        Private(KStatusBar* sb, KWView* v)
            : statusbar(sb)
            , view(v)
            , controller(0)
            , currentPageNumber(1)
            , modifiedLabel(0)
            , pageLabel(0)
            , mousePosLabel(0)
            , zoomWidget(0)
        {
        }
        ~Private() {
            //if(modifiedLabel) statusbar->removeWidget(modifiedLabel);
            delete modifiedLabel;
            //if(pageLabel) statusbar->removeWidget(pageLabel);
            delete pageLabel;
            //if(mousePosLabel) statusbar->removeWidget(mousePosLabel);
            delete mousePosLabel;
            //if(zoomWidget) statusbar->removeWidget(zoomWidget);
            delete zoomWidget;
        }
};

KWStatusBar::KWStatusBar(KStatusBar* statusBar, KWView* view)
    : QObject(view)
    , d(new Private(statusBar, view))
{
    //KoMainWindow* mainwin = view->shell();
    //d->statusLabel = mainwin ? mainwin->statusBarLabel() : 0;

    d->statusbar->setContextMenuPolicy(Qt::ActionsContextMenu);

    KWDocument* const kwdoc = d->view->kwdocument();
    Q_ASSERT(kwdoc);
    KWCanvas* const canvas =  d->view->kwcanvas();
    Q_ASSERT(canvas);

    {
        d->pageLabel = new QLabel(d->statusbar);
        d->pageLabel->setFrameShape(QFrame::Panel);
        d->pageLabel->setFrameShadow(QFrame::Sunken);
        const QString s = i18nPage.subs("999").subs("999").toString();
        d->pageLabel->setMinimumWidth( QFontMetrics(d->pageLabel->font()).width(s) );
        d->statusbar->addWidget(d->pageLabel);
        slotPagesChanged();
        connect(kwdoc, SIGNAL(pageSetupChanged()), this, SLOT(slotPagesChanged()));

        QAction* action = new KAction(i18n("Page: current/total"), this);
        action->setObjectName("pages_current_total");
        action->setCheckable(true);
        action->setChecked(true);
        d->statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), d->pageLabel, SLOT(setVisible(bool)));
    }

    {
        d->modifiedLabel = new QLabel(d->statusbar);
        d->modifiedLabel->setFrameShape(QFrame::Panel);
        d->modifiedLabel->setFrameShadow(QFrame::Sunken);
        QFontMetrics fm(d->modifiedLabel->font());
        d->modifiedLabel->setMinimumWidth( qMax(fm.width(i18nModified), fm.width(i18nSaved)) );
        d->statusbar->addWidget(d->modifiedLabel);
        slotModifiedChanged(kwdoc->isModified());
        connect(kwdoc, SIGNAL(modified(bool)), this, SLOT(slotModifiedChanged(bool)));

        QAction* action = new KAction(i18n("State: saved/modified"), this);
        action->setObjectName("doc_save_state");
        action->setCheckable(true);
        action->setChecked(true);
        d->statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), d->modifiedLabel, SLOT(setVisible(bool)));
    }

    {
        d->mousePosLabel = new QLabel(d->statusbar);
        d->mousePosLabel->setFrameShape(QFrame::Panel);
        d->mousePosLabel->setFrameShadow(QFrame::Sunken);
        d->mousePosLabel->setMinimumWidth( QFontMetrics(d->mousePosLabel->font()).width("9999:9999") );
        d->mousePosLabel->setVisible(false);
        d->statusbar->addWidget(d->mousePosLabel);

        QAction* action = new KAction(i18n("Mouseposition: X:Y"), this);
        action->setObjectName("mousecursor_pos");
        action->setCheckable(true);
        action->setChecked(false);
        d->statusbar->addAction(action);
        connect(action, SIGNAL(toggled(bool)), d->mousePosLabel, SLOT(setVisible(bool)));
    }

    /*
    d->toolproxy = canvas->toolProxy();
    if( d->toolproxy ) {
        d->selectionLabel = new QLabel(d->statusbar);
        d->selectionLabel->setFrameShape(QFrame::Panel);
        d->selectionLabel->setFrameShadow(QFrame::Sunken);
        d->statusbar->addWidget(d->selectionLabel);
        slotSelectionChanged(false);
        connect(d->toolproxy, SIGNAL(selectionChanged(bool)), this, SLOT(slotSelectionChanged(bool)));
    }
    */

    d->statusLabel = new KSqueezedTextLabel(d->statusbar);
    d->statusbar->addWidget(d->statusLabel, 1);
    connect(d->statusbar, SIGNAL(messageChanged(const QString&)), this, SLOT(setText(const QString&)));

    {
        KActionCollection* collection = d->view->actionCollection();
        KoZoomAction* zoomaction = dynamic_cast<KoZoomAction*>( collection->action("view_zoom") );
        d->zoomWidget = zoomaction ? zoomaction->createWidget(d->statusbar) : 0;
        if( d->zoomWidget ) {
            d->statusbar->addWidget(d->zoomWidget);

            QAction* action = new KAction(i18n("Zoom Controller"), this);
            action->setObjectName("zoom_controller");
            action->setCheckable(true);
            action->setChecked(true);
            d->statusbar->addAction(action);
            connect(action, SIGNAL(toggled(bool)), d->zoomWidget, SLOT(setVisible(bool)));
        }
    }

    slotChangedTool();
    connect(KoToolManager::instance(), SIGNAL(changedTool(const KoCanvasController*,int)), this, SLOT(slotChangedTool()));

    KoCanvasResourceProvider* resourceprovider = canvas->resourceProvider();
    Q_ASSERT(resourceprovider);
    connect(resourceprovider, SIGNAL(resourceChanged(int,QVariant)), this, SLOT(slotResourceChanged(int,QVariant)));
}

KWStatusBar::~KWStatusBar()
{
    delete d;
}

void KWStatusBar::setText(const QString& text)
{
    d->statusLabel->setText(text);
}

void KWStatusBar::slotModifiedChanged(bool modified)
{
    d->modifiedLabel->setText(modified ? i18nModified : i18nSaved);
}

void KWStatusBar::slotPagesChanged()
{
    KWDocument* kwdoc = d->view->kwdocument();
    Q_ASSERT(kwdoc);
    d->pageLabel->setText( i18nPage.subs(d->currentPageNumber).subs(kwdoc->pageCount()).toString() );
}

void KWStatusBar::slotResourceChanged(int key, const QVariant& value)
{
    switch( key ) {
        case KWord::CurrentPage: {
            d->currentPageNumber = value.toInt();
            slotPagesChanged();
        } break;
        default: {
            kDebug()<<"KWStatusBar::slotResourceChanged Unhandled key="<<key<<" value="<<value<<endl;
        } break;
    }
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
    kDebug()<<"KWStatusBar::slotChangedTool"<<endl;
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
    //kDebug()<<"KWStatusBar::slotMousePositionChanged"<<endl;
    d->mousePosLabel->setText( QString("%1:%2").arg(pos.x()).arg(pos.y()) );
}

