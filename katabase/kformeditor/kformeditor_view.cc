/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Michael Koch <koch@kde.org>

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
#include <qcolor.h>

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

#include "kformeditor_doc.h"
#include "kformeditor_view.h"
#include "kformeditor_shell.h"

#include "form.h"
#include "resizewidget.h"

/*****************************************************************************
 *
 * KformEditorView
 *
 *****************************************************************************/

KformEditorView::KformEditorView( QWidget* _parent, const char* _name, KformEditorDoc* _doc )
  : QWidget( _parent, _name )
  , KoViewIf( _doc )
  , OPViewIf( _doc )
  , KformEditor::View_skel()
{
  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

  m_pDoc = _doc;
  m_pForm = NULL;

  QObject::connect( m_pDoc, SIGNAL( sigUpdateView() ), this, SLOT( slotUpdateView() ) );

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

/*
  OpenParts::StatusBarManager_var status_bar_manager = m_vMainWindow->statusBarManager();

  if( !CORBA::is_nil( status_bar_manager ) )
    m_vStatusBar = status_bar_manager->registerClient( id() );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a status bar manager" );

  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::String_var text = Q2C( i18n( "Form editor started" ) );
    m_vStatusBar->insertItem( text, 1);

    m_vStatusBar->enable( ::OpenPartsUI::Show );
  }
*/

  /******************************************************
   * Form
   ******************************************************/

  m_pForm = new Form( m_pDoc, this );

  ResizeWidget* rw1 = new ResizeWidget( m_pForm, ResizeWidget::Right, blue );
  rw1->move( m_pDoc->getFormWidth(), 0 );
  rw1->raise();

  ResizeWidget* rw2 = new ResizeWidget( m_pForm, ResizeWidget::Bottom, blue );
  rw1->move( 0, m_pDoc->getFormHeight() );
  rw1->raise();
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

/*
  OpenParts::StatusBarManager_var status_bar_manager = m_vMainWindow->statusBarManager();
  if ( !CORBA::is_nil( status_bar_manager ) )
  {
    status_bar_manager->unregisterClient( id() );
    m_vStatusBar = 0L;
  }
*/

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
    m_vToolBarAlignment = 0L;
    return true;
  }

  CORBA::WString_var tooltip;
  OpenPartsUI::Pixmap_var pix;

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarEdit->setFullWidth( false );

  tooltip = Q2C( i18n( "Undo" ) );
  pix = OPICON( "undo.xpm" ) ;
  m_idToolBarEdit_Undo = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editUndo", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Redo" ) );
  pix = OPICON( "redo.xpm" );
  m_idToolBarEdit_Redo = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editRedo", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Cut" ) );
  pix = OPICON( "editcut.xpm" );
  m_idToolBarEdit_Cut = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCut", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Copy" ) );
  pix = OPICON( "editcopy.xpm" );
  m_idToolBarEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editCopy", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Past" ) );
  pix = OPICON( "editpaste.xpm" );
  m_idToolBarEdit_Paste = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "editPaste", true, tooltip, -1 );

  m_vToolBarEdit->enable( OpenPartsUI::Show );

  m_vToolBarInsert = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarInsert->setFullWidth( false );

  tooltip = Q2C( i18n( "Insert button" ) );
  pix = OPICON( "button.xpm" );
  m_idToolBarInsert_Button = m_vToolBarInsert->insertButton2( pix, 1, SIGNAL( clicked() ), this, "insertButton", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert label" ) );
  pix = OPICON( "label.xpm" );
  m_idToolBarInsert_Label = m_vToolBarInsert->insertButton2( pix, 2, SIGNAL( clicked() ), this, "insertLabel", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert lineedit" ) );
  pix = OPICON( "lineedit.xpm" );
  m_idToolBarInsert_LineEdit = m_vToolBarInsert->insertButton2( pix, 3, SIGNAL( clicked() ), this, "insertLineEdit", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert listbox" ) );
  pix = OPICON( "listbox.xpm" );
  m_idToolBarInsert_ListBox = m_vToolBarInsert->insertButton2( pix, 4, SIGNAL( clicked() ), this, "insertListBox", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Insert checkbox" ) );
  pix = OPICON( "checkbox.xpm" );
  m_idToolBarInsert_CheckBox = m_vToolBarInsert->insertButton2( pix, 5, SIGNAL( clicked() ), this, "insertCheckBox", true, tooltip, -1 );

  m_vToolBarInsert->enable( OpenPartsUI::Show );

  m_vToolBarAlignment = _factory->create( OpenPartsUI::ToolBarFactory::Transient );

  m_vToolBarAlignment->setFullWidth( false );

  tooltip = Q2C( i18n( "Fit view to form" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Center = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentFitViewToForm", true, tooltip, -1 );

  m_vToolBarAlignment->insertSeparator( -1 );

  tooltip = Q2C( i18n( "Center widgets" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Center = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentCenter", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets left" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Left = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentLeft", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets horizontal center" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_HorizontalCenter = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentHorizontalCenter", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets right" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Right = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentRight", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets top" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Top = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentTop", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets vertical center" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_VerticalCenter = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentVerticalCenter", true, tooltip, -1 );

  tooltip = Q2C( i18n( "Widgets bottom" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Bottom = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentBottom", true, tooltip, -1 );

  m_vToolBarAlignment->insertSeparator( -1 );

  tooltip = Q2C( i18n( "Horizontal balance" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Bottom = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentBottom", true, tooltip, -1 );

tooltip = Q2C( i18n( "Vertical balance" ) );
  pix = OPICON( "unknown.xpm" );
  m_idToolBarAlignment_Bottom = m_vToolBarAlignment->insertButton2( pix, 6, SIGNAL( clicked() ), this, "alignmentBottom", true, tooltip, -1 );

  m_vToolBarAlignment->enable( OpenPartsUI::Show );

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
  pix = OPICON( "undo.xpm" );
  m_vMenuEdit->insertItem6( pix, text, this, "editUndo", CTRL + Key_U, -1, -1 ); 

  text = Q2C( i18n( "&Redo" ) );
  pix = OPICON( "redo.xpm" );
  m_vMenuEdit->insertItem6( pix, text, this, "editRedo", CTRL + Key_R, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = Q2C( i18n( "&Cut" ) );
  pix = OPICON( "editcut.xpm" );
  m_vMenuEdit->insertItem6( pix, text, this, "editCut", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "C&opy" ) );
  pix = OPICON( "editcopy.xpm" );
  m_vMenuEdit->insertItem6(  pix, text, this, "editCopy", CTRL + Key_C, -1, -1 );

  text = Q2C( i18n( "&Paste" ) );
  pix = OPICON( "editpaste.xpm" );
  m_vMenuEdit->insertItem6(  pix, text, this, "editPaste", CTRL + Key_V, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  text = Q2C( i18n( "&Size of form" ) );
  m_idMenuEdit_FormSize = m_vMenuEdit->insertItem( text, this, "editFormSize", 0 );

  text = Q2C( i18n( "&Background" ) );
  m_idMenuEdit_Background = m_vMenuEdit->insertItem( text, this, "editBackground", 0 );

  m_vMenuEdit->insertSeparator( -1 );

  text = Q2C( i18n( "&Form properties" ) );
  m_idMenuEdit_FormSize = m_vMenuEdit->insertItem( text, this, "editFormSize", 0 );

  text = Q2C( i18n( "&Insert" ) );
  _menubar->insertMenu( text, m_vMenuInsert, -1, -1 );

  text = Q2C( i18n( "&Button" ) );
  pix = OPICON( "button.xpm" );
  m_vMenuInsert->insertItem6( pix, text, this, "insertButton", 0, -1, -1 );

  text = Q2C( i18n( "&Checkbox" ) );
  pix = OPICON( "checkbox.xpm" );
  m_vMenuInsert->insertItem6( pix, text, this, "insertCheckBox", 0, -1, -1 );

  text = Q2C( i18n( "&Label" ) );
  pix = OPICON( "label.xpm" );
  m_vMenuInsert->insertItem6( pix, text, this, "insertLabel", 0, -1, -1 );

  text = Q2C( i18n( "Line&edit" ) );
  pix = OPICON( "lineedit.xpm" );
  m_vMenuInsert->insertItem6( pix, text, this, "insertLineEdit", 0, -1, -1 );

  text = Q2C( i18n( "List&box" ) );
  pix = OPICON( "listbox.xpm" );
  m_vMenuInsert->insertItem6( pix, text, this, "insertListBox", 0, -1, -1 );

  // TODO: make own CORBA func for "insert subform"

  text = Q2C( i18n( "&Subform" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuInsert->insertItem6( pix, text, this, "insertListBox", 0, -1, -1 );

  text = Q2C( i18n( "&Alignment" ) );
  _menubar->insertMenu( text, m_vMenuAlignment, -1, -1 );

  text = Q2C( i18n( "&Fit view to form" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentFitViewToForm", CTRL + Key_X, -1, -1 );

  m_vMenuAlignment->insertSeparator( -1 );

  text = Q2C( i18n( "&Center widgets" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentCenter", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Left" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentLeft", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Horizontal center" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentHorizontalCenter", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Right" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentRight", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Top" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentTop", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Vertical center" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentVerticalCenter", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Bottom" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentBottom", CTRL + Key_X, -1, -1 );

  m_vMenuAlignment->insertSeparator( -1 );

  text = Q2C( i18n( "&Horizontal balance" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentBottom", CTRL + Key_X, -1, -1 );

  text = Q2C( i18n( "&Vertical balance" ) );
  pix = OPICON( "unknown.xpm" );
  m_vMenuAlignment->insertItem6( pix, text, this, "alignmentBottom", CTRL + Key_X, -1, -1 );

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

/*
void KformEditorView::initChilds()
{
  // Remove all existing childs

  // insert childs, data from m_pDoc
  QValueList<FormObject*>::Iterator it = m_pDoc->m_lstFormObjects.begin();
  for( ; it != m_pDoc->m_lstFormObjects.end(); ++it )
  {
    cerr << "KFormViewer: Inserting child" << endl;
 
//  QWidget* wrapper = new WidgetWrapper( (*it)->create( this ) );
    QWidget* wrapper = new WidgetWrapper( 0 );
 
    if( wrapper )
    {
      QScrollView::addChild( wrapper );
      QScrollView::moveChild( wrapper, (*it)->posx(), (*it)->posy() );
 
      QObject::connect( wrapper,  SIGNAL( clicked( WidgetWrapper* ) ),
                        this, SLOT( slotClick( WidgetWrapper* ) ) );
      QObject::connect( wrapper,  SIGNAL( clickedShift( WidgetWrapper* ) ),
                        this, SLOT( slotShiftClick( WidgetWrapper* ) ) );
      QObject::connect( this, SIGNAL( unselectAll() ),
                        wrapper,  SLOT( slotUnselect() ) );
      QObject::connect( wrapper, SIGNAL( moveWidget( WidgetWrapper*, const QPoint& ) ),
                        this,  SLOT( slotMoveWidget( WidgetWrapper*, const QPoint& ) ) );
      wrapper->setBackgroundColor( m_pDoc->backgroundColor() );
    }
  }

  m_primaryWidget = NULL;
  m_countSelectedWidgets = 0;
}

void KformEditorView::drawContents( QPainter* _painter, int _clipx, int _clipy, int _clipw, int _cliph )
{
  QColor color( 80, 80, 80 );
  _painter->fillRect( _clipx, _clipy, _clipw, _cliph, QBrush( color ) );
}
*/

void KformEditorView::editUndo()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Undo" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::editRedo()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Redo" ) );

    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::editCut()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Cut widgets" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::editCopy()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Copy widgets" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::editPaste()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Paste widgets" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::editFormSize()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Change form size" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::editBackground()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Change background" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }

/*
  BackgroundDlg* dlg = new BackgroundDlg( m_pDoc->backgroundColor() );

  if( dlg->exec() == QDialog::Accepted )
  {
    cerr << "Farbe setzen" << endl;

    m_pDoc->setBackgroundColor( dlg->color() );

    m_background->setBackgroundColor( dlg->color() );
  }
*/

  slotUpdateView();
}

void KformEditorView::insertButton()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Insert Button" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::insertLabel()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Insert Label" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::insertLineEdit()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Insert LineEdit" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::insertListBox()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Insert ListBox" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::insertCheckBox()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Insert CheckBox" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentFitViewToForm()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Fit view to form" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentCenter()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Center the selected widgets" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentLeft()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Position the selected widgets to left" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentHorizontalCenter()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Position the selected widgets to horizontal center" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentRight()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Position the selected widgets to right" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentTop()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Position the selected widgets to top" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentVerticalCenter()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Position the selected widgets to vertical center" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

void KformEditorView::alignmentBottom()
{
  if( !CORBA::is_nil( m_vStatusBar ) )
  {
    CORBA::WString_var text = Q2C( i18n ( "Position the selected widgets to bottom" ) );
 
    m_vStatusBar->changeItem( text, 1 );
  }
}

/*
bool KformEditorView::eventFilter( QObject* _obj, QEvent* _event )
{
  if( ( _event->type() == QEvent::MouseButtonPress ) ||
      ( _event->type() == QEvent::MouseButtonDblClick ) )
  {
    cerr << "KformEditorView::eventFilter()" << endl;

    return TRUE;
  }

  return FALSE;
}

void KformEditorView::resizeEvent( QResizeEvent* _event )
{
  QScrollView::resizeEvent( _event );

  slotUpdateView();
}
*/

void KformEditorView::slotUpdateView()
{
  if( !m_pDoc->isEmpty() )
  {
    //resizeContents( m_pDoc->getFormWidth(), m_pDoc->getFormHeight() );
  }

  QWidget::update();
//QScrollView::update();
}

/*
void KformEditorView::slotClick( WidgetWrapper* _widget )
{
  if( m_countSelectedWidgets == 0 )
  {
    emit unselectAll();

    m_primaryWidget = _widget;
    m_primaryWidget->slotSelectPrimary();
    m_countSelectedWidgets = 1;
  }
  else if( ( m_countSelectedWidgets == 1 ) &&
           ( _widget->selectState() != WidgetWrapper::PrimarySelect ) )
  {
    emit unselectAll();

    m_primaryWidget = _widget;
    m_primaryWidget->slotSelectPrimary(); 
    m_countSelectedWidgets = 1;
  }
  else if( ( m_countSelectedWidgets > 1 ) &&
           ( _widget->selectState() != WidgetWrapper::PrimarySelect ) )
  {
    emit unselectAll();

    m_primaryWidget = _widget;
    m_primaryWidget->slotSelectPrimary();
    m_countSelectedWidgets = 1;
  }
  else if( ( m_countSelectedWidgets > 0 ) &&
           ( _widget->selectState() == WidgetWrapper::PrimarySelect ) )
  {
    // do nothing for now
    // this mode is for moving widgets
  }
  else
  {
    cerr << "ERROR IN DESIGN : Impossible Mouse select" << endl;
  }

  slotUpdateView();
}

void KformEditorView::slotShiftClick( WidgetWrapper* _widget )
{
  if( m_countSelectedWidgets == 0 )
  {
    emit unselectAll();

    m_primaryWidget = _widget;
    m_primaryWidget->slotSelectPrimary();
    m_countSelectedWidgets = 1;
  }
  else if( ( m_countSelectedWidgets == 1 ) &&
           ( _widget->selectState() != WidgetWrapper::PrimarySelect ) )
  {
    _widget->slotSelectSecondary();
    m_countSelectedWidgets++;
  }
  else if( m_countSelectedWidgets > 1 )
  {
    if( _widget->selectState() == WidgetWrapper::SecondarySelect )
    {
      _widget->slotUnselect();
      m_countSelectedWidgets--;
    }
    else if( _widget->selectState() == WidgetWrapper::NoSelect )
    {
      _widget->slotSelectSecondary();
      m_countSelectedWidgets++;
    }
    else
    {
      cerr << "ERROR IN DESIGN : Impossible Mouse select" << endl;
    }
  }
  else if( ( m_countSelectedWidgets > 0 ) &&
           ( _widget->selectState() == WidgetWrapper::PrimarySelect ) )
  {
    _widget->slotUnselect();
    m_countSelectedWidgets = 0;
  }
  else
  {
    cerr << "ERROR IN DESIGN : Impossible Mouse select" << endl;
  }

  slotUpdateView();
}

void KformEditorView::slotMoveWidget( WidgetWrapper* _widget, const QPoint& _pos )
{
//moveChild( _widget, _pos.x(), _pos.y() );
  _widget->resize( _pos.x(), _pos.y() );
}
*/

#include "kformeditor_view.moc"
