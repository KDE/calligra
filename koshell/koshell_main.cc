#include <qprinter.h>

#include <koApplication.h>
#include <kdebug.h>

#include "koshell_main.h"
#include "koshell_shell.h"

KoShellApp::KoShellApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "KoShell" )
{
}

KoShellApp::~KoShellApp()
{
}

KoMainWindow* KoShellApp::createNewShell()
{
  return new KoShellWindow();
}

int main( int argc, char **argv )
{
  KoShellApp app( argc, argv );

  app.exec();

  kdebug( KDEBUG_INFO, 0, "============ BACK from event loop ===========" );

  return 0;
}

#include "koshell_main.moc"
