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

KOFFICE_DOCUMENT_FACTORY( KDiagrammDoc, KDiagrammFactory, KDiagramm::DocumentFactory_skel )
typedef OPAutoLoader<KDiagrammFactory> KDiagrammAutoLoader;

KDiagrammApp::KDiagrammApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kdiagramm" )
{
  //  m_pShell = 0L;
}

KDiagrammApp::~KDiagrammApp()
{
}

int main( int argc, char **argv )
{
  KDiagrammAutoLoader loader( "IDL:KDiagramm/DocumentFactory:1.0", "KDiagramm" );

  KDiagrammApp app( argc, argv );

  app.exec();

  cerr << "============ BACK from event loop ===========" << endl;

  return 0;
}

#include "kdiagramm_main.moc"
