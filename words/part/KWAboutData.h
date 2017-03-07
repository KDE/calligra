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

#include <KAboutData>
#include <klocalizedstring.h>
#include <kcoreaddons_version.h>

#include <calligraversion.h>

KAboutData * newWordsAboutData()
{
    KAboutData * aboutData = new KAboutData(
        QStringLiteral("calligrawords"),
        i18nc("application name", "Calligra Words"),
        QStringLiteral(CALLIGRA_VERSION_STRING),
        i18n("Word processor"),
        KAboutLicense::LGPL,
        i18n("Copyright 1998-%1, The Words Team", QStringLiteral(CALLIGRA_YEAR)),
        QString(),
        QStringLiteral("https://www.calligra.org/words/"));
    aboutData->setProductName("calligrawords"); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
#if KCOREADDONS_VERSION >= 0x051600
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligrawords"));
#endif
    aboutData->addAuthor(i18n("Pierre Ducroquet"), i18n("Co maintainer"), "");
    aboutData->addAuthor(i18n("C. Boemann"), i18n("Co maintainer"), "cbo@boemann.dk");
    aboutData->addAuthor(i18n("Sebastian Sauer"), i18n("Everything"), "mail@dipe.org");
    aboutData->addAuthor(i18n("Boudewijn Rempt"), i18n("Everything"), "boud@kde.org");
    aboutData->addAuthor(i18n("Pierre Stirnweiss"), i18n("Everything"), "");
    aboutData->addAuthor(i18n("Inge Wallin"), i18n("Formatting stuff"), "inge@lysator.liu.se");
    aboutData->addAuthor(i18n("Thorsten Zachmann"), i18n("Everything"), "zachmann@kde.org");
    aboutData->addAuthor(i18n("Matus Uzak"), i18n("Filter"), "matus.uzak@ixonos.com");
    aboutData->addAuthor(i18n("Pavol Korinek"), i18n("Layout and Painting"), "pavol.korinek@ixonos.com");
    aboutData->addAuthor(i18n("Shreya Pandit"), i18n("Statistics docker"), "");
    aboutData->addAuthor(i18n("Brijesh Patel"), i18n("Foot and endnotes"), "");
    aboutData->addAuthor(i18n("Smit Patel"), i18n("Bibliography"), "");
    aboutData->addAuthor(i18n("Mojtaba Shahi"), i18n("Style Manager"), "");
    aboutData->addAuthor(i18n("Lassi Nieminen"), i18n("Filter"), "lassniem@gmail.com");
    aboutData->addAuthor(i18n("Hanzes Matus"), i18n("Filter"), "matus.hanzes@ixonos.com");
    aboutData->addAuthor(i18n("Lukáš Tvrdý"), i18n("Filter"), "lukast.dev@gmail.com");
    aboutData->addAuthor(i18n("Thomas Zander"), QString(), "zander@kde.org");
    aboutData->addAuthor(i18n("Girish Ramakrishnan"), i18n("ODF Support"), "girish@forwardbias.in");
    aboutData->addAuthor(i18n("Robert Mathias Marmorstein"), i18n("ODF Support"), "robert@narnia.homeunix.com");
    aboutData->addAuthor(i18n("David Faure"), QString(), "faure@kde.org");
    aboutData->addAuthor(i18n("Laurent Montel"), QString(), "montel@kde.org");
    aboutData->addAuthor(i18n("Sven Lüppken"), QString(), "sven@kde.org");
    aboutData->addAuthor(i18n("Frank Dekervel"), QString(), "Frank.dekervel@student.kuleuven.ac.Be");
    aboutData->addAuthor(i18n("Krister Wicksell Eriksson"), QString(), "krister.wicksell@spray.se");
    aboutData->addAuthor(i18n("Dag Andersen"), QString(), "danders@get2net.dk");
    aboutData->addAuthor(i18n("Nash Hoogwater"), QString(), "nrhoogwater@wanadoo.nl");
    aboutData->addAuthor(i18n("Ulrich Kuettler"), i18n("KFormula"), "ulrich.kuettler@mailbox.tu-dresden.de");
    aboutData->addAuthor(i18n("Shaheed Haque"), i18n("Filter"), "srhaque@iee.org");
    aboutData->addAuthor(i18n("Werner Trobin"), i18n("Filter"), "trobin@kde.org");
    aboutData->addAuthor(i18n("Nicolas Goutte"), i18n("Filter"), "goutte@kde.org");
    aboutData->addAuthor(i18n("Ariya Hidayat"), i18n("Filter"), "ariya@kde.org");
    aboutData->addAuthor(i18n("Clarence Dang"), i18n("Filter"), "dang@kde.org");
    aboutData->addAuthor(i18n("Robert Jacolin"), i18n("Filter"), "rjacolin@ifrance.com");
    aboutData->addAuthor(i18n("Enno Bartels"), i18n("Filter"), "ebartels@nwn.de");
    aboutData->addAuthor(i18n("Ewald Snel"), i18n("Filter"), "ewald@rambo.its.tudelft.nl");
    aboutData->addAuthor(i18n("Tomasz Grobelny"), i18n("Filter"), "grotk@poczta.onet.pl");
    aboutData->addAuthor(i18n("Michael Johnson"), i18n("Filter"));
    aboutData->addAuthor(i18n("Fatcow Web Hosting"), i18n("Page break icon"), "https://www.fatcow.com/free-icons");
    // standard ki18n translator strings
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                             i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    return aboutData;
}

#endif
