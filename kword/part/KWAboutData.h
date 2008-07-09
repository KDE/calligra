/* This file is part of the KDE project
   Copyright (C) 1998 - 2001 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KWABOUTDATA_H
#define KWABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* KWORD_DESCRIPTION=I18N_NOOP("KOffice Word Processor");
// First official public release Oct 2000 version 0.8
// KOffice-1.1-beta1: version 0.9
// KOffice-1.1-beta2: version 1.1 (to remove confusion)
// KOffice-1.2: version 1.2
// From then on: always the same as the KOffice version
static const char* KWORD_VERSION=KOFFICE_VERSION_STRING;

KAboutData * newKWordAboutData()
{
    KAboutData * aboutData=new KAboutData( "kword", 0, ki18n("KWord"),
                                           KWORD_VERSION, ki18n(KWORD_DESCRIPTION), KAboutData::License_LGPL,
                                           ki18n("(c) 1998-2008, The KWord Team"), KLocalizedString(),
                                           "http://www.koffice.org/kword/" );
    aboutData->addAuthor(ki18n("David Faure"), KLocalizedString(), "faure@kde.org");
    aboutData->addAuthor(ki18n("Thomas Zander"));
    aboutData->addAuthor(ki18n("Laurent Montel"), KLocalizedString(), "montel@kde.org");
    aboutData->addAuthor(ki18n("Reginald Stadlbauer"), KLocalizedString(), "reggie@kde.org");
    aboutData->addAuthor(ki18n("Sven Lüppken"), KLocalizedString(), "sven@kde.org");
    aboutData->addAuthor(ki18n("Frank Dekervel"), KLocalizedString(), "Frank.dekervel@student.kuleuven.ac.Be");
    aboutData->addAuthor(ki18n("Krister Wicksell Eriksson"), KLocalizedString(), "krister.wicksell@spray.se");
    aboutData->addAuthor(ki18n("Dag Andersen"), KLocalizedString(), "danders@get2net.dk");
    aboutData->addAuthor(ki18n("Nash Hoogwater"), KLocalizedString(), "nrhoogwater@wanadoo.nl");
    aboutData->addAuthor(ki18n("Ulrich Kuettler"), ki18n("KFormula"), "ulrich.kuettler@mailbox.tu-dresden.de");
    // filter developers
    aboutData->addAuthor(ki18n("Shaheed Haque"), KLocalizedString(), "srhaque@iee.org");
    aboutData->addAuthor(ki18n("Werner Trobin"), KLocalizedString(), "trobin@kde.org");
    aboutData->addAuthor(ki18n("Nicolas Goutte"), KLocalizedString(), "goutte@kde.org");
    aboutData->addAuthor(ki18n("Ariya Hidayat"), KLocalizedString(), "ariya@kde.org");
    aboutData->addAuthor(ki18n("Clarence Dang"), KLocalizedString(), "dang@kde.org");
    aboutData->addAuthor(ki18n("Robert Jacolin"), KLocalizedString(), "rjacolin@ifrance.com");
    aboutData->addAuthor(ki18n("Enno Bartels"), KLocalizedString(), "ebartels@nwn.de");
    aboutData->addAuthor(ki18n("Ewald Snel"), KLocalizedString(), "ewald@rambo.its.tudelft.nl");
    aboutData->addAuthor(ki18n("Tomasz Grobelny"), KLocalizedString(), "grotk@poczta.onet.pl");
    aboutData->addAuthor(ki18n("Michael Johnson"));
    return aboutData;
}

#endif
