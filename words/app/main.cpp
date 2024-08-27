/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include <KWAboutData.h>
#include <KWDocument.h>

#include <KoApplication.h>

#include <QLoggingCategory>

int main(int argc, char **argv)
{
    /**
     * Disable debug output by default, only log warnings.
     * Debug logs can be controlled by the environment variable QT_LOGGING_RULES.
     *
     * For example, to get full debug output, run the following:
     * QT_LOGGING_RULES="calligra.*=true" calligrawords
     *
     * See: http://doc.qt.io/qt-5/qloggingcategory.html
     */
    QLoggingCategory::setFilterRules(
        "calligra.*.debug=false\n"
        "calligra.*.warning=true");

    KoApplication app(WORDS_MIME_TYPE, QStringLiteral("org.kde.calligra.words"), newWordsAboutData, argc, argv);
    KLocalizedString::setApplicationDomain("calligrawords");

    if (!app.start()) {
        return 1;
    }

    return KoApplication::exec();
}
