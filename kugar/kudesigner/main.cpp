/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Jun  6 11:31:39 EEST 2002
    copyright            : (C) 2002 by Alexander Dymo
    email                : cloudtemple@mksat.net
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

#include "kudesigner.h"

static const char *description =
	I18N_NOOP("Kugar Report Designer");
// INSERT A DESCRIPTION FOR YOUR APPLICATION HERE
	
	
static KCmdLineOptions options[] =
{
  { "+[File]", I18N_NOOP("file to open"), 0 },
  { 0, 0, 0 }
  // INSERT YOUR COMMANDLINE OPTIONS HERE
};


int main(int argc, char *argv[])
{
   KLocale::setMainCatalogue("kugar");
	KAboutData aboutData( "kudesigner", I18N_NOOP("Kugar Report Designer"),
		VERSION, description, KAboutData::License_GPL,
		"(c) 2002, Alexander Dymo", 0, 0, "cloudtemple@mksat.net");
	aboutData.addAuthor("Alexander Dymo",
     I18N_NOOP("Maintainer, Report Designer, Kugar library enhancements") ,
     "cloudtemple@mksat.net", "http://www.cloudtemple.mksat.net");
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

  KApplication app;
 
  if (app.isRestored())
  {
     RESTORE(KuDesignerApp);
  }
  else
  {
    KuDesignerApp *kudesigner = new KuDesignerApp();
    kudesigner->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
		
		if (args->count())
		{
        kudesigner->openDocumentFile(args->url(0));
		}
		else
		{
		  kudesigner->openDocumentFile();
		}
		args->clear();
  }

  return app.exec();
}  
