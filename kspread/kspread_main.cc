#include "kspread_main.h"
#include <koScanParts.h>
#include <factory_impl.h>

bool g_bWithGUI = true;

FACTORY( KSpreadDoc, KSpread::Factory_skel, KSpreadFactory )
typedef AutoLoader<KSpreadFactory> KSpreadAutoLoader;

KSpreadApp::KSpreadApp( int argc, char** argv ) : OPApplication( argc, argv )
{
  m_pShell = 0L;
}

KSpreadApp::~KSpreadApp()
{
}

void KSpreadApp::start()
{
  if ( g_bWithGUI )
  {
    printf("Started\n");
    koScanParts();
    printf("Parsed\n");
    
    m_pShell = new KSpreadShell_impl;
    m_pShell->enableMenuBar();
    m_pShell->PartShell_impl::enableStatusBar();
    m_pShell->enableToolBars();
    m_pShell->show();
  }
}

int main( int argc, char **argv )
{
  for( int i = 1; i < argc; i++ )
  {
    if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
      g_bWithGUI = false;
  }

  KSpreadAutoLoader loader( "IDL:KSpread/Factory:1.0" );

  KSpreadApp app( argc, argv );
  app.exec();

  return 0;
}

#include "kspread_main.moc"
