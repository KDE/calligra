/* This file is part of the KDE project
   Copyright (C) 1998 - 2001 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2007 - 2011 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATOWORK_ABOUTDATA
#define KPLATOWORK_ABOUTDATA

#include <KAboutData>
#include <KLocalizedString>
#include <kcoreaddons_version.h>

#include <config.h>

namespace KPlatoWork
{

static const char PLANWORK_DESCRIPTION[] = I18N_NOOP("PlanWork - Work Package handler for the Plan Project Planning Tool");
static const char PLANWORK_VERSION[] = PLAN_VERSION_STRING;

KAboutData * newAboutData()
{
    KAboutData * aboutData = new KAboutData(
        QStringLiteral("calligraplanwork"),
        i18nc("application name", "Plan WorkPackage Handler"),
        QStringLiteral(PLAN_VERSION_STRING),
        i18n("PlanWork - Work Package handler for the Plan Project Planning Tool"),
        KAboutLicense::GPL,
        i18n("Copyright 1998-%1, The Plan Team", QStringLiteral(PLAN_YEAR)),
        QString(),
        QStringLiteral("https://www.calligra.org/plan/"));

    aboutData->addAuthor(i18n("Dag Andersen"), QString(), QStringLiteral("danders@get2net.dk"));
    // standard ki18n translator strings
    aboutData->setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"),
                             i18nc("EMAIL OF TRANSLATORS", "Your emails"));

    aboutData->setProductName( "calligraplan/work" );
#if KCOREADDONS_VERSION >= 0x051600
    aboutData->setDesktopFileName(QStringLiteral("org.kde.calligraplanworks"));
#endif

    return aboutData;
}

}  //KPlatoWork namespace

#endif
