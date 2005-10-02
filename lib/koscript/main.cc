/* This file is part of the KDE project
   Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>

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

#include "koscript.h"

#include <kapplication.h>
#include <klocale.h>
#include <kcmdlineargs.h>
#include <dcopclient.h>
#include <qstring.h>
#include <stdio.h> /* for printf */
#include <stdlib.h>
#include <qdir.h>
#include <iostream>

static KCmdLineOptions options[] =
{
  { "+script", I18N_NOOP("Script to run"), 0 },
  { "+-- [options]", I18N_NOOP("Options to pass to the script"), 0 },
  KCmdLineLastOption
};

int main( int argc, char** argv )
{
  const char *appName = (argc > 1) ? argv[1] : "koscript";
  KCmdLineArgs::init(argc, argv, appName,
		     I18N_NOOP("KoScript"),
		     I18N_NOOP("KOffice script interpreter."),
		     "2.0.0");

  KCmdLineArgs::addCmdLineOptions(options);

  KApplication app;

  KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

  if (args->count() < 1)
  {
     fprintf(stderr, i18n("You must specify a script.\n").local8Bit());
     ::exit(1);
  }

  qDebug("..... KScript started");
  {
      QStringList argList;
      for( int i = 1; i < args->count(); ++i )
          argList.append( args->arg(i) );

      KSInterpreter script;
      script.addSearchPath( QDir::currentDirPath() );
      QString ex = script.runScript( args->arg(0), argList );
      if ( !ex.isEmpty() )
          printf("%s\n",ex.local8Bit().data());
  }
  std::cout << std::endl; // till I can come up with a working "flush" :}
  return 0;
}
