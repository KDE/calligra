#include "kspread_shell.h"

KSpreadShell_impl::KSpreadShell_impl()
{
}

KSpreadShell_impl::~KSpreadShell_impl()
{
  sdeb("KSpreadShell_impl::~KSpreadShell_impl()\n");
  cleanUp();
  edeb("...KSpreadShell_impl::~KSpreadShell_impl()\n");
}

void KSpreadShell_impl::cleanUp()
{
  if ( m_bIsClean )
    return;

  DefaultShell_impl::cleanUp();

  mdeb("========DOC=======================\n");
  m_rDoc = 0L;
}

void KSpreadShell_impl::setDocument( KSpreadDoc *_doc )
{
  m_rDoc = OPParts::Document::_duplicate( _doc );

  OPParts::View_var v = _doc->createView();  
  v->setPartShell( this );
  setRootPart( v );

  mdeb("setDocument %i | %i\n",v->refCount(),v->_refcnt());
}

void KSpreadShell_impl::fileNew()
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate( new KSpreadDoc );
  
  OPParts::View_var view = m_rDoc->createView();
  view->setPartShell( this );
  setRootPart( view );
}

bool KSpreadShell_impl::openDocument( const char *_filename )
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate( new KSpreadDoc );
  if ( !m_rDoc->open( _filename ) )
    return false;
  
  OPParts::View_var view = m_rDoc->createView();
  view->setPartShell( this );
  setRootPart( view );

  return true;
}

bool KSpreadShell_impl::saveDocument( const char *_filename, const char *_format )
{
  assert( !CORBA::is_nil( m_rDoc ) );

  if ( _format == 0L || *_format == 0 )
    _format = "ksp";
  
  return m_rDoc->saveAs( _filename, _format );
}

#include "kspread_shell.moc"
