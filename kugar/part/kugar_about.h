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

#include <kofficeversion.h>
#include <kaboutdata.h>
#include <klocale.h>

KAboutData *newKugarAboutData()
{
    KAboutData * aboutData = new KAboutData( "kugar", 0, ki18n( "Kugar" ),
                             KOFFICE_VERSION_STRING, ki18n( "Report viewer(generator)" ), KAboutData::License_GPL,
                             ki18n( "(c) 1999-2007, The Kugar Team" ), KLocalizedString(), "http://www.koffice.org" );

    aboutData->addAuthor( ki18n("Alexander Dymo"),
                          ki18n( "Maintainer, Report Designer, Kugar library enhancements" ), "cloudtemple@mksat.net",
                          "http://www.cloudtemple.mksat.net" );
    aboutData->addAuthor( ki18n("Joseph Wenninger"),
                          ki18n( "Smaller fixes and Kexi integration" ), "jowenn@kde.org");
    aboutData->addAuthor( ki18n("Joris Marcillac"),
                          ki18n( "Direct database support" ), "joris@marcillac.org");
    aboutData->addAuthor( ki18n("Phil Thompson"),
                          ki18n( "Former maintainer" ), "phil@river-bank.demon.co.uk");
    aboutData->addAuthor( ki18n("Keith Davis (Mutiny Bay Software)"),
                          ki18n( "Original author of Metaphrast" ), "info@mutinybaysoftware.com",
                          "http://www.mutinybaysoftware.com" );
    aboutData->setTranslator( ki18nc( "NAME OF TRANSLATORS", "Your names" ), ki18nc( "EMAIL OF TRANSLATORS", "Your emails" ) );
    return aboutData;
}
#endif
