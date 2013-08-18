/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KPRABOUTDATA_H
#define KPRABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <calligraversion.h>

static const char description[] = I18N_NOOP("Calligra Presentation Tool");
static const char version[] = CALLIGRA_VERSION_STRING;

KAboutData * newKPresenterAboutData()
{
    KAboutData * aboutData=new KAboutData( "stage", 0, "Stage",
                                           version, description, KAboutData::License_LGPL,
                                           "(c) 1998-2011, The Stage Team", ""
                                           "http://www.calligra.org/stage/");
    aboutData->setProductName("calligrastage"); // for bugs.kde.org
    aboutData->setProgramIconName(QLatin1String("calligrastage"));
    aboutData->addAuthor("Thorsten Zachmann", "Maintainer", "zachmann@kde.org");
    aboutData->addAuthor("Laurent Montel", "Former maintainer", "montel@kde.org");
    aboutData->addAuthor("Reginald Stadlbauer""Original author", "reggie@kde.org");
    aboutData->addAuthor("Werner Trobin", "", "trobin@kde.org");
    aboutData->addAuthor("David Faure", "", "faure@kde.org");
    aboutData->addAuthor("Toshitaka Fujioka", "", "fujioka@kde.org");
    aboutData->addAuthor("Lukáš Tinkl", "", "lukas@kde.org");
    aboutData->addAuthor("Ariya Hidayat", "", "ariya@kde.org");
    aboutData->addAuthor("Percy Leonhardt", "", "percy@eris23.de");
    aboutData->addAuthor("Thomas Zander");
    aboutData->addAuthor("Peter Simonsson", "", "peter.simonsson@gmail.com");
    aboutData->addAuthor("Boudewijn Rempt ", "", "boud@valdyas.org");
    aboutData->addAuthor("Carlos Licea ", "", "carlos.licea@kdemail.net");
    aboutData->addAuthor("C. Boemann ", "", "cbo@boemann.dk");
    aboutData->addAuthor("Fredy Yanardi ", "", "fyanardi@gmail.com");
    aboutData->addAuthor("Jan Hambrecht ", "", "jaham@gmx.net");
    aboutData->addAuthor("Marijn Kruisselbrink ", "", "mkruisselbrink@kde.org");
    aboutData->addAuthor("Martin Pfeiffer ", "", "hubipete@gmx.net");
    aboutData->addAuthor("Sven Langkamp ", "", "sven.langkamp@gmail.com");
    aboutData->addAuthor("Timothée Lacroix ", "", "dakeyras.khan@gmail.com");

    return aboutData;
}

#endif
