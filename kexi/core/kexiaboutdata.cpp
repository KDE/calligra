/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexiaboutdata.h"
#include <kexi_version.h>
#include <kdeversion.h>
#include <kofficeversion.h> //only for KOFFICE_VERSION_STRING
#include <klocale.h>

static const char *description =
	I18N_NOOP("Database creation for everyone")
#ifndef CUSTOM_VERSION
#ifdef KEXI_STANDALONE
	"\n\n" I18N_NOOP("This is standalone version of the application distributed outside of KOffice suite.")
#else
	"\n\n" I18N_NOOP("This application version is distributed with KOffice suite.")
#endif
#endif
	;

using namespace Kexi;

KAboutData* Kexi::createAboutData()
{
	KAboutData *aboutData=new KAboutData( "kexi", KEXI_APP_NAME,
		KEXI_VERSION_STRING 
#ifndef CUSTOM_VERSION
		" (KOffice " KOFFICE_VERSION_STRING ")"
#endif
		, description, 
		KAboutData::License_LGPL_V2,
		I18N_NOOP(	"(c) 2002-2007, Kexi Team\n"
					"(c) 2003-2007, OpenOffice Polska LLC\n"),
		I18N_NOOP(	"This software is developed by Kexi Team - an international group\n"
					"of independent developers, with additional assistance and support\n"
					"from the OpenOffice Polska company.\n\n"
					"Visit the company Home Page: http://www.openoffice.com.pl"),
		"http://www.koffice.org/kexi",
		"submit@bugs.kde.org"
	);
	// authors sorted by last contribution date
	aboutData->addAuthor("Jarosław Staniek / OpenOffice Polska", I18N_NOOP("Project maintainer & developer, design, KexiDB, commercially supported version, win32 port"), "js@iidea.pl");
	aboutData->addAuthor("Lucijan Busch",I18N_NOOP("Former project maintainer & developer"), "lucijan@kde.org");
	aboutData->addAuthor("Cedric Pasteur", I18N_NOOP("KexiPropertyEditor and FormDesigner"), "cedric.pasteur@free.fr");
	aboutData->addAuthor("Adam Pigg", I18N_NOOP("PostgreSQL database driver, Migration module"), "adam@piggz.fsnet.co.uk");
	aboutData->addAuthor("Martin Ellis", I18N_NOOP("Contributions for MySQL and KexiDB, fixes, Migration module, MDB support"), "martin.ellis@kdemail.net");
	aboutData->addAuthor("Sebastian Sauer", I18N_NOOP("Scripting module (KROSS), Python language bindings, design"), "mail@dipe.org");
	aboutData->addAuthor("Christian Nitschkowski", I18N_NOOP("Graphics effects, helper dialogs"), "segfault_ii@web.de");
	aboutData->addAuthor("Peter Simonsson",I18N_NOOP("Former developer"),"psn@linux.se");
	aboutData->addAuthor("Joseph Wenninger", I18N_NOOP("Original Form Designer, original user interface & much more"), "jowenn@kde.org");
	aboutData->addAuthor("Seth Kurzenberg",I18N_NOOP("CQL++, SQL assistance"),  "seth@cql.com");
	aboutData->addAuthor("Laurent Montel", I18N_NOOP("Original code cleanings"), "montel@kde.org");
	aboutData->addAuthor("Till Busch", I18N_NOOP("Bugfixes, original Table Widget"), "till@bux.at");
	aboutData->addCredit("Daniel Molkentin",I18N_NOOP("Initial design improvements"),  "molkentin@kde.org");
	aboutData->addCredit("Kristof Borrey", I18N_NOOP("Icons and user interface research"), "kristof.borrey@skynet.be");
	aboutData->addCredit("Tomas Krassnig", I18N_NOOP("Coffee sponsoring"), "tkrass05@hak1.at");
	aboutData->addCredit("Paweł Wirecki / OpenOffice Polska", I18N_NOOP("Numerous bug reports, usability tests, technical support"), "");
	aboutData->setTranslator(I18N_NOOP("_: NAME OF TRANSLATORS\nYour names"), I18N_NOOP("_: EMAIL OF TRANSLATORS\nYour emails"));
#if defined(CUSTOM_VERSION) && defined(Q_WS_WIN)
	aboutData->setProgramLogo(KEXI_APP_LOGO);
#endif
	return aboutData;
}
