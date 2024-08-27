/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "core/DocBase.h"
#include "part/AboutData.h"

#include <KoApplication.h>

#include <QLoggingCategory>

using namespace Calligra::Sheets;

int main(int argc, char **argv)
{
    /**
     * Disable debug output by default, only log warnings.
     * Debug logs can be controlled by the environment variable QT_LOGGING_RULES.
     *
     * For example, to get full debug output, run the following:
     * QT_LOGGING_RULES="calligra.*=true" calligrasheets
     *
     * See: http://doc.qt.io/qt-5/qloggingcategory.html
     */
    QLoggingCategory::setFilterRules(
        "calligra.*.debug=false\n"
        "calligra.*.warning=true");

    // QT5TODO: support custom options
    //     options.add("scriptfile <scriptfile>", ki18n("Execute the scriptfile after startup."));

    KoApplication app(SHEETS_MIME_TYPE, QStringLiteral("org.kde.calligra.sheets"), newAboutData, argc, argv);
    KLocalizedString::setApplicationDomain("calligrasheets");

    if (!app.start()) {
        return 1;
    }

    return app.exec();
}
