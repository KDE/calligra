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

#include <kaboutdata.h>
#include <klocale.h>
#include <calligraversion.h>

namespace Calligra
{
namespace Sheets
{

static const char description[] = I18N_NOOP("Calligra Spreadsheet Application");

// Always the same as the Calligra version
static const char version[] = CALLIGRA_VERSION_STRING;

static KAboutData * newAboutData()
{
    KAboutData * aboutData = new KAboutData("sheets", 0, "Calligra Sheets",
                                            version, description, KAboutData::License_LGPL,
                                            "(c) 1998-2011, The Calligra Sheets Team", "",
                                            "http://www.calligra.org/sheets/");
    aboutData->setProductName("calligrasheets"); // for bugs.kde.org
    aboutData->setProgramIconName(QLatin1String("calligrasheets"));
    aboutData->addAuthor("Torben Weis", "Original Author", "weis@kde.org");
    aboutData->addAuthor("Marijn Kruisselbrink", "Maintainer", "mkruisselbrink@kde.org");
    aboutData->addAuthor("Sebastian Sauer", "ODS and Excel, functions, scripting", "mail@dipe.org");
    aboutData->addAuthor("Laurent Montel", "", "montel@kde.org");
    aboutData->addAuthor("John Dailey", "", "dailey@vt.edu");
    aboutData->addAuthor("Philipp Müller", "", "philipp.mueller@gmx.de");
    aboutData->addAuthor("Ariya Hidayat", "", "ariya@kde.org");
    aboutData->addAuthor("Norbert Andres", "", "nandres@web.de");
    aboutData->addAuthor("Shaheed Haque",
                         "Import/export filter developer",
                         "srhaque@iee.org");
    aboutData->addAuthor("Werner Trobin",
                         "Import/export filter developer",
                         "trobin@kde.org");
    aboutData->addAuthor("Nikolas Zimmermann",
                         "Import/export filter developer",
                         "wildfox@kde.org");
    aboutData->addAuthor("David Faure", "", "faure@kde.org");
    aboutData->addAuthor("Helge Deller",
                         "Import/export filter developer",
                         "deller@gmx.de");
    aboutData->addAuthor("Percy Leonhart",
                         "Import/export filter developer",
                         "percy@eris23.org");
    aboutData->addAuthor("Eva Brucherseifer",
                         "Import/export filter developer",
                         "eva@kde.org");
    aboutData->addAuthor("Phillip Ezolt",
                         "Import/export filter developer",
                         "phillipezolt@hotmail.com");
    aboutData->addAuthor("Enno Bartels",
                         "Import/export filter developer",
                         "ebartels@nwn.de");
    aboutData->addAuthor("Graham Short",
                         "Import/export filter developer",
                         "grahshrt@netscape.net");
    aboutData->addAuthor("Lukáš Tinkl", "", "lukas@kde.org");
    aboutData->addAuthor("Tomas Mecir", "", "mecirt@gmail.com");
    aboutData->addAuthor("Raphael Langerhorst", "", "raphael.langerhorst@kdemail.net");
    aboutData->addAuthor("John Tapsell", "", "john.tapsell@kdemail.net");
    aboutData->addAuthor("Robert Knight", "" , "robertknight@gmail.com");
    aboutData->addAuthor("Stefan Nikolaus", "", "stefan.nikolaus@kdemail.net");
    aboutData->addAuthor("Sascha Pfau", "Developer (functions)", "mrpeacock@gmail.com");
    aboutData->addAuthor("Brad Hards",
                         "Implemented, reviewed and tested various functions",
                         "bradh@frogmouth.net");
    return aboutData;
}

} // namespace Sheets
} // namespace Calligra

#endif
