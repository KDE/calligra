#include "kspread_shell.h"

#include <koAboutDia.h>

#include <qmsgbox.h>

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

  m_vView = 0L;
  
  DefaultShell_impl::cleanUp();

  m_rDoc = 0L;
}

void KSpreadShell_impl::setDocument( KSpreadDoc *_doc )
{
  m_rDoc = OPParts::Document::_duplicate( _doc );

  m_vView = _doc->createView();  
  m_vView->setPartShell( this );
  setRootPart( m_vView );
}

void KSpreadShell_impl::fileNew()
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate( new KSpreadDoc );
  if ( !m_rDoc->init() )
  {
    QMessageBox::critical( this, i18n("KSpread Error"), i18n("Could not init"), i18n("Ok") );
    return;
  }
  
  m_vView = m_rDoc->createView();
  m_vView->setPartShell( this );
  setRootPart( m_vView );

  m_rMenuBar->setItemEnabled( m_idMenuFile_SaveAs, true );
  m_rToolBarFile->setItemEnabled( m_idButtonFile_Print, true );
}

bool KSpreadShell_impl::openDocument( const char *_filename )
{
  m_rDoc = 0L;
  
  m_rDoc = OPParts::Document::_duplicate( new KSpreadDoc );
  if ( !m_rDoc->open( _filename ) )
    return false;
  
  m_vView = m_rDoc->createView();
  m_vView->setPartShell( this );
  setRootPart( m_vView );

  m_rMenuBar->setItemEnabled( m_idMenuFile_SaveAs, true );
  m_rToolBarFile->setItemEnabled( m_idButtonFile_Print, true );

  return true;
}

bool KSpreadShell_impl::saveDocument( const char *_filename, const char *_format )
{
  assert( !CORBA::is_nil( m_rDoc ) );

  if ( _format == 0L || *_format == 0 )
    _format = "ksp";
  
  return m_rDoc->saveAs( _filename, _format );
}

bool KSpreadShell_impl::printDlg()
{
  assert( !CORBA::is_nil( m_vView ) );

  return m_vView->printDlg();
}

void KSpreadShell_impl::helpAbout()
{
  KoAboutDia::about( KoAboutDia::KSpread, "0.0.1" );
}

#include "kspread_shell.moc"
