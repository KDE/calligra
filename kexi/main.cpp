/***************************************************************************
                          main.cpp  -  description
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


#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kstartupinfo.h>
#include <kurl.h>

#include "kexiapplication.h"

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbinterfacemanager.h"

static const char *description =
	I18N_NOOP("Database Management");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

	KAboutData aboutData( "kexi", I18N_NOOP("Kexi"),
		"0.1", description, KAboutData::License_GPL,
		"(c) 2002, Kexi Team", 0, "http://luci.bux.at/projects/kexi", "lucijan@gmx.at");
	aboutData.addAuthor("Lucijan Busch",I18N_NOOP("Developer & Maintainer"), "lucijan@gmx.at");
	aboutData.addAuthor("Peter Simonsson",I18N_NOOP("Developer"),"psn@linux.se");
	aboutData.addAuthor("Joseph Wenninger", I18N_NOOP("Formdesigner, UIModes & much more"), "jowenn@kde.org");
	aboutData.addAuthor("Daniel Molkentin",I18N_NOOP("Design, Improvements"),  "molkentin@kde.org");
	aboutData.addAuthor("Laurent Montel", I18N_NOOP("Code cleanings"), "montell@club-internet.fr");
	aboutData.addAuthor("Till Busch", I18N_NOOP("Buxfixes, Orignial Table Widget"), "till@bux.at");
	aboutData.addCredit("Kristof Borrey", I18N_NOOP("Icons and UI-Research"), "kristof.borrey@skynet.be");
	aboutData.setTranslator(I18N_NOOP("_:NAME OF TRANSLATORS\nNames"), I18N_NOOP("_:EMAIL OF TRANSLATORS\ne-mail"));

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KexiApplication app;
	if (app.exitImmediatly()) {
		KStartupInfo::appStarted();
		return 0;
	}

//	KexiDB *db = new KexiDB(0);
//	QStringList drivers = db->getDrivers();
//	db->add("mySQL");

	return app.exec();
}  
