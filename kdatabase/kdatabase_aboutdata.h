/* This file is part of the KDE project
   Copyright (C) 2002 Chris Machemer <machey@ceinetworks.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef KDATABASE_ABOUTDATA
#define KDATABASE_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KDatabase");
static const char* version="0.1";

KAboutData * newKDatabaseAboutData()
{
    // Change this, of course
    KAboutData * aboutData=new KAboutData( "kdatabase", I18N_NOOP("KDatabase"),
                                           version, description, KAboutData::License_GPL,
                                           "(c) 2002, Chris Machemer");
    aboutData->addAuthor("Chris Machemer","Author", "machey@ceinetworks.com");
    return aboutData;
}

#endif
