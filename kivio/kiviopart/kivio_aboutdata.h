/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2003 theKompany.com & Dave Marotti,
 *                         Peter Simonsson
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef KIVIO_ABOUTDATA
#define KIVIO_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* description = I18N_NOOP("KOffice Flowchart And Diagram Tool");
static const char* version = KOFFICE_VERSION_STRING;

KAboutData * newKivioAboutData()
{
    KAboutData * aboutData=new KAboutData( "kivio", 0, ki18n("Kivio"),
                                           version, ki18n(description), KAboutData::License_GPL,
                                           ki18n("(C) 2000-2007, The Kivio Team"), KLocalizedString(),
                                           "http://www.koffice.org/kivio/");
    aboutData->addAuthor(ki18n("Peter Simonsson"), ki18n("Maintainer"), "psn@linux.se");
    aboutData->addAuthor(ki18n("Ian Reinhart Geiser"), ki18n("Development"), "geiseri@kde.org");
    aboutData->addAuthor(ki18n("Laurent Montel"), ki18n("Development"), "montel@kde.org");
    aboutData->addAuthor(ki18n("Frauke Oster"), ki18n("Development/Documentation"), "frauke@frsv.de");
    aboutData->addAuthor(ki18n("Kristof Borrey"), ki18n("Artwork (stencils and icons)"), "borrey@kde.org");
    aboutData->addAuthor(ki18n("Joerg de la Haye"), ki18n("Nassi Schneiderman stencils"), "haye@ritterstrasse.org");
    aboutData->addAuthor(ki18n("Dave Marotti"), ki18n("Initial idea, development and basic stencil sets."),"lndshark@verticaladdiction.net");
    aboutData->addAuthor(ki18n("The Kompany.com - \"Where Open Source is at Home\"(TM)"), ki18n("Development and additional stencil sets."), "info@thekompany.com");
    return aboutData;
}

#endif
