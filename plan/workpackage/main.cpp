/* This file is part of the KDE project
   Copyright (C) 2001 Thomas zander <zander@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/


#include "kplatowork_export.h"
#include "aboutdata.h"
#include "application.h"

#include <kuniqueapplication.h>
#include <kdemacros.h>
#include <kcmdlineargs.h>
#include <kmessagebox.h>
#include <kdesktopfile.h>
#include <kstandarddirs.h>
#include <kdebug.h>

#include <QtDBus/QtDBus>
#include <QFile>


extern "C" KDE_EXPORT int kdemain( int argc, char **argv ) {
    KCmdLineArgs::init( argc, argv, KPlatoWork::newAboutData());
    KCmdLineOptions options;
    options.add("+[file]", ki18n("File to open"));
    KCmdLineArgs::addCmdLineOptions( options );

    if (!KUniqueApplication::start()) {
       fprintf(stderr, "PlanWork is already running!\n");
       return 0;
    }

    KPlatoWork_Application app;
    fprintf(stderr, "app created\n");

    app.exec();
    return 0;
}
