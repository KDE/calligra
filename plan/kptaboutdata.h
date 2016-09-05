/* This file is part of the KDE project
   Copyright (C) 1998 - 2001 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2004 - 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPTABOUTDATA_H
#define KPTABOUTDATA_H

#include <KAboutData>
#include <KLocalizedString>
#include <kcoreaddons_version.h>

#include <calligraversion.h>

namespace KPlato
{

KAboutData * newAboutData()
{
    KAboutData *aboutData = new KAboutData(
        QStringLiteral("calligraplan"),
        i18nc("application name", "Plan"),
        QStringLiteral(CALLIGRA_VERSION_STRING),
        i18n("Project Planning and Management Tool"),
        KAboutLicense::GPL,
        i18n("Copyright 1998-%1, The Plan Team", QStringLiteral(CALLIGRA_YEAR)),
        QString(),
        QStringLiteral("https://www.calligra.org/plan/"));
    aboutData->addAuthor(i18n("Thomas Zander")); // please don't re-add, I don't like getting personal emails :)
    aboutData->addAuthor(i18n("Bo Thorsen"), QString(), "bo@sonofthor.dk");
    aboutData->addAuthor(i18n("Dag Andersen"), QString(), "danders@get2net.dk");
    aboutData->addAuthor(i18n("Raphael Langerhorst"),QString(),"raphael.langerhorst@kdemail.net");

    aboutData->setProductName( "calligraplan" ); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
#if KCOREADDONS_VERSION >= 0x051600
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligraplan"));
#endif

    return aboutData;
}

}  //KPlato namespace

#endif
