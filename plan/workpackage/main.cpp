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
#include "application.h"

#include <KDBusService>

#include <QDir>


extern "C" KPLATOWORK_EXPORT int kdemain( int argc, char **argv )
{
    KPlatoWork_Application app(argc, argv);

    KDBusService service(KDBusService::Unique);
    QObject::connect(&service, &KDBusService::activateRequested,
                     &app, &KPlatoWork_Application::handleActivateRequest);

    app.handleCommandLine(QDir::current());
    app.exec();

    return 0;
}
