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

KformEditorView::~KformEditorView()
{
  kdebug( KDEBUG_INFO, 0, "KformEditorView::~KformEditorView() %li", _refcnt() );
 
  cleanUp();
}

void KformEditorView::init()
{
  /******************************************************
   * Menu
   ******************************************************/

  kdebug( KDEBUG_INFO, 0, "Registering menu as %li", id() );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a menu bar manager" );

  /******************************************************
   * Toolbar
   ******************************************************/

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a tool bar manager" );

  /******************************************************
   * Statusbar
   ******************************************************/

  OpenParts::StatusBarManager_var status_bar_manager = m_vMainWindow->statusBarManager();
  if( !CORBA::is_nil( status_bar_manager ) )
    m_vStatusBar = status_bar_manager->registerClient( id() );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a status bar manager" );

  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    m_vStatusBar->insertItem( Q2C( i18n( "Form editor started" ) ), 1);

    m_vStatusBar->enable( ::OpenPartsUI::Show );
  } 
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

  OpenParts::StatusBarManager_var status_bar_manager = m_vMainWindow->statusBarManager();
  if ( !CORBA::is_nil( status_bar_manager ) )
  {
    status_bar_manager->unregisterClient( id() );
    m_vStatusBar = 0L;
  }

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

bool KformEditorView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  kdebug( KDEBUG_INFO, 0, "bool KformEditorView::mappingCreateToolbar()" );

  if ( CORBA::is_nil( _factory ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vToolBarEdit = 0L;
    m_vToolBarInsert = 0L;
    m_vToolBarOrientation = 0L;
    return true;
  }

  CORBA::WString_var tooltip;
  OpenPartsUI::Pixmap_var pix;

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarEdit->setFullWidth( false );

  tooltip = Q2C( i18n( "Undo" ) );
  pix = OPUIUtils::convertPixmap( ICON( "undo.xpm" ) );
  m_idToolBarEdit_Undo = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editUndo", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Redo" ) );
  pix = OPUIUtils::convertPixmap( ICON( "redo.xpm" ) );
  m_idToolBarEdit_Redo = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editRedo", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Cut" ) );
  pix = OPUIUtils::convertPixmap( ICON( "editcut.xpm" ) );
  m_idToolBarEdit_Cut = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCut", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Copy" ) );
  pix = OPUIUtils::convertPixmap( ICON( "editcopy.xpm" ) );
  m_idToolBarEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCopy", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Past" ) );
  pix = OPUIUtils::convertPixmap( ICON( "editpaste.xpm" ) );
  m_idToolBarEdit_Paste = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editPaste", true, tooltip, -1 );

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarInsert->setFullWidth( false );

  tooltip = Q2C( i18n( "Insert button" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarInsert_Button = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertButton", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert label" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarInsert_Label = m_vToolBarInsert->insertButton2( pix, 2, SIGNAL( clicked() ), this, "insertLabel", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert lineedit" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarInsert_LineEdit = m_vToolBarInsert->insertButton2( pix, 3, SIGNAL( clicked() ), this, "insertLineEdit", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert listbox" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarInsert_ListBox = m_vToolBarInsert->insertButton2( pix, 4, SIGNAL( clicked() ), this, "insertListBox", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert checkbox" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarInsert_CheckBox = m_vToolBarInsert->insertButton2( pix, 5, SIGNAL( clicked() ), this, "insertCheckBox", true, tooltip, -1 );

  m_vToolBarInsert->enable( OpenPartsUI::Show );

  m_vToolBarOrientation = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarOrientation->setFullWidth( false );

  tooltip = Q2C( i18n( "Fit view to form" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_Center = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationFitViewToForm", true, tooltip, -1 );

  m_vToolBarOrientation->insertSeparator( -1 );

  tooltip = Q2C( i18n( "Center widgets" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_Center = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationCenter", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets left" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_Left = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationLeft", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets horizontal center" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_HorizontalCenter = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationHorizontalCenter", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets right" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_Right = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationRight", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets top" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_Top = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationTop", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets vertical center" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_VerticalCenter = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationVerticalCenter", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets bottom" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_idToolBarOrientation_Bottom = m_vToolBarOrientation->insertButton2( pix, 6, SIGNAL( clicked() ), this, "orientationBottom", true, tooltip, -1 );

  m_vToolBarOrientation->enable( OpenPartsUI::Show );

  return true;
}

bool KformEditorView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  kdebug( KDEBUG_INFO, 0, "bool KformEditorView::mappingCreateMenubar()" );

  if ( CORBA::is_nil( _menubar ) )
  {
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vMenuEdit = 0L;
    return true;
  }

  CORBA::WString_var text;
  OpenPartsUI::Pixmap_var pix;

  text = Q2C( i18n( "&Edit" ) );
  _menubar->insertMenu( text, m_vMenuEdit, -1, -1 );

  text = Q2C( i18n( "&Undo" ) );
  pix = OPUIUtils::convertPixmap( ICON( "undo.xpm" ) );
  m_vMenuEdit->insertItem6( pix, text, this, "editUndo", CTRL + Key_U, -1, -1 ); 

  text = Q2C( i18n( "&Redo" ) );
  pix = OPUIUtils::convertPixmap( ICON( "redo.xpm" ) );
  m_vMenuEdit->insertItem6( pix, text, this, "editRedo", CTRL + Key_R, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = Q2C( i18n( "&Cut" ) );
  pix = OPUIUtils::convertPixmap( ICON( "editcut.xpm" ) );
  m_vMenuEdit->insertItem6( pix, text, this, "editCut", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "C&opy" ) );
  pix = OPUIUtils::convertPixmap( ICON( "editcopy.xpm" ) );
  m_vMenuEdit->insertItem6(  pix, text, this, "editCopy", CTRL + Key_C, -1, -1 );

  text = Q2C( i18n( "&Paste" ) );
  pix = OPUIUtils::convertPixmap( ICON( "editpaste.xpm" ) );
  m_vMenuEdit->insertItem6(  pix, text, this, "editPaste", CTRL + Key_V, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = Q2C( i18n( "&Size of form" ) );
  m_idMenuEdit_FormSize = m_vMenuEdit->insertItem( text, this, "editFormSize", 0 );

  text = Q2C( i18n( "&Background" ) );
  m_idMenuEdit_Background = m_vMenuEdit->insertItem( text, this, "editBackground", 0 );

  text = Q2C( i18n( "&Insert" ) );
  _menubar->insertMenu( text, m_vMenuInsert, -1, -1 );

  text = Q2C( i18n( "&Button" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuInsert->insertItem6( pix, text, this, "insertButton", 0, -1, -1 );

  text = Q2C( i18n( "&Label" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuInsert->insertItem6( pix, text, this, "insertLabel", 0, -1, -1 );

  text = Q2C( i18n( "Line&edit" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuInsert->insertItem6( pix, text, this, "insertLineEdit", 0, -1, -1 );

  text = Q2C( i18n( "List&box" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuInsert->insertItem6( pix, text, this, "insertListBox", 0, -1, -1 );

  text = Q2C( i18n( "&Checkbox" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuInsert->insertItem6( pix, text, this, "insertCheckBox", 0, -1, -1 );

  text = Q2C( i18n( "&Orientation" ) );
  _menubar->insertMenu( text, m_vMenuOrientation, -1, -1 );

  text = Q2C( i18n( "&Fit view to form" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationFitViewToForm", CTRL + Key_X, -1, -1 );

  m_vMenuOrientation->insertSeparator( -1 );

  text = Q2C( i18n( "&Center widgets" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationCenter", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Left" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationLeft", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Horizontal center" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationHorizontalCenter", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Right" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationRight", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Top" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationTop", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Vertical center" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationVerticalCenter", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Bottom" ) );
  pix = OPUIUtils::convertPixmap( ICON( "unknown.xpm" ) );
  m_vMenuOrientation->insertItem6( pix, text, this, "orientationBottom", CTRL + Key_X, -1, -1 );

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

void KformEditorView::editUndo()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Undo" ) ), 1 );
}

void KformEditorView::editRedo()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Redo" ) ), 1 );
}

void KformEditorView::editCut()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Cut widgets" ) ), 1 );
}

void KformEditorView::editCopy()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Copy widgets" ) ), 1 );
}

void KformEditorView::editPaste()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Paste widgets" ) ), 1 );
}

void KformEditorView::editFormSize()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Change form size" ) ), 1 );
}

void KformEditorView::editBackground()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Change background" ) ), 1 );
}

void KformEditorView::insertButton()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Insert Button" ) ), 1 );
}

void KformEditorView::insertLabel()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Insert Label" ) ), 1 );
}

void KformEditorView::insertLineEdit()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Insert LineEdit" ) ), 1 );
}

void KformEditorView::insertListBox()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Insert ListBox" ) ), 1 );
}

void KformEditorView::insertCheckBox()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Insert CheckBox" ) ), 1 );
}

void KformEditorView::orientationFitViewToForm()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Fit view to form" ) ), 1 );
}

void KformEditorView::orientationCenter()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Center the selected widgets" ) ), 1 );
}

void KformEditorView::orientationLeft()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Position the selected widgets to left" ) ), 1 );
}

void KformEditorView::orientationHorizontalCenter()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Position the selected widgets to horizontal center" ) ), 1 );
}

void KformEditorView::orientationRight()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Position the selected widgets to right" ) ), 1 );
}

void KformEditorView::orientationTop()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Position the selected widgets to top" ) ), 1 );
}

void KformEditorView::orientationVerticalCenter()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Position the selected widgets to vertical center" ) ), 1 );
}

void KformEditorView::orientationBottom()
{
  m_vStatusBar->changeItem( Q2C( i18n ( "Position the selected widgets to bottom" ) ), 1 );
}

void KformEditorView::resizeEvent( QResizeEvent* _event )
{
  QScrollView::resizeEvent( _event );

  slotUpdateView();
}

void KformEditorView::mouseMoveEvent( QMouseEvent* _event )
{
  cerr << "KformEditorView::mouseMoveEvent: moving" << endl;
}

void KformEditorView::slotUpdateView()
{
  if( !m_pDoc->isEmpty() )
  {
    //want to make the backgound white
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

#include "kformeditor_view.moc"
