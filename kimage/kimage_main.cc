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

#include "formats.h"
#include "kimage_shell.h"
#include "kimage_main.h"
#include "kimage_doc.h"
#include "kimage.h"

KOFFICE_DOCUMENT_FACTORY( KImageDoc, KImageFactory, KImage::DocumentFactory_skel )
typedef OPAutoLoader<KImageFactory> KImageAutoLoader;

KImageApp::KImageApp( int& argc, char** argv ) 
  : KoApplication( argc, argv, "kimage" )
  , m_params( argc, argv ) // sollte in KoApplcation
  , m_bWithGUI( true ) // sollte in KoApplication
{
  FormatManager* formatManager;
  formatManager = new FormatManager();

  // sollte in KoApplication
  QStringList::Iterator it;
  if( m_params.paramIsPresent( "--server", "-s", true, it ) )
  {
    m_bWithGUI = false;
    m_params.deleteParam( it );
  }
}

KImageApp::~KImageApp()
{
}

/*************************************************************************************************/
// sollte in KoApplication.h als pur virtuell definiert sein,
// so daß jede Applikation sie überschreiben muß

KoShell* KImageApp::createNewShell()
{
  return new KImageShell;
}

/*************************************************************************************************/
// sollte in KoApplication.h definiert sein
// 
// Das mit KoShell hab ich nur gemacht, weil newDocument() und openDucoment() keine pur virtuellen
// Prototypen in KoMainWindow besitzen. Vielleicht sollten sowas dort vorhanden sein.

void KImageApp::start()
{
  KoShell* pShell;
  QStringList openFiles;

  if( m_bWithGUI )
  {
    for( uint i = 0; i < m_params.countParams(); i++ )
    {
      if( m_params.getParam( i ).left( 1 ) != "-" )
      {
        openFiles.append( m_params.getParam( i ) );
      }
    }
    if( openFiles.isEmpty() )
    {
      pShell = createNewShell();
      pShell->show();
      pShell->newDocument();
    }
    else
    {
      QStringList::Iterator it;
 
      for( it = openFiles.begin() ; it != openFiles.end() ; ++it )
      {
        pShell = createNewShell();
        pShell->show();
        pShell->openDocument( *it, "" );
      }
    }
  }
}

/*************************************************************************************************/

int main( int argc, char** argv )
{
  KImageAutoLoader loader( "IDL:KImage/DocumentFactory:1.0", "KImage" );
  KImageApp app( argc, argv );

  app.exec();

  return 0;
}

#include "kimage_main.moc"
