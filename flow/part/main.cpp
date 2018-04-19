/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#include <KoApplication.h>
#include <Calligra2Migration.h>
#include <memory>

#include "FlowAboutData.h"
#include "FlowDocument.h"

extern "C" Q_DECL_EXPORT int kdemain( int argc, char* argv[] )
{
    KoApplication app(QByteArray(FLOW_MIME_TYPE), QStringLiteral("calligraflow"), newFlowAboutData, argc, argv);
    // Migrate data from kde4 to kf5 locations
    Calligra2Migration m("flow");
    m.setConfigFiles(QStringList() << QStringLiteral("flowrc"));
    m.setUiFiles(QStringList() << QStringLiteral("flow.rc"));
    m.migrate();

    if (!app.start())
        return 1;
    return app.exec();
}
