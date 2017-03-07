/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#ifndef CALLIGRA_SHEETS_ABOUTDATA
#define CALLIGRA_SHEETS_ABOUTDATA

#include <KAboutData>
#include <KLocalizedString>
#include <kcoreaddons_version.h>

#include <calligraversion.h>

namespace Calligra
{
namespace Sheets
{

// Used currently by OpenCalcExport filter
static const char version[] = CALLIGRA_VERSION_STRING;

static KAboutData * newAboutData()
{
    KAboutData * aboutData = new KAboutData(
        QStringLiteral("calligrasheets"),
        i18nc("application name", "Calligra Sheets"),
        QStringLiteral(CALLIGRA_VERSION_STRING),
        i18n("Spreadsheet Application"),
        KAboutLicense::LGPL,
        i18n("Copyright 1998-%1, The Calligra Sheets Team", QStringLiteral(CALLIGRA_YEAR)),
        QString(),
        QStringLiteral("https://www.calligra.org/sheets/"));
    aboutData->setProductName("calligrasheets"); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
#if KCOREADDONS_VERSION >= 0x051600
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligrasheets"));
#endif
    aboutData->addAuthor(i18n("Torben Weis"), i18n("Original Author"), "weis@kde.org");
    aboutData->addAuthor(i18n("Marijn Kruisselbrink"), i18n("Maintainer"), "mkruisselbrink@kde.org");
    aboutData->addAuthor(i18n("Sebastian Sauer"), i18n("ODS and Excel, functions, scripting"), "mail@dipe.org");
    aboutData->addAuthor(i18n("Laurent Montel"), QString(), "montel@kde.org");
    aboutData->addAuthor(i18n("John Dailey"), QString(), "dailey@vt.edu");
    aboutData->addAuthor(i18n("Philipp Müller"), QString(), "philipp.mueller@gmx.de");
    aboutData->addAuthor(i18n("Ariya Hidayat"), QString(), "ariya@kde.org");
    aboutData->addAuthor(i18n("Norbert Andres"), QString(), "nandres@web.de");
    aboutData->addAuthor(i18n("Shaheed Haque"),
                         i18n("Import/export filter developer"),
                         "srhaque@iee.org");
    aboutData->addAuthor(i18n("Werner Trobin"),
                         i18n("Import/export filter developer"),
                         "trobin@kde.org");
    aboutData->addAuthor(i18n("Nikolas Zimmermann"),
                         i18n("Import/export filter developer"),
                         "wildfox@kde.org");
    aboutData->addAuthor(i18n("David Faure"), QString(), "faure@kde.org");
    aboutData->addAuthor(i18n("Helge Deller"),
                         i18n("Import/export filter developer"),
                         "deller@gmx.de");
    aboutData->addAuthor(i18n("Percy Leonhart"),
                         i18n("Import/export filter developer"),
                         "percy@eris23.org");
    aboutData->addAuthor(i18n("Eva Brucherseifer"),
                         i18n("Import/export filter developer"),
                         "eva@kde.org");
    aboutData->addAuthor(i18n("Phillip Ezolt"),
                         i18n("Import/export filter developer"),
                         "phillipezolt@hotmail.com");
    aboutData->addAuthor(i18n("Enno Bartels"),
                         i18n("Import/export filter developer"),
                         "ebartels@nwn.de");
    aboutData->addAuthor(i18n("Graham Short"),
                         i18n("Import/export filter developer"),
                         "grahshrt@netscape.net");
    aboutData->addAuthor(i18n("Lukáš Tinkl"), QString(), "lukas@kde.org");
    aboutData->addAuthor(i18n("Tomas Mecir"), QString(), "mecirt@gmail.com");
    aboutData->addAuthor(i18n("Raphael Langerhorst"), QString(), "raphael.langerhorst@kdemail.net");
    aboutData->addAuthor(i18n("John Tapsell"), QString(), "john.tapsell@kdemail.net");
    aboutData->addAuthor(i18n("Robert Knight"), QString() , "robertknight@gmail.com");
    aboutData->addAuthor(i18n("Stefan Nikolaus"), QString(), "stefan.nikolaus@kdemail.net");
    aboutData->addAuthor(i18n("Sascha Pfau"), i18n("Developer (functions)"), "mrpeacock@gmail.com");
    aboutData->addAuthor(i18n("Brad Hards"),
                         i18n("Implemented, reviewed and tested various functions"),
                         "bradh@frogmouth.net");
    // standard ki18n translator strings
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                             i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    return aboutData;
}

} // namespace Sheets
} // namespace Calligra

#endif
