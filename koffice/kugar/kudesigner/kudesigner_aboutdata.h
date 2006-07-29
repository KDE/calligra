/* This file is part of the KDE project
  Copyright (C) 2002 Alexander Dymo <cloudtemple@mksat.net>

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

#ifndef KUDESIGNER_ABOUTDATA
#define KUDESIGNER_ABOUTDATA

#include <kaboutdata.h>
#include <klocale.h>
#include <kofficeversion.h>

static const char* description = I18N_NOOP( "GUI report template designer for Kugar report engine" );
static const char* version = KOFFICE_VERSION_STRING;

KAboutData * newKudesignerAboutData()
{
    // Change this, of course
    // The first argument of the KAboutData constructor is the instance name.
    // It is very important, it's what's used for many things, like the subdir in share/apps, etc.
    // It must also match the name of the application's .desktop file.
    KAboutData * aboutData = new KAboutData( "kudesigner", I18N_NOOP( "Kugar Report Designer" ),
                             version, description, KAboutData::License_LGPL,
                             "(c) 2002,2003, Alexander Dymo (and the Kugar Team)" );
    aboutData->addAuthor( "Alexander Dymo",
                          I18N_NOOP( "Maintainer, Report Designer, Kugar library enhancements" ),
                          "cloudtemple@mksat.net",
                          "http://www.cloudtemple.mksat.net" );
    aboutData->addAuthor( "Joseph Wenninger",
                          I18N_NOOP( "Major bugfixes, additional features and Kexi integration" ),
                          "jowenn@kde.org",
                          "" );
    aboutData->addCredit( "Per Winkvist",
                          I18N_NOOP( "Fast property editing for report items" ),
                          "per.winkvist@cellnetwork.com",
                          "" );
    aboutData->setTranslator( I18N_NOOP( "_:NAME OF TRANSLATORS\nNames" ),
                              I18N_NOOP( "_:EMAIL OF TRANSLATORS\ne-mail" ) );
    return aboutData;
}

#endif
