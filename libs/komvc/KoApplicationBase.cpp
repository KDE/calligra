/*
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2013 Boudewijn Rempt <boud@kde.org>

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
   Boston, MA 02110-1301, USA.
*/
#include "KoApplicationBase.h"

#include <QSplashScreen>
#include <QTime>

#include "KoMainWindowBase.h"
#include "KoDocumentBase.h"
#include "KoControllerBase.h"
#include "KoViewBase.h"

bool KoApplicationBase::m_starting = true;

namespace {
    const QTime appStartTime(QTime::currentTime());
}

class KoApplicationBase::Private
{
public:
    Private()
        : splashScreen(0)
        , windowFactory(0)
        , documentFactory(0)
        , viewFactory(0)
        , controllerBase(0)
    {}

    QSplashScreen *splashScreen;
    KoMainWindowFactory *windowFactory;
    KoDocumentFactory *documentFactory;
    KoViewFactory *viewFactory;
    KoControllerBase *controllerBase;
};

// Small helper for start() so that we don't forget to reset m_starting before a return
class KoApplicationBase::ResetStarting
{
public:
    ResetStarting(QSplashScreen *splash = 0)
        : m_splash(splash)
    {
    }

    ~ResetStarting()  {
        KoApplicationBase::m_starting = false;
        if (m_splash) {
            m_splash->hide();
        }
    }

    QSplashScreen *m_splash;
};

KoApplicationBase::KoApplicationBase(int &argc, char **argv)
    : QApplication(argc, argv)
    , d(new Private())
{
    m_starting = true;
}

bool KoApplicationBase::initialize(KoMainWindowFactory *windowFactory, KoDocumentFactory *documentFactory, KoViewFactory *viewFactory)
{
    d->windowFactory = windowFactory;
    d->documentFactory = documentFactory;
    d->viewFactory = viewFactory;
    d->controllerBase = new KoControllerBase(this);

    if (d->splashScreen) {
        d->splashScreen->show();
        d->splashScreen->showMessage(".");
    }

    ResetStarting resetStarting(d->splashScreen); // reset m_starting to false when we're done
    Q_UNUSED(resetStarting);

    // read the commandline options
    // check for autosave files to restore
    // check whether we are restored from session management
    // create an empty window
    KoMainWindowBase *window = d->windowFactory->create(d->controllerBase);
    window->setDocumentFactory(d->documentFactory);
    window->setViewFactory(d->viewFactory);
    window->show();
    return true;
}

KoApplicationBase::~KoApplicationBase()
{
    delete d->windowFactory;
    delete d->documentFactory;
    delete d->viewFactory;
    delete d;
}

bool KoApplicationBase::isStarting()
{
    return KoApplicationBase::m_starting;
}

void KoApplicationBase::setSplashScreen(QSplashScreen *splashScreen)
{
    d->splashScreen = splashScreen;
}

bool KoApplicationBase::notify(QObject *receiver, QEvent *event)
{
    try {
        return QApplication::notify(receiver, event);
    } catch (std::exception &e) {
        qWarning("Error %s sending event %i to object %s",
                 e.what(), event->type(), qPrintable(receiver->objectName()));
    } catch (...) {
        qWarning("Error <unknown> sending event %i to object %s",
                 event->type(), qPrintable(receiver->objectName()));
    }
    return false;

}

#include <KoApplicationBase.moc>
