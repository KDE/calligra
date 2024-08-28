/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRABOUTDATA_H
#define KPRABOUTDATA_H

#include <KAboutData>
#include <KLocalizedString>
#include <kcoreaddons_version.h>

#include <calligra-version.h>

KAboutData *newKPresenterAboutData()
{
    KAboutData *aboutData = new KAboutData(QStringLiteral("calligrastage"),
                                           i18nc("application name", "Calligra Stage"),
                                           QLatin1String(CALLIGRA_VERSION_STRING),
                                           i18n("Presentation Tool"),
                                           KAboutLicense::LGPL,
                                           i18n("Copyright 1998-%1, The Stage Team", QString::number(CALLIGRA_YEAR)),
                                           QString(),
                                           QStringLiteral("https://www.calligra.org/stage/"));
    aboutData->setProductName("calligrastage"); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligra.stage"));
    aboutData->addAuthor(i18n("Carl Schwan"),
                         i18n("Maintainer/Port to Qt6"),
                         QStringLiteral("carl@carlschwan.eu"),
                         QStringLiteral("https://carlschwan.eu"),
                         QUrl(QStringLiteral("https://carlschwan.eu/avatar.png")));
    aboutData->addAuthor(i18n("Thorsten Zachmann"), i18n("Former maintainer"), "zachmann@kde.org");
    aboutData->addAuthor(i18n("Laurent Montel"), i18n("Former maintainer"), "montel@kde.org");
    aboutData->addAuthor(i18n("Reginald Stadlbauer"), i18n("Original author"), "reggie@kde.org");
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
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    return aboutData;
}

#endif
