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
#include <calligraversion.h>

static const char* WORDS_DESCRIPTION = I18N_NOOP("Word processor");
static const char* WORDS_VERSION = CALLIGRA_VERSION_STRING;

KAboutData * newWordsAboutData()
{
    KAboutData * aboutData = new KAboutData("words", 0, ki18nc("application name", "Calligra Words"),
                                            WORDS_VERSION, ki18n(WORDS_DESCRIPTION), KAboutData::License_LGPL,
                                            ki18n("© 1998-2012, The Words Team"), KLocalizedString(),
                                            "http://www.calligra.org/words/");
    aboutData->setProductName("calligrawords"); // for bugs.kde.org
    aboutData->addAuthor(ki18n("Pierre Ducroquet"), ki18n("Co maintainer"), "");
    aboutData->addAuthor(ki18n("C. Boemann"), ki18n("Co maintainer"), "cbo@boemann.dk");
    aboutData->addAuthor(ki18n("Sebastian Sauer"), ki18n("Everything"), "mail@dipe.org");
    aboutData->addAuthor(ki18n("Boudewijn Rempt"), ki18n("Everything"), "boud@kde.org");
    aboutData->addAuthor(ki18n("Pierre Stirnweiss"), ki18n("Everything"), "");
    aboutData->addAuthor(ki18n("Inge Wallin"), ki18n("Formatting stuff"), "inge@lysator.liu.se");
    aboutData->addAuthor(ki18n("Thorsten Zachmann"), ki18n("Everything"), "zachmann@kde.org");
    aboutData->addAuthor(ki18n("Matus Uzak"), ki18n("Filter"), "matus.uzak@ixonos.com");
    aboutData->addAuthor(ki18n("Pavol Korinek"), ki18n("Layout and Painting"), "pavol.korinek@ixonos.com");
    aboutData->addAuthor(ki18n("Shreya Pandit"), ki18n("Statistics docker"), "");
    aboutData->addAuthor(ki18n("Brijesh Patel"), ki18n("Foot and endnotes"), "");
    aboutData->addAuthor(ki18n("Smit Patel"), ki18n("Bibliography"), "");
    aboutData->addAuthor(ki18n("Mojtaba Shahi"), ki18n("Style Manager"), "");
    aboutData->addAuthor(ki18n("Lassi Nieminen"), ki18n("Filter"), "lassniem@gmail.com");
    aboutData->addAuthor(ki18n("Hanzes Matus"), ki18n("Filter"), "matus.hanzes@ixonos.com");
    aboutData->addAuthor(ki18n("Lukáš Tvrdý"), ki18n("Filter"), "lukast.dev@gmail.com");
    aboutData->addAuthor(ki18n("Thomas Zander"), KLocalizedString(), "zander@kde.org");
    aboutData->addAuthor(ki18n("Girish Ramakrishnan"), ki18n("ODF Support"), "girish@forwardbias.in");
    aboutData->addAuthor(ki18n("Robert Mathias Marmorstein"), ki18n("ODF Support"), "robert@narnia.homeunix.com");
    aboutData->addAuthor(ki18n("David Faure"), KLocalizedString(), "faure@kde.org");
    aboutData->addAuthor(ki18n("Laurent Montel"), KLocalizedString(), "montel@kde.org");
    aboutData->addAuthor(ki18n("Sven Lüppken"), KLocalizedString(), "sven@kde.org");
    aboutData->addAuthor(ki18n("Frank Dekervel"), KLocalizedString(), "Frank.dekervel@student.kuleuven.ac.Be");
    aboutData->addAuthor(ki18n("Krister Wicksell Eriksson"), KLocalizedString(), "krister.wicksell@spray.se");
    aboutData->addAuthor(ki18n("Dag Andersen"), KLocalizedString(), "danders@get2net.dk");
    aboutData->addAuthor(ki18n("Nash Hoogwater"), KLocalizedString(), "nrhoogwater@wanadoo.nl");
    aboutData->addAuthor(ki18n("Ulrich Kuettler"), ki18n("KFormula"), "ulrich.kuettler@mailbox.tu-dresden.de");
    aboutData->addAuthor(ki18n("Shaheed Haque"), ki18n("Filter"), "srhaque@iee.org");
    aboutData->addAuthor(ki18n("Werner Trobin"), ki18n("Filter"), "trobin@kde.org");
    aboutData->addAuthor(ki18n("Nicolas Goutte"), ki18n("Filter"), "goutte@kde.org");
    aboutData->addAuthor(ki18n("Ariya Hidayat"), ki18n("Filter"), "ariya@kde.org");
    aboutData->addAuthor(ki18n("Clarence Dang"), ki18n("Filter"), "dang@kde.org");
    aboutData->addAuthor(ki18n("Robert Jacolin"), ki18n("Filter"), "rjacolin@ifrance.com");
    aboutData->addAuthor(ki18n("Enno Bartels"), ki18n("Filter"), "ebartels@nwn.de");
    aboutData->addAuthor(ki18n("Ewald Snel"), ki18n("Filter"), "ewald@rambo.its.tudelft.nl");
    aboutData->addAuthor(ki18n("Tomasz Grobelny"), ki18n("Filter"), "grotk@poczta.onet.pl");
    aboutData->addAuthor(ki18n("Michael Johnson"), ki18n("Filter"));
    aboutData->addAuthor(ki18n("Fatcow Web Hosting"), ki18n("Page break icon"), "http://www.fatcow.com/free-icons");
    return aboutData;
}

#endif
