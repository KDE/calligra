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
#include <kapplication.h>

#include "kexiglobal.h"
#include "kexi.h"
#include "kexidb.h"
//#include "kexiinterfacemanager.h"

static const char *description =
	I18N_NOOP("Kexi");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

KexiGlobal *g_Global = 0;

int main(int argc, char *argv[])
{

	KAboutData aboutData( "kexi", I18N_NOOP("Kexi"),
		"0.0.1", description, KAboutData::License_GPL,
		"(c) 2002, lucijan busch", 0, 0, "lucijan@gmx.at");
	aboutData.addAuthor("lucijan busch","developer & maintainer", "lucijan@gmx.at");
	aboutData.addAuthor("Daniel Molkentin","developer",  "molkentin@kde.org");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication app;

	g_Global = new KexiGlobal;	
	 
		
	g_Global->g_db = new KexiDB();
	
	Kexi *kexi = new Kexi();
	kexi->show();

	return app.exec();
}  
