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

#include <kaboutdata.h>
#include <klocale.h>
#include <config.h>

static const char *description =
	I18N_NOOP("Database Management");

static const char *version=VERSION;

KAboutData *newKexiAboutData()
{
	KAboutData *aboutData=new KAboutData( "kexi", I18N_NOOP("Kexi"),
		version, description, KAboutData::License_LGPL,
		I18N_NOOP("(c) 2002, Kexi Team"), 0, "http://luci.bux.at/projects/kexi");
	aboutData->addAuthor("Lucijan Busch",I18N_NOOP("Developer & Maintainer"), "lucijan@gmx.at");
	aboutData->addAuthor("Peter Simonsson",I18N_NOOP("Developer"),"psn@linux.se");
	aboutData->addAuthor("Joseph Wenninger", I18N_NOOP("Form Designer, UIModes & much more"), "jowenn@kde.org");
	aboutData->addAuthor("Seth Kurzenberg",I18N_NOOP("CQL++, SQL assistance"),  "seth@cql.com");
	aboutData->addAuthor("Laurent Montel", I18N_NOOP("Code cleanings"), "montell@club-internet.fr");
	aboutData->addAuthor("Till Busch", I18N_NOOP("Buxfixes, Original Table Widget"), "till@bux.at");
	aboutData->addCredit("Daniel Molkentin",I18N_NOOP("Initial design improvements"),  "molkentin@kde.org");
	aboutData->addCredit("Kristof Borrey", I18N_NOOP("Icons and UI-Research"), "kristof.borrey@skynet.be");
	aboutData->setTranslator(I18N_NOOP("_:NAME OF TRANSLATORS\nNames"), I18N_NOOP("_:EMAIL OF TRANSLATORS\ne-mail"));

	return aboutData;
}

#endif
