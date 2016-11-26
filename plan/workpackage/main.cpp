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

#include "commandlineparser.h"

#include <KDBusService>

#include <QApplication>
#include <QDir>

#include <Calligra2Migration.h>

extern "C" KPLATOWORK_EXPORT int kdemain( int argc, char **argv )
{
    QApplication app(argc, argv);
    KDBusService service(KDBusService::Unique);
    // we come here only once...

    // Migrate data from kde4 to kf5 locations
    Calligra2Migration m("calligraplanwork", "planwork");
    m.setConfigFiles(QStringList() << QStringLiteral("planworkrc"));
    m.setUiFiles(QStringList() << QStringLiteral("planwork.rc") << QStringLiteral("planwork_readonly.rc") << QStringLiteral("planworkui.rc"));
    m.migrate();

    CommandLineParser cmd;
    QObject::connect(&service, &KDBusService::activateRequested, &cmd, &CommandLineParser::handleActivateRequest);
    cmd.handleCommandLine(QDir::current());
    return app.exec();
}
