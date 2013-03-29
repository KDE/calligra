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
#include "KoApplicationBase.h"

#include <QDebug>
#include <QCoreApplication>
#include <QSplashScreen>
#include <QPixmap>
#include <QString>

#include <KoDocumentManager.h>

class KoApplicationBase::Private {
public:
    Private()
        : app(0)
        , splash(0)
    {}

    QCoreApplication *app;
    QSplashScreen *splash;
};

KoApplicationBase::KoApplicationBase()
    : d(new Private)
{
}

KoApplicationBase::~KoApplicationBase()
{
    delete d;
}

void KoApplicationBase::setApplication(QCoreApplication *app)
{
    d->app = app;
}


bool KoApplicationBase::start(KoDocumentManager *documentManager)
{
    Q_ASSERT(d->app);
#ifdef Q_OS_WIN
    QDir appdir(applicationDirPath());
    appdir.cdUp();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // If there's no kdehome, set it and restart the process.
    if (!env.contains("KDEHOME")) {
        qputenv("KDEHOME", QFile::encodeName(QDesktopServices::storageLocation(QDesktopServices::DataLocation)));
    }
    if (!env.contains("KDESYCOCA")) {
        qputenv("KDESYCOCA", QFile::encodeName(appdir.absolutePath() + "/sycoca"));
    }
    if (!env.contains("XDG_DATA_DIRS")) {
        qputenv("XDG_DATA_DIRS", QFile::encodeName(appdir.absolutePath() + "/share"));
    }
    if (!env.contains("KDEDIR")) {
        qputenv("KDEDIR", QFile::encodeName(appdir.absolutePath()));
    }
    if (!env.contains("KDEDIRS")) {
        qputenv("KDEDIRS", QFile::encodeName(appdir.absolutePath()));
    }
    qputenv("PATH", QFile::encodeName(appdir.absolutePath() + "/bin" + ";"
                                      + appdir.absolutePath() + "/lib" + ";"
                                      + appdir.absolutePath() + "/lib"  +  "/kde4" + ";"
                                      + appdir.absolutePath()));
#endif

    if (d->splash) {
        d->splash->show();
        d->splash->showMessage(".");
    }

    QStringList args = d->app->arguments();
    qDebug() << args;
    documentManager->initialize(args);

    if (d->splash) {
        d->splash->hide();
        delete d->splash;
        d->splash = 0;
    }
    return true;
}


void KoApplicationBase::setSplashScreen(const QPixmap &splash)
{
    d->splash = new QSplashScreen(splash);
}

void KoApplicationBase::showSplashMessage(const QString &message)
{
    if (d->splash) {
        d->splash->showMessage(message);
    }
}
