/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>

#include "mainwindow.h"

int main( int argc, char** argv )
{
    KAboutData aboutData(
                             // The program name used internally.
                             "kritasketchmockup",
                             // The message catalog name
                             // If null, program name is used instead.
                             0,
                             // A displayable program name string.
                             ki18n("Krita Sketch"),
                             // The program version string.
                             "1.0",
                             // Short description of what the app does.
                             ki18n("Touch based painting application for proffessionals"),
                             // The license this code is released under
                             KAboutData::License_GPL,
                             // Copyright Statement
                             ki18n("(c) 2012"),
                             // Optional text shown in the About box.
                             // Can contain any information desired.
                             ki18n("Paint your heart's desire"),
                             // The program homepage string.
                             "http://krita.org/",
                             // The bug report email address
                             "submit@bugs.kde.org");
    KCmdLineArgs::init( argc, argv, &aboutData );
    KApplication app;

    MainWindow win;
    win.show();

    return app.exec();
}
