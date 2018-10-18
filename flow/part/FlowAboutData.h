/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef FLOWABOUTDATA_H
#define FLOWABOUTDATA_H

#include <kaboutdata.h>
#include <KLocalizedString>
#include <calligraversion.h>
#include <kcoreaddons_version.h>

KAboutData* newFlowAboutData()
{
    KAboutData *aboutData = new KAboutData(
        QStringLiteral("flow"),
        i18n("Flow"),
        QStringLiteral(CALLIGRA_VERSION_STRING),
        i18n("Calligra Flowchart And Diagram Tool"),
        KAboutLicense::LGPL,
        i18n("(c) 2001-%1, The Flow Team", QStringLiteral(CALLIGRA_YEAR)),
        QStringLiteral("https://www.calligra.org/flow/"));
    aboutData->setProductName("flow"); // for bugs.kde.org
    aboutData->setOrganizationDomain("kde.org");
#if KCOREADDONS_VERSION >= 0x051600
    aboutData->setDesktopFileName(QStringLiteral("org.kde.flow"));
#endif
    aboutData->addAuthor(i18n("Yue Liu"), i18n("Maintainer"), QStringLiteral("yue.liu@mail.com"));
    aboutData->addAuthor(i18n("Peter Simonsson"), i18n("Former Maintainer"), QStringLiteral("peter.simonsson@gmail.com"));
    aboutData->addAuthor(i18n("Laurent Montel"), i18n("KF5 Porting"), QStringLiteral("montel@kde.org"));
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                             i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    return aboutData;
}

#endif
