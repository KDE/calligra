/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>

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
#include <qprinter.h>

#include <koApplication.h>
#include <kdebug.h>

#include "koshell_main.h"
#include "koshell_shell.h"

KoShellApp::KoShellApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "KoShell" )
{
}

KoShellApp::~KoShellApp()
{
}

KoMainWindow* KoShellApp::createNewShell()
{
  return new KoShellWindow();
}

int main( int argc, char **argv )
{
  KoShellApp app( argc, argv );

  app.exec();

  kdebug( KDEBUG_INFO, 0, "============ BACK from event loop ===========" );

  return 0;
}

#include "koshell_main.moc"
