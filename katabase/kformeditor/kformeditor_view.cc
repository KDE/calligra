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
#include <koPrintDia.h>
#include <koAboutDia.h>

#include "kformeditor_doc.h"
#include "kformeditor_view.h"
#include "kformeditor_shell.h"
#include "formobject.h"
#include "widgetwrapper.h"

/*****************************************************************************
 *
 * KformEditorView
 *
 *****************************************************************************/

KformEditorView::KformEditorView( QWidget* _parent, const char* _name, KformEditorDoc* _doc )
  : QScrollView( _parent, _name )
  , KoViewIf( _doc )
  , OPViewIf( _doc )
  , KformEditor::View_skel()
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

    QWidget* obj = new WidgetWrapper( (*it)->create( this ) );
    
    if( obj )
    {
      QScrollView::addChild( obj );
      QScrollView::moveChild( obj, (*it)->posx(), (*it)->posy() );

      QObject::connect( obj,  SIGNAL( clicked( WidgetWrapper* ) ),
                        this, SLOT( slotWidgetSelected( WidgetWrapper* ) ) );
      QObject::connect( this, SIGNAL( unselectAll() ),
                        obj,  SLOT( slotUnselect() ) );
    }
  }

  slotUpdateView();
}

void KformEditorView::init()
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

KformEditorView::~KformEditorView()
{
  kdebug( KDEBUG_INFO, 0, "KformEditorView::~KformEditorView() %li", _refcnt() );

  cleanUp();
}

void KformEditorView::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "void KformEditorView::cleanUp() " );

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

KformEditorDoc* KformEditorView::doc()
{
  return m_pDoc;
}

bool KformEditorView::event( const char* _event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

  END_EVENT_MAPPER;

  return false;
}

#define TEXT( text ) Q2C( i18n( text ) )
#define PIX( pix ) *OPUIUtils::convertPixmap( ICON( pix ) )
 
#define MENU( menu, text ) _menubar->insertMenu( TEXT( text ), menu, -1, -1 );
#define ITEM1( id, menu, text, func ) id = menu->insertItem( TEXT( text ), this, func, 0 );
#define ITEM2( id, menu, text, func, key ) id = menu->insertItem4( TEXT( text ), this, func, key, -1, -1 );
#define ITEM3( id, menu, pix, text, func ) id = menu->insertItem6( PIX( pix ), TEXT( text ), this, func, 0, -1, -1 );
#define ITEM4( id, menu, pix, text, func, key ) id = menu->insertItem6( PIX( pix ), TEXT( text ), this, func, key, -1, -1 );
#define MN_SEPARATOR( menu ) menu->insertSeparator( -1 );

#define TOOLBAR( bar ) bar = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
#define BUTTON1( id, bar, pix, num, func, text ) id = bar->insertButton2( PIX( pix ), num, SIGNAL( clicked() ), this, func, true, TEXT( text ), -1 );
#define TB_SEPARATOR( bar ) bar->insertSeparator( -1 );                 

bool KformEditorView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  kdebug( KDEBUG_INFO, 0, "bool KformEditorView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" );

  if ( CORBA::is_nil( _factory ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vToolBarInsert = 0L;
    m_vToolBarOrientation = 0L;
    return true;
  }

  TOOLBAR( m_vToolBarInsert )
  BUTTON1( m_idToolBarInsert_Button, m_vToolBarInsert, "unknown.xpm", 1, "insertButton", "Insert button" )
  BUTTON1( m_idToolBarInsert_Label, m_vToolBarInsert, "unknown.xpm", 2, "insertLabel", "Insert label" )
  BUTTON1( m_idToolBarInsert_LineEdit, m_vToolBarInsert, "unknown.xpm", 3, "insertLineEdit", "Insert lineedit" )
  BUTTON1( m_idToolBarInsert_ListBox, m_vToolBarInsert, "unknown.xpm", 4, "insertListBox", "Insert listbox" )
  BUTTON1( m_idToolBarInsert_CheckBox, m_vToolBarInsert, "unknown.xpm", 5, "insertCheckBox", "Insert checkbox" )

  m_vToolBarInsert->enable( OpenPartsUI::Show);

  TOOLBAR( m_vToolBarOrientation )
  BUTTON1( m_idToolBarOrientation_Center, m_vToolBarOrientation , "unknown.xpm", 1, "orientationCenter", "Center widgets" )

  m_vToolBarOrientation->enable( OpenPartsUI::Show);

  return true;
}

bool KformEditorView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  kdebug( KDEBUG_INFO, 0, "bool KformEditorView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )" );

  if ( CORBA::is_nil( _menubar ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vMenuEdit = 0L;
    return true;
  }

  CORBA::WString_var text;

  text = Q2C( i18n( "&Edit" ) );
  _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

  ITEM1( m_idMenuEdit_FormSize, m_vMenuEdit, "&Size of form", "editFormSize" );
  ITEM1( m_idMenuEdit_Background, m_vMenuEdit, "&Background", "editBackground" );

  return true;
}

void KformEditorView::helpUsing()
{
  kapp->invokeHTMLHelp( "kformeditor/kimage.html", QString::null );
}

CORBA::Boolean KformEditorView::printDlg()
{
  QPrinter prt;

  if( QPrintDialog::getPrinterSetup( &prt ) )
  {
    //m_pDoc->print( &prt );
  }
  return true;
}

void KformEditorView::newView()
{
  ASSERT( m_pDoc != 0L );

  KformEditorShell* shell = new KformEditorShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

void KformEditorView::editFormSize()
{
}

void KformEditorView::editBackground()
{
}

void KformEditorView::insertButton()
{
}

void KformEditorView::insertLabel()
{
}

void KformEditorView::insertLineEdit()
{
}

void KformEditorView::insertListBox()
{
}

void KformEditorView::insertCheckBox()
{
}

void KformEditorView::orientationCenter()
{
}

void KformEditorView::resizeEvent( QResizeEvent* )
{
  // TODO: remove this here. Why resize it every time ?
  resizeContents( m_pDoc->getFormWidth(), m_pDoc->getFormHeight() );

  slotUpdateView();
}

void KformEditorView::mouseMoveEvent( QMouseEvent* _event )
{
  //if( _event->button() != QMouseEvent::NoButton )
  {
    cerr << "KformEditorView::mouseMoveEvent: moving" << endl;
  }
}

void KformEditorView::slotUpdateView()
{
  if( !m_pDoc->isEmpty() )
  {
    //setPalette( QPalette( white ) );
    resizeContents( m_pDoc->getFormWidth(), m_pDoc->getFormHeight() );
  }

  QScrollView::update();
}

void KformEditorView::slotWidgetSelected( WidgetWrapper* _widget )
{
  cerr << "KformEditorView::slotWidgetSelected()" << endl;

  emit unselectAll();
  _widget->select( TRUE );

  slotUpdateView();
}

/*
void KformEditorView::drawContentsOffset( QPainter * p, int offsetx, int offsety, int , int , int , int )
{
  if( m_pDoc && !m_pDoc->isEmpty() )
  {
    // Begrenzung des Formulars zeichnen
    p->drawRect( 10 - offsetx, 10 - offsety, 
                  m_pDoc->getFormWidth() - 5 - offsetx, m_pDoc->getFormHeight() - 5 - offsety);
  }                                                                                                 
}
*/

#include "kformeditor_view.moc"
