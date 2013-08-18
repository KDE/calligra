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

static const char WORDS_DESCRIPTION[] = I18N_NOOP("Word processor");
static const char WORDS_VERSION[] = CALLIGRA_VERSION_STRING;

KAboutData * newWordsAboutData()
{
    KAboutData * aboutData = new KAboutData("words", 0, "Calligra Words",
                                            WORDS_VERSION, WORDS_DESCRIPTION, KAboutData::License_LGPL,
                                            "© 1998-2012, The Words Team", "",
                                            "http://www.calligra.org/words/");

    aboutData->setProductName("calligrawords"); // for bugs.kde.org
    aboutData->setProgramIconName(QLatin1String("calligrawords"));
    aboutData->addAuthor("Pierre Ducroquet", "Co maintainer", "");
    aboutData->addAuthor("C. Boemann", "Co maintainer", "cbo@boemann.dk");
    aboutData->addAuthor("Sebastian Sauer", "Everything", "mail@dipe.org");
    aboutData->addAuthor("Boudewijn Rempt", "Everything", "boud@kde.org");
    aboutData->addAuthor("Pierre Stirnweiss", "Everything", "");
    aboutData->addAuthor("Inge Wallin", "Formatting stuff", "inge@lysator.liu.se");
    aboutData->addAuthor("Thorsten Zachmann", "Everything", "zachmann@kde.org");
    aboutData->addAuthor("Matus Uzak", "Filter", "matus.uzak@ixonos.com");
    aboutData->addAuthor("Pavol Korinek", "Layout and Painting", "pavol.korinek@ixonos.com");
    aboutData->addAuthor("Shreya Pandit", "Statistics docker", "");
    aboutData->addAuthor("Brijesh Patel", "Foot and endnotes", "");
    aboutData->addAuthor("Smit Patel", "Bibliography", "");
    aboutData->addAuthor("Mojtaba Shahi", "Style Manager", "");
    aboutData->addAuthor("Lassi Nieminen", "Filter", "lassniem@gmail.com");
    aboutData->addAuthor("Hanzes Matus", "Filter", "matus.hanzes@ixonos.com");
    aboutData->addAuthor("Lukáš Tvrdý", "Filter", "lukast.dev@gmail.com");
    aboutData->addAuthor("Thomas Zander", "", "zander@kde.org");
    aboutData->addAuthor("Girish Ramakrishnan", "ODF Support", "girish@forwardbias.in");
    aboutData->addAuthor("Robert Mathias Marmorstein", "ODF Support", "robert@narnia.homeunix.com");
    aboutData->addAuthor("David Faure", "", "faure@kde.org");
    aboutData->addAuthor("Laurent Montel", "", "montel@kde.org");
    aboutData->addAuthor("Sven Lüppken", "", "sven@kde.org");
    aboutData->addAuthor("Frank Dekervel", "", "Frank.dekervel@student.kuleuven.ac.Be");
    aboutData->addAuthor("Krister Wicksell Eriksson", "", "krister.wicksell@spray.se");
    aboutData->addAuthor("Dag Andersen", "", "danders@get2net.dk");
    aboutData->addAuthor("Nash Hoogwater", "", "nrhoogwater@wanadoo.nl");
    aboutData->addAuthor("Ulrich Kuettler", "KFormula", "ulrich.kuettler@mailbox.tu-dresden.de");
    aboutData->addAuthor("Shaheed Haque", "Filter", "srhaque@iee.org");
    aboutData->addAuthor("Werner Trobin", "Filter", "trobin@kde.org");
    aboutData->addAuthor("Nicolas Goutte", "Filter", "goutte@kde.org");
    aboutData->addAuthor("Ariya Hidayat", "Filter", "ariya@kde.org");
    aboutData->addAuthor("Clarence Dang", "Filter", "dang@kde.org");
    aboutData->addAuthor("Robert Jacolin", "Filter", "rjacolin@ifrance.com");
    aboutData->addAuthor("Enno Bartels", "Filter", "ebartels@nwn.de");
    aboutData->addAuthor("Ewald Snel", "Filter", "ewald@rambo.its.tudelft.nl");
    aboutData->addAuthor("Tomasz Grobelny", "Filter", "grotk@poczta.onet.pl");
    aboutData->addAuthor("Michael Johnson", "Filter");
    aboutData->addAuthor("Fatcow Web Hosting", "Page break icon", "http://www.fatcow.com/free-icons");
    return aboutData;
}

#endif
