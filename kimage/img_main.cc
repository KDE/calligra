#include "img_main.h"

ImageApp::ImageApp( int argc, char** argv ) : OPApplication( argc, argv )
{
  m_pShell = 0L;
}

ImageApp::~ImageApp()
{
}

void ImageApp::start()
{
  printf("Started\n");
  
  m_pShell = new ImageShell_impl;
  m_pShell->enableMenuBar();
  m_pShell->PartShell_impl::enableStatusBar();
  m_pShell->enableToolBars();
  m_pShell->show();
}

int main( int argc, char **argv )
{
  ImageApp app( argc, argv );
  app.exec();

  return 0;
}

#include "img_main.moc"
