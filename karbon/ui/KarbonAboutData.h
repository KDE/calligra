/* This file is part of the KDE project
   Copyright (C) 2004 Rob Buis <buis@kde.org>

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
#ifndef KARBON_ABOUTDATA_H
#define KARBON_ABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* description = I18N_NOOP("A Vector Graphics Drawing Application.");
static const char* version = KOFFICE_VERSION_STRING;

// This is all implemented here so that the executable and the part can share it
// without sharing an object file.
KAboutData * newKarbonAboutData()
{
    KAboutData * aboutData = new KAboutData(
        "karbon", 0,
        ki18n("Karbon14"),
        version,
        ki18n(description),
        KAboutData::License_LGPL,
        ki18n("(c) 2001-2009, The Karbon Developers"),
        ki18n("You are invited to participate in any way."),

        "http://www.koffice.org/karbon/");
    aboutData->addAuthor(
        ki18n("Rob Buis"),
        KLocalizedString(),
        "buis@kde.org");
    aboutData->addAuthor(
        ki18n("Tomislav Lukman"),
        KLocalizedString(),
        "tomislav.lukman@ck.t-com.hr");
    aboutData->addAuthor(
        ki18n("Benoît Vautrin"),
        KLocalizedString(),
        "benoit.vautrin@free.fr");
    aboutData->addCredit(
        ki18n("Jan Hambrecht"),
        ki18n("Bug fixes and improvements"),
        "jaham@gmx.net");
    aboutData->addCredit(
        ki18n("Peter Simonsson"),
        ki18n("Bug fixes and improvements"),
        "psn@linux.se");
    aboutData->addCredit(
        ki18n("Tim Beaulen"),
        ki18n("Bug fixes and improvements"),
        "tbscope@gmail.com");
    aboutData->addCredit(
        ki18n("Boudewijn Rempt"),
        ki18n("Bug fixes and improvements"),
        "boud@valdyas.org");
    aboutData->addCredit(
        ki18n("Pierre Stirnweiss"),
        ki18n("Bug fixes and improvements"),
        "pierre.stirnweiss_kde@gadz.org");
    aboutData->addCredit(
        ki18n("Inge Wallin"),
        ki18n("Bug fixes"),
        "inge@lysator.liu.se");
    aboutData->addCredit(
        ki18n("Alan Horkan"),
        ki18n("Helpful patches and advice"));
    // TODO: add the names of some helpful souls
    return aboutData;
}

#endif /* KARBON_ABOUTDATA_H */

