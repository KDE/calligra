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

#include <opAutoLoader.h>

#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <koQueryTypes.h>

#include "kimage_shell.h"
#include "kimage_main.h"
#include "kimage_doc.h"
#include "kimage.h"

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KImageDoc, KImageFactory, KImage::DocumentFactory_skel )
typedef OPAutoLoader<KImageFactory> KImageAutoLoader;

KImageApp::KImageApp( int& argc, char** argv ) 
  : KoApplication( argc, argv, "kimage" )
{
  m_pShell = 0L;
}

KImageApp::~KImageApp()
{
}

void KImageApp::start()
{
  if( g_bWithGUI )
  {
    imr_init();
    koInitTrader();
    if( !g_openFiles.size() )
    {
      m_pShell = new KImageShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
        m_pShell = new KImageShell;
        m_pShell->show();
        m_pShell->openDocument( it->c_str(), "" );
      }
    }
  }
}

int main( int argc, char** argv )
{
  KImageAutoLoader loader( "IDL:KImage/DocumentFactory:1.0", "KImage" );

  KImageApp app( argc, argv );

  int i = 1;
  if( strcmp( argv[ i ], "-s" ) == 0 || strcmp( argv[ i ], "--server" ) == 0 )
  {
    i++;
    g_bWithGUI = false;
  }

  for( ; i < argc; i++ )
  {
    g_openFiles.push_back( (const char*)argv[i] );
  }

  app.exec();

  debug( "============ BACK from event loop ===========" );

  return 0;
}

#include "kimage_main.moc"
