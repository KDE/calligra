/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef _KEXI_ABOU_DATA_
#define _KEXI_ABOU_DATA_

#include "kexi_global.h"
#include <kaboutdata.h>
#include <config.h>

static const char *description =
	I18N_NOOP("Easy database creation for everyone");

KAboutData *newKexiAboutData()
{
	KAboutData *aboutData=new KAboutData( "kexi", KEXI_APP_NAME,
		KEXI_VERSION, description, KAboutData::License_LGPL,
		I18N_NOOP(	"(c) 2002, 2003, Kexi Team\n"
					"(c) 2003, OpenOffice Polska Ltd.\n"), 
		I18N_NOOP(	"This software is developed by Kexi Team - international group\n"
					"of independent developers, with additional assistance and support\n"
					"from OpenOffice Polska company.\n\n"
					"Visit Company Home Page: http://www.openoffice.com.pl"),
		"http://www.koffice.org/kexi",
		"submit@bugs.kde.org"
	);
	aboutData->addAuthor("Lucijan Busch",I18N_NOOP("Developer & Maintainer"), "lucijan@kde.org");
	aboutData->addAuthor("Peter Simonsson",I18N_NOOP("Developer"),"psn@linux.se");
	aboutData->addAuthor("Joseph Wenninger", I18N_NOOP("Form Designer, UIModes & much more"), "jowenn@kde.org");
	aboutData->addAuthor("Seth Kurzenberg",I18N_NOOP("CQL++, SQL assistance"),  "seth@cql.com");
	aboutData->addAuthor("Laurent Montel", I18N_NOOP("Code cleanings"), "montell@club-internet.fr");
	aboutData->addAuthor("Till Busch", I18N_NOOP("Buxfixes, Original Table Widget"), "till@bux.at");
	aboutData->addAuthor("Jaroslaw Staniek / OpenOffice Polska", I18N_NOOP("Developer, Designer, commercially supported version, win32 port"), "js@iidea.pl");
	aboutData->addCredit("Daniel Molkentin",I18N_NOOP("Initial design improvements"),  "molkentin@kde.org");
	aboutData->addCredit("Kristof Borrey", I18N_NOOP("Icons and UI-Research"), "kristof.borrey@skynet.be");
	aboutData->setTranslator(I18N_NOOP("_:NAME OF TRANSLATORS\nNames"), I18N_NOOP("_:EMAIL OF TRANSLATORS\ne-mail"));
	aboutData->addCredit("Tomas Krassnig", I18N_NOOP("Coffie sponsoring"), "tkrass05@hak1.at");
#ifdef OOPL_VERSION
	aboutData->setProgramLogo(KEXI_APP_LOGO);
#endif
	return aboutData;
}

#endif
