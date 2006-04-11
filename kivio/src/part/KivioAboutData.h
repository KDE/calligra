/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

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
   Boston, MA 02110-1301, USA.
*/

#ifndef KIVIOABOUTDATA_H
#define KIVIOABOUTDATA_H

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* KIVIO_DESCRIPTION = I18N_NOOP("KOffice Flowchart And Diagram Tool");
static const char* KIVIO_VERSION = KOFFICE_VERSION_STRING;

KAboutData* newKivioAboutData()
{
  KAboutData* aboutData = new KAboutData("kivio", I18N_NOOP("Kivio"),
                                KIVIO_VERSION, KIVIO_DESCRIPTION, KAboutData::License_LGPL,
                                I18N_NOOP("(C) 2000-2006, The Kivio Team"), 0,
                                "http://www.koffice.org/kivio/");
  aboutData->addAuthor("Peter Simonsson", I18N_NOOP("Maintainer"), "peter.simonsson@gmail.com");

  return aboutData;
}

#endif
