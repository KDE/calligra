#include <qprinter.h>

#include "koshell_main.h"

#include <koIMR.h>
#include <koApplication.h>
#include <koQueryTypes.h>

// DEBUG
#include <iostream>

list<string> g_openFiles;

KoShellApp::KoShellApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "KoShell" )
{
  m_pShell = 0L;
}

KoShellApp::~KoShellApp()
{
}

void KoShellApp::start()
{
  imr_init();
  koInitTrader();

  if ( g_openFiles.size() == 0 )
  {
    m_pShell = new KoShellWindow;
    m_pShell->show();
    // m_pShell->newDocument();
  }
  else
  {
    m_pShell = new KoShellWindow;
    m_pShell->show();

    list<string>::iterator it = g_openFiles.begin();
    for( ; it != g_openFiles.end(); ++it )
    {
      // m_pShell->openDocument( it->c_str(), "" );
    }
  }
}

int main( int argc, char **argv )
{
  KoShellApp app( argc, argv );

  int i = 1;
  if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
  {
    i++;
  }

  for( ; i < argc; i++ )
    g_openFiles.push_back( (const char*)argv[i] );
  
  app.exec();

  cerr << "============ BACK from event loop ===========" << endl;

  return 0;
}

#include "koshell_main.moc"
