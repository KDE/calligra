/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Oct  23 22:30:00 CEST 2002
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


#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>
#include <kapplication.h>

#include "kexisetupwizard.h"

static const char *description =
	I18N_NOOP("A Database Frontend Setup Wizard");
	
	
static KCmdLineOptions options[] =
{
  { "startkexi", I18N_NOOP("Start kexi after the wizard has finished"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};

int main(int argc, char *argv[])
{

	KAboutData aboutData( "kexi", I18N_NOOP("Kexi Setup Wizard"),
		"0.1", description, KAboutData::License_GPL,
		"(c) 2002, Joseph Wenninger", 0, 0, "jowenn@kde.org");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

	KApplication app;
	KexiSetupWizard *sw=new KexiSetupWizard;
	app.setMainWidget(sw);
	sw->show();
	return app.exec();
}  
