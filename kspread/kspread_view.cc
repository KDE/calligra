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

#include <qprinter.h> // has to be first

#include "kspread_view.h"
#include <iostream.h>
#include <stdlib.h>
#include <time.h>

#include <qpushbt.h>
#include <qmsgbox.h>
#include <qprndlg.h>
#include <qobjcoll.h>
#include <qkeycode.h>

#include <kbutton.h>
#include <kapp.h>
#include <klocale.h>
#include <kiconloader.h>
#include <kstdaccel.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koPrintDia.h>
#include <koAboutDia.h>
#include <koScanTools.h>
#include <koQueryTypes.h>

#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_dlg_scripts.h"
#include "kspread_doc.h"
#include "kspread_shell.h"
#include "kspread_dlg_cons.h"
#include "kspread_util.h"
#include "kspread_canvas.h"
#include "kspread_tabbar.h"
#include <kglobal.h>

/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

KSpreadScripts* KSpreadView::m_pGlobalScriptsDialog = 0L;

KSpreadView::KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc* _doc ) :
  QWidget( _parent, _name ), KoViewIf( _doc ), OPViewIf( _doc ), KSpread::View_skel()
{
  ADD_INTERFACE( "IDL:KSpread/View:1.0" );

  m_bInitialized = false;

  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

  m_lstFrames.setAutoDelete( true );
#ifdef USE_PICTURE
  m_lstPictures.setAutoDelete( true );
#endif

  m_pDoc = _doc;

  m_bUndo = false;
  m_bRedo = false;

  m_pluginManager = 0L;
  m_pTable = 0L;

  m_pPopupMenu = 0L;

  // Vert. Scroll Bar
  m_pVertScrollBar = new QScrollBar( this, "ScrollBar_2" );
  m_pVertScrollBar->setRange( 0, 4096 );
  m_pVertScrollBar->setOrientation( QScrollBar::Vertical );

  // Horz. Scroll Bar
  m_pHorzScrollBar = new QScrollBar( this, "ScrollBar_1" );
  m_pHorzScrollBar->setRange( 0, 4096 );
  m_pHorzScrollBar->setOrientation( QScrollBar::Horizontal );

  // Tab Bar
  m_pTabBarFirst = newIconButton( "tab_first.xpm" );
  QObject::connect( m_pTabBarFirst, SIGNAL( clicked() ), SLOT( slotScrollToFirstTable() ) );
  m_pTabBarLeft = newIconButton( "tab_left.xpm" );
  QObject::connect( m_pTabBarLeft, SIGNAL( clicked() ), SLOT( slotScrollToLeftTable() ) );
  m_pTabBarRight = newIconButton( "tab_right.xpm" );
  QObject::  connect( m_pTabBarRight, SIGNAL( clicked() ), SLOT( slotScrollToRightTable() ) );
  m_pTabBarLast = newIconButton( "tab_last.xpm" );
  QObject::connect( m_pTabBarLast, SIGNAL( clicked() ), SLOT( slotScrollToLastTable() ) );

  m_pTabBar = new KSpreadTabBar( this );
  QObject::connect( m_pTabBar, SIGNAL( tabChanged( const char *) ), this, SLOT( slotChangeTable( const char* ) ) );

  // Paper and Border widgets
  m_pFrame = new QWidget( this );
  m_pFrame->raise();

  // Edit Bar
  m_pToolWidget = new QFrame( this );
  m_pToolWidget->setFrameStyle( 49 );

  m_pPosWidget = new QLabel( m_pToolWidget );
  m_pPosWidget->setAlignment( AlignCenter );
  m_pPosWidget->setFrameStyle( QFrame::WinPanel|QFrame::Sunken );
  m_pPosWidget->setGeometry( 2,2,50,26 );

  m_pCancelButton = newIconButton( "abort.xpm", TRUE, m_pToolWidget );
  m_pCancelButton->setGeometry( 60, 2, 26, 26 );
  m_pOkButton = newIconButton( "done.xpm", TRUE, m_pToolWidget );
  m_pOkButton->setGeometry( 90, 2, 26, 26 );

  m_pEditWidget = new KSpreadEditWidget( m_pToolWidget, this );
  m_pEditWidget->setGeometry( 125, 2, 200, 26 );
  m_pEditWidget->setFocusPolicy( QWidget::StrongFocus );

  QObject::connect( m_pCancelButton, SIGNAL( clicked() ), m_pEditWidget, SLOT( slotAbortEdit() ) );
  QObject::connect( m_pOkButton, SIGNAL( clicked() ), m_pEditWidget, SLOT( slotDoneEdit() ) );

  // The widget on which we display the table
  m_pCanvas = new KSpreadCanvas( m_pFrame, this, _doc );

  m_pHBorderWidget = new KSpreadHBorder( m_pFrame, m_pCanvas );
  m_pVBorderWidget = new KSpreadVBorder( m_pFrame, m_pCanvas );

  m_pCanvas->setFocusPolicy( QWidget::StrongFocus );
  QWidget::setFocusPolicy( QWidget::StrongFocus );
  setFocusProxy( m_pCanvas );

  QObject::connect( m_pVertScrollBar, SIGNAL( valueChanged(int) ), m_pCanvas, SLOT( slotScrollVert(int) ) );
  QObject::connect( m_pHorzScrollBar, SIGNAL( valueChanged(int) ), m_pCanvas, SLOT( slotScrollHorz(int) ) );

  m_pCanvas->init();

  KSpreadTable *tbl;
  for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
    addTable( tbl );

  QObject::connect( m_pDoc, SIGNAL( sig_addTable( KSpreadTable* ) ), SLOT( slotAddTable( KSpreadTable* ) ) );
}

void KSpreadView::init()
{
  m_pluginManager = new KoPluginManager();
  
  m_pluginManager->setView( this );
  
  /******************************************************
   * Menu
   ******************************************************/

  cerr << "Registering menu as " << id() << endl;

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    cerr << "Did not get a menu bar manager" << endl;

  /******************************************************
   * Toolbar
   ******************************************************/

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    cerr << "Did not get a tool bar manager" << endl;

  /******************************************************
   * Create views for child documents
   ******************************************************/

  QListIterator<KSpreadChild> it = m_pTable->childIterator();
  for( ; it.current(); ++it )
    slotInsertChild( it.current() );
}

KSpreadView::~KSpreadView()
{
  cerr << "KSpreadView::~KSpreadView() " << _refcnt() << endl;

  cleanUp();
}

void KSpreadView::cleanUp()
{
  cerr << "void KSpreadView::cleanUp() " << endl;

  cerr << "1) VIEW void KOMBase::incRef() = " << m_ulRefCount << endl;
  cerr << "1) VIEW void KOMBase::refcnt() = " << _refcnt() << endl;

  if ( m_bIsClean )
    return;

  cerr << "1a) Deactivate Frames" << endl;

  QListIterator<KSpreadChildFrame> it( m_lstFrames );
  for( ; it.current() != 0L; ++it )
  {
    it.current()->detach();
  }

  cerr << "1b) Unregistering menu and toolbar" << endl;

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  m_pDoc->removeView( this );

  m_pluginManager->cleanUp();
  delete m_pluginManager;
  
  KoViewIf::cleanUp();

  cerr << "2) VIEW void KOMBase::incRef() = " << m_ulRefCount << endl;
  cerr << "2) VIEW void KOMBase::refcnt() = " << _refcnt() << endl;
}

KSpread::Book_ptr KSpreadView::book()
{
  return m_pDoc->book();
}

bool KSpreadView::event( const char* _event, const CORBA::Any& _value )
{
  cerr << "CALLED" << endl;

  EVENT_MAPPER( _event, _value );

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr,
	   mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr,
	   mappingCreateToolbar );
  MAPPING( DataTools::eventDone, DataTools::typeDone, mappingToolDone );
  MAPPING( KSpread::eventSetText, KSpread::EventSetText, mappingEventSetText );
  MAPPING( KSpread::eventKeyPressed, KSpread::EventKeyPressed, mappingEventKeyPressed );
  MAPPING( KSpread::eventChartInserted, KSpread::EventChartInserted,
	   mappingEventChartInserted );

  END_EVENT_MAPPER;

  return false;
}

bool KSpreadView::mappingEventChartInserted( KSpread::EventChartInserted& _event )
{
  if ( m_pTable == 0L )
    return true;
  
  // Request the best matching document which supports the charting interface.
  QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:Chart/SimpleChart:1.0' in RepoIds", 1 );
  if ( vec.isEmpty() )
  {
    cerr << "Got no results" << endl;
    QMessageBox::critical( 0L, i18n("Error"), i18n("Sorry, no charting component registered"), i18n("Ok") );
    return true;
  }

  int xpos, ypos;
  xpos = m_pTable->columnPos( m_pCanvas->markerColumn(), m_pCanvas );
  ypos = m_pTable->rowPos( m_pCanvas->markerRow(), m_pCanvas );

  QRect r( xpos + _event.dx, ypos + _event.dy, _event.width, _event.height );

  cerr << "USING component " << vec[0].name << endl;

  // Insert the document. This will lead to a signal and as response
  // to that signal we will finally create a view and embed the chart visually.
  m_pTable->insertChart( r, vec[0], m_pTable->selectionRect() );

  return true;
}

bool KSpreadView::mappingEventKeyPressed( KSpread::EventKeyPressed& _event )
{
  if ( m_pTable == 0L )
    return true;

  int x, y;
  RowLayout *rl;
  ColumnLayout *cl;
  KSpreadCell *cell;
  // Flag that indicates wether we make a selection right now
  bool make_select = FALSE;

  QRect selection( m_pTable->selectionRect() );

  // Are we making a selection right now ? Go thru this only if no selection is made
  // or if we neither selected complete rows nor columns.
  if ( ( _event.state & ShiftButton ) == ShiftButton &&
       ( selection.left() == 0 || ( selection.right() != 0 && selection.bottom() != 0 ) ) &&
       ( _event.key == Key_Down || _event.key == Key_Up || _event.key == Key_Left || _event.key == Key_Right ) )
    make_select = TRUE;

  // Do we have an old selection ? If yes, unselect everything
  if ( selection.left() != 0 && !make_select )
    m_pTable->unselect();

  switch( _event.key )
    {
    case Key_Return:
    case Key_Enter:
    case Key_Down:	
	m_pCanvas->hideMarker();

	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	  m_pCanvas->setMarkerRow( m_pCanvas->markerRow() + 1 + cell->extraYCells() );
	else
  	  m_pCanvas->setMarkerRow( m_pCanvas->markerRow() + 1 );

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  m_pCanvas->setMarkerRow( cell->obscuringCellsRow() );
	  m_pCanvas->setMarkerColumn( cell->obscuringCellsColumn() );
	}

	y = m_pTable->rowPos( m_pCanvas->markerRow(), m_pCanvas );
	rl = m_pTable->rowLayout( m_pCanvas->markerRow() );
	if ( y + rl->height( m_pCanvas ) > m_pCanvas->height() )
	  vertScrollBar()->setValue( m_pCanvas->yOffset() + ( y + rl->height( m_pCanvas )
								    - m_pCanvas->height() ) );

	// If we have been at the top of the selection ...
	if ( selection.top() == m_pCanvas->markerRow() - 1 && selection.top() != selection.bottom() && make_select )
	    selection.setTop( m_pCanvas->markerRow() );
	else if ( make_select )
	    selection.setBottom( m_pCanvas->markerRow() );

	if ( make_select )
	  m_pTable->setSelection( selection, m_pCanvas );
	
	m_pCanvas->showMarker();

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Up:	
	m_pCanvas->hideMarker();

	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

	m_pCanvas->setMarkerRow( m_pCanvas->markerRow() - 1 );
	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  m_pCanvas->setMarkerRow( cell->obscuringCellsRow() );
	  m_pCanvas->setMarkerColumn( cell->obscuringCellsColumn() );
	}
	
	y = m_pTable->rowPos( m_pCanvas->markerRow(), m_pCanvas );
	rl = m_pTable->rowLayout( m_pCanvas->markerRow() );
	if ( y < 0 )
	    vertScrollBar()->setValue( m_pCanvas->yOffset() + y );

	// If we have been at the top of the selection ...
	if ( selection.top() == m_pCanvas->markerRow() + 1 && make_select )
	    selection.setTop( m_pCanvas->markerRow() );
	else if ( make_select )
	    selection.setBottom( m_pCanvas->markerRow() );

	if ( make_select )
	  m_pTable->setSelection( selection, m_pCanvas );

	m_pCanvas->showMarker();

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Left:
	m_pCanvas->hideMarker();
	
	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

	m_pCanvas->setMarkerColumn( m_pCanvas->markerColumn() - 1 );
	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  m_pCanvas->setMarkerRow( cell->obscuringCellsRow() );
	  m_pCanvas->setMarkerColumn( cell->obscuringCellsColumn() );
	}

	x = m_pTable->columnPos( m_pCanvas->markerColumn(), m_pCanvas );
	cl = m_pTable->columnLayout( m_pCanvas->markerColumn() );
	if ( x < 0 )
	  horzScrollBar()->setValue( m_pCanvas->xOffset() + x );

	// If we have been at the left side of the selection ...
	if ( selection.left() == m_pCanvas->markerColumn() + 1 && make_select )
	    selection.setLeft( m_pCanvas->markerColumn() );
	else if ( make_select )
	    selection.setRight( m_pCanvas->markerColumn() );

	if ( make_select )
	  m_pTable->setSelection( selection, m_pCanvas );

	m_pCanvas->showMarker();

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Right:
	m_pCanvas->hideMarker();

	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	  m_pCanvas->setMarkerColumn( m_pCanvas->markerColumn() + 1 + cell->extraXCells() );
	else
	  m_pCanvas->setMarkerColumn( m_pCanvas->markerColumn() + 1 );

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	// Go to the upper left corner of the obscuring object ( if there is one )
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  m_pCanvas->setMarkerRow( cell->obscuringCellsRow() );
	  m_pCanvas->setMarkerColumn( cell->obscuringCellsColumn() );
	}

	x = m_pTable->columnPos( m_pCanvas->markerColumn(), m_pCanvas );
	cl = m_pTable->columnLayout( m_pCanvas->markerColumn() );
	if ( x + cl->width( m_pCanvas ) > m_pCanvas->width() )
	    horzScrollBar()->setValue( m_pCanvas->xOffset() + ( x + cl->width( m_pCanvas )
								- m_pCanvas->width() ) );

	// If we have been at the right side of the selection ...
	if ( selection.right() == m_pCanvas->markerColumn() - 1 && make_select )
	    selection.setRight( m_pCanvas->markerColumn() );
	else if ( make_select )
	    selection.setLeft( m_pCanvas->markerColumn() );

	if ( make_select )
	  m_pTable->setSelection( selection, m_pCanvas );

	m_pCanvas->showMarker();

	cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Escape:
	if ( m_pCanvas->editDirtyFlag() )
	{
	  cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
	  if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	  else
	    editWidget()->setText( "" );
	}
	break;
    }

  return true;
}

bool KSpreadView::mappingEventSetText( KSpread::EventSetText& _event )
{
  cerr << "GOT EventSetText ´" << _event.text.in() << "´" << endl;

  m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), _event.text.in() );

  return true;
}

bool KSpreadView::mappingToolDone( DataTools::Answer& _answer )
{
  char* str;
  _answer.value >>= CORBA::Any::to_string( str, 0 );
  KSpread::DataToolsId id;
  _answer.id >>= id;

  cerr << "CORRECTED ´" << str << "´" << endl;
  cerr << "r=" << id.row << " c=" << id.column << endl;

  // TODO: check time
  m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), str );

  return true;
}

bool KSpreadView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  cerr << "bool KSpreadView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )" << endl;

  if ( CORBA::is_nil( _factory ) )
  {
    cerr << "Setting to nil" << endl;
    m_vToolBarEdit = 0L;
    m_vToolBarLayout = 0L;

    m_pluginManager->fillToolBar( _factory );

    cerr << "niled" << endl;
    return true;
  }

  CORBA::WString_var wstr;

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarEdit->setFullWidth(false);

  OpenPartsUI::Pixmap_var pix;
  pix = OPUIUtils::convertPixmap( ICON("editcopy.xpm") );
  m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "copySelection", true, ( wstr = Q2C( i18n( "Copy" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("editcut.xpm") );
  m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ), this, "cutSelection", true, ( wstr = Q2C( i18n( "Cut" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("editpaste.xpm") );
  m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ), this, "paste", true, ( wstr = Q2C( i18n( "Paste" ) ) ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("rowout.xpm") );
  m_idButtonEdit_DelRow = m_vToolBarEdit->insertButton2( pix, 4, SIGNAL( clicked() ), this, "deleteRow", true, ( wstr = Q2C( i18n( "Delete Row" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("colout.xpm") );
  m_idButtonEdit_DelCol = m_vToolBarEdit->insertButton2( pix, 5, SIGNAL( clicked() ), this, "deleteColumn", true, ( wstr = Q2C( i18n( "Delete Column") ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("rowin.xpm") );
  m_idButtonEdit_InsRow = m_vToolBarEdit->insertButton2( pix, 6, SIGNAL( clicked() ), this, "insertRow", true, ( wstr = Q2C( i18n( "Insert Row"  ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("colin.xpm") );
  m_idButtonEdit_InsCol = m_vToolBarEdit->insertButton2( pix, 7, SIGNAL( clicked() ), this, "insertColumn", true, ( wstr = Q2C( i18n( "Insert Column"  ) ) ), -1 );

  m_vToolBarEdit->setFullWidth(false);
  m_vToolBarEdit->enable( OpenPartsUI::Show );

  m_vToolBarLayout = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarLayout->setFullWidth(false);

  OpenPartsUI::StrList fonts;
  fonts.length( 4 );
  fonts[0] = CORBA::string_dup( "Courier" );
  fonts[1] = CORBA::string_dup( "Helvetica" );
  fonts[2] = CORBA::string_dup( "Symbol" );
  fonts[3] = CORBA::string_dup( "Times" );

  m_idComboLayout_Font = m_vToolBarLayout->insertCombo( fonts, 1, false, SIGNAL( activated( const QString & ) ), this,
							"fontSelected", true, ( wstr = Q2C( i18n( "Font") ) ),
							120, -1, OpenPartsUI::AtBottom );

  OpenPartsUI::StrList sizelist;
  int sizes[24] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 24, 26, 28, 32, 48, 64 };
  sizelist.length( 24 );
  for( int i = 0; i < 24; i++ )
  {
    char buffer[ 10 ];
    sprintf( buffer, "%i", sizes[i] );
    sizelist[i] = CORBA::string_dup( buffer );
  }
  m_idComboLayout_FontSize = m_vToolBarLayout->insertCombo( sizelist, 2, true, SIGNAL( activated( const QString & ) ),
							    this, "fontSizeSelected", true,
							    ( wstr = Q2C( i18n( "Font Size"  ) ) ), 50, -1, OpenPartsUI::AtBottom );

  pix = OPUIUtils::convertPixmap( ICON("bold.xpm") );
  m_idButtonLayout_Bold = m_vToolBarLayout->insertButton2( pix, 3, SIGNAL( clicked() ), this, "bold", true, ( wstr = Q2C( i18n( "Bold" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("italic.xpm") );
  m_idButtonLayout_Italic = m_vToolBarLayout->insertButton2( pix, 4, SIGNAL( clicked() ), this, "italic", true, ( wstr = Q2C( i18n( "Italic" ) ) ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("money.xpm") );
  m_idButtonLayout_Money = m_vToolBarLayout->insertButton2( pix, 5, SIGNAL( clicked() ), this, "moneyFormat", true, ( wstr = Q2C( i18n( "Money Format" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("percent.xpm") );
  m_idButtonLayout_Percent = m_vToolBarLayout->insertButton2( pix, 6, SIGNAL( clicked() ), this, "percent", true, ( wstr = Q2C( i18n( "Percent Format" ) ) ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("left.xpm") );
  m_idButtonLayout_Left = m_vToolBarLayout->insertButton2( pix, 7, SIGNAL( clicked() ), this, "alignLeft", true, ( wstr = Q2C( i18n( "Align Left" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("center.xpm") );
  m_idButtonLayout_Center = m_vToolBarLayout->insertButton2( pix, 8, SIGNAL( clicked() ), this, "alignCenter", true, ( wstr = Q2C( i18n( "Align Center" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("right.xpm") );
  m_idButtonLayout_Right = m_vToolBarLayout->insertButton2( pix, 9, SIGNAL( clicked() ), this, "alignRight", true, ( wstr = Q2C( i18n( "Align Right" ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("multirow.xpm") );
  m_idButtonLayout_MultiRows = m_vToolBarLayout->insertButton2( pix, 10, SIGNAL( clicked() ), this, "multiRow", true,
							       ( wstr = Q2C( i18n( "Allow multiple lines" ) ) ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("precminus.xpm") );
  m_idButtonLayout_PrecMinus = m_vToolBarLayout->insertButton2( pix, 11, SIGNAL( clicked() ), this, "precisionMinus", true,
								 ( wstr = Q2C( i18n( "Lower Precision"  ) ) ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("precplus.xpm") );
  m_idButtonLayout_PrecPlus = m_vToolBarLayout->insertButton2( pix, 12, SIGNAL( clicked() ), this, "precisionPlus", true,
							      ( wstr = Q2C( i18n( "Higher Precision" ) ) ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("chart.xpm") );
  m_idButtonLayout_Chart = m_vToolBarLayout->insertButton2( pix, 13, SIGNAL( clicked() ), this, "insertChart", true, ( wstr = Q2C( i18n( "Insert Chart" ) ) ), -1 );

  m_vToolBarLayout->enable( OpenPartsUI::Show );

  m_pluginManager->fillToolBar( _factory );

  /* m_vToolBarLayout->enable( OpenPartsUI::Hide );
  m_vToolBarLayout->setBarPos(OpenPartsUI::Floating);
  m_vToolBarLayout->setBarPos(OpenPartsUI::Top);
  m_vToolBarLayout->enable( OpenPartsUI::Show ); */

  return true;
}

bool KSpreadView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr _menubar )
{
  KStdAccel stdAccel;
  if ( CORBA::is_nil( _menubar ) )
  {
    cerr << "********************** DELETING Menu bar stuff ****************" << endl;

    m_vMenuEdit = 0L;
    m_vMenuEdit_Insert = 0L;
    m_vMenuView = 0L;
    m_vMenuData = 0L;
    m_vMenuFolder = 0L;
    m_vMenuFormat = 0L;
    m_vMenuScripts = 0L;
    m_vMenuHelp = 0L;
    return true;
  }

  CORBA::WString_var wstr;

  // Edit
  _menubar->insertMenu( ( wstr = Q2C( i18n( "&Edit" ) ) ), m_vMenuEdit, -1, -1 );

  OpenPartsUI::Pixmap_var pix;
  pix = OPUIUtils::convertPixmap( ICON("undo.xpm") );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, ( wstr = Q2C( i18n( "Un&do") ) ), this, "undo", stdAccel.undo(), -1, -1 );
  pix = OPUIUtils::convertPixmap( ICON("redo.xpm") );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, ( wstr = Q2C( i18n( "&Redo") ) ), this, "redo", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("editcut.xpm") );
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, ( wstr = Q2C( i18n( "C&ut") ) ), this, "cutSelection", stdAccel.cut(), -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("editcopy.xpm") );
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, ( wstr = Q2C( i18n( "&Copy") ) ), this, "copySelection", stdAccel.copy(), -1, -1 );

  pix = OPUIUtils::convertPixmap( ICON("editpaste.xpm") );
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, ( wstr = Q2C( i18n( "&Paste") ) ), this, "paste", stdAccel.paste(), -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  m_vMenuEdit->insertItem8( ( wstr = Q2C( i18n( "&Insert" ) ) ), m_vMenuEdit_Insert, -1, -1 );
  m_idMenuEdit_Insert_Table = m_vMenuEdit_Insert->insertItem( ( wstr = Q2C( i18n( "&Table" ) ) ), this, "insertTable", 0 );
  m_idMenuEdit_Insert_Image = m_vMenuEdit_Insert->insertItem( ( wstr = Q2C( i18n( "&Image" ) ) ), this, "insertImage", 0 );
  m_idMenuEdit_Insert_Chart = m_vMenuEdit_Insert->insertItem( ( wstr = Q2C( i18n( "&Chart" ) ) ), this, "insertChart", 0 );
  m_idMenuEdit_Insert_Object = m_vMenuEdit_Insert->insertItem( ( wstr = Q2C( i18n( "&Object ..." ) ) ), this, "insertObject", 0 );

  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Cell = m_vMenuEdit->insertItem( ( wstr = Q2C( i18n( "C&ell" ) ) ), this, "editCell", CTRL+Key_E );
	
  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Layout = m_vMenuEdit->insertItem( ( wstr = Q2C( i18n( "Paper &Layout" ) ) ), this, "paperLayoutDlg", 0 );

  // View
  _menubar->insertMenu( ( wstr = Q2C( i18n( "&View" ) ) ), m_vMenuView, -1, -1 );
  m_vMenuView->setCheckable( true );

  m_idMenuView_NewView = m_vMenuView->insertItem( ( wstr = Q2C( i18n( "New View" ) ) ), this, "newView", 0 );

  m_vMenuView->insertSeparator( -1 );

  m_idMenuView_ShowPageBorders = m_vMenuView->insertItem( ( wstr = Q2C( i18n( "Show Page Borders" ) ) ), this, "togglePageBorders", 0 );
  m_vMenuView->setItemChecked( m_idMenuView_ShowPageBorders, m_pTable->isShowPageBorders() );

  // Data
  _menubar->insertMenu( ( wstr = Q2C( i18n( "D&ata" ) ) ), m_vMenuData, -1, -1 );

  m_idMenuData_Consolidate = m_vMenuData->insertItem( ( wstr = Q2C( i18n( "Consolidate" ) ) ), this, "consolidate", 0 );

  // Folder
  _menubar->insertMenu( ( wstr = Q2C( i18n( "F&older" ) ) ), m_vMenuFolder, -1, -1 );

  m_idMenuFolder_NewTable = m_vMenuFolder->insertItem( ( wstr = Q2C( i18n( "New Table" ) ) ), this, "insertNewTable", 0 );

  // Format
  _menubar->insertMenu( ( wstr = Q2C( i18n( "Fo&rmat" ) ) ), m_vMenuFormat, -1, -1 );

  m_idMenuFormat_AutoFill = m_vMenuFormat->insertItem( ( wstr = Q2C( i18n( "&Auto Fill ..." ) ) ), this, "autoFill", 0 );

  // Scripts
  _menubar->insertMenu( ( wstr = Q2C( i18n( "&Scripts" ) ) ), m_vMenuScripts, -1, -1 );

  m_idMenuScripts_EditGlobal = m_vMenuScripts->insertItem( ( wstr = Q2C( i18n( "Edit &global scripts..." ) ) ), this, "editGlobalScripts", 0 );
  m_idMenuScripts_EditLocal = m_vMenuScripts->insertItem( ( wstr = Q2C( i18n( "Edit &local script" ) ) ), this, "editLocalScripts", 0 );
  m_idMenuScripts_Reload = m_vMenuScripts->insertItem( ( wstr = Q2C( i18n( "&Reload scripts" ) ) ), this, "reloadScripts", 0 );
  m_idMenuScripts_Run = m_vMenuScripts->insertItem( ( wstr = Q2C( i18n( "R&un local script" ) ) ), this, "runLocalScript", 0 );

  // Help
  m_vMenuHelp = _menubar->helpMenu();
  if ( CORBA::is_nil( m_vMenuHelp ) )
  {
    _menubar->insertSeparator( -1 );
    _menubar->setHelpMenu( _menubar->insertMenu( ( wstr = Q2C( i18n( "&Help" ) ) ), m_vMenuHelp, -1, -1 ) );
  }

  // m_idMenuHelp_About = m_vMenuHelp->insertItem( ( wstr = Q2C( i18n( "&About" ) ) ), this, "helpAbout", 0 );
  m_idMenuHelp_Using = m_vMenuHelp->insertItem( ( wstr = Q2C( i18n( "&Using KSpread" ) ) ), this, "helpUsing", 0 );
	
  enableUndo( false );
  enableRedo( false );

  return true;
}

/*
void KSpreadView::setMode( OPParts::Part::Mode _mode )
{
  Part_impl::setMode( _mode );

  if ( mode() == OPParts::Part::ChildMode && !m_bFocus )
    m_bShowGUI = false;
  else
    m_bShowGUI = true;
}

void KSpreadView::setFocus( CORBA::Boolean _mode )
{
  Part_impl::setFocus( _mode );

  bool old = m_bShowGUI;

  if ( mode() == OPParts::Part::ChildMode && !m_bFocus )
    m_bShowGUI = false;
  else
    m_bShowGUI = true;

  if ( old != m_bShowGUI )
    resizeEvent( 0L );
}
*/

void KSpreadView::helpUsing()
{
  kapp->invokeHTMLHelp( "kspread/kspread.html", QString::null );
}

QButton * KSpreadView::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0L )
    _parent = this;

  QPixmap *pixmap = 0L;

  pixmap = new QPixmap(ICON(_file) );

  QButton *pb;
  if ( !_kbutton )
    pb = new QPushButton( _parent );
  else
    pb = new KButton( _parent );
  if ( pixmap )
    pb->setPixmap( *pixmap );

  return pb;
}

void KSpreadView::enableUndo( bool _b )
{
  if ( CORBA::is_nil( m_vMenuEdit ) )
    return;

  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, _b );
  m_bUndo = _b;
}

void KSpreadView::enableRedo( bool _b )
{
  if ( CORBA::is_nil( m_vMenuEdit ) )
    return;

  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, _b );
  m_bRedo = _b;
}

void KSpreadView::undo()
{
  m_pDoc->undo();
}

void KSpreadView::redo()
{
  m_pDoc->redo();
}

void KSpreadView::deleteColumn()
{
  if ( !m_pTable )
    return;
  m_pTable->deleteColumn( m_pCanvas->markerColumn() );
}

void KSpreadView::deleteRow()
{
  if ( !m_pTable )
    return;
  m_pTable->deleteRow( m_pCanvas->markerRow() );
}

void KSpreadView::insertColumn()
{
  if ( !m_pTable )
    return;
  m_pTable->insertColumn( m_pCanvas->markerColumn() );
}

void KSpreadView::insertRow()
{
  if ( !m_pTable )
    return;
  m_pTable->insertRow( m_pCanvas->markerRow() );
}

void KSpreadView::fontSelected( const CORBA::WChar *_font )
{
    if ( m_pTable != 0L )
      m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), C2Q( _font ) );
}

void KSpreadView::fontSizeSelected( const CORBA::WChar *_size )
{
  if ( m_pTable != 0L )
      m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, C2Q( _size ).toInt() );
}

void KSpreadView::bold()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, -1, 1 );
}

void KSpreadView::italic()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionFont( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 0L, -1, -1, 1 );
}

void KSpreadView::reloadScripts()
{
  // TODO
}

void KSpreadView::runLocalScript()
{
  // TODO
}

void KSpreadView::editGlobalScripts()
{
  if ( KSpreadView::m_pGlobalScriptsDialog == 0L )
    KSpreadView::m_pGlobalScriptsDialog = new KSpreadScripts();
  KSpreadView::m_pGlobalScriptsDialog->show();
  KSpreadView::m_pGlobalScriptsDialog->raise();
}

void KSpreadView::editLocalScripts()
{
  // TODO
  /* if ( !m_pDoc->editPythonCode() )
  {
    QMessageBox::message( i18n( "KSpread Error" ), i18n( "Could not start editor" ) );
    return;
    } */
}

void KSpreadView::addTable( KSpreadTable *_t )
{
  m_pTabBar->addTab( _t->name() );

  setActiveTable( _t );

  QObject::connect( _t, SIGNAL( sig_updateView( KSpreadTable* ) ), SLOT( slotUpdateView( KSpreadTable* ) ) );
  QObject::connect( _t, SIGNAL( sig_updateView( KSpreadTable *, const QRect& ) ),
		    SLOT( slotUpdateView( KSpreadTable*, const QRect& ) ) );
  QObject::connect( _t, SIGNAL( sig_updateCell( KSpreadTable *, KSpreadCell*, int, int ) ),
		    SLOT( slotUpdateCell( KSpreadTable *, KSpreadCell*, int, int ) ) );
  QObject::connect( _t, SIGNAL( sig_unselect( KSpreadTable *, const QRect& ) ),
		    SLOT( slotUnselect( KSpreadTable *, const QRect& ) ) );
  QObject::connect( _t, SIGNAL( sig_updateHBorder( KSpreadTable * ) ),
		    SLOT( slotUpdateHBorder( KSpreadTable * ) ) );
  QObject::connect( _t, SIGNAL( sig_updateVBorder( KSpreadTable * ) ),
		    SLOT( slotUpdateVBorder( KSpreadTable * ) ) );
  QObject::connect( _t, SIGNAL( sig_changeSelection( KSpreadTable *, const QRect &, const QRect & ) ),
		    SLOT( slotChangeSelection( KSpreadTable *, const QRect &, const QRect & ) ) );
  QObject::connect( _t, SIGNAL( sig_insertChild( KSpreadChild* ) ), SLOT( slotInsertChild( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_updateChildGeometry( KSpreadChild* ) ),
		    SLOT( slotUpdateChildGeometry( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_removeChild( KSpreadChild* ) ), SLOT( slotRemoveChild( KSpreadChild* ) ) );
  QObject::connect( _t, SIGNAL( sig_maxColumn( int ) ), m_pCanvas, SLOT( slotMaxColumn( int ) ) );
  QObject::connect( _t, SIGNAL( sig_maxRow( int ) ), m_pCanvas, SLOT( slotMaxRow( int ) ) );
}

void KSpreadView::removeTable( KSpreadTable *_t )
{
  m_pTabBar->removeTab( _t->name() );

  if ( m_pDoc->map()->firstTable() )
    setActiveTable( m_pDoc->map()->firstTable() );
  else
    m_pTable = 0L;
}

void KSpreadView::removeAllTables()
{
  m_pTabBar->removeAllTabs();

  setActiveTable( 0L );
}

void KSpreadView::setActiveTable( KSpreadTable *_t )
{
  if ( _t == m_pTable )
    return;

  m_pTable = _t;
  if ( m_pTable == 0L )
    return;

  m_pTabBar->setActiveTab( _t->name() );

  // Create views for child documents
  if ( m_bInitialized )
  {
    m_lstFrames.clear();
    QListIterator<KSpreadChild> it = m_pTable->childIterator();
    for( ; it.current(); ++it )
      slotInsertChild( it.current() );
  }

  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
  m_pCanvas->repaint();

  m_pCanvas->slotMaxColumn( m_pTable->maxColumn() );
  m_pCanvas->slotMaxRow( m_pTable->maxRow() );
}

void KSpreadView::slotChangeTable( const char *_name )
{
  KSpreadTable *t;

  for ( t = m_pDoc->map()->firstTable(); t != 0L; t = m_pDoc->map()->nextTable() )
  {
    if ( strcmp( _name, t->name() ) == 0 )
    {
      setActiveTable( t );
      return;
    }
  }

  warning("Unknown table '%s'\n",_name);
}

void KSpreadView::slotScrollToFirstTable()
{
  m_pTabBar->scrollFirst();
}

void KSpreadView::slotScrollToLeftTable()
{
  m_pTabBar->scrollLeft();
}

void KSpreadView::slotScrollToRightTable()
{
  m_pTabBar->scrollRight();
}

void KSpreadView::slotScrollToLastTable()
{
  m_pTabBar->scrollLast();
}

void KSpreadView::insertNewTable()
{
  KSpreadTable *t = m_pDoc->createTable();
  m_pDoc->addTable( t );
}

void KSpreadView::copySelection()
{
  if ( m_pTable )
    m_pTable->copySelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::cutSelection()
{
  if ( m_pTable )
    m_pTable->cutSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::paste()
{
  if ( m_pTable )
    m_pTable->paste( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::consolidate()
{
  KSpreadConsolidate* dlg = new KSpreadConsolidate( this, "Consolidate" );
  dlg->show();
}

void KSpreadView::newView()
{
  assert( (m_pDoc != 0L) );

  KSpreadShell* shell = new KSpreadShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

CORBA::Boolean KSpreadView::printDlg()
{
  // Open some printer dialog
  /* QPrinter prt;
  if ( !KoPrintDia::print( prt, m_pDoc, SLOT( paperLayoutDlg() ) ) )
    return true; */

  QPrinter prt;
  if ( QPrintDialog::getPrinterSetup( &prt ) )
  {
    QPainter painter;
    painter.begin( &prt );
    // Print the table and tell that m_pDoc is NOT embedded.
    m_pTable->print( painter, &prt );
    painter.end();
  }

  return true;
}

#ifdef USE_PICTURES
void KSpreadView::markChildPicture( KSpreadChildPicture *_pic )
{
  KSpreadChildFrame *p = new KSpreadChildFrame( this, (KSpreadChild*)_pic->child() );
  p->setGeometry( _pic->geometry() );
  p->attach( _pic->picture() );
  p->partChangedState( OPParts::Part::Marked );
  p->show();

  m_lstFrames.append( p );

  QObject::connect( p, SIGNAL( sig_geometryEnd( KoFrame* ) ),
		    this, SLOT( slotChildGeometryEnd( KoFrame* ) ) );
  QObject::connect( p, SIGNAL( sig_moveEnd( KoFrame* ) ),
		    this, SLOT( slotChildMoveEnd( KoFrame* ) ) );

  m_lstPictures.removeRef( _pic );
}
#endif

void KSpreadView::insertChart( const QRect& _geometry )
{
  int xpos, ypos;
  xpos = m_pTable->columnPos( m_pCanvas->markerColumn(), m_pCanvas );
  ypos = m_pTable->rowPos( m_pCanvas->markerRow(), m_pCanvas );

  KSpread::EventChartInserted event;
  event.dx = _geometry.left() - xpos;
  event.dy = _geometry.top() - ypos;
  event.width = _geometry.width();
  event.height = _geometry.height();

  EMIT_EVENT( this, KSpread::eventChartInserted, event );
}

void KSpreadView::insertChild( const QRect& _geometry, KoDocumentEntry& _e )
{
  m_pTable->insertChild( _geometry, _e );
}

void KSpreadView::slotRemoveChild( KSpreadChild *_child )
{
  if ( _child->table() != m_pTable )
    return;

  // TODO
}

void KSpreadView::slotInsertChild( KSpreadChild *_child )
{
  if ( _child->table() != m_pTable )
    return;

#ifdef USE_PICTURE
  if ( _child->draw( false ) )
  {
    cout << "!!!!!!!! DOING IT WITH A PICTURE !!!!!!!!!" << endl;

    KSpreadChildPicture *p = new KSpreadChildPicture( this, _child );
    m_lstPictures.append( p );
    update();
    return;
  }
#endif

  KSpreadChildFrame *p = new KSpreadChildFrame( this, _child );
  p->setGeometry( _child->geometry() );
  m_lstFrames.append( p );

  OpenParts::View_var v = _child->createView( m_vKoMainWindow );
  if ( !CORBA::is_nil( v ) )
  {
    KOffice::View_var kv = KOffice::View::_narrow( v );
    kv->setMode( KOffice::View::ChildMode );
    assert( !CORBA::is_nil( kv ) );
    p->attachView( kv );

    KOffice::View::EventNewPart event;
    event.view = KOffice::View::_duplicate( kv );
    cerr << "------------------ newPart -----------" << endl;
    EMIT_EVENT( this, KOffice::View::eventNewPart, event );
  }

  QObject::connect( p, SIGNAL( sig_geometryEnd( KoFrame* ) ),
		    this, SLOT( slotChildGeometryEnd( KoFrame* ) ) );
  QObject::connect( p, SIGNAL( sig_moveEnd( KoFrame* ) ),
		    this, SLOT( slotChildMoveEnd( KoFrame* ) ) );

  p->show();
}

void KSpreadView::slotChildGeometryEnd( KoFrame* _frame )
{
  // ATTENTION: This is an upcast
  KSpreadChildFrame *f = (KSpreadChildFrame*)_frame;
  // TODO zooming
  m_pTable->changeChildGeometry( f->child(), _frame->partGeometry() );
}

void KSpreadView::slotChildMoveEnd( KoFrame* _frame )
{
  // ATTENTION: This is an upcast
  KSpreadChildFrame *f = (KSpreadChildFrame*)_frame;
  // TODO zooming
  m_pTable->changeChildGeometry( f->child(), _frame->partGeometry() );
}

void KSpreadView::slotUpdateChildGeometry( KSpreadChild *_child )
{
  if ( _child->table() != m_pTable )
    return;

  // Find frame for child
  KSpreadChildFrame *f = 0L;
  QListIterator<KSpreadChildFrame> it( m_lstFrames );
  for ( ; it.current() && !f; ++it )
    if ( it.current()->child() == _child )
      f = it.current();

  assert( f != 0L );

  // Are we already up to date ?
  if ( _child->geometry() == f->partGeometry() )
    return;

  // TODO zooming
  f->setPartGeometry( _child->geometry() );
}

void KSpreadView::togglePageBorders()
{
   if ( !m_pTable )
       return;

   m_vMenuView->setItemChecked( m_idMenuView_ShowPageBorders, !m_pTable->isShowPageBorders() );
   m_pTable->setShowPageBorders( !m_pTable->isShowPageBorders() );
}

void KSpreadView::editCell()
{
  m_pEditWidget->setFocus();
}

void KSpreadView::keyPressEvent ( QKeyEvent* _ev )
{
  // Dont eat accelerators
  if ( _ev->state() & ( Qt::AltButton | Qt::ControlButton ) )
    QWidget::keyPressEvent( _ev );
  else
    QApplication::sendEvent( m_pCanvas, _ev );
}

void KSpreadView::setFocus( CORBA::Boolean mode )
{
  CORBA::Boolean old = m_bFocus;

  KoViewIf::setFocus( mode );

  if ( old == m_bFocus )
    return;

  if ( KoViewIf::mode() != KOffice::View::RootMode )
    resizeEvent( 0L );
}

CORBA::ULong KSpreadView::leftGUISize()
{
  return YBORDER_WIDTH;
}

CORBA::ULong KSpreadView::rightGUISize()
{
  return 20;
}

CORBA::ULong KSpreadView::topGUISize()
{
  return 30 + XBORDER_HEIGHT;
}

CORBA::ULong KSpreadView::bottomGUISize()
{
  return 20;
}

void KSpreadView::resizeEvent( QResizeEvent * )
{
  // HACK
  if ( x() == 5000 && y() == 5000 )
    return;

  if ( KoViewIf::hasFocus() || mode() == KOffice::View::RootMode )
  {
    m_pToolWidget->show();
    m_pToolWidget->setGeometry( 0, 0, width(), 30 );
    int top = 30;
    m_pPosWidget->setGeometry( 2,2,50,26 );
    m_pCancelButton->setGeometry( 60, 2, 26, 26 );
    m_pOkButton->setGeometry( 90, 2, 26, 26 );
    m_pEditWidget->setGeometry( 125, 2, 200, 26 );
	
    m_pTabBarFirst->setGeometry( 0, height() - 20, 20, 20 );
    m_pTabBarFirst->show();
    m_pTabBarLeft->setGeometry( 20, height() - 20, 20, 20 );
    m_pTabBarLeft->show();
    m_pTabBarRight->setGeometry( 40, height() - 20, 20, 20 );
    m_pTabBarRight->show();
    m_pTabBarLast->setGeometry( 60, height() - 20, 20, 20 );
    m_pTabBarLast->show();
    m_pTabBar->setGeometry( 80, height() - 20, width() / 2 - 80, 20 );
    m_pTabBar->show();
	
    m_pHorzScrollBar->setGeometry( width() / 2, height() - 20, width() / 2 - 20, 20 );
    m_pHorzScrollBar->show();
    m_pVertScrollBar->setGeometry( width() - 20, top , 20, height() - 20 - top );
    m_pVertScrollBar->show();

    m_pFrame->setGeometry( 0, top, width() - 20, height() - 20 - top );
    m_pFrame->show();

    m_pCanvas->setGeometry( YBORDER_WIDTH, XBORDER_HEIGHT,
								  m_pFrame->width() - YBORDER_WIDTH, m_pFrame->height() - XBORDER_HEIGHT );
   
    m_pHBorderWidget->setGeometry( YBORDER_WIDTH, 0, m_pFrame->width() - YBORDER_WIDTH, XBORDER_HEIGHT );
    m_pHBorderWidget->show();

    m_pVBorderWidget->setGeometry( 0, XBORDER_HEIGHT, YBORDER_WIDTH,
								   m_pFrame->height() - XBORDER_HEIGHT );
    m_pVBorderWidget->show();
  }
  else
  {
    m_pToolWidget->hide();
    m_pToolWidget->hide();
    m_pTabBarFirst->hide();
    m_pTabBarLeft->hide();
    m_pTabBarRight->hide();
    m_pTabBarLast->hide();
    m_pHorzScrollBar->hide();
    m_pVertScrollBar->hide();
    m_pHBorderWidget->hide();
    m_pVBorderWidget->hide();

    m_pFrame->setGeometry( 0, 0, width(), height() );
    m_pFrame->show();
    m_pCanvas->raise();
    m_pCanvas->setGeometry( 0, 0, width(), height() );
  }
}

/* void KSpreadView::slotSave()
{
      if ( xclPart == 0L )
	return;

    if ( xclPart->getURL() == 0L )
    {
	slotSaveAs();
	return;
    }

    xclPart->save( xclPart->getURL() );
}

void KSpreadView::slotSaveAs()
{
   if ( xclPart == 0L )
	return;

    QString h = getenv( "HOME" );

    if ( saveDlg == 0L )
    {
	saveDlg = new KFileSelect( 0L, "save", h.data(), xclPart->getURL(), "Kxcl: Save As" );
	connect( saveDlg, SIGNAL( fileSelected( const char* ) ),
		 this, SLOT( slotSaveFileSelected( const char* ) ) );
	saveDlg->show();
    }
    else
	saveDlg->show();
}

void KSpreadView::slotSaveFileSelected( const char *_file )
{
   QFileInfo info;

    info.setFile( _file );

    if( info.exists() )
    {
	if( !( QMessageBox::query("Warning:", "A Document with this Name exists already\n",
				  "Do you want to overwrite it ?") ) )
	    return;
    }

    if ( xclPart )
	xclPart->save( _file );
}

void KSpreadView::slotOpen()
{
   if ( xclPart == 0L )
	return;

    QString h = getenv( "HOME" );

    if ( openDlg == 0L )
    {
	openDlg = new KFileSelect( 0L, "open", h.data(), 0L, "Kxcl: Open" );
	connect( openDlg, SIGNAL( fileSelected( const char* ) ),
		 this, SLOT( slotOpenFileSelected( const char* ) ) );
	openDlg->show();
    }
    else
	openDlg->show();
}

void KSpreadView::slotOpenFileSelected( const char *_file )
{
   if ( xclPart->hasDocumentChanged() )
	if ( !QMessageBox::query( "Kxcl Warning", "The current document has been modified.\n\rDo you really want to close it?\n\rAll Changes will be lost!", "Yes", "No" ) )
	    return;

    xclPart->load( _file );
}

void KSpreadView::slotQuit()
{
   KPartShell* shell = xclPart->shell();
    if ( shell->closeShells() )
	exit(1);
}

void KSpreadView::slotClose()
{
   if ( xclPart->closePart() )
    {
	KPartShell *shell = xclPart->shell();
	delete shell;
    }
}

void KSpreadView::slotNewWindow()
{
   KPartShell *shell = new KPartShell();
    shell->show();

    printf("Creating part xcl\n");

    KPart *part = shell->newPart( "xcl" );

    printf("Got it\n");

    shell->setActiveDocument( part );
}
*/

/*
void KSpreadView::print()
{
  // Open some printer dialog
  KSpreadPrintDlg* dlg = new KSpreadPrintDlg( this );

  if ( !dlg->exec() )
  {
    delete dlg;
    return;
  }

  QPrinter prt;
  dlg->configurePrinter( prt );

  QPainter painter;
  painter.begin( &prt );
  // Print the table and tell that m_pDoc is NOT embedded.
  m_pTable->print( painter, FALSE, &prt );
  painter.end();
}
*/

void KSpreadView::openPopupMenu( const QPoint & _point )
{
    assert( m_pTable );

    if ( m_pPopupMenu != 0L )
	delete m_pPopupMenu;

    m_pPopupMenu = new QPopupMenu();

    m_pPopupMenu->insertItem( "Copy", this, SLOT( slotCopy() ) );
    m_pPopupMenu->insertItem( "Cut", this, SLOT( slotCut() ) );
    m_pPopupMenu->insertItem( "Paste", this, SLOT( slotPaste() ) );
    m_pPopupMenu->insertItem( "Delete", this, SLOT( slotDelete() ) );
    m_pPopupMenu->insertSeparator();
    m_pPopupMenu->insertItem( "Layout", this, SLOT( slotLayoutDlg() ) );

    m_lstTools.clear();
    m_lstTools.setAutoDelete( true );
    KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
    if ( !cell->isFormular() && !cell->isValue() )
    {
      m_popupMenuFirstToolId = 10;
      int i = 0;
      QList<KoToolEntry> tools = KoToolEntry::findTools( "text/x-single-word" );
      if( tools.count() > 0 )
      {
	m_pPopupMenu->insertSeparator();
	KoToolEntry* entry;
	for( entry = tools.first(); entry != 0L; entry = tools.next() )
        {
	  QStringList lst = entry->commandsI18N();
	  QStringList::ConstIterator it = lst.begin();
	  
	  for (; it != lst.end(); ++it )
	    m_pPopupMenu->insertItem( *it, m_popupMenuFirstToolId + i++ );
	  
	  lst = entry->commands();
          it = lst.begin();
	  for (; it != lst.end(); ++it )
	  {
	    ToolEntry *t = new ToolEntry;
	    t->command = *it;
	    t->entry = entry;
	    m_lstTools.append( t );
	  }
	}
	
	QObject::connect( m_pPopupMenu, SIGNAL( activated( int ) ), this, SLOT( slotActivateTool( int ) ) );
      }
    }

    m_pPopupMenu->popup( _point );
}

void KSpreadView::slotActivateTool( int _id )
{
  assert( m_pTable );

  if( _id < m_popupMenuFirstToolId )
    return;

  ToolEntry* entry = m_lstTools.at( _id - m_popupMenuFirstToolId );

//  CORBA::Object_var obj = imr_activate( entry->entry->name(), "IDL:DataTools/Tool:1.0" );
  CORBA::Object_var obj = entry->entry->ref();
  if ( CORBA::is_nil( obj ) )
    // TODO: error message
    return;

  DataTools::Tool_var tool = DataTools::Tool::_narrow( obj );
  if ( CORBA::is_nil( tool ) )
    // TODO: error message
    return;

  KSpreadCell *cell = m_pTable->cellAt( m_pCanvas->markerColumn(), m_pCanvas->markerRow() );
  if ( !cell->isFormular() && !cell->isValue() )
  {
    QString text = cell->text();
    CORBA::Any value;
    value <<= CORBA::Any::from_string( (char*)text.data(), 0 );
    CORBA::Any anyid;
    KSpread::DataToolsId id;
    id.time = (CORBA::ULong)time( 0L );
    id.row = m_pCanvas->markerRow();
    id.column = m_pCanvas->markerColumn();
    anyid <<= id;
    tool->run( entry->command, this, value, anyid );
    return;
  }
}

void KSpreadView::slotCopy()
{
  assert( m_pTable );

  m_pTable->copySelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::slotCut()
{
  assert( m_pTable );

  m_pTable->cutSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::slotPaste()
{
  assert( m_pTable );

  m_pTable->paste( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::slotDelete()
{
  assert( m_pTable );

  m_pTable->deleteSelection( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::slotLayoutDlg()
{
  QRect selection( m_pTable->selectionRect() );

  m_pCanvas->hideMarker();

  cout << "#######################################" << endl;

  if ( selection.contains( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) ) )
    CellLayoutDlg dlg( this, m_pTable, selection.left(), selection.top(),
		       selection.right(), selection.bottom() );
  else
    CellLayoutDlg dlg( this, m_pTable, m_pCanvas->markerColumn(), m_pCanvas->markerRow(), m_pCanvas->markerColumn(), m_pCanvas->markerRow() );

  m_pDoc->setModified( true );

  cout << "------------------------------------------" << endl;

  m_pCanvas->showMarker();
}

void KSpreadView::paperLayoutDlg()
{
  m_pDoc->paperLayoutDlg();
}

void KSpreadView::multiRow()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionMultiRow( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::alignLeft()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Left );
}

void KSpreadView::alignRight()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Right );
}

void KSpreadView::alignCenter()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionAlign( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), KSpreadLayout::Center );
}

void KSpreadView::moneyFormat()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionMoneyFormat( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::precisionPlus()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPrecision( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), 1 );
}

void KSpreadView::precisionMinus()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPrecision( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ), -1 );
}

void KSpreadView::percent()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPercent( QPoint( m_pCanvas->markerColumn(), m_pCanvas->markerRow() ) );
}

void KSpreadView::insertTable()
{
  QValueList<KoDocumentEntry> vec = KoDocumentEntry::query( "'IDL:KSpread/DocumentFactory:1.0#KSpread' in RepoIds", 1 );
  if ( vec.isEmpty() )
  {
    cout << "Got no results" << endl;
    QMessageBox::critical( 0L, i18n("Error"), i18n("Sorry, no spread sheet  component registered"),
			   i18n("Ok") );
    return;
  }

  cerr << "USING component " << vec[0].name << endl;

  m_pCanvas->setAction( KSpreadCanvas::InsertChild, vec[0] );
}

void KSpreadView::insertImage()
{
  // m_pCanvas->setAction( KSpreadCanvas::InsertChild, "KImage" );
}

void KSpreadView::insertObject()
{
  KoDocumentEntry e = KoPartSelectDia::selectPart();
  if ( e.isEmpty() )
    return;

  m_pCanvas->setAction( KSpreadCanvas::InsertChild, e );
}

void KSpreadView::insertChart()
{
  m_pCanvas->setAction( KSpreadCanvas::InsertChart );
}

void KSpreadView::autoFill()
{
  // TODO
  /* if( m_pTable )
  {
    if( !m_pAutoFillDialog )
      m_pAutoFillDialog = new AutoFillDialog;

    m_pAutoFillDialog->setTable( m_pTable );
    m_pAutoFillDialog->show();
  } */
}

/*
void KSpreadView::zoomMinus()
{
  if ( m_fZoom <= 0.25 )
    return;

  m_fZoom -= 0.25;

  if ( m_pTable != 0L )
    m_pTable->setLayoutDirtyFlag();

  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}

void KSpreadView::zoomPlus()
{
  if ( m_fZoom >= 3 )
    return;

  m_fZoom += 0.25;

  if ( m_pTable != 0L )
    m_pTable->setLayoutDirtyFlag();

  m_pCanvas->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}
*/

void KSpreadView::setText( const QString& _text )
{
  if ( m_pTable == 0L )
    return;

  // Take the shortcut if nobody is listening!
  if ( m_lstFilters.isEmpty() )
  {
    m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), _text );
    return;
  }

  KSpread::EventSetText event;
  event.text = CORBA::string_dup( _text );
  EMIT_EVENT( this, KSpread::eventSetText, event );

  // m_pTable->setText( m_pCanvas->markerRow(), m_pCanvas->markerColumn(), _text );
}

//------------------------------------------------
//
// Document signals
//
//------------------------------------------------

void KSpreadView::slotAddTable( KSpreadTable *_table )
{
  addTable( _table );
}

void KSpreadView::slotUpdateView( KSpreadTable *_table )
{
  printf("void KSpreadView::slotUdateView( KSpreadTable *_table )\n");

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  m_pCanvas->update();
}

void KSpreadView::slotUpdateView( KSpreadTable *_table, const QRect& _rect )
{
  printf("void KSpreadView::slotUpdateView( KSpreadTable *_table, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  /* QPainter painter;
     painter.begin( m_pCanvas ); */

  m_pCanvas->updateCellRect( _rect );
  /* int x,y;
  for( y = _rect.top(); y <= _rect.bottom(); y++ )
    for( x = _rect.left(); x <= _rect.right(); x++ )
    drawCell( painter, m_pTable->cellAt( x, y ), x, y ); */

  // painter.end();
}

void KSpreadView::slotUpdateHBorder( KSpreadTable *_table )
{
  printf("void KSpreadView::slotUpdateHBorder( KSpreadTable *_table )\n");

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  m_pHBorderWidget->update();
}

void KSpreadView::slotUpdateVBorder( KSpreadTable *_table )
{
  printf("void KSpreadView::slotUpdateVBorder( KSpreadTable *_table )\n");

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  m_pVBorderWidget->update();
}

void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old, const QRect &_new )
{
  // printf("void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old %i %i|%i %i, const QRect &_new %i %i|%i %i )\n",_old.left(),_old.top(),_old.right(),_old.bottom(),_new.left(),_new.top(),_new.right(),_new.bottom());

  // Emit a signal for internal use
  emit sig_selectionChanged( _table, _new );

  // Send some event around
  KSpread::View::EventSelectionChanged ev;
  ev.range.top = _new.top();
  ev.range.left = _new.left();
  ev.range.right = _new.right();
  ev.range.bottom = _new.bottom();
  ev.range.table = CORBA::string_dup( activeTable()->name() );
  EMIT_EVENT( this, KSpread::View::eventSelectionChanged, ev );

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  QRect uni( _old );
  uni = uni.unite( _new );

  m_pCanvas->updateCellRect( uni );

  if ( _old.right() == 0x7fff || _new.right() == 0x7fff )
    m_pVBorderWidget->update();
  else if ( _old.bottom() == 0x7fff || _new.bottom() == 0x7fff )
    m_pHBorderWidget->update();
}

void KSpreadView::slotUpdateCell( KSpreadTable *_table, KSpreadCell *_cell, int _col, int _row )
{
  printf("void KSpreadView::slotUpdateCell( KSpreadTable *_table, KSpreadCell *_cell, _col=%i, _row=%i )\n",_col,_row);

  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  m_pCanvas->drawCell( _cell, _col, _row );

  if ( _col == m_pCanvas->markerColumn() && _row == m_pCanvas->markerRow() )
    editWidget()->setText( _cell->text() );
}

void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect& _old )
{
  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  printf("void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect &_old %i %i|%i %i\n",_old.left(),_old.top(),_old.right(),_old.bottom());

  m_pCanvas->hideMarker();

  int ypos;
  int xpos;
  int left_col = m_pTable->leftColumn( 0, xpos, m_pCanvas );
  int right_col = m_pTable->rightColumn( m_pCanvas->width(), m_pCanvas );
  int top_row = m_pTable->topRow( 0, ypos, m_pCanvas );
  int bottom_row = m_pTable->bottomRow( m_pCanvas->height(), m_pCanvas );

  for ( int x = left_col; x <= right_col; x++ )
    for ( int y = top_row; y <= bottom_row; y++ )
    {
      KSpreadCell *cell = m_pTable->cellAt( x, y );
      m_pCanvas->drawCell( cell, x, y );
    }

  // Are complete columns selected ?
  if ( _old.bottom() == 0x7FFF )
    m_pHBorderWidget->update();	
  // Are complete rows selected ?
  else if ( _old.right() == 0x7FFF )
    m_pVBorderWidget->update();

  m_pCanvas->showMarker();
}


/**********************************************************
 *
 * KSpreadChildFrame
 *
 **********************************************************/

KSpreadChildFrame::KSpreadChildFrame( KSpreadView* _view, KSpreadChild* _child ) :
  KoFrame( _view->canvasWidget() )
{
  m_pView = _view;
  m_pChild = _child;
}

/**********************************************************
 *
 * KSpreadChildPicture
 *
 **********************************************************/

#ifdef USE_PICTURE
KSpreadChildPicture::KSpreadChildPicture( KSpreadView* _view, KSpreadChild* _child )
  : KoDocumentChildPicture( _child )
{
  m_pView = _view;
}

KSpreadChildPicture::~KSpreadChildPicture()
{
}
#endif

#include "kspread_view.moc"

