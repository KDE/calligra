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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KWORD_ABOUTDATA
#define KWORD_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* KWORD_DESCRIPTION=I18N_NOOP("KOffice Word Processor");
// First official public release Oct 2000 version 0.8
// KOffice-1.1-beta1: version 0.9
// KOffice-1.1-beta2: version 1.1 (to remove confusion)
static const char* KWORD_VERSION="1.2 Beta2";

KAboutData * newKWordAboutData()
{
    KAboutData * aboutData=new KAboutData( "kword", I18N_NOOP("KWord"),
                                           KWORD_VERSION, KWORD_DESCRIPTION, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2002, The KWord Team"), 0,
					   "http://www.koffice.org/kword/" );
    aboutData->addAuthor("Reginald Stadlbauer", 0, "reggie@kde.org");
    aboutData->addAuthor("Thomas Zander", 0, "zander@kde.org");
    aboutData->addAuthor("David Faure", 0, "david@mandrakesoft.com");
    aboutData->addAuthor("Laurent Montel", 0, "lmontel@mandrakesoft.com");
    aboutData->addAuthor("Sven Lüppken", 0, "sven@kde.org");
    aboutData->addAuthor("Frank Dekervel", 0, "Frank.dekervel@student.kuleuven.ac.Be");
    aboutData->addAuthor("Krister Wicksell Eriksson", 0, "krister.wicksell@spray.se");
    aboutData->addAuthor("Dag Andersen", 0, "danders@get2net.dk");
    aboutData->addAuthor("Nash Hoogwater", 0, "nrhoogwater@wanadoo.nl");
    aboutData->addAuthor("Ulrich Kuettler", I18N_NOOP("KFormula"), "ulrich.kuettler@mailbox.tu-dresden.de");
    // filter developers
    aboutData->addAuthor("Shaheed Haque", 0, "srhaque@iee.org");
    aboutData->addAuthor("Werner Trobin", 0, "trobin@kde.org");
    aboutData->addAuthor("Nicolas Goutte", 0, "nicog@snafu.de");
    aboutData->addAuthor("Ariya Hidayat", 0, "ariya@kde.org");
    aboutData->addAuthor("Clarence Dang", 0, "dang@kde.org");
    aboutData->addAuthor("Robert Jacolin", 0, "rjacolin@ifrance.com");
    aboutData->addAuthor("Enno Bartels", 0, "ebartels@nwm.de");
    aboutData->addAuthor("Ewald Snel", 0, "ewald@rambo.its.tudelft.nl");
    aboutData->addAuthor("Tomasz Grobelny", 0, "grotk@poczta.onet.pl");
    aboutData->addAuthor("Michael Johnson", 0, "mikej@xnet.com");
    return aboutData;
}

#endif
