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

#ifndef KWORD_ABOUTDATA
#define KWORD_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* KWORD_DESCRIPTION=I18N_NOOP("KOffice Word Processor");
// First official public release Oct 2000 version 0.8
// KOffice-1.1-beta1: version 0.9
// KOffice-1.1-beta2: version 1.1 (to remove confusion)
static const char* KWORD_VERSION="1.1 (RC1)";

KAboutData * newKWordAboutData()
{
    KAboutData * aboutData=new KAboutData( "kword", I18N_NOOP("KWord"),
                                           KWORD_VERSION, KWORD_DESCRIPTION, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2001, The KWord Team"), 0, 
					   "http://www.koffice.org/kword/" );
    aboutData->addAuthor("Reginald Stadlbauer", 0, "reggie@kde.org");
    aboutData->addAuthor("Thomas Zander", 0, "zander@kde.org");
    aboutData->addAuthor("David Faure", 0, "david@mandrakesoft.com");
    aboutData->addAuthor("Laurent Montel", 0, "lmontel@mandrakesoft.com");
    aboutData->addAuthor("Sven Lüppken", 0, "sven@kde.org");
    return aboutData;
}

#endif
