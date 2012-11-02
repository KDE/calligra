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

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <kstandarddirs.h>
#include <kglobal.h>

#include "MainWindow.h"

#include "DocumentListModel.h"
#include "KisSketchView.h"
#include "SketchInputContext.h"
#include "cpuid.h"


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

    KApplication app;
    QDir appdir(app.applicationDirPath());
    appdir.cdUp();

#ifdef Q_OS_WIN
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // If there's no kdehome, set it and restart the process.
    //QMessageBox::information(0, "krita sketch", "KDEHOME: " + env.value("KDEHOME"));
    if (!env.contains("KDEHOME") ) {
        //QMessageBox::information(0, "sketch: data env", QDesktopServices::storageLocation(QDesktopServices::DataLocation));
        //QMessageBox::information(0, "sketch: home env", QDesktopServices::storageLocation(QDesktopServices::HomeLocation));
        env.insert("KDEHOME", QDesktopServices::storageLocation(QDesktopServices::HomeLocation)
                   + "/AppData"
                   + "/Roaming"
                   + "/kritasketch");
        env.insert("KDEDIR", appdir.currentPath());
        env.insert("KDEDIRS", appdir.currentPath());
        QString currentPath = env.value("PATH");
        env.insert("PATH", appdir.currentPath() + "/bin" + ";"
                   + appdir.currentPath() + "/lib" + ";"
                   + appdir.currentPath() + "/lib"  +  "/kde4" + ";"
                   + currentPath);

        QProcess *p = new QProcess();
        p->setProcessEnvironment(env);
        //QMessageBox::information(0, "Arguments", fileNames.join(", "));
        p->start(app.applicationFilePath(), fileNames);
        // the process doesn't get deleted -- we leak it, but that's fine.
        exit(0);
    }
#endif

    app.addLibraryPath(appdir.currentPath());
    app.addLibraryPath(appdir.currentPath() + "/bin");
    app.addLibraryPath(appdir.currentPath() + "/lib");
    app.addLibraryPath(appdir.currentPath() + "/lib/kde4");

    QApplication::setAttribute(Qt::AA_X11InitThreads);


//    QApplication::setStyle("plastique");

    QStringList fonts = KGlobal::dirs()->findAllResources( "appdata", "fonts/*.otf" );
    foreach( const QString &font, fonts ) {
        QFontDatabase::addApplicationFont( font );
    }

    QFontDatabase db;
    QApplication::setFont( db.font( "Source Sans Pro", "Regular", 12 ) );

    MainWindow window(fileNames);

    if (!isUltraBook()) {
        app.setInputContext(new SketchInputContext(&app));
    }

#ifdef Q_OS_WIN
    window.showFullScreen();
#else
    window.show();
#endif

    return app.exec();
}
