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

#include <kaboutdata.h>
#include <klocale.h>
#include <calligraversion.h>

namespace KPlatoWork
{

static const char* PLANWORK_DESCRIPTION=I18N_NOOP("PlanWork - Work Package handler for the Plan Project Planning Tool");
static const char* PLANWORK_VERSION=CALLIGRA_VERSION_STRING;

KAboutData * newAboutData()
{
    KAboutData * aboutData=new KAboutData( "planwork", 0, ki18nc("application name", "Plan WorkPackage Handler"),
                                           PLANWORK_VERSION, ki18n(PLANWORK_DESCRIPTION),
                                           KAboutData::License_GPL,
                                           ki18n("(c) 2007-2011, The Plan Team"),
                                           KLocalizedString(),
                                           "http://www.calligra.org/plan/" );

    aboutData->addAuthor(ki18n("Dag Andersen"), KLocalizedString(), "danders@get2net.dk");

    aboutData->setProductName( "calligra-plan/work" );
    aboutData->setProgramIconName( "calligraplanwork" );

    return aboutData;
}

}  //KPlatoWork namespace

#endif
