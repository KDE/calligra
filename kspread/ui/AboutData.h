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
    KAboutData * aboutData = new KAboutData( "kspread", 0, ki18n("KSpread"),
                                             version, ki18n(description), KAboutData::License_LGPL,
                                             ki18n("(c) 1998-2007, The KSpread Team"), KLocalizedString(),
               "http://www.koffice.org/kspread/");
    aboutData->addAuthor(ki18n("Torben Weis"), ki18n("Original Author"), "weis@kde.org");
    aboutData->addAuthor(ki18n("Laurent Montel"), ki18n("Maintainer"), "montel@kde.org");
    aboutData->addAuthor(ki18n("John Dailey"), KLocalizedString(), "dailey@vt.edu");
    aboutData->addAuthor(ki18n("Philipp Müller"), KLocalizedString(), "philipp.mueller@gmx.de");
    aboutData->addAuthor(ki18n("Ariya Hidayat"), KLocalizedString(), "ariya@kde.org");
    aboutData->addAuthor(ki18n("Norbert Andres"), KLocalizedString(), "nandres@web.de");
    aboutData->addAuthor(ki18n("Shaheed Haque"), KLocalizedString(), "srhaque@iee.org");
    aboutData->addAuthor(ki18n("Werner Trobin"), KLocalizedString(), "trobin@kde.org");
    aboutData->addAuthor(ki18n("Nikolas Zimmermann"), KLocalizedString(), "wildfox@kde.org");
    aboutData->addAuthor(ki18n("David Faure"), KLocalizedString(), "faure@kde.org");
    aboutData->addAuthor(ki18n("Helge Deller"), KLocalizedString(), "deller@gmx.de");
    aboutData->addAuthor(ki18n("Percy Leonhart"), KLocalizedString(), "percy@eris23.org");
    aboutData->addAuthor(ki18n("Eva Brucherseifer"), KLocalizedString(), "eva@kde.org");
    aboutData->addAuthor(ki18n("Phillip Ezolt"), KLocalizedString(), "phillipezolt@hotmail.com");
    aboutData->addAuthor(ki18n("Enno Bartels"), KLocalizedString(), "ebartels@nwn.de");
    aboutData->addAuthor(ki18n("Graham Short"), KLocalizedString(), "grahshrt@netscape.net");
    aboutData->addAuthor(ki18n("Lukáš Tinkl"), KLocalizedString(), "lukas@kde.org");
    aboutData->addAuthor(ki18n("Tomas Mecir"), KLocalizedString(), "mecirt@gmail.com");
    aboutData->addAuthor(ki18n("Raphael Langerhorst"), KLocalizedString(), "raphael.langerhorst@kdemail.net");
    aboutData->addAuthor(ki18n("John Tapsell"), KLocalizedString(), "john.tapsell@kdemail.net");
    aboutData->addAuthor(ki18n("Robert Knight"), KLocalizedString() , "robertknight@gmail.com");
    aboutData->addAuthor(ki18n("Stefan Nikolaus"), KLocalizedString(), "stefan.nikolaus@kdemail.net");
    // "Thanks to" section
    aboutData->addCredit(ki18n("Sascha Pfau"),
                         ki18n("Functions:\tDATE2UNIX, DATEDIF, IMLOG2, IMLOG10, NETWORKDAY,\n"
                         "\tUNIX2DATE, WEEKNUM, WORKDAY, YEARFRAC"), "mrpeacock@gmail.com");
    aboutData->addCredit(ki18n("Brad Hards"), ki18n("Various functions reviewed for OpenFormula compliance"),
                         "bradh@frogmouth.net");
    return aboutData;
}

} // namespace KSpread

#endif
