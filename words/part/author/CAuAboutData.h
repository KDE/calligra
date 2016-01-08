/* This file is part of the KDE project
   Copyright (C) 1998 - 2001 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2012        Inge Wallin <inge@lysator.liu.se>

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

#ifndef CAUABOUTDATA_H
#define CAUABOUTDATA_H

#include <KAboutData>
#include <klocalizedstring.h>

#include <calligraversion.h>

KAboutData * newAuthorAboutData()
{
    KAboutData *aboutData = new KAboutData(
        QStringLiteral("author"),
        i18nc("application name", "Calligra Author"),
        QStringLiteral(CALLIGRA_VERSION_STRING),
        i18n("Author tool"),
        KAboutLicense::LGPL,
        i18n("Copyright 2012-%1, The Author Team", QStringLiteral(CALLIGRA_YEAR)),
        QString(),
        QStringLiteral("https://www.calligra.org/author/"));
    aboutData->setProductName("calligraauthor"); // for bugs.kde.org
    aboutData->setOrganizationDomain("calligra.org");
    //                          Name             Function               email (if any)
    aboutData->addAuthor(i18n("Inge Wallin"), i18n("Co-maintainer"), "");
    aboutData->addAuthor(i18n("Gopalakrishna Bhat"), i18n("Co-maintainer"), "");
    aboutData->addAuthor(i18n("Mojtaba Shahi Senobari"), i18n("EPUB export"), "mojtaba.shahi3000@gmail.com");
    return aboutData;
}

#endif
