#include <qprinter.h>
#include "kimage_main.h"
#include <koIMR.h>
#include <koFactory.h>
#include <koDocument.h>
#include <koApplication.h>
#include <koQueryTypes.h>
#include <opAutoLoader.h>
#include "kimage_shell.h"
#include "kimage_doc.h"
#include "kimage.h"

// DEBUG
#include <iostream>

bool g_bWithGUI = true;

list<string> g_openFiles;

KOFFICE_DOCUMENT_FACTORY( KImageDoc, KImageFactory, KImage::DocumentFactory_skel )
typedef OPAutoLoader<KImageFactory> KImageAutoLoader;

KImageApp::KImageApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "kimage" )
{
  m_pShell = 0L;
}

KImageApp::~KImageApp()
{
}

void KImageApp::start()
{
  if ( g_bWithGUI )
  {
    imr_init();
    koInitTrader();

    if ( g_openFiles.size() == 0 )
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

int main( int argc, char **argv )
{
  KImageAutoLoader loader( "IDL:KImage/DocumentFactory:1.0", "KImage" );

  KImageApp app( argc, argv );

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

#include "kimage_main.moc"
