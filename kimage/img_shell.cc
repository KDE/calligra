#include "img_shell.h"

ImageShell_impl::ImageShell_impl()
{
}

ImageShell_impl::~ImageShell_impl()
{
  sdeb("ImageShell_impl::~ImageShell_impl()\n");
  cleanUp();
  edeb("...ImageShell_impl::~ImageShell_impl()\n");
}

void ImageShell_impl::cleanUp()
{
  if ( m_bIsClean )
    return;

  DefaultShell_impl::cleanUp();

  mdeb("========DOC=======================\n");
  m_rDoc = 0L;
}

void ImageShell_impl::setDocument( ImageDocument_impl *_doc )
{
  m_rDoc = OPParts::Document::_duplicate( _doc );

  OPParts::View_var v = _doc->createView();  
  v->setPartShell( this );
  setRootPart( v );

  mdeb("setDocument %i | %i\n",v->refCount(),v->_refcnt());
}

bool ImageShell_impl::openDocument( const char *_filename )
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate( new ImageDocument_impl );
  if ( !m_rDoc->open( _filename ) )
    return false;
  
  OPParts::View_var view = m_rDoc->createView();
  view->setPartShell( this );
  setRootPart( view );

  m_rMenuBar->setItemEnabled( m_idMenuFile_SaveAs, true );
  m_rToolBarFile->setItemEnabled( m_idButtonFile_Print, true );

  return true;
}

bool ImageShell_impl::saveDocument( const char *_filename, const char *_format )
{
  assert( !CORBA::is_nil( m_rDoc ) );

  if ( _format == 0L || *_format == 0 )
    // HACK
    //_format = "img";
    _format = "ppm";
  
  return m_rDoc->saveAs( _filename, _format );
}

#include "img_shell.moc"
