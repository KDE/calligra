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

#include <qprinter.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qprndlg.h>

#include <kfiledialog.h>
#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

#include <opMenu.h>
#include <opToolBar.h>
#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <komApplication.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

#include "kformviewer_doc.h"
#include "kformviewer_view.h"
#include "kformviewer_shell.h"
#include "formobject.h"

/*****************************************************************************
 *
 * KformViewerView
 *
 *****************************************************************************/

KformViewerView::KformViewerView( QWidget* _parent, const char* _name, KformViewerDoc* _doc )
  : QScrollView( _parent, _name )
  , KoViewIf( _doc )
  , OPViewIf( _doc )
  , KformViewer::View_skel()
{
  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

  m_pDoc = _doc;

  QObject::connect( m_pDoc, SIGNAL( sigUpdateView() ), this, SLOT( slotUpdateView() ) );

  // initialize Childs
  QValueList<FormObject*>::Iterator it = m_pDoc->m_lstFormObjects.begin();
  for( ; it != m_pDoc->m_lstFormObjects.end(); ++it )
  {
    cerr << "KFormViewer: Inserting child" << endl;

    QWidget* obj = (*it)->create( this );

    if( obj )
    {
      QScrollView::addChild( obj );
      QScrollView::moveChild( obj, (*it)->posx(), (*it)->posy() );

      if( (*it)->type() == FormObject::Button )
      {
        if( (*it)->action() == "quit" )
        {
          QObject::connect( obj, SIGNAL( clicked() ), komapp, SLOT( quit() ) );
        }
      }
    }
  }

  slotUpdateView();
}

void KformViewerView::init()
{
  /******************************************************
   * Menu
   ******************************************************/

  kdebug( KDEBUG_INFO, 0, "Registering menu as %li", id() );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a menu bar manager" );

  /******************************************************
   * Toolbar
   ******************************************************/

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a tool bar manager" );
}

KformViewerView::~KformViewerView()
{
  kdebug( KDEBUG_INFO, 0, "KformViewerView::~KformViewerView() %li", _refcnt() );

  cleanUp();
}

void KformViewerView::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "void KformViewerView::cleanUp() " );

  if ( m_bIsClean )
  {
    return;
  }

  kdebug( KDEBUG_INFO, 0, "1b) Unregistering menu and toolbar" );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  m_pDoc->removeView( this );

  KoViewIf::cleanUp();
}

KformViewerDoc* KformViewerView::doc()
{
  return m_pDoc;
}

bool KformViewerView::event( const QCString &_event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

  END_EVENT_MAPPER;

  return false;
}

#define TEXT( text ) Q2C( i18n( text ) )

#define MENU( menu, text ) _menubar->insertMenu( TEXT( text ), menu, -1, -1 );
#define ITEM1( id, menu, text, func ) id = menu->insertItem( TEXT( text ), this, func, 0 );
#define ITEM2( id, menu, text, func, key ) id = menu->insertItem4( TEXT( text ), this, func, key, -1, -1 );
#define ITEM3( id, menu, pix, text, func ) id = menu->insertItem6( OPICON( pix ), TEXT( text ), this, func, 0, -1, -1 );
#define ITEM4( id, menu, pix, text, func, key ) id = menu->insertItem6( OPICON( pix ), TEXT( text ), this, func, key, -1, -1 );
#define MN_SEPARATOR( menu ) menu->insertSeparator( -1 );

#define TOOLBAR( bar ) bar = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
#define BUTTON1( id, bar, pix, num, func, text ) id = bar->insertButton2( OPICON( pix ), num, SIGNAL( clicked() ), this, func, true, TEXT( text ), -1 );
#define TB_SEPARATOR( bar ) bar->insertSeparator( -1 );

bool KformViewerView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  kdebug( KDEBUG_INFO, 0, "bool KformViewerView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" );

  if ( CORBA::is_nil( _factory ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    return true;
  }

  return true;
}

bool KformViewerView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  kdebug( KDEBUG_INFO, 0, "bool KformViewerView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )" );

  if ( CORBA::is_nil( _menubar ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    return true;
  }

  return true;
}

void KformViewerView::helpUsing()
{
  kapp->invokeHTMLHelp( "kformviewer/kimage.html", QString::null );
}

bool KformViewerView::printDlg()
{
  QPrinter prt;

  if( QPrintDialog::getPrinterSetup( &prt ) )
  {
    //m_pDoc->print( &prt );
  }
  return true;
}

void KformViewerView::newView()
{
  ASSERT( m_pDoc != 0L );

  KformViewerShell* shell = new KformViewerShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

void KformViewerView::resizeEvent( QResizeEvent* )
{
  // TODO: remove this here. Why resize it every time ?
  resizeContents( m_pDoc->getFormWidth(), m_pDoc->getFormHeight() );

  slotUpdateView();
}

void KformViewerView::slotUpdateView()
{
  if( !m_pDoc->isEmpty() )
  {
    //setPalette( QPalette( white ) );
    resizeContents( m_pDoc->getFormWidth(), m_pDoc->getFormHeight() );
  }

  QScrollView::update();
}

/*
void KformViewerView::drawContentsOffset( QPainter * p, int offsetx, int offsety, int , int , int , int )
{
  if( m_pDoc && !m_pDoc->isEmpty() )
  {
    // Begrenzung des Formulars zeichnen
    p->drawRect( 10 - offsetx, 10 - offsety,
                  m_pDoc->getFormWidth() - 5 - offsetx, m_pDoc->getFormHeight() - 5 - offsety);
  }
}
*/

#include "kformviewer_view.moc"
