/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Sun Jun  9 12:15:11 CEST 2002
    copyright            : (C) 2002 by lucijan busch
    email                : lucijan@gmx.at
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

#include "kexiapplication.h"

static const char *description =
	I18N_NOOP("A Database Frontend");
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
		"(c) 2002, Lucijan Busch", 0, 0, "lucijan@gmx.at");
	aboutData.addAuthor("Lucijan Busch","Developer & Maintainer", "lucijan@gmx.at");
	aboutData.addAuthor("Daniel Molkentin","Developer",  "molkentin@kde.org");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KexiApplication app;

	return app.exec();
}  
