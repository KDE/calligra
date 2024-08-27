/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KPrAboutData.h>
#include <KPrDocument.h>

#include <KoApplication.h>

#include <QLoggingCategory>

int main(int argc, char **argv)
{
    /**
     * Disable debug output by default, only log warnings.
     * Debug logs can be controlled by the environment variable QT_LOGGING_RULES.
     *
     * For example, to get full debug output, run the following:
     * QT_LOGGING_RULES="calligra.*=true" calligrastage
     *
     * See: http://doc.qt.io/qt-5/qloggingcategory.html
     */
    QLoggingCategory::setFilterRules(
        "calligra.*.debug=false\n"
        "calligra.*.warning=true");

    KoApplication app(STAGE_MIME_TYPE, QStringLiteral("org.kde.calligra.stage"), newKPresenterAboutData, argc, argv);
    KLocalizedString::setApplicationDomain("calligrastage");

    if (!app.start()) {
        return 1;
    }

    return app.exec();
}
