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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KSPREAD_ABOUTDATA
#define KSPREAD_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

namespace KSpread
{

static const char* description=I18N_NOOP("KOffice Spreadsheet Application");

// Always the same as the KOffice version
static const char* version=KOFFICE_VERSION_STRING;

KAboutData * newAboutData()
{
    KAboutData * aboutData = new KAboutData( "kspread", I18N_NOOP("KSpread"),
                                             version, description, KAboutData::License_LGPL,
                                             I18N_NOOP("(c) 1998-2006, The KSpread Team"), 0,
               "http://www.koffice.org/kspread/");
    aboutData->addAuthor("Torben Weis", 0, "weis@kde.org");
    aboutData->addAuthor("Laurent Montel", 0, "montel@kde.org");
    aboutData->addAuthor("John Dailey", 0, "dailey@vt.edu");
    aboutData->addAuthor("Philipp Müller", 0, "philipp.mueller@gmx.de");
    aboutData->addAuthor("Ariya Hidayat", 0, "ariya@kde.org");
    aboutData->addAuthor("Norbert Andres", 0, "nandres@web.de");
    aboutData->addAuthor("Shaheed Haque", 0, "srhaque@iee.org");
    aboutData->addAuthor("Werner Trobin", 0, "trobin@kde.org");
    aboutData->addAuthor("Nikolas Zimmermann", 0, "wildfox@kde.org");
    aboutData->addAuthor("David Faure", 0, "faure@kde.org");
    aboutData->addAuthor("Helge Deller", 0, "deller@gmx.de");
    aboutData->addAuthor("Percy Leonhart", 0, "percy@eris23.org");
    aboutData->addAuthor("Eva Brucherseifer", 0, "eva@kde.org");
    aboutData->addAuthor("Phillip Ezolt", 0, "phillipezolt@hotmail.com");
    aboutData->addAuthor("Enno Bartels", 0, "ebartels@nwn.de");
    aboutData->addAuthor("Graham Short", 0, "grahshrt@netscape.net");
    aboutData->addAuthor("Lukáš Tinkl", 0, "lukas@kde.org");
    aboutData->addAuthor("Tomas Mecir", 0, "mecirt@gmail.com");
    aboutData->addAuthor("Raphael Langerhorst", 0, "raphael.langerhorst@kdemail.net");
    aboutData->addAuthor("John Tapsell", 0, "john.tapsell@kdemail.net");
    aboutData->addAuthor("Robert Knight", 0 , "robertknight@gmail.com");
    aboutData->addAuthor("Stefan Nikolaus", 0, "stefan.nikolaus@kdemail.net");
    return aboutData;
}

} // namespace KSpread

#endif
