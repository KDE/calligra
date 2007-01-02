/* This file is part of the KDE project

   Copyright (C) 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>
   Copyright (C) 2005-2006 Sebastian Sauer <mail@dipe.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef KEXIABOUTDATA_H
#define KEXIABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* KEXIDESCRIPTION = I18N_NOOP("KOffice Database Application");
static const char* KEXIVERSION = KOFFICE_VERSION_STRING;

KAboutData* newAboutData()
{
    KAboutData * aboutData = new KAboutData( "kexi", I18N_NOOP("Kexi"),
                                             KEXIVERSION, KEXIDESCRIPTION, KAboutData::License_LGPL,
                                             I18N_NOOP("(c) 2002-2007, The Kexi Team"), 0,
               "http://www.koffice.org/kexi/");
    //aboutData->addAuthor("Name", 0, "my@mail.address");
    //aboutData->addCredit("Other Name", "bla bla");
    return aboutData;
}

#endif
