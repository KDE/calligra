/* This file is part of the KDE project
   Copyright (C) 1998 - 2001 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KPLATO_ABOUTDATA
#define KPLATO_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* KPLATO_DESCRIPTION=I18N_NOOP("KPlato - KDE Planning Tool");
static const char* KPLATO_VERSION="0.0.-1";

KAboutData * newKPTAboutData()
{
    KAboutData * aboutData=new KAboutData( "kplato", I18N_NOOP("KPlato"),
                                           KPLATO_VERSION, KPLATO_DESCRIPTION, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2002, The KPlato Team"), 0,
					   "http://www.koffice.org/kplato/" );
    aboutData->addAuthor("Thomas Zander", 0, "zander@kde.org");
    aboutData->addAuthor("Bo Thorsen", 0, "bo@sonofthor.dk");
    return aboutData;
}

#endif
