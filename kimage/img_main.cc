#include "img_main.h"

#include <string.h>

#include <factory_impl.h>

bool g_bWithGUI = true;

// This line creates a new factory class. The class has a function
// 'create' which generates a new 'ImageDocument_impl' on request.
//
// The CORBA Interface of the factory is called "KImage::Factory_skel".
// The factory will be derived from this class. Mention that the idl
// definition of the factory must not include any additional method.
// An example is:
//
// module KImage
// {
// ...
//    interface Factory : OPParts::Factory
//    {
//    };
// };
//
// The name of the generated factory class is 'KImageFactory'
// The first line in your 'main' function should look like this:
//     KImageAutoLoader loader( "IDL:KImage/Factory:1.0" );
// Please mention that "IDL:KImage/Factory:1.0" is the IDL name of
// KImage::Factory_skel. The Autoloader publishes the factory.
// You need the factory stuff only if one should be able to embed
// your application. The function "imr_newdoc" and "imr_activate"
// use the CORBA::BOA which in turn uses the factory to create
// new documents.
FACTORY( ImageDocument_impl, KImage::Factory_skel, KImageFactory )
// This line is for convenience.
typedef AutoLoader<KImageFactory> KImageAutoLoader;

/*******************************************************************
 *
 * ImageApp
 *
 *******************************************************************/

ImageApp::ImageApp( int argc, char** argv ) : OPApplication( argc, argv )
{
}

ImageApp::~ImageApp()
{
}

void ImageApp::start()
{
  // Are we going to create a GUI ?
  if ( g_bWithGUI )
  {
    ImageShell_impl* m_pShell;
    // Create a GUI
    m_pShell = new ImageShell_impl;
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
  KImageAutoLoader loader( "IDL:KImage/Factory:1.0" );

  // Lets rock
  ImageApp app( argc, argv );
  app.exec();

  return 0;
}

#include "img_main.moc"
