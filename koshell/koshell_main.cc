#include <qprinter.h>

#include "koshell_main.h"

#include <koApplication.h>
#include <kdebug.h>

KoShellApp::KoShellApp( int &argc, char** argv ) : 
  KoApplication( argc, argv, "KoShell" )
{
  m_pShell = 0L;
}

KoShellApp::~KoShellApp()
{
}

int main( int argc, char **argv )
{
  KoShellApp app( argc, argv );

  app.exec();

  kdebug( KDEBUG_INFO, 0, "============ BACK from event loop ===========" );

  return 0;
}

#include "koshell_main.moc"
