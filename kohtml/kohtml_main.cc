/*

	Copyright (C) 1998 Simon Hausmann
                       <tronical@gmx.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/
//
// $Id$
//

#include "kohtml_main.h"
#include "kohtml_main.moc"
#include <koFactory.h>
#include <koDocument.h>
#include <koQueryTypes.h>
#include <opAutoLoader.h>

#include <kio_job.h>
#include <kio_cache.h>
#include <kregistry.h>
#include <kregfactories.h>
#include <kmimetypes.h>
#include <kmimemagic.h>

#include "kohtml_shell.h"
#include "kohtml_doc.h"

bool g_bWithGUI = true;

#include <list>

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KoHTMLDoc, KoHTMLFactory, KoHTML::DocumentFactory_skel );
typedef OPAutoLoader<KoHTMLFactory> KoHTMLAutoLoader;

KoHTMLApp::KoHTMLApp( int argc, char** argv ) :
     KoApplication( argc, argv, "kohtml" )
{
}

KoHTMLApp::~KoHTMLApp()
{
}

void KoHTMLApp::start()
{

  if ( g_bWithGUI )
  {
    if (g_openFiles.size() == 0)
    {
      KoHTMLShell *m_pShell = new KoHTMLShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for (; it != g_openFiles.end(); ++it);
      {
        KoHTMLShell *m_pShell = new KoHTMLShell;
	m_pShell->show();
	m_pShell->openDocument(it->c_str(), "");
      }
    }
  }
}

int main( int argc, char **argv )
{
  KoHTMLAutoLoader loader("IDL:KoHTML/DocumentFactory:1.0", "KoHTML");

  KoHTMLApp app(argc, argv);

  KIOCache::initStatic();
  KRegistry * registry = new KRegistry;
  registry->addFactory( new KServiceTypeFactory );
  registry->load();

  int i = 1;
  if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
     {
       i++;
       g_bWithGUI = false;
     }

  for (; i < argc; i++)
    g_openFiles.push_back((const char *) argv[i]);

  app.exec();

  return 0;
}
