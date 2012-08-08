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
#include <QFile>
#include <QStringList>
#include <QString>

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>

#include "MainWindow.h"

#include "DocumentListModel.h"
#include "KisSketchView.h"

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
    //app.setApplicationName("org.krita.sketch");
    QApplication::setStyle("plastique");

    MainWindow window(fileNames);
    window.showMaximized();

    return app.exec();
}
