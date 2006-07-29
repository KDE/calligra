/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#ifndef EXAMPLE_ABOUTDATA
#define EXAMPLE_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("Example KOffice Program");
static const char* version="0.1";

KAboutData * newExampleAboutData()
{
    // Change this, of course
    // The first argument of the KAboutData constructor is the instance name.
    // It is very important, it's what's used for many things, like the subdir in share/apps, etc.
    // It must also match the name of the application's .desktop file.
    KAboutData * aboutData=new KAboutData( "example", I18N_NOOP("Example"),
                                           version, description, KAboutData::License_GPL,
                                           "(c) 1998-2000, Torben Weis");
    aboutData->addAuthor("Torben Weis",0, "weis@kde.org");
    return aboutData;
}

#endif
