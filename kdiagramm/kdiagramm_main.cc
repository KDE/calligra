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

#include <qprinter.h>
#include "kdiagramm_main.h"
#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <koApplication.h>
#include <koQueryTypes.h>
#include <opAutoLoader.h>
#include "kdiagramm_shell.h"
#include "kdiagramm_doc.h"
#include "kdiagramm.h"

// DEBUG
#include <iostream>

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KDiagrammDoc, KDiagrammFactory, KDiagramm::DocumentFactory_skel )
typedef OPAutoLoader<KDiagrammFactory> KDiagrammAutoLoader;

KDiagrammApp::KDiagrammApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kdiagramm" )
{
  m_pShell = 0L;
}

KDiagrammApp::~KDiagrammApp()
{
}

void KDiagrammApp::start()
{
  if ( g_bWithGUI )
  {
    if ( g_openFiles.size() == 0 )
    {
      m_pShell = new KDiagrammShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
	m_pShell = new KDiagrammShell;
	m_pShell->show();
	m_pShell->openDocument( it->c_str(), "" );
      }
    }
  }
}

int main( int argc, char **argv )
{
  KDiagrammAutoLoader loader( "IDL:KDiagramm/DocumentFactory:1.0", "KDiagramm" );

  KDiagrammApp app( argc, argv );

  int i = 1;
  if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
  {
    i++;
    g_bWithGUI = false;
  }

  for( ; i < argc; i++ )
    g_openFiles.push_back( (const char*)argv[i] );
  
  app.exec();

  cerr << "============ BACK from event loop ===========" << endl;

  return 0;
}

#include "kdiagramm_main.moc"
