#include <qprinter.h>
#include "kformula_main.h"
#include <koQueryTypes.h>
#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <opAutoLoader.h>
#include "kformula_shell.h"
#include "kformula_doc.h"
#include <koApplication.h>

// DEBUG
#include <iostream>

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KFormulaDoc, KFormulaFactory, KFormula::DocumentFactory_skel )
typedef OPAutoLoader<KFormulaFactory> KFormulaAutoLoader;

KFormulaApp::KFormulaApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kformula" )
{
  m_pShell = 0L;
}

KFormulaApp::~KFormulaApp()
{
}

void KFormulaApp::start()
{
  if ( g_bWithGUI )
  {
    imr_init();
    koInitTrader();
    
    if ( g_openFiles.size() == 0 )
    {
      m_pShell = new KFormulaShell;
      m_pShell->show();
      m_pShell->newDocument();
    }
    else
    {
      list<string>::iterator it = g_openFiles.begin();
      for( ; it != g_openFiles.end(); ++it )
      {
	m_pShell = new KFormulaShell;
	m_pShell->show();
	m_pShell->openDocument( it->c_str(), "" );
      }
    }
  }
}

int main( int argc, char **argv )
{
  KFormulaAutoLoader loader( "IDL:KFormula/DocumentFactory:1.0", "KFormula" );

  KFormulaApp app( argc, argv );

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

#include "kformula_main.moc"
