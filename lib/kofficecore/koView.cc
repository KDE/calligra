/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
 
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.
 
   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.
 
   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/     

#include "koView.h"
#include "koDocument.h"

#include <kdebug.h>

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
    kdebug( KDEBUG_ERROR, 30003, "You must NOT combine KOffice Views with an OpenParts MainWindow." );
	kdebug( KDEBUG_ERROR, 30003, "Please use KoMainWindow." );
    return;
  }
  
  OPViewIf::setMainWindow( _main );
}

bool KoViewIf::isMarked()
{
  return m_bMarked;
}

void KoViewIf::setFocus( bool mode )
{
  bool old = m_bFocus;
  
  OPPartIf::setFocus( mode );
  
  if ( old == m_bFocus )
    return;

  SIGNAL_CALL2( "stateChanged", CORBA::Any::from_boolean( isMarked() ), CORBA::Any::from_boolean( hasFocus() ) );
}

void KoViewIf::setMarked( bool _marked )
{
  if ( _marked == m_bMarked )
    return;
  
  m_bMarked = _marked;

  SIGNAL_CALL2( "stateChanged", CORBA::Any::from_boolean( isMarked() ), CORBA::Any::from_boolean( hasFocus() ) );
}

bool KoViewIf::printDlg()
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

