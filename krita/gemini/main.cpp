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
#include <QDebug>
#include <QFontDatabase>
#include <QFile>
#include <QStringList>
#include <QString>
#include <QDesktopServices>
#include <QProcessEnvironment>
#include <QDir>
#include <QMessageBox>
#include <QSplashScreen>
#include <QTextStream>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <kcrash.h>
#include <KoConfig.h>

#include "data/splash/splash_screen.xpm"
#include "MainWindow.h"

#include "sketch/SketchInputContext.h"

#ifdef USE_BREAKPAD
#include "../kis_crash_handler.h"
#endif

#if defined HAVE_STEAMWORKS
#include <unistd.h>
#include "steam/kritasteam.h"

#ifndef STEAM_APP_ID_GEMINI
#pragma warning "No Steam APP ID! You will require steam_appid.txt in your executable directory to define this."
#include <steam/steamtypes.h>
#define STEAM_APP_ID_GEMINI k_uAppIdInvalid
#endif

#endif

#if defined Q_OS_WIN
#include "stdlib.h"
#include <ui/input/wintab/kis_tablet_support_win.h>

// Using breakpad support rather than Steam's minidump error reporting
/*
void MiniDumpFunction( unsigned int nExceptionCode, EXCEPTION_POINTERS *pException )
{
    QMessageBox box(0);
    box.setText("Testing");
    box.setModal(true);
    box.show();
    QString comment = "Minidump comment: kritagemini.exe\n";
    KritaSteamClient::MiniDumpFunction(comment, nExceptionCode, (void*) pException);
}
*/
#elif defined Q_WS_X11
#include <ui/input/wintab/kis_tablet_support_x11.h>
#endif



// Startup Message Handler (write to disk)
QFile *startupLogFile = 0;
QTextStream *startupLogStream = 0;

void prepareStartupLogfile() {
    startupLogFile = new QFile(QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/krita-startup.txt");
    startupLogFile->open(QIODevice::WriteOnly);
    startupLogStream = new QTextStream(startupLogFile);
    *startupLogStream << QString("Krita Gemini - Startup") << endl << endl;
}

void closeStartupLogfile() {
    if (startupLogStream != 0) {
        startupLogStream->flush();
        delete startupLogStream;
        startupLogStream = 0;
    }

    if (startupLogFile != 0) {
        startupLogFile->close();
        delete startupLogFile;
        startupLogFile = 0;
    }
}

void customDebugMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    if (startupLogStream != 0) {
        *startupLogStream << txt << endl;
    }
}

int main( int argc, char** argv )
{
    int result;

#ifdef Q_OS_WIN
    prepareStartupLogfile();
    qInstallMsgHandler(customDebugMessageHandler);

    // TODO: re-enabled Steam error reporting if Breakpad support is disabled
    //qDebug("Registering Steam Error handler");
    //_set_se_translator(MiniDumpFunction);
#endif

#ifdef USE_BREAKPAD
    qDebug() << "Enabling breakpad";
    qputenv("KDE_DEBUG", "1");

#ifdef HAVE_STEAMWORKS
    KisCrashHandler crashHandler("kritageministeam");
#else
    KisCrashHandler crashHandler("kritagemini");
#endif
    Q_UNUSED(crashHandler);
#else
    qDebug() << "BREAKPAD IS NOT ENABLED";
#endif

#if defined HAVE_STEAMWORKS
    KritaSteamClient* steamClient = KritaSteamClient::instance();
    steamClient->initialise(STEAM_APP_ID_GEMINI);
#endif

    KAboutData aboutData("kritagemini",
                         "krita",
                         ki18n("Krita Gemini"),
                         "0.1",
                         ki18n("Krita Gemini: Painting at Home and on the Go for Artists"),
                         KAboutData::License_GPL,
                         ki18n("(c) 1999-2014 The Krita team and KO GmbH.\n"),
                         KLocalizedString(),
                         "http://www.krita.org",
                         "submit@bugs.kde.org");

    KCmdLineArgs::init (argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add( "+[files]", ki18n( "Images to open" ) );
    options.add( "vkb", ki18n( "Use the virtual keyboard" ) );
    options.add( "fullscreen", ki18n( "Use full-screen display" ) );
    options.add( "sketch", ki18n( "Start with the Sketch interface" ) );
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
    app.setApplicationName("kritagemini");
    KIconLoader::global()->addAppDir("krita");
    KIconLoader::global()->addAppDir("kritasketch");

#ifdef Q_OS_WIN
    QDir appdir(app.applicationDirPath());
    appdir.cdUp();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // If there's no kdehome, set it and restart the process.
    //QMessageBox::information(0, "krita sketch", "KDEHOME: " + env.value("KDEHOME"));
	/*
    if (!env.contains("KDEHOME") ) {
        _putenv_s("KDEHOME", QDesktopServices::storageLocation(QDesktopServices::DataLocation).toLocal8Bit());
    }
	*/
    if (!env.contains("KDESYCOCA")) {
        _putenv_s("KDESYCOCA", QString(appdir.absolutePath() + "/sycoca").toLocal8Bit());
    }
    if (!env.contains("XDG_DATA_DIRS")) {
        _putenv_s("XDG_DATA_DIRS", QString(appdir.absolutePath() + "/share").toLocal8Bit());
    }
    if (!env.contains("KDEDIR")) {
        _putenv_s("KDEDIR", appdir.absolutePath().toLocal8Bit());
    }
    if (!env.contains("KDEDIRS")) {
        _putenv_s("KDEDIRS", appdir.absolutePath().toLocal8Bit());
    }
    _putenv_s("PATH", QString(appdir.absolutePath() + "/bin" + ";"
                              + appdir.absolutePath() + "/lib" + ";"
                              + appdir.absolutePath() + "/lib"  +  "/kde4" + ";"
                              + appdir.absolutePath()).toLocal8Bit());

    app.addLibraryPath(appdir.absolutePath());
    app.addLibraryPath(appdir.absolutePath() + "/bin");
    app.addLibraryPath(appdir.absolutePath() + "/lib");
    app.addLibraryPath(appdir.absolutePath() + "/lib/kde4");
#endif

#if defined Q_OS_WIN
    KisTabletSupportWin::init();
    app.setEventFilter(&KisTabletSupportWin::eventFilter);
#elif defined Q_WS_X11
    KisTabletSupportX11::init();
    app.setEventFilter(&KisTabletSupportX11::eventFilter);
#endif

    if (qgetenv("KDE_FULL_SESSION").isEmpty()) {
        // There are two themes that work for Krita, oxygen and plastique. Try to set plastique first, then oxygen
        qobject_cast<QApplication*>(QApplication::instance())->setStyle("Plastique");
        qobject_cast<QApplication*>(QApplication::instance())->setStyle("Oxygen");
    }

    // Prepare to show window fullscreen if required
    bool showFullscreen = false;

#ifdef HAVE_STEAMWORKS
    if (steamClient->isInBigPictureMode()) {
        // Show main window full screen
        showFullscreen = true;
    }
#endif

    if (args->isSet("sketch")) {
        showFullscreen = true;
    }

	if (args->isSet("fullscreen")) {
        showFullscreen = true;
    }

    // then create the pixmap from an xpm: we cannot get the
    // location of our datadir before we've started our components,
    // so use an xpm.
    QPixmap pm(splash_screen_xpm);
    QSplashScreen splash(pm);
    splash.show();
    splash.showMessage(".");
    app.processEvents();

#if defined Q_WS_X11 && QT_VERSION >= 0x040800
    QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif

    MainWindow window(fileNames);

    if (args->isSet("vkb")) {
        app.setInputContext(new SketchInputContext(&app));
    }

    if (args->isSet("sketch")) {
        window.setSlateMode(true);
    }

    if (showFullscreen) {
        window.showFullScreen();
        window.forceFullScreen(true);
    } else {
#ifdef Q_OS_WIN
        window.showMaximized();
#else
        window.show();
#endif
    }

    // Revert to standard message handler
    qInstallMsgHandler(0);
    closeStartupLogfile();

    splash.finish(&window);

#if defined HAVE_STEAMWORKS
    if (!steamClient->isInitialised()) {
        // Occasionally it seems that the Steam API might not initialise
        // Alert the user
        QMessageBox::warning(&window,
                             i18n("Steam initialisation error"),
                             i18n("There was a problem starting Steam services. Krita Gemini will run without Steam features. If you are seeing this message, please let us know on the Steam forum."));
    }
    steamClient->mainWindowCreated();
#endif

    result = app.exec();

#if defined HAVE_STEAMWORKS
    steamClient->mainWindowBeingDestroyed();
    steamClient->shutdown();
#endif
    return result;
}
