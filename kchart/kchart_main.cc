#include "kchart_main.h"

#include <string.h>

#include <factory_impl.h>

#include <koScanParts.h>

bool g_bWithGUI = true;

FACTORY( KChartDocument, KCharter::Factory_skel, KChartFactory )
// This line is for convenience.
typedef AutoLoader<KChartFactory> KChartAutoLoader;

/*******************************************************************
 *
 * KChartApp
 *
 *******************************************************************/

KChartApp::KChartApp( int argc, char** argv ) : OPApplication( argc, argv )
{
}

KChartApp::~KChartApp()
{
}

void KChartApp::start()
{
  koScanParts();

  // Are we going to create a GUI ?
  if ( g_bWithGUI )
  {
    KChartShell* m_pShell;
    // Create a GUI
    m_pShell = new KChartShell;
    // Allow status/menu/toolbars
    m_pShell->enableMenuBar();
    m_pShell->PartShell_impl::enableStatusBar();
    m_pShell->enableToolBars();
    // Display
    m_pShell->show();
  }
}

int main( int argc, char **argv )
{
  // Parse command line parameters.
  for( int i = 1; i < argc; i++ )
  {
    // Are we started as server? => Someones wants to embed us
    // => We dont create a shell
    if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
      g_bWithGUI = false;
  }
  
  // Publish our factory
  KChartAutoLoader loader( "IDL:KCharter/Factory:1.0" );

  // Lets rock
  KChartApp app( argc, argv );
  app.exec();

  return 0;
}

#include "kchart_main.moc"
