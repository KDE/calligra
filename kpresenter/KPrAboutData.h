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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRESENTER_ABOUTDATA
#define KPRESENTER_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* description=I18N_NOOP("KOffice Presentation Tool");
static const char* version=KOFFICE_VERSION_STRING;

KAboutData * newKPresenterAboutData()
{
    KAboutData * aboutData=new KAboutData( "kpresenter", I18N_NOOP("KPresenter"),
                                           version, description, KAboutData::License_LGPL,
                                           I18N_NOOP("(c) 1998-2006, The KPresenter Team"), 0,
                                           "http://www.koffice.org/kpresenter/");
    aboutData->addAuthor("Laurent Montel", I18N_NOOP("current maintainer"), "montel@kde.org");
    aboutData->addAuthor("Werner Trobin", 0, "trobin@kde.org");
    aboutData->addAuthor("David Faure", 0, "faure@kde.org");
    aboutData->addAuthor("Toshitaka Fujioka", 0, "fujioka@kde.org");
    aboutData->addAuthor("Lukáš Tinkl", 0, "lukas@kde.org");
    aboutData->addAuthor("Thorsten Zachmann", 0, "t.zachmann@zagge.de");
    aboutData->addAuthor("Ariya Hidayat", 0, "ariya@kde.org");
    aboutData->addAuthor("Percy Leonhardt", 0, "percy@eris23.de");
    aboutData->addAuthor("Thomas Zander", 0, "");
    aboutData->addAuthor("Reginald Stadlbauer",I18N_NOOP("original author"), "reggie@kde.org");
    return aboutData;
}

#endif
