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

#include <koFactory.h>
#include <opAutoLoader.h>

#include "kspread_main.h"
#include "kspread_doc.h"

#include <kglobal.h>
#include <klocale.h>

// HACK: For KScript
extern CORBA::ORB* orb()
{
  return komapp_orb;
}

KOFFICE_DOCUMENT_FACTORY( KSpreadDoc, KSpreadFactory, KSpread::DocumentFactory_skel )
typedef OPAutoLoader<KSpreadFactory> KSpreadAutoLoader;

KSpreadApp::KSpreadApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kspread" )
{
  // Tell klocale that we can handle localized numeric and monetary
  // Well not yet, but that will come :))
  KGlobal::locale()->enableNumericLocale( true );
}

KSpreadApp::~KSpreadApp()
{
}

int main( int argc, char **argv )
{
  KSpreadAutoLoader loader( "IDL:KSpread/DocumentFactory:1.0", "KSpread" );

  KSpreadApp app( argc, argv );

  app.exec();

  cerr << "============ BACK from event loop ===========" << endl;

  return 0;
}

#include "kspread_main.moc"
