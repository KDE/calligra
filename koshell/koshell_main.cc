/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 2000, 2001 David Faure <david@mandrakesoft.com>

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

#include "koshell_shell.h"

static const char* description=I18N_NOOP("KOffice Workspace");
static const char* version="1.1";

int main( int argc, char **argv )
{
  KAboutData * aboutData = new KAboutData( "koshell", I18N_NOOP("KOffice Workspace"),
     version, description, KAboutData::License_GPL,
       "(c) 1998-2001, Torben Weis");
  aboutData->addAuthor("Torben Weis", 0, "weis@kde.org");
  aboutData->addAuthor("David Faure", 0, "david@mandrakesoft.com");

  KCmdLineArgs::init( argc, argv, aboutData );
  //KCmdLineArgs::addCmdLineOptions( options );

  KoApplication app;

  KoShellWindow *shell = new KoShellWindow;

  shell->show();

  return app.exec();
}

