/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000, 2001 David Faure <faure@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/
#include <koApplication.h>

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kdebug.h>
#include <config.h>

#include "koshell_shell.h"

static const char* description=I18N_NOOP("KOffice Workspace");
static const char* version=VERSION;

extern "C" KOSHELL_EXPORT int kdemain( int argc, char **argv )
{
  KAboutData * aboutData = new KAboutData( "koshell", I18N_NOOP("KOffice Workspace"),
     version, description, KAboutData::License_GPL,
       "(c) 1998-2002, Torben Weis\n(c) 2002-2005, David Faure\n(c) 2005, Sven Lüppken");
  aboutData->addAuthor("Sven Lüppken", I18N_NOOP("Current Maintainer"), "sven@kde.org");
  aboutData->addAuthor("Torben Weis", 0, "weis@kde.org");
  aboutData->addAuthor("David Faure", 0, "faure@kde.org");

  KCmdLineArgs::init( argc, argv, aboutData );
  //KCmdLineArgs::addCmdLineOptions( options );

  KoApplication app;

  KoShellWindow *shell = new KoShellWindow;

  shell->show();

  return app.exec();
}

