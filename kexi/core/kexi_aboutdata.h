/***************************************************************************
                          kexi_aboutdata.h  -  description
                             -------------------
    begin                : Sun Jun  9 12:15:11 CEST 2002
    copyright            : (C) 2002 by lucijan busch, Joseph Wenninger
    email                : lucijan@gmx.at, jowenn@kde.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _KEXI_ABOU_DATA_
#define _KEXI_ABOU_DATA_

#include <kaboutdata.h>
#include <klocale.h>
#include <config.h>

static const char *description =
	I18N_NOOP("Database Management");

static const char *version=VERSION;

KAboutData *newKexiAboutData()
{
	KAboutData *aboutData=new KAboutData( "kexi", I18N_NOOP("Kexi"),
		version, description, KAboutData::License_GPL,
		I18N_NOOP("(c) 2002, Kexi Team"), 0, "http://luci.bux.at/projects/kexi", "lucijan@gmx.at");
	aboutData->addAuthor("Lucijan Busch",I18N_NOOP("Developer & Maintainer"), "lucijan@gmx.at");
	aboutData->addAuthor("Peter Simonsson",I18N_NOOP("Developer"),"psn@linux.se");
	aboutData->addAuthor("Joseph Wenninger", I18N_NOOP("Form Designer, UIModes & much more"), "jowenn@kde.org");
	aboutData->addAuthor("Seth Kurzenberg",I18N_NOOP("CQL++, SQL assistance"),  "seth@cql.com");
	aboutData->addAuthor("Daniel Molkentin",I18N_NOOP("Design, Improvements"),  "molkentin@kde.org");
	aboutData->addAuthor("Laurent Montel", I18N_NOOP("Code cleanings"), "montell@club-internet.fr");
	aboutData->addAuthor("Till Busch", I18N_NOOP("Buxfixes, Original Table Widget"), "till@bux.at");
	aboutData->addCredit("Kristof Borrey", I18N_NOOP("Icons and UI-Research"), "kristof.borrey@skynet.be");
	aboutData->setTranslator(I18N_NOOP("_:NAME OF TRANSLATORS\nNames"), I18N_NOOP("_:EMAIL OF TRANSLATORS\ne-mail"));

	return aboutData;
}

#endif
