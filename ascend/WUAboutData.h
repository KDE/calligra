/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu>

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

#ifndef WRITEUP_ABOUTDATA
#define WRITEUP_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* WRITEUP_DESCRIPTION=I18N_NOOP("KOffice Simplified Word Processor");
static const char* WRITEUP_VERSION=KOFFICE_VERSION_STRING;

KAboutData * newWriteupAboutData()
{
    KAboutData * aboutData=new KAboutData( "writeup", I18N_NOOP("WriteUp"),
                                           WRITEUP_VERSION, WRITEUP_DESCRIPTION, KAboutData::License_LGPL,
                                           I18N_NOOP("(c) 2005-2006, The Ascend and KOffice Teams"), 0,
                                           "http://www.koffice.org/kword/" );
    aboutData->addAuthor("Inge Wallin", 0, "inge@lysator.liu");
    aboutData->addAuthor("David Faure", 0, "faure@kde.org");
    aboutData->addAuthor("Danny Allen");
    return aboutData;
}

#endif
