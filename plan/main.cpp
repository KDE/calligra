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

#include "kptaboutdata.h"
#include "kptmaindocument.h"

#include <KoApplication.h>
#include <Calligra2Migration.h>

#include <QLoggingCategory>

extern "C" KPLATO_EXPORT int kdemain( int argc, char **argv )
{
    /**
     * Disable debug output by default, only log warnings.
     * Debug logs can be controlled by the environment variable QT_LOGGING_RULES.
     *
     * For example, to get full debug output, run the following:
     * QT_LOGGING_RULES="calligra.*=true" calligraplan
     *
     * See: http://doc.qt.io/qt-5/qloggingcategory.html
     */
    QLoggingCategory::setFilterRules("calligra.*.debug=false\n"
                                     "calligra.*.warning=true");

    KoApplication app(PLAN_MIME_TYPE, QStringLiteral("calligraplan"), KPlato::newAboutData, argc, argv);

    // Migrate data from kde4 to kf5 locations
    Calligra2Migration m("calligraplan", "plan");
    m.setConfigFiles(QStringList() << QStringLiteral("planrc"));
    m.setUiFiles(QStringList() << QStringLiteral("plan.rc") << QStringLiteral("plan_readonly.rc"));
    m.migrate();
    
    if (!app.start()) {
	return 1;
    }

    return app.exec();
}
