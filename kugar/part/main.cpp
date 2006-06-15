/***************************************************************************
                         main.cpp  -  description
                            -------------------
   begin                : Mon Nov 25 17:45:00 CET 2002
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

#include <KoApplication.h>
#include <kcmdlineargs.h>

#include "kugar_about.h"

static KCmdLineOptions options[] =
    {
        { "+[File]", I18N_NOOP( "File to open" ), 0 },
        // INSERT YOUR COMMANDLINE OPTIONS HERE
        KCmdLineLastOption
    };

extern "C" KUGAR_EXPORT int kdemain( int argc, char *argv[] )
{

    KCmdLineArgs::init( argc, argv, newKugarAboutData() );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.

    KoApplication app;

    if ( !app.start() )
        return 1;

    app.exec();
    return 0;
}
