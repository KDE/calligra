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

// #include <qprinter.h>
#include <qstringlist.h>

#include <koQueryTypes.h>
#include <koIMR.h>
#include <koApplication.h>
#include <koFactory.h>
#include <koDocument.h>
#include <opAutoLoader.h>

#include "kspread_main.h"
#include "kspread_shell.h"
#include "kspread_doc.h"
#include "kspread.h"

bool g_bWithGUI = true;

QStringList g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KSpreadDoc, KSpreadFactory, KSpread::DocumentFactory_skel )
typedef OPAutoLoader<KSpreadFactory> KSpreadAutoLoader;

KSpreadApp::KSpreadApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kspread" )
{
  m_pShell = 0L;
}

KSpreadApp::~KSpreadApp()
{
}

void KSpreadApp::start()
{
  if ( g_bWithGUI )
  {
    if ( g_openFiles.count() == 0 )
    {
      m_pShell = new KSpreadShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      QStringList::Iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
	m_pShell = new KSpreadShell;
	m_pShell->show();
	m_pShell->openDocument( *it, "" );
      }
    }
  }
}

int main( int argc, char **argv )
{
  KSpreadAutoLoader loader( "IDL:KSpread/DocumentFactory:1.0", "KSpread" );

  KSpreadApp app( argc, argv );

  int i = 1;
  if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
  {
    i++;
    g_bWithGUI = false;
  }

  for( ; i < argc; i++ )
    g_openFiles.append( (const char*)argv[i] );
  
  app.exec();

  cerr << "============ BACK from event loop ===========" << endl;

  return 0;
}

#include "kspread_main.moc"
