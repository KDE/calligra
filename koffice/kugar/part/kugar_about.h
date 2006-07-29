/***************************************************************************
                         kugar_about.h  -  description
                            -------------------
   begin                : Mon Nov 25 17:45:00 CET 2002
   copyright            : (C) 2002 by Joseph Wenninger
   email                : jowenn@kde.org
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef KUGAR_ABOUT_H
#define KUGAR_ABOUT_H

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include <kaboutdata.h>
#include <klocale.h>

KAboutData *newKugarAboutData()
{
    KAboutData * aboutData = new KAboutData( "kugar", I18N_NOOP( "Kugar" ),
                             VERSION, I18N_NOOP( "Report viewer(generator)" ), KAboutData::License_GPL,
                             I18N_NOOP( "(c) 1999-2006, The Kugar Team" ), 0, "http://www.koffice.org" );

    aboutData->addAuthor( "Alexander Dymo",
                          I18N_NOOP( "Maintainer, Report Designer, Kugar library enhancements" ), "cloudtemple@mksat.net",
                          "http://www.cloudtemple.mksat.net" );
    aboutData->addAuthor( "Joseph Wenninger",
                          I18N_NOOP( "Smaller fixes and Kexi integration" ), "jowenn@kde.org",
                          "" );
    aboutData->addAuthor( "Joris Marcillac",
                          I18N_NOOP( "Direct database support" ), "joris@marcillac.org",
                          "" );
    aboutData->addAuthor( "Phil Thompson",
                          I18N_NOOP( "Former maintainer" ), "phil@river-bank.demon.co.uk",
                          "" );
    aboutData->addAuthor( "Keith Davis (Mutiny Bay Software)",
                          I18N_NOOP( "Original author of Metaphrast" ), "info@mutinybaysoftware.com",
                          "http://www.mutinybaysoftware.com" );
    aboutData->setTranslator( I18N_NOOP( "_:NAME OF TRANSLATORS\nNames" ), I18N_NOOP( "_:EMAIL OF TRANSLATORS\ne-mail" ) );
    return aboutData;
}
#endif
