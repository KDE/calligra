#include <qprinter.h>
#include "kspread_main.h"
#include <koScanParts.h>
#include <koScanTools.h>
#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include "kspread_shell.h"
#include "kspread_doc.h"

// DEBUG
#include <iostream>

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KSpreadDoc, KSpreadFactory )
typedef OPAutoLoader<KSpreadFactory> KSpreadAutoLoader;

KSpreadApp::KSpreadApp( int &argc, char** argv ) : 
  OPApplication( argc, argv, "kspread" )
{
  getLocale()->insertCatalogue("koffice");
  m_pShell = 0L;
}

KSpreadApp::~KSpreadApp()
{
}

void KSpreadApp::start()
{
  if ( g_bWithGUI )
  {
    imr_init();
    koScanParts();
    koScanTools();

    if ( g_openFiles.size() == 0 )
    {
      m_pShell = new KSpreadShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
	m_pShell = new KSpreadShell;
	m_pShell->show();
	m_pShell->openDocument( it->c_str(), "" );
      }
    }
  }
}

int main( int argc, char **argv )
{
  KSpreadAutoLoader loader( "IDL:KOffice/DocumentFactory:1.0", "KSpread" );

  KSpreadApp app( argc, argv );

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

#include "kspread_main.moc"
