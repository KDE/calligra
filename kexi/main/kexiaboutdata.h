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

#include <kexi_version.h>
#include <kaboutdata.h>
#include <config.h>
#include <kdeversion.h>

static const char *description =
	I18N_NOOP("Database creation for everyone");

KAboutData *newKexiAboutData()
{
	KAboutData *aboutData=new KAboutData( "kexi", KEXI_APP_NAME,
		KEXI_VERSION_STRING, description, KAboutData::License_LGPL_V2,
		I18N_NOOP(	"(c) 2002-2004, Kexi Team\n"
					"(c) 2003, 2004, OpenOffice Polska Ltd.\n"),
		I18N_NOOP(	"This software is developed by Kexi Team - an international group\n"
					"of independent developers, with additional assistance and support\n"
					"from the OpenOffice Polska company.\n\n"
					"Visit the company Home Page: http://www.openoffice.com.pl"),
		"http://www.koffice.org/kexi",
		"submit@bugs.kde.org"
	);
	aboutData->addAuthor("Lucijan Busch",I18N_NOOP("Developer & Maintainer"), "lucijan@kde.org");
	aboutData->addAuthor("Cedric Pasteur", I18N_NOOP("KexiPropertyEditor and FormDesigner"), "cedric.pasteur@free.fr");
	aboutData->addAuthor("Jarosław Staniek / OpenOffice Polska", I18N_NOOP("Developer, Designer, KexiDB, commercially supported version, win32 port"), "js@iidea.pl");
	aboutData->addAuthor("Adam Pigg", I18N_NOOP("PostgreSQL database driver"), "adam@piggz.fsnet.co.uk");
	aboutData->addAuthor("Peter Simonsson",I18N_NOOP("Former Developer"),"psn@linux.se");
	aboutData->addAuthor("Joseph Wenninger", I18N_NOOP("Original Form Designer, Original UIModes & much more"), "jowenn@kde.org");
	aboutData->addAuthor("Seth Kurzenberg",I18N_NOOP("CQL++, SQL assistance"),  "seth@cql.com");
	aboutData->addAuthor("Laurent Montel", I18N_NOOP("Original code cleanings"), "montel@kde.org");
	aboutData->addAuthor("Till Busch", I18N_NOOP("Bugfixes, Original Table Widget"), "till@bux.at");
	aboutData->addCredit("Daniel Molkentin",I18N_NOOP("Initial design improvements"),  "molkentin@kde.org");
	aboutData->addCredit("Kristof Borrey", I18N_NOOP("Icons and UI-Research"), "kristof.borrey@skynet.be");
	aboutData->addCredit("Tomas Krassnig", I18N_NOOP("Coffee sponsoring"), "tkrass05@hak1.at");
	aboutData->setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"), I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"));
#if defined(OOPL_VERSION) && defined(Q_WS_WIN)
	aboutData->setProgramLogo(KEXI_APP_LOGO);
#endif
	return aboutData;
}

#endif
