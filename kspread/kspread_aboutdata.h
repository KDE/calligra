/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KOffice Spreadsheet Application");
static const char* version="1.1";

KAboutData * newKSpreadAboutData()
{
    KAboutData * aboutData = new KAboutData( "kspread", I18N_NOOP("KSpread"),
                                             version, description, KAboutData::License_GPL,
                                             "(c) 1998-2000, Torben Weis");
    aboutData->addAuthor("Torben Weis",0, "weis@kde.org");
    aboutData->addAuthor("Laurent Montel",0, "lmontel@mandrakesoft.com");
    aboutData->addAuthor("David Faure",0, "faure@kde.org");
    return aboutData;
}
