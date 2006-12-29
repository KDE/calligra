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

#ifndef KWORD_ABOUTDATA
#define KWORD_ABOUTDATA

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
    KAboutData * aboutData=new KAboutData( "kword", I18N_NOOP("KWord"),
                                           KWORD_VERSION, KWORD_DESCRIPTION, KAboutData::License_LGPL,
                                           I18N_NOOP("(c) 1998-2006, The KWord Team"), 0,
                                           "http://www.koffice.org/kword/" );
    aboutData->addAuthor("David Faure", 0, "faure@kde.org");
    aboutData->addAuthor("Reginald Stadlbauer", 0, "reggie@kde.org");
    aboutData->addAuthor("Thomas Zander");
    aboutData->addAuthor("Laurent Montel", 0, "montel@kde.org");
    aboutData->addAuthor("Sven Lüppken", 0, "sven@kde.org");
    aboutData->addAuthor("Frank Dekervel", 0, "Frank.dekervel@student.kuleuven.ac.Be");
    aboutData->addAuthor("Krister Wicksell Eriksson", 0, "krister.wicksell@spray.se");
    aboutData->addAuthor("Dag Andersen", 0, "danders@get2net.dk");
    aboutData->addAuthor("Nash Hoogwater", 0, "nrhoogwater@wanadoo.nl");
    aboutData->addAuthor("Ulrich Kuettler", I18N_NOOP("KFormula"), "ulrich.kuettler@mailbox.tu-dresden.de");
    // filter developers
    aboutData->addAuthor("Shaheed Haque", 0, "srhaque@iee.org");
    aboutData->addAuthor("Werner Trobin", 0, "trobin@kde.org");
    aboutData->addAuthor("Nicolas Goutte", 0, "goutte@kde.org");
    aboutData->addAuthor("Ariya Hidayat", 0, "ariya@kde.org");
    aboutData->addAuthor("Clarence Dang", 0, "dang@kde.org");
    aboutData->addAuthor("Robert Jacolin", 0, "rjacolin@ifrance.com");
    aboutData->addAuthor("Enno Bartels", 0, "ebartels@nwn.de");
    aboutData->addAuthor("Ewald Snel", 0, "ewald@rambo.its.tudelft.nl");
    aboutData->addAuthor("Tomasz Grobelny", 0, "grotk@poczta.onet.pl");
    aboutData->addAuthor("Michael Johnson");
    return aboutData;
}

#endif
