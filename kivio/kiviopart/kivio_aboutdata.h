/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef KIVIO_ABOUTDATA
#define KIVIO_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KOffice Kivio Application");
static const char* version="0.1";

KAboutData * newKivioAboutData()
{
    KAboutData * aboutData=new KAboutData( "kivio", I18N_NOOP("Kivio"),
                                           version, description, KAboutData::License_GPL,
                                           "theKompany.com - Kivio",0,"www.thekompany.com");
    aboutData->addAuthor("Dave Marotti","Main author and the original author of Queesio, from which this source is based","lndshark@speakeasy.net");
    aboutData->addAuthor("Max Judin","GUI widgets","max@thekompany.com");
    return aboutData;
}

#endif
