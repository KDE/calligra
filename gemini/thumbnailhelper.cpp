/*
 * Assists in creating thumbnails for Gemini's file views
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

#include <kapplication.h>
#include <k4aboutdata.h>
#include <kcmdlineargs.h>

#include <QFile>
#include <QDir>
#include <QProcessEnvironment>
#include <QDesktopServices>
#include <QDebug>
#include <QTimer>

#include <calligraversion.h>
#include <calligragitversion.h>
#include "ThumbnailHelperImpl.h"

#ifdef Q_OS_WIN
#include <windows.h>
#endif

int main( int argc, char** argv )
{
#ifdef Q_OS_WIN
    SetErrorMode(SEM_NOGPFAULTERRORBOX); 
#endif

    QString calligraVersion(CALLIGRA_VERSION_STRING);
    QString version;

#ifdef CALLIGRA_GIT_SHA1_STRING
    QString gitVersion(CALLIGRA_GIT_SHA1_STRING);
    version = QString("%1 (git %2)").arg(calligraVersion).arg(gitVersion).toLatin1();
#else
    version = calligraVersion;
#endif


    K4AboutData aboutData("calligrageminithumbnailer",
                         "calligrawords",
                         ki18n("Calligra Gemini Thumbnailer"),
                         version.toLatin1(),
                         ki18n("Calligra Gemini: Writing and Presenting at Home and on the Go"),
                         K4AboutData::License_GPL,
                         ki18n("(c) 1999-%1 The Calligra team and KO GmbH.\n").subs(CALLIGRA_YEAR),
                         KLocalizedString(),
                         "http://www.calligra.org",
                         "submit@bugs.kde.org");

    KCmdLineArgs::init (argc, argv, &aboutData);

    KCmdLineOptions options;
    options.add( "in <local-url>", ki18n( "Document to thumbnail" ) );
    options.add( "out <local-url>", ki18n( "The full path for the thumbnail file" ) );
    options.add( "width <pixels>", ki18n( "The width in pixels of the thumbnail" ) );
    options.add( "height <pixels>", ki18n( "The height in pixels of the thumbnail" ) );
    KCmdLineArgs::addCmdLineOptions( options );

    KCmdLineArgs* args = KCmdLineArgs::parsedArgs();

    KApplication app;
    app.setApplicationName("calligrageminithumbnailer");

#ifdef Q_OS_WIN
    QDir appdir(app.applicationDirPath());
    appdir.cdUp();

    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // If there's no kdehome, set it and restart the process.
    //QMessageBox::information(0, "krita sketch", "KDEHOME: " + env.value("KDEHOME"));
    if (!env.contains("KDEHOME") ) {
        _putenv_s("KDEHOME", QDesktopServices::storageLocation(QDesktopServices::DataLocation).toLocal8Bit());
    }
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

#if defined HAVE_X11 && QT_VERSION >= 0x040800
    QApplication::setAttribute(Qt::AA_X11InitThreads);
#endif

    QString inFile = args->getOption("in");
    QString outFile = args->getOption("out");
    // Only create the thunbnail if:
    // 1) The infile exists and
    // 2) The outfile does /not/ exist
    if(!QFile::exists(inFile)) {
        qDebug() << "The document you are attempting to create a thumbnail of does not exist on disk:" << inFile;
    }
    else if(QFile::exists(outFile)) {
        qDebug() << "The thumbnail file you are asking to have used already exists on disk. We will refuse to overwrite it." << outFile;
    }
    else {
        ThumbnailHelperImpl helper;
        helper.convert(inFile, outFile, args->getOption("width").toInt(), args->getOption("height").toInt());
    }
    QTimer::singleShot(0, &app, SLOT(quit()));

    return app.exec();
}
