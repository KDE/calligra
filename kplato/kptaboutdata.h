/* This file is part of the KDE project
   Copyright (C) 1998 - 2001 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2004 Dag Andersen <danders@get2net.dk>

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

#ifndef KPLATO_ABOUTDATA
#define KPLATO_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

namespace KPlato
{

static const char* KPLATO_DESCRIPTION=I18N_NOOP("KPlato - KDE Planning Tool");
static const char* KPLATO_VERSION="0.5.92";

KAboutData * newAboutData()
{
    KAboutData * aboutData=new KAboutData( "kplato", I18N_NOOP("KPlato"),
                                           KPLATO_VERSION, KPLATO_DESCRIPTION, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2006, The KPlato Team"), 0,
					   "http://www.koffice.org/kplato/" );
    aboutData->addAuthor("Thomas Zander", 0, 0 ); // please don't re-add, I don't like getting personal emails :)
    aboutData->addAuthor("Bo Thorsen", 0, "bo@sonofthor.dk");
    aboutData->addAuthor("Dag Andersen", 0, "danders@get2net.dk");
    aboutData->addAuthor("Raphael Langerhorst",0,"raphael.langerhorst@kdemail.net");
    aboutData->addCredit("Nuno Pinheiro and Danny Allen", I18N_NOOP("Application icon for kplato"), "danny@dannyallen.co.uk");
    return aboutData;
}

}  //KPlato namespace

#endif
