/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <KAboutData>
#include <KLocalizedString>
#include <kcoreaddons_version.h>

#include <calligra-version.h>

namespace Calligra
{
namespace Sheets
{

// Used currently by OpenCalcExport filter
static const char version[] = CALLIGRA_VERSION_STRING;

static KAboutData *newAboutData()
{
    auto aboutData = new KAboutData(QStringLiteral("calligrasheets"),
                                    i18nc("application name", "Calligra Sheets"),
                                    QStringLiteral(CALLIGRA_VERSION_STRING),
                                    i18n("Spreadsheet Application"),
                                    KAboutLicense::LGPL,
                                    i18n("Copyright 1998-%1, The Calligra Sheets Team", QString::number(CALLIGRA_YEAR)),
                                    QString(),
                                    QStringLiteral("https://www.calligra.org/sheets/"));
    aboutData->setProductName("calligrasheets"); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligra.sheets"));
    aboutData->addAuthor(i18n("Carl Schwan"),
                         i18n("Maintainer/Port to Qt6"),
                         QStringLiteral("carl@carlschwan.eu"),
                         QStringLiteral("https://carlschwan.eu"),
                         QUrl(QStringLiteral("https://carlschwan.eu/avatar.png")));
    aboutData->addAuthor(i18n("Torben Weis"), i18n("Original Author"), "weis@kde.org");
    aboutData->addAuthor(i18n("Marijn Kruisselbrink"), i18n("Former Maintainer"), "mkruisselbrink@kde.org");
    aboutData->addAuthor(i18n("Sebastian Sauer"), i18n("ODS and Excel, functions, scripting"), "mail@dipe.org");
    aboutData->addAuthor(i18n("Laurent Montel"), QString(), "montel@kde.org");
    aboutData->addAuthor(i18n("John Dailey"), QString(), "dailey@vt.edu");
    aboutData->addAuthor(i18n("Philipp Müller"), QString(), "philipp.mueller@gmx.de");
    aboutData->addAuthor(i18n("Ariya Hidayat"), QString(), "ariya@kde.org");
    aboutData->addAuthor(i18n("Norbert Andres"), QString(), "nandres@web.de");
    aboutData->addAuthor(i18n("Shaheed Haque"), i18n("Import/export filter developer"), "srhaque@iee.org");
    aboutData->addAuthor(i18n("Werner Trobin"), i18n("Import/export filter developer"), "trobin@kde.org");
    aboutData->addAuthor(i18n("Nikolas Zimmermann"), i18n("Import/export filter developer"), "wildfox@kde.org");
    aboutData->addAuthor(i18n("David Faure"), QString(), "faure@kde.org");
    aboutData->addAuthor(i18n("Helge Deller"), i18n("Import/export filter developer"), "deller@gmx.de");
    aboutData->addAuthor(i18n("Percy Leonhart"), i18n("Import/export filter developer"), "percy@eris23.org");
    aboutData->addAuthor(i18n("Eva Brucherseifer"), i18n("Import/export filter developer"), "eva@kde.org");
    aboutData->addAuthor(i18n("Phillip Ezolt"), i18n("Import/export filter developer"), "phillipezolt@hotmail.com");
    aboutData->addAuthor(i18n("Enno Bartels"), i18n("Import/export filter developer"), "ebartels@nwn.de");
    aboutData->addAuthor(i18n("Graham Short"), i18n("Import/export filter developer"), "grahshrt@netscape.net");
    aboutData->addAuthor(i18n("Lukáš Tinkl"), QString(), "lukas@kde.org");
    aboutData->addAuthor(i18n("Tomas Mecir"), QString(), "mecirt@gmail.com");
    aboutData->addAuthor(i18n("Raphael Langerhorst"), QString(), "raphael.langerhorst@kdemail.net");
    aboutData->addAuthor(i18n("John Tapsell"), QString(), "john.tapsell@kdemail.net");
    aboutData->addAuthor(i18n("Robert Knight"), QString(), "robertknight@gmail.com");
    aboutData->addAuthor(i18n("Stefan Nikolaus"), QString(), "stefan.nikolaus@kdemail.net");
    aboutData->addAuthor(i18n("Sascha Pfau"), i18n("Developer (functions)"), "mrpeacock@gmail.com");
    aboutData->addAuthor(i18n("Brad Hards"), i18n("Implemented, reviewed and tested various functions"), "bradh@frogmouth.net");
    // standard ki18n translator strings
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    return aboutData;
}

} // namespace Sheets
} // namespace Calligra
