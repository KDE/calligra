/* This file is part of the KDE project
 * Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 * Copyright (C) 2000-2005 David Faure <faure@kde.org>
 * Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>
 * Copyright (C) 2010 Boudewijn Rempt <boud@kogmbh.com>
 * Copyright (C) 2011 Inge Wallin <ingwa@kogmbh.com>
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
 * Boston, MA 02110-1301, USA.
 */

#include "KoApplication.h"
#include "KoPart.h"
#include "KoMainWindow.h"
#include "KoDocument.h"
#include "KoView.h"
#include "KoCanvasController.h"
#include "KoCanvasControllerWidget.h"

#include <QGraphicsScene>
#include <QGraphicsProxyWidget>


class KoPart::Private
{
public:
    Private()
        : document(0)
        , canvasItem(0)
    {
    }

    QList<KoView*> views;
    QList<KoMainWindow*> shells;
    KoDocument *document;
    QGraphicsItem *canvasItem;

};


KoPart::KoPart(QObject *parent)
        : KParts::ReadWritePart(parent)
        , d(new Private)
{
    // we're not a part in a part, so we cannot be selected, we're always top-level
    setSelectable(false);
}

KoPart::~KoPart()
{

    // Tell our views that the document is already destroyed and
    // that they shouldn't try to access it.
    foreach(KoView *view, views()) {
        view->setDocumentDeleted();
    }

    while (!d->shells.isEmpty()) {
        delete d->shells.takeFirst();
    }


    delete d;
}



void KoPart::setDocument(KoDocument *document)
{
    d->document = document;
    connect(d->document, SIGNAL(titleModified(QString,bool)), SLOT(setTitleModified(QString,bool)));
}

KoDocument *KoPart::document() const
{
    return d->document;
}

void KoPart::setReadWrite(bool readwrite)
{
    KParts::ReadWritePart::setReadWrite(readwrite);

    foreach(KoView *view, d->views) {
        view->updateReadWrite(readwrite);
    }

    foreach(KoMainWindow *mainWindow, d->shells) {
        mainWindow->setReadWrite(readwrite);
    }
}

bool KoPart::saveFile()
{
    return false; // FIXME should actually save the document, dialogs and all
}

KoView *KoPart::createView(QWidget *parent)
{
    KoView *view = createViewInstance(parent);
    addView(view);
    return view;
}

void KoPart::addView(KoView *view)
{
    if (!view)
        return;

    d->views.append(view);
    view->updateReadWrite(isReadWrite());

    if (d->views.size() == 1) {
        KoApplication *app = qobject_cast<KoApplication*>(KApplication::kApplication());
        if (0 != app) {
            emit app->documentOpened('/'+objectName());
        }
    }
}

void KoPart::removeView(KoView *view)
{
    d->views.removeAll(view);

    if (d->views.isEmpty()) {
        KoApplication *app = qobject_cast<KoApplication*>(KApplication::kApplication());
        if (0 != app) {
            emit app->documentClosed('/'+objectName());
        }
    }
}

QList<KoView*> KoPart::views() const
{
    return d->views;
}

int KoPart::viewCount() const
{
    return d->views.count();
}

QGraphicsItem *KoPart::canvasItem(bool create)
{
    if (create && !d->canvasItem) {
        d->canvasItem = createCanvasItem();
    }
    return d->canvasItem;
}

QGraphicsItem *KoPart::createCanvasItem()
{
    KoView *view = createView();
    QGraphicsProxyWidget *proxy = new QGraphicsProxyWidget();
    QWidget *canvasController = view->findChild<KoCanvasControllerWidget*>();
    proxy->setWidget(canvasController);
    return proxy;
}

void KoPart::addShell(KoMainWindow *shell)
{
    if (d->shells.indexOf(shell) == -1) {
        //kDebug(30003) <<"shell" << (void*)shell <<"added to doc" << this;
        d->shells.append(shell);
        // XXX!!!
        //connect(shell, SIGNAL(documentSaved()), d->undoStack, SLOT(setClean()));
    }
}

void KoPart::removeShell(KoMainWindow *shell)
{
    //kDebug(30003) <<"shell" << (void*)shell <<"removed from doc" << this;
    if (shell) {
        disconnect(shell, SIGNAL(documentSaved()), d->document->undoStack(), SLOT(setClean()));
        d->shells.removeAll(shell);
    }
}

const QList<KoMainWindow*>& KoPart::shells() const
{
    return d->shells;
}

int KoPart::shellCount() const
{
    return d->shells.count();
}


KoMainWindow *KoPart::currentShell() const
{
    QWidget *widget = qApp->activeWindow();
    KoMainWindow *shell = qobject_cast<KoMainWindow*>(widget);
    while (!shell && widget) {
        widget = widget->parentWidget();
        shell = qobject_cast<KoMainWindow*>(widget);
    }

    if (!shell && d->document && shells().size() > 0) {
        shell = shells().first();
    }
    return shell;

}

void KoPart::addRecentURLToAllShells(KUrl url)
{
    // Add to recent actions list in our shells
    foreach(KoMainWindow *mainWindow, d->shells) {
        mainWindow->addRecentURL(url);
    }

}

void KoPart::setTitleModified(const QString &caption, bool mod)
{
    // we must be root doc so update caption in all related windows
    foreach(KoMainWindow *mainWindow, d->shells) {
        mainWindow->updateCaption(caption, mod);
        mainWindow->updateReloadFileAction(d->document);
        mainWindow->updateVersionsFileAction(d->document);
    }
}



#include <KoPart.moc>
