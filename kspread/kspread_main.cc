#include "kspread_main.h"

KSpreadApp::KSpreadApp( int argc, char** argv ) : OPApplication( argc, argv )
{
  m_pShell = 0L;
}

KSpreadApp::~KSpreadApp()
{
}

void KSpreadApp::start()
{
  printf("Started\n");
  
  m_pShell = new KSpreadShell_impl;
  m_pShell->enableMenuBar();
  m_pShell->PartShell_impl::enableStatusBar();
  m_pShell->enableToolBars();
  m_pShell->show();
}

int main( int argc, char **argv )
{
  KSpreadApp app( argc, argv );
  app.exec();

  return 0;
}

#include "kspread_main.moc"
