#include "kformula_main.h"

#include <string.h>

#include <factory_impl.h>

#include <koScanParts.h>

bool g_bWithGUI = true;

FACTORY( KFormulaDocument, KFormula::Factory_skel, KFormulaFactory )
// This line is for convenience.
typedef AutoLoader<KFormulaFactory> KFormulaAutoLoader;

/*******************************************************************
 *
 * KFormulaApp
 *
 *******************************************************************/

KFormulaApp::KFormulaApp( int argc, char** argv ) : OPApplication( argc, argv )
{
    getLocale()->insertCatalogue("kformula");
}

KFormulaApp::~KFormulaApp()
{
}

void KFormulaApp::start()
{
  koScanParts();

  // Are we going to create a GUI ?
  if ( g_bWithGUI )
  {
    KFormulaShell* m_pShell;
    // Create a GUI
    m_pShell = new KFormulaShell;
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
  KFormulaAutoLoader loader( "IDL:KFormula/Factory:1.0" );

  // Lets rock
  KFormulaApp app( argc, argv );
  app.exec();

  return 0;
}

#include "kformula_main.moc"
