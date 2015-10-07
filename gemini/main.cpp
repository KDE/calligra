/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include <QApplication>
#include <QFontDatabase>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QDir>
#include <QMessageBox>
#include <QSplashScreen>
#include <QDebug>

#include <kapplication.h>
#include <k4aboutdata.h>
#include <kcmdlineargs.h>
#include <kstandarddirs.h>
#include <kglobal.h>
#include <kiconloader.h>

#include "MainWindow.h"

//#include "sketch/SketchInputContext.h"

#include <CalligraVersionWrapper.h>
#include <calligragitversion.h>

int main( int argc, char** argv )
{
    QString version = CalligraVersionWrapper::versionString(true);

    K4AboutData aboutData("calligragemini",
                         "calligrawords",
                         ki18n("Calligra Gemini"),
                         version.toLatin1(),
                         ki18n("Calligra Gemini: Writing and Presenting at Home and on the Go"),
                         K4AboutData::License_GPL,
                         ki18n("(c) 1999-%1 The Calligra team and KO GmbH.\n").subs(CalligraVersionWrapper::versionYear()),
                         KLocalizedString(),
                         "https://www.calligra.org",
                         "submit@bugs.kde.org");

    KCmdLineArgs::init (argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add( "+[files]", ki18n( "Document to open" ) );
    options.add( "vkb", ki18n( "Use the virtual keyboard" ) );
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();
    QStringList fileNames;
    if (args->count() > 0) {
        for (int i = 0; i < args->count(); ++i) {
            QString fileName = args->arg(i);
            if (QFile::exists(fileName)) {
                fileNames << fileName;
            }
        }
    }

    KApplication app;
    app.setApplicationName("calligragemini");
    KIconLoader::global()->addAppDir("calligrawords");
    KIconLoader::global()->addAppDir("words");
    KIconLoader::global()->addAppDir("calligrastage");
    KIconLoader::global()->addAppDir("stage");

#ifdef Q_OS_WIN
    QDir appdir(app.applicationDirPath());
    appdir.cdUp();

    QString envStringSet;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    if (!env.contains("KDESYCOCA")) {
        _putenv_s("KDESYCOCA", QString(appdir.absolutePath() + "/sycoca").toLocal8Bit());
        envStringSet.append("KDESYCOCA ");
    }
    if (!env.contains("XDG_DATA_DIRS")) {
        _putenv_s("XDG_DATA_DIRS", QString(appdir.absolutePath() + "/share").toLocal8Bit());
        envStringSet.append("XDG_DATA_DIRS ");
    }
    _putenv_s("PATH", QString(appdir.absolutePath() + "/bin" + ";"
              + appdir.absolutePath() + "/lib" + ";"
              + appdir.absolutePath() + "/lib"  +  "/kde4" + ";"
              + appdir.absolutePath()).toLocal8Bit());

    if(envStringSet.length() > 0) {
        qDebug() << envStringSet << "were set from main, restarting application in new environment!";
        // Pass all the arguments along, but don't include the application name...
        QProcess::startDetached(app.applicationFilePath(), KCmdLineArgs::allArguments().mid(1));
        exit(0);
    }

    app.addLibraryPath(appdir.absolutePath());
    app.addLibraryPath(appdir.absolutePath() + "/bin");
    app.addLibraryPath(appdir.absolutePath() + "/lib");
    app.addLibraryPath(appdir.absolutePath() + "/lib/kde4");

    QStringList iconThemePaths;
    iconThemePaths << appdir.absolutePath() + "/share/icons";
    QIcon::setThemeSearchPaths(iconThemePaths);
    QIcon::setThemeName("oxygen");
#endif

    if (qgetenv("KDE_FULL_SESSION").isEmpty()) {
        // There are two themes that work for Krita, oxygen and plastique. Try to set plastique first, then oxygen
        qobject_cast<QApplication*>(QApplication::instance())->setStyle("Plastique");
        qobject_cast<QApplication*>(QApplication::instance())->setStyle("Oxygen");
    }

    // then create the pixmap from an xpm: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
//     QPixmap pm(splash_screen_xpm);
//     QSplashScreen splash(pm);
//     splash.show();
//     splash.showMessage(".");
    app.processEvents();

#if defined HAVE_X11
    QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif

    MainWindow window(fileNames);

    if (args->isSet("vkb")) {
//        app.setInputContext(new SketchInputContext(&app));
    }

#ifdef Q_OS_WIN
    window.showMaximized();
#else
    window.show();
#endif
//    splash.finish(&window);

    return app.exec();
}
