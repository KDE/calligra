/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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

#ifndef KPRESENTER_ABOUTDATA
#define KPRESENTER_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KOffice Presentation Tool");
static const char* version="1.1";

KAboutData * newKPresenterAboutData()
{
    KAboutData * aboutData=new KAboutData( "kpresenter", I18N_NOOP("KPresenter"),
                                           version, description, KAboutData::License_GPL,
                                           I18N_NOOP("(c) 1998-2001, The KPresenter Team"), 0,
                                           "http://www.koffice.org/kpresenter/");
    aboutData->addAuthor("Reginald Stadlbauer",I18N_NOOP("original author"), "reggie@kde.org");
    aboutData->addAuthor("Werner Trobin", I18N_NOOP("current maintainer"), "trobin@kde.org");
    aboutData->addAuthor("David Faure", 0, "dfaure@kde.org");
    aboutData->addAuthor("Toshitaka Fujioka", 0, "fujioka@kde.org");
    aboutData->addAuthor("Lukáš Tinkl", 0, "lukas@kde.org");
    return aboutData;
}

#endif
