#include "koView.h"
#include "koDocument.h"

#include <iostream>

KoViewIf::KoViewIf( KoDocument* _doc ) : OPViewIf( _doc )
{
  ADD_INTERFACE( "IDL:KOffice/View:1.0" );
  
  SIGNAL_IMPL( "stateChanged" );
  
  m_bMarked = false;
  m_eMode = KOffice::View::RootMode;
}

KoViewIf::~KoViewIf()
{
}

void KoViewIf::setMainWindow( OpenParts::MainWindow_ptr _main )
{
  m_vKoMainWindow = KOffice::MainWindow::_narrow( _main );
  if ( CORBA::is_nil( m_vKoMainWindow ) )
  {
    cerr << "You must NOT combine KOffice Views with an OpenParts MainWindow." << endl
	 << "Please use KoMainWindow." << endl;
    return;
  }
  
  OPViewIf::setMainWindow( _main );
}

CORBA::Boolean KoViewIf::isMarked()
{
  return m_bMarked;
}

void KoViewIf::setFocus( CORBA::Boolean mode )
{
  CORBA::Boolean old = m_bFocus;
  
  OPPartIf::setFocus( mode );
  
  if ( old == m_bFocus )
    return;

  SIGNAL_CALL2( "stateChanged", CORBA::Any::from_boolean( isMarked() ), CORBA::Any::from_boolean( hasFocus() ) );
}

void KoViewIf::setMarked( CORBA::Boolean _marked )
{
  if ( _marked == m_bMarked )
    return;
  
  m_bMarked = _marked;

  SIGNAL_CALL2( "stateChanged", CORBA::Any::from_boolean( isMarked() ), CORBA::Any::from_boolean( hasFocus() ) );
}

CORBA::Boolean KoViewIf::printDlg()
{
  return false;
}

void KoViewIf::setMode( KOffice::View::Mode _mode )
{
  m_eMode = _mode;
}

KOffice::View::Mode KoViewIf::mode()
{
  return m_eMode;
}

