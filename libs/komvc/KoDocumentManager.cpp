/*
 *  Copyright (c) 2013 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "KoDocumentManager.h"

#include <QStringList>
#include <QDebug>

#include <KoDocumentFactory.h>
#include <KoViewFactory.h>
#include <KoMainWindowFactory.h>

class KoDocumentManager::Private {
public:
    Private()
        : viewFactory(0)
        , documentFactory(0)
        , mainwindowFactory(0)
    {}

    KoViewFactory *viewFactory;
    KoDocumentFactory *documentFactory;
    KoMainWindowFactory *mainwindowFactory;
};

KoDocumentManager::KoDocumentManager(QObject *parent)
    : QObject(parent)
    , d(new Private())
{
}

void KoDocumentManager::setDocumentFactory(KoDocumentFactory *documentFactory)
{
    d->documentFactory = documentFactory;
}

void KoDocumentManager::setViewFactory(KoViewFactory *viewFactory)
{
    d->viewFactory = viewFactory;
}

void KoDocumentManager::setMainWindowFactory(KoMainWindowFactory *mainWindowFactory)
{
    d->mainwindowFactory = mainWindowFactory;
}

bool KoDocumentManager::initialize(const QStringList &urls)
{
    KoMainWindowBase *mainWindow = d->mainwindowFactory->create(this);
    Q_UNUSED(mainWindow);
    foreach(const QString &url, urls) {
        qDebug() << url;
    }



    return true;
}
