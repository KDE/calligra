/* This file is part of the KDE project
   Copyright (C) 2001 Ian Reinhart Geiser <geiseri@yahoo.com>
   This is based off of the KOffice Example found in the KOffice
   CVS.  Torben Weis <weis@kde.org> is the original author.

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

#ifndef KOSoap_ABOUTDATA
#define KOSoap_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KOSoap KOffice Part");
static const char* version="0.1";

KAboutData * newKOSoapAboutData()
{
    KAboutData * aboutData=new KAboutData( "kosoap", I18N_NOOP("KOSoap"),
                                           version, description, KAboutData::License_GPL,
                                           "(c) 2001, Ian Geiser");
    aboutData->addAuthor("Ian Reinhart Gieser",0, "geiseri@yahoo.com");
    return aboutData;
}

#endif
