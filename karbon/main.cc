/* This file is part of the KDE project
   Copyright (C) 2001, The Karbon Developers
*/

#include <koApplication.h>
#include <koDocument.h>
#include <koMainWindow.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <dcopclient.h>

#include <kdebug.h>

#include "karbon_factory.h"

static const KCmdLineOptions options[] =
{
	{ "+[file]", I18N_NOOP("File To Open"), 0 } ,
	{ 0, 0, 0 }
};

int main( int argc, char* argv[] )
{
	KCmdLineArgs::init( argc, argv, KarbonFactory::aboutData() );
	KCmdLineArgs::addCmdLineOptions( options );
	KoApplication app;

	app.dcopClient()->attach();
	app.dcopClient()->registerAs( "karbon" );

	if ( !app.start() ) // parses command line args, create initial docs and shells
	return 1;
	return app.exec();
}
