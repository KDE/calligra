/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#include <koApplication.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kglobal.h>
#include <koMainWindow.h>

KoApplication::KoApplication(int &argc, char **argv, const QString& rAppName)
    : OPApplication(argc, argv, rAppName)
    , kded( argc, argv, opapp_orb )
    , m_params( argc, argv )
    , m_bWithGUI( true )
{
    KGlobal::locale()->insertCatalogue("koffice");
    KGlobal::iconLoader()->insertDirectory(0, kde_datadir() + 
				     "/koffice/toolbar");
    KGlobal::iconLoader()->insertDirectory(3, localkdedir() + 
				     "/share/apps/koffice/toolbar");
}

KoApplication::~KoApplication()
{
  // checking wether start the app as server or not
  QStringList::Iterator it;
  if( m_params.find( "--server", "-s", true, it ) )
  {
    m_bWithGUI = false;
    m_params.del( it );
  }
}
    
void KoApplication::start()
{
  KoMainWindow* pShell;
  QStringList openFiles;
  QString tmpFilename;
 
  if( m_bWithGUI )
  {
    for( uint i = 0; i < m_params.count(); i++ )
    {
      tmpFilename = m_params.get( i );
      if( tmpFilename.left( 1 ) != "-" )
      {
        openFiles.append( tmpFilename );
      }
    }
    if( openFiles.isEmpty() )
    {
      pShell = createNewShell();
      if( !pShell )
      {
        pShell->show();
        pShell->newDocument();
      }
      else
        kdebug( KDEBUG_FATAL, 30003, "Cannot create new shell. KoApplication::createNewShell() has to be overloded" );
    }
    else
    {
      QStringList::Iterator it;
 
      for( it = openFiles.begin() ; it != openFiles.end() ; ++it )
      {
        pShell = createNewShell();
        if( !pShell )
        {
          pShell->show();
          pShell->openDocument( *it, "" );
        }
        else
          kdebug( KDEBUG_FATAL, 30003, "Cannot create new shell. KoApplication::createNewShell() has to be overloded" );
      }
    }
  }
}                                                                                       

#include "koApplication.moc"
