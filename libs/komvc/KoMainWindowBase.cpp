/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000-2006 David Faure <faure@kde.org>
   Copyright (C) 2007, 2009 Thomas zander <zander@kde.org>
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KoMainWindowBase.h"
#include <QCloseEvent>
#include "KoDocumentBase.h"
#include "KoViewBase.h"

class KoMainWindowBase::Private
{
public:

    Private()
        : viewFactory(0)
        , documentFactory(0)
    {}

        KoViewFactory *viewFactory;
        KoDocumentFactory *documentFactory;
};

KoMainWindowBase::KoMainWindowBase()
    : QMainWindow()
    , d(new Private())
{

}

KoMainWindowBase::~KoMainWindowBase()
{
    delete d;
}

void KoMainWindowBase::setDocumentFactory(KoDocumentFactory *factory)
{
    d->documentFactory = factory;
}

void KoMainWindowBase::setViewFactory(KoViewFactory *factory)
{
    d->viewFactory = factory;
}

bool KoMainWindowBase::fileNew()
{
    if (!d->documentFactory) return false;
    if (!d->viewFactory) return false;

    KoDocumentBase *document = d->documentFactory->create();
    KoViewBase *view = d->viewFactory->create(document, this);

    return addView(view);
}


bool KoMainWindowBase::fileOpen()
{
    return false;
}

bool KoMainWindowBase::fileSave()
{
    return false;
}

bool KoMainWindowBase::fileSaveAs()
{
    return false;
}

bool KoMainWindowBase::fileSaveAll()
{
    return false;
}

bool KoMainWindowBase::fileExport()
{
    return false;
}

bool KoMainWindowBase::fileClose()
{
    return false;
}

bool KoMainWindowBase::fileCloseAll()
{
    return false;
}


void KoMainWindowBase::closeEvent(QCloseEvent *event)
{
    emit aboutToClose();

    // XXX: check whether we want to save all windows here

    event->accept();

}
