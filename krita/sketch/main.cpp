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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <KStandardDirs>

#include "MainWindow.h"

#include "DocumentListModel.h"
#include "KisSketchView.h"
#include "SketchInputContext.h"

int main( int argc, char** argv )
{


    KAboutData aboutData("kritasketch",
                         0,
                         ki18n("Krita Sketch"),
                         "0.1",
                         ki18n("Krita Sketch: Painting on the Go for Artists"),
                         KAboutData::License_GPL,
                         ki18n("(c) 1999-2012 The Krita team and KO GmbH.\n"),
                         KLocalizedString(),
                         "http://www.krita.org",
                         "submit@bugs.kde.org");


    KCmdLineArgs::init (argc, argv, &aboutData);
    KCmdLineOptions options;

    options.add( "+[files]", ki18n( "Images to open" ) );
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

    QApplication::setAttribute(Qt::AA_X11InitThreads);
    KApplication app;

//#ifdef Q_OS_WIN
//    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
//    QDir appdir(app.applicationDirPath());
//    appdir.cdUp();
//    env.insert("KDEDIR", appdir.currentPath() + "..");
//    env.insert("KDEDIRS", appdir.currentPath() );
//    env.insert("XDG_DATA_DIRS", appdir.currentPath() + QDir::separator() +  "/share");
//    env.insert("XDG_DATA_HOME", appdir.currentPath() + QDir::separator() + "/share");
//    env.insert("KDEHOME", QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + QDir::separator() + "AppData" + QDir::separator() + "Roaming" + QDir::separator() + "kritasketch");
//    app.addLibraryPath(appdir.currentPath());
//    app.addLibraryPath(appdir.currentPath() + "/bin");
//    app.addLibraryPath(appdir.currentPath() + "/lib");
//    app.addLibraryPath(appdir.currentPath() + "/lib/kde4");
//#endif

//    QApplication::setStyle("plastique");

    QStringList fonts = KGlobal::dirs()->findAllResources( "appdata", "fonts/*.otf" );
    foreach( const QString &font, fonts ) {
        QFontDatabase::addApplicationFont( font );
    }

    QFontDatabase db;
    QApplication::setFont( db.font( "Source Sans Pro", "Regular", 12 ) );

    MainWindow window(fileNames);
    app.setInputContext(new SketchInputContext(&app));

#ifdef Q_OS_WIN
    window.showFullScreen();
#else
    window.show();
#endif

    return app.exec();
}
