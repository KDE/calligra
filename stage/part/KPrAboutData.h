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

#include <KAboutData>
#include <klocalizedstring.h>
#include <kcoreaddons_version.h>

#include <calligraversion.h>


KAboutData * newKPresenterAboutData()
{
    KAboutData *aboutData = new KAboutData(
        QStringLiteral("calligrastage"),
        i18nc("application name", "Calligra Stage"),
        QStringLiteral(CALLIGRA_VERSION_STRING),
        i18n("Presentation Tool"),
        KAboutLicense::LGPL,
        i18n("Copyright 1998-%1, The Stage Team", QStringLiteral(CALLIGRA_YEAR)),
        QString(),
        QStringLiteral("https://www.calligra.org/stage/"));
    aboutData->setProductName("calligrastage"); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
#if KCOREADDONS_VERSION >= 0x051600
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligrastage"));
#endif
    aboutData->addAuthor(i18n("Thorsten Zachmann"), i18n("Former maintainer"), "zachmann@kde.org");
    aboutData->addAuthor(i18n("Laurent Montel"), i18n("Former maintainer"), "montel@kde.org");
    aboutData->addAuthor(i18n("Reginald Stadlbauer"),i18n("Original author"), "reggie@kde.org");
    aboutData->addAuthor(i18n("Werner Trobin"), QString(), "trobin@kde.org");
    aboutData->addAuthor(i18n("David Faure"), QString(), "faure@kde.org");
    aboutData->addAuthor(i18n("Toshitaka Fujioka"), QString(), "fujioka@kde.org");
    aboutData->addAuthor(i18n("Lukáš Tinkl"), QString(), "lukas@kde.org");
    aboutData->addAuthor(i18n("Ariya Hidayat"), QString(), "ariya@kde.org");
    aboutData->addAuthor(i18n("Percy Leonhardt"), QString(), "percy@eris23.de");
    aboutData->addAuthor(i18n("Thomas Zander"));
    aboutData->addAuthor(i18n("Peter Simonsson"), QString(), "peter.simonsson@gmail.com");
    aboutData->addAuthor(i18n("Boudewijn Rempt "), QString(), "boud@valdyas.org");
    aboutData->addAuthor(i18n("Carlos Licea "), QString(), "carlos.licea@kdemail.net");
    aboutData->addAuthor(i18n("C. Boemann "), QString(), "cbo@boemann.dk");
    aboutData->addAuthor(i18n("Fredy Yanardi "), QString(), "fyanardi@gmail.com");
    aboutData->addAuthor(i18n("Jan Hambrecht "), QString(), "jaham@gmx.net");
    aboutData->addAuthor(i18n("Marijn Kruisselbrink "), QString(), "mkruisselbrink@kde.org");
    aboutData->addAuthor(i18n("Martin Pfeiffer "), QString(), "hubipete@gmx.net");
    aboutData->addAuthor(i18n("Sven Langkamp "), QString(), "sven.langkamp@gmail.com");
    aboutData->addAuthor(i18n("Timothée Lacroix "), QString(), "dakeyras.khan@gmail.com");
    // standard ki18n translator strings
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                             i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    return aboutData;
}

#endif
