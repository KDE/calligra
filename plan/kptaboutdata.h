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

#include <kaboutdata.h>
#include <klocale.h>

#include <calligraversion.h>

namespace KPlato
{

static const char PLAN_DESCRIPTION[] = I18N_NOOP("Project Planning and Management Tool");
static const char PLAN_VERSION[] = CALLIGRA_VERSION_STRING;

KAboutData * newAboutData()
{
    KAboutData * aboutData=new KAboutData( "plan", QByteArray(), ki18nc("application name", "Plan"),
                                           PLAN_VERSION, ki18n(PLAN_DESCRIPTION), KAboutData::License_GPL,
                                           ki18n("Copyright (C) 1998-2013, The Plan Team"),
                                           KLocalizedString(),
                                           "http://www.calligra.org/plan/" );
    aboutData->addAuthor(ki18n("Thomas Zander")); // please don't re-add, I don't like getting personal emails :)
    aboutData->addAuthor(ki18n("Bo Thorsen"), KLocalizedString(), "bo@sonofthor.dk");
    aboutData->addAuthor(ki18n("Dag Andersen"), KLocalizedString(), "danders@get2net.dk");
    aboutData->addAuthor(ki18n("Raphael Langerhorst"),KLocalizedString(),"raphael.langerhorst@kdemail.net");

    aboutData->setProgramIconName( "calligraplan" );

    aboutData->setProductName( "calligraplan" ); // for bugs.kde.org

    return aboutData;
}

}  //KPlato namespace

#endif
