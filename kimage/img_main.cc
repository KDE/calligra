#include "img_main.h"

#include <string.h>

#include <factory_impl.h>

bool g_bWithGUI = true;

FACTORY( ImageDocument_impl, KImage::Factory_skel, KImageFactory )
typedef AutoLoader<KImageFactory> KImageAutoLoader;

/*******************************************************************
 *
 * ImageApp
 *
 *******************************************************************/

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

  if ( g_bWithGUI )
  {
    printf("Started with GUI\n");
    
    m_pShell = new ImageShell_impl;
    m_pShell->enableMenuBar();
    m_pShell->PartShell_impl::enableStatusBar();
    m_pShell->enableToolBars();
    m_pShell->show();
  }
}

int main( int argc, char **argv )
{
  printf("!!!!!!!!!!!!!!!!!!!!! IMG STARTED !!!!!!!!!!!!!!!!!!!!!!!!!!\n");
  
  for( int i = 1; i < argc; i++ )
  {
    if ( strcmp( argv[i], "-s" ) == 0 || strcmp( argv[i], "--server" ) == 0 )
      g_bWithGUI = false;
  }
  
  KImageAutoLoader loader( "IDL:KImage/Factory:1.0" );

  ImageApp app( argc, argv );
  app.exec();

  return 0;
}

#include "img_main.moc"
