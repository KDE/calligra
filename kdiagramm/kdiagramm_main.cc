#include <qprinter.h>
#include "kdiagramm_main.h"
#include <koScanParts.h>
#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <koApplication.h>
#include <opAutoLoader.h>
#include "kdiagramm_shell.h"
#include "kdiagramm_doc.h"

// DEBUG
#include <iostream>

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KDiagrammDoc, KDiagrammFactory )
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
    imr_init();
    koScanParts();

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
  KDiagrammAutoLoader loader( "IDL:KOffice/DocumentFactory:1.0", "KDiagramm" );

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
