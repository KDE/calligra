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
#include <klocale.h>
#include <kapp.h>
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
#include <koIMR.h>

#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_dlg_scripts.h"
#include "kspread_doc.h"
#include "kspread_shell.h"
#include "kspread_dlg_cons.h"

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
  m_pluginManager.setView( this );
  
  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus ); 

  m_lstFrames.setAutoDelete( true );  
#ifdef USE_PICTURE
  m_lstPictures.setAutoDelete( true );
#endif

  m_pDoc = _doc;

  m_iXOffset = 0;
  m_iYOffset = 0;

  m_bUndo = false;
  m_bRedo = false;
    
  m_pTable = 0L;
  m_fZoom = 1.0;

  m_iMarkerColumn = 1;
  m_iMarkerRow = 1;
  m_iMarkerVisible = 1;

  m_pPopupMenu = 0L;

  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );

  // Vert. Scroll Bar
  m_pVertScrollBar = new QScrollBar( this, "ScrollBar_2" );
  QObject::connect( m_pVertScrollBar, SIGNAL( valueChanged(int) ), this, SLOT( slotScrollVert(int) ) );
  m_pVertScrollBar->setRange( 0, 4096 );
  m_pVertScrollBar->setOrientation( QScrollBar::Vertical );

  // Horz. Scroll Bar
  m_pHorzScrollBar = new QScrollBar( this, "ScrollBar_1" );
  QObject::connect( m_pHorzScrollBar, SIGNAL( valueChanged(int) ), this, SLOT( slotScrollHorz(int) ) );
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
  m_pCanvasWidget = new KSpreadCanvas( m_pFrame, this );
  m_pCanvasWidget->setFocusPolicy( QWidget::StrongFocus );
  setFocusProxy( m_pCanvasWidget );
  
  m_pHBorderWidget = new KSpreadHBorder( m_pFrame, this );
  m_pVBorderWidget = new KSpreadVBorder( m_pFrame, this );
  
  KSpreadTable *tbl;
  for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
    addTable( tbl );

  QObject::connect( m_pDoc, SIGNAL( sig_addTable( KSpreadTable* ) ), SLOT( slotAddTable( KSpreadTable* ) ) );
}

void KSpreadView::init()
{
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

  m_pluginManager.cleanUp();
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

  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_var,
	   mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_var,
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

  int xpos, ypos;
  xpos = m_pTable->columnPos( markerColumn(), this );
  ypos = m_pTable->rowPos( markerRow(), this );
  
  vector<KoDocumentEntry> vec = koQueryDocuments( "'IDL:Chart/SimpleChart:1.0' in RepoID", 1 );
  if ( vec.size() == 0 )
  {    
    cout << "Got no results" << endl;
    QMessageBox::critical( 0L, i18n("Error"), i18n("Sorry, no charting component registered"), i18n("Ok") );
    return true;
  }

  QRect r( xpos + _event.dx, ypos + _event.dy, _event.width, _event.height );
  
  cerr << "USING component " << vec[0].name << endl;
  
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
	hideMarker();

	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_iMarkerColumn, m_iMarkerRow, m_iMarkerColumn, m_iMarkerRow );

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	    setMarkerRow( m_iMarkerRow + 1 + cell->extraYCells() );
	else
	    setMarkerRow( m_iMarkerRow + 1 );

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	    setMarkerRow( cell->obscuringCellsRow() );
	    setMarkerColumn( cell->obscuringCellsColumn() );
	}

	y = m_pTable->rowPos( m_iMarkerRow, this );
	rl = m_pTable->rowLayout( m_iMarkerRow );
	if ( y + rl->height( this ) > height() )
	  vertScrollBar()->setValue( yOffset() + ( y + rl->height( this ) - height() ) );

	// If we have been at the top of the selection ...
	if ( selection.top() == m_iMarkerRow - 1 && selection.top() != selection.bottom() && make_select )
	    selection.setTop( m_iMarkerRow );
	else if ( make_select )
	    selection.setBottom( m_iMarkerRow );

	if ( make_select )
	  m_pTable->setSelection( selection, this );
	
	showMarker();

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Up:	
	hideMarker();

	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_iMarkerColumn, m_iMarkerRow, m_iMarkerColumn, m_iMarkerRow );

	setMarkerRow( m_iMarkerRow - 1 );
	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	    setMarkerRow( cell->obscuringCellsRow() );
	    setMarkerColumn( cell->obscuringCellsColumn() );
	}
	
	y = m_pTable->rowPos( m_iMarkerRow, this );
	rl = m_pTable->rowLayout( m_iMarkerRow );
	if ( y < 0 )
	    vertScrollBar()->setValue( yOffset() + y );

	// If we have been at the top of the selection ...
	if ( selection.top() == m_iMarkerRow + 1 && make_select )
	    selection.setTop( m_iMarkerRow );
	else if ( make_select )
	    selection.setBottom( m_iMarkerRow );

	if ( make_select )
	  m_pTable->setSelection( selection, this );

	showMarker();

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Left:
	hideMarker();
	
	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_iMarkerColumn, m_iMarkerRow, m_iMarkerColumn, m_iMarkerRow );

	setMarkerColumn( m_iMarkerColumn - 1 );
	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	    setMarkerRow( cell->obscuringCellsRow() );
	    setMarkerColumn( cell->obscuringCellsColumn() );
	}

	x = m_pTable->columnPos( m_iMarkerColumn, this );
	cl = m_pTable->columnLayout( m_iMarkerColumn );
	if ( x < 0 )
	  horzScrollBar()->setValue( xOffset() + x );

	// If we have been at the left side of the selection ...
	if ( selection.left() == m_iMarkerColumn + 1 && make_select )
	    selection.setLeft( m_iMarkerColumn );
	else if ( make_select )
	    selection.setRight( m_iMarkerColumn );

	if ( make_select )
	  m_pTable->setSelection( selection, this );

	showMarker();

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Right:
	hideMarker();

	if ( selection.left() == 0 && make_select )
	    selection.setCoords( m_iMarkerColumn, m_iMarkerRow, m_iMarkerColumn, m_iMarkerRow );

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	    setMarkerColumn( m_iMarkerColumn + 1 + cell->extraXCells() );
	else
	    setMarkerColumn( m_iMarkerColumn + 1 );

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	// Go to the upper left corner of the obscuring object ( if there is one )
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  setMarkerRow( cell->obscuringCellsRow() );
	  setMarkerColumn( cell->obscuringCellsColumn() );
	}

	x = m_pTable->columnPos( m_iMarkerColumn, this );
	cl = m_pTable->columnLayout( m_iMarkerColumn );
	if ( x + cl->width( this ) > width() )
	    horzScrollBar()->setValue( xOffset() + ( x + cl->width( this ) - width() ) );

	// If we have been at the right side of the selection ...
	if ( selection.right() == m_iMarkerColumn - 1 && make_select )
	    selection.setRight( m_iMarkerColumn );
	else if ( make_select )
	    selection.setLeft( m_iMarkerColumn );

	if ( make_select )
	  m_pTable->setSelection( selection, this );

	showMarker();

	cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
	if ( cell->text() != 0L )
	    editWidget()->setText( cell->text() );
	else
	    editWidget()->setText( "" );

	break;

    case Key_Escape:
	if ( m_pCanvasWidget->editDirtyFlag() )
	{
	  cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
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
  
  m_pTable->setText( m_iMarkerRow, m_iMarkerColumn, _event.text.in() );

  return true;
}

bool KSpreadView::mappingToolDone( DataTools::Answer& _answer )
{
  CORBA::String_var str;
  _answer.value >>= CORBA::Any::to_string( str, 0 );
  KSpread::DataToolsId id;
  _answer.id >>= id;
  
  cerr << "CORRECTED ´" << str.in() << "´" << endl;
  cerr << "r=" << id.row << " c=" << id.column << endl;
  
  // TODO: check time
  m_pTable->setText( m_iMarkerRow, m_iMarkerColumn, str.in() );

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

    m_pluginManager.fillToolBar( _factory );

    cerr << "niled" << endl;
    return true;
  }

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarEdit->setFullWidth(false);
  
  OpenPartsUI::Pixmap_var pix;
  pix = OPUIUtils::convertPixmap( ICON("editcopy.xpm") );
  m_idButtonEdit_Copy = m_vToolBarEdit->insertButton2( pix, 1, SIGNAL( clicked() ), this, "copySelection", true, i18n( "Copy" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("editcut.xpm") );
  m_idButtonEdit_Cut = m_vToolBarEdit->insertButton2( pix, 2, SIGNAL( clicked() ), this, "cutSelection", true, i18n( "Cut" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("editpaste.xpm") );
  m_idButtonEdit_Paste = m_vToolBarEdit->insertButton2( pix , 3, SIGNAL( clicked() ), this, "paste", true, i18n( "Paste" ), -1 );

  m_vToolBarEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("rowout.xpm") );
  m_idButtonEdit_DelRow = m_vToolBarEdit->insertButton2( pix, 4, SIGNAL( clicked() ), this, "deleteRow", true, i18n( "Delete Row" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("colout.xpm") );
  m_idButtonEdit_DelCol = m_vToolBarEdit->insertButton2( pix, 5, SIGNAL( clicked() ), this, "deleteColumn", true, i18n( "Delete Column"), -1 );

  pix = OPUIUtils::convertPixmap( ICON("rowin.xpm") );
  m_idButtonEdit_InsRow = m_vToolBarEdit->insertButton2( pix, 6, SIGNAL( clicked() ), this, "insertRow", true, i18n( "Insert Row"  ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("colin.xpm") );
  m_idButtonEdit_InsCol = m_vToolBarEdit->insertButton2( pix, 7, SIGNAL( clicked() ), this, "insertColumn", true, i18n( "Insert Column"  ), -1 );

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

  m_idComboLayout_Font = m_vToolBarLayout->insertCombo( fonts, 1, false, SIGNAL( activated( const char* ) ), this,
							"fontSelected", true, i18n("Font"),
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
  m_idComboLayout_FontSize = m_vToolBarLayout->insertCombo( sizelist, 2, true, SIGNAL( activated( const char* ) ),
							    this, "fontSizeSelected", true,
							    i18n( "Font Size"  ), 50, -1, OpenPartsUI::AtBottom );

  pix = OPUIUtils::convertPixmap( ICON("bold.xpm") );
  m_idButtonLayout_Bold = m_vToolBarLayout->insertButton2( pix, 3, SIGNAL( clicked() ), this, "bold", true, i18n( "Bold" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("italic.xpm") );
  m_idButtonLayout_Italic = m_vToolBarLayout->insertButton2( pix, 4, SIGNAL( clicked() ), this, "italic", true, i18n( "Italic" ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("money.xpm") );
  m_idButtonLayout_Money = m_vToolBarLayout->insertButton2( pix, 5, SIGNAL( clicked() ), this, "moneyFormat", true, i18n( "Money Format" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("percent.xpm") );
  m_idButtonLayout_Percent = m_vToolBarLayout->insertButton2( pix, 6, SIGNAL( clicked() ), this, "percent", true, i18n( "Percent Format" ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("left.xpm") );
  m_idButtonLayout_Left = m_vToolBarLayout->insertButton2( pix, 7, SIGNAL( clicked() ), this, "alignLeft", true, i18n( "Align Left" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("center.xpm") );
  m_idButtonLayout_Center = m_vToolBarLayout->insertButton2( pix, 8, SIGNAL( clicked() ), this, "alignCenter", true, i18n( "Align Center" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("right.xpm") );
  m_idButtonLayout_Right = m_vToolBarLayout->insertButton2( pix, 9, SIGNAL( clicked() ), this, "alignRight", true, i18n( "Align Right" ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("multirow.xpm") );
  m_idButtonLayout_MultiRows = m_vToolBarLayout->insertButton2( pix, 10, SIGNAL( clicked() ), this, "multiRow", true,
							       i18n( "Allow multiple lines" ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("precminus.xpm") );
  m_idButtonLayout_PrecMinus = m_vToolBarLayout->insertButton2( pix, 11, SIGNAL( clicked() ), this, "precisionMinus", true,
								 i18n( "Lower Precision"  ), -1 );

  pix = OPUIUtils::convertPixmap( ICON("precplus.xpm") );
  m_idButtonLayout_PrecPlus = m_vToolBarLayout->insertButton2( pix, 12, SIGNAL( clicked() ), this, "precisionPlus", true,
							      i18n( "Higher Precision" ), -1 );

  m_vToolBarLayout->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("chart.xpm") );
  m_idButtonLayout_Chart = m_vToolBarLayout->insertButton2( pix, 13, SIGNAL( clicked() ), this, "insertChart", true, i18n( "Insert Chart" ), -1 );

  m_vToolBarLayout->enable( OpenPartsUI::Show );

  m_pluginManager.fillToolBar( _factory );

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

  // Edit  
  _menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  OpenPartsUI::Pixmap_var pix;
  pix = OPUIUtils::convertPixmap( ICON("undo.xpm") );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem6( pix, i18n("Un&do"), this, "undo", stdAccel.undo(), -1, -1 );
  pix = OPUIUtils::convertPixmap( ICON("redo.xpm") );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem6( pix, i18n("&Redo"), this, "redo", 0, -1, -1 );

  m_vMenuEdit->insertSeparator( -1 );

  pix = OPUIUtils::convertPixmap( ICON("editcut.xpm") );
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem6( pix, i18n("C&ut"), this, "cutSelection", stdAccel.cut(), -1, -1 );
  
  pix = OPUIUtils::convertPixmap( ICON("editcopy.xpm") );
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem6( pix, i18n("&Copy"), this, "copySelection", stdAccel.copy(), -1, -1 );
  
  pix = OPUIUtils::convertPixmap( ICON("editpaste.xpm") );
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem6( pix, i18n("&Paste"), this, "paste", stdAccel.paste(), -1, -1 );
    
  m_vMenuEdit->insertSeparator( -1 );

  m_vMenuEdit->insertItem8( i18n( "&Insert" ), m_vMenuEdit_Insert, -1, -1 );
  m_idMenuEdit_Insert_Table = m_vMenuEdit_Insert->insertItem( i18n( "&Table" ), this, "insertTable", 0 );
  m_idMenuEdit_Insert_Image = m_vMenuEdit_Insert->insertItem( i18n( "&Image" ), this, "insertImage", 0 );
  m_idMenuEdit_Insert_Chart = m_vMenuEdit_Insert->insertItem( i18n( "&Chart" ), this, "insertChart", 0 );
  m_idMenuEdit_Insert_Object = m_vMenuEdit_Insert->insertItem( i18n( "&Object ..." ), this, "insertObject", 0 );
    
  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Cell = m_vMenuEdit->insertItem( i18n( "C&ell" ), this, "editCell", 0 );
	
  m_vMenuEdit->insertSeparator( -1 );

  m_idMenuEdit_Layout = m_vMenuEdit->insertItem( i18n( "Paper &Layout" ), this, "paperLayoutDlg", 0 );

  // View
  _menubar->insertMenu( i18n( "&View" ), m_vMenuView, -1, -1 );
  m_vMenuView->setCheckable( true );

  m_idMenuView_NewView = m_vMenuView->insertItem( i18n( "New View" ), this, "newView", 0 );

  m_vMenuView->insertSeparator( -1 );

  m_idMenuView_ShowPageBorders = m_vMenuView->insertItem( i18n( "Show Page Borders" ), this, "togglePageBorders", 0 );
  m_vMenuView->setItemChecked( m_idMenuView_ShowPageBorders, m_pTable->isShowPageBorders() );

  // Data
  _menubar->insertMenu( i18n( "D&ata" ), m_vMenuData, -1, -1 );

  m_idMenuData_Consolidate = m_vMenuData->insertItem( i18n( "Consolidate" ), this, "consolidate", 0 );

  // Folder
  _menubar->insertMenu( i18n( "F&older" ), m_vMenuFolder, -1, -1 );

  m_idMenuFolder_NewTable = m_vMenuFolder->insertItem( i18n( "New Table" ), this, "insertNewTable", 0 );

  // Format
  _menubar->insertMenu( i18n( "Fo&rmat" ), m_vMenuFormat, -1, -1 );

  m_idMenuFormat_AutoFill = m_vMenuFormat->insertItem( i18n( "&Auto Fill ..." ), this, "autoFill", 0 );

  // Scripts
  _menubar->insertMenu( i18n( "&Scripts" ), m_vMenuScripts, -1, -1 );

  m_idMenuScripts_EditGlobal = m_vMenuScripts->insertItem( i18n( "Edit &global scripts..." ), this, "editGlobalScripts", 0 );
  m_idMenuScripts_EditLocal = m_vMenuScripts->insertItem( i18n( "Edit &local script" ), this, "editLocalScripts", 0 );
  m_idMenuScripts_Reload = m_vMenuScripts->insertItem( i18n( "&Reload scripts" ), this, "reloadScripts", 0 );
  m_idMenuScripts_Run = m_vMenuScripts->insertItem( i18n( "R&un local script" ), this, "runLocalScript", 0 );

  // Help
  m_vMenuHelp = _menubar->helpMenu();
  if ( CORBA::is_nil( m_vMenuHelp ) )
  {
    _menubar->insertSeparator( -1 );
    _menubar->setHelpMenu( _menubar->insertMenu( i18n( "&Help" ), m_vMenuHelp, -1, -1 ) );
  }
    
  // m_idMenuHelp_About = m_vMenuHelp->insertItem( i18n( "&About" ), this, "helpAbout", 0 );
  m_idMenuHelp_Using = m_vMenuHelp->insertItem( i18n( "&Using KSpread" ), this, "helpUsing", 0 );
	
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
  kapp->invokeHTMLHelp( "kspread/kspread.html", 0 );
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
  m_pTable->deleteColumn( m_iMarkerColumn );
}

void KSpreadView::deleteRow()
{
  if ( !m_pTable )
    return;
  m_pTable->deleteRow( m_iMarkerRow ); 
}

void KSpreadView::insertColumn()
{
  if ( !m_pTable )
    return;
  m_pTable->insertColumn( m_iMarkerColumn );
}

void KSpreadView::insertRow()
{
  if ( !m_pTable )
    return;
  m_pTable->insertRow( m_iMarkerRow );
}

void KSpreadView::fontSelected( const char *_font )
{
    if ( m_pTable != 0L )
      m_pTable->setSelectionFont( QPoint( m_iMarkerColumn, m_iMarkerRow ), _font );
}

void KSpreadView::fontSizeSelected( const char *_size )
{
  if ( m_pTable != 0L )
      m_pTable->setSelectionFont( QPoint( m_iMarkerColumn, m_iMarkerRow ), 0L, atoi( _size ) );
}

void KSpreadView::bold()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionFont( QPoint( m_iMarkerColumn, m_iMarkerRow ), 0L, -1, 1 );
}

void KSpreadView::italic()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionFont( QPoint( m_iMarkerColumn, m_iMarkerRow ), 0L, -1, -1, 1 );
}

void KSpreadView::reloadScripts()
{
  m_pDoc->reloadScripts(); // reload global scripts
  m_pDoc->endEditPythonCode(); // get local scripts from file -> probably
  // should be moved to another menu item. Hum, not sure about the UI.
}

void KSpreadView::runLocalScript()
{
  debug("KSpreadView::runLocalScript()");
  m_pDoc->runPythonCode();
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
  if ( !m_pDoc->editPythonCode() )
  {
    QMessageBox::message( i18n( "KSpread Error" ), i18n( "Could not start editor" ) );
    return;
  }
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
  QObject::connect( _t, SIGNAL( sig_maxColumn( int ) ), SLOT( slotMaxColumn( int ) ) );
  QObject::connect( _t, SIGNAL( sig_maxRow( int ) ), SLOT( slotMaxRow( int ) ) );
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
  m_pCanvasWidget->repaint();

  slotMaxColumn( m_pTable->maxColumn() );
  slotMaxRow( m_pTable->maxRow() );
}

KSpreadTable* KSpreadView::findTable( const char *_name )
{
  KSpreadTable *t;
  
  for ( t = m_pDoc->map()->firstTable(); t != 0L; t = m_pDoc->map()->nextTable() )
  {
    if ( strcmp( _name, t->name() ) == 0 )
      return t;
  }
  
  return 0L;
}

void KSpreadView::slotMaxColumn( int _max_column )
{
  int xpos = m_pTable->columnPos( _max_column + 10, this );

  m_pHorzScrollBar->setRange( 0, xpos + xOffset() );
}

void KSpreadView::slotMaxRow( int _max_row )
{
  int ypos = m_pTable->rowPos( _max_row + 10, this );

  m_pVertScrollBar->setRange( 0, ypos + yOffset() );
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
    m_pTable->copySelection( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}

void KSpreadView::cutSelection()
{
  if ( m_pTable )
    m_pTable->cutSelection( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}

void KSpreadView::paste()
{
  if ( m_pTable )
    m_pTable->paste( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
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
  xpos = m_pTable->columnPos( markerColumn(), this );
  ypos = m_pTable->rowPos( markerRow(), this );

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
  QApplication::sendEvent( m_pCanvasWidget, _ev );
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
      
    m_pCanvasWidget->setGeometry( YBORDER_WIDTH, XBORDER_HEIGHT,
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
    m_pCanvasWidget->raise();
    m_pCanvasWidget->setGeometry( 0, 0, width(), height() );
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
    KSpreadCell *cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
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
	  QStrListIterator it = entry->commandsI18N();
	  for( ; it.current() != 0L; ++it )
	    m_pPopupMenu->insertItem( it.current(), m_popupMenuFirstToolId + i++ );
	  QStrListIterator it2 = entry->commands();
	  for( ; it2.current() != 0L; ++it2 )
	  {    
	    ToolEntry *t = new ToolEntry;
	    t->command = it2.current();
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

  CORBA::Object_var obj = imr_activate( entry->entry->name(), "IDL:DataTools/Tool:1.0" );
  if ( CORBA::is_nil( obj ) )
    // TODO: error message
    return;
  
  DataTools::Tool_var tool = DataTools::Tool::_narrow( obj );
  if ( CORBA::is_nil( tool ) )
    // TODO: error message
    return;
  
  KSpreadCell *cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
  if ( !cell->isFormular() && !cell->isValue() )
  {    
    QString text = cell->text();
    CORBA::Any value;
    value <<= CORBA::Any::from_string( (char*)text.data(), 0 );
    CORBA::Any anyid;
    KSpread::DataToolsId id;
    id.time = (CORBA::ULong)time( 0L );
    id.row = m_iMarkerRow;
    id.column = m_iMarkerColumn;
    anyid <<= id;
    tool->run( entry->command, this, value, anyid );
    return;
  }
}

void KSpreadView::slotCopy()
{
  assert( m_pTable );
  
  m_pTable->copySelection( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}

void KSpreadView::slotCut()
{
  assert( m_pTable );

  m_pTable->cutSelection( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}

void KSpreadView::slotPaste()
{
  assert( m_pTable );

  m_pTable->paste( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}

void KSpreadView::slotDelete()
{
  assert( m_pTable );

  m_pTable->deleteSelection( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}

void KSpreadView::slotLayoutDlg()
{
  QRect selection( m_pTable->selectionRect() );

  hideMarker();

  cout << "#######################################" << endl;
  
  if ( selection.contains( QPoint( m_iMarkerColumn, m_iMarkerRow ) ) )
    CellLayoutDlg dlg( this, m_pTable, selection.left(), selection.top(),
		       selection.right(), selection.bottom() );
  else
    CellLayoutDlg dlg( this, m_pTable, m_iMarkerColumn, m_iMarkerRow, m_iMarkerColumn, m_iMarkerRow );

  m_pDoc->setModified( true );

  cout << "------------------------------------------" << endl;
  
  showMarker();
}

void KSpreadView::paperLayoutDlg()
{
  m_pDoc->paperLayoutDlg();
}

void KSpreadView::multiRow()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionMultiRow( QPoint( m_iMarkerColumn, m_iMarkerRow ) );
}
    
void KSpreadView::alignLeft()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionAlign( QPoint( m_iMarkerColumn, m_iMarkerRow ), KSpreadLayout::Left );
}

void KSpreadView::alignRight()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionAlign( QPoint( m_iMarkerColumn, m_iMarkerRow ), KSpreadLayout::Right );
}

void KSpreadView::alignCenter()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionAlign( QPoint( m_iMarkerColumn, m_iMarkerRow ), KSpreadLayout::Center );
}

void KSpreadView::moneyFormat()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionMoneyFormat( QPoint( m_iMarkerColumn, m_iMarkerRow ) ); 
}

void KSpreadView::precisionPlus()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPrecision( QPoint( m_iMarkerColumn, m_iMarkerRow ), 1 ); 
}

void KSpreadView::precisionMinus()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPrecision( QPoint( m_iMarkerColumn, m_iMarkerRow ), -1 ); 
}

void KSpreadView::percent()
{
  if ( m_pTable != 0L )
    m_pTable->setSelectionPercent( QPoint( m_iMarkerColumn, m_iMarkerRow ) ); 
}

void KSpreadView::insertTable()
{
  vector<KoDocumentEntry> vec = koQueryDocuments( "'IDL:KSpread/DocumentFactory:1.0' in RepoID", 1 );
  if ( vec.size() == 0 )
  {    
    cout << "Got no results" << endl;
    QMessageBox::critical( 0L, i18n("Error"), i18n("Sorry, no spread sheet  component registered"),
			   i18n("Ok") );
    return;
  }

  cerr << "USING component " << vec[0].name << endl;

  m_pCanvasWidget->setAction( KSpreadCanvas::InsertChild, vec[0] );
}

void KSpreadView::insertImage()
{
  // m_pCanvasWidget->setAction( KSpreadCanvas::InsertChild, "KImage" );
}

void KSpreadView::insertObject()
{
  KoDocumentEntry e = KoPartSelectDia::selectPart();
  if ( e.name.isEmpty() )
    return;
  
  m_pCanvasWidget->setAction( KSpreadCanvas::InsertChild, e );
}

void KSpreadView::insertChart()
{
  m_pCanvasWidget->setAction( KSpreadCanvas::InsertChart );
  // m_pCanvasWidget->setAction( KSpreadCanvas::Chart );
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
    
  m_pCanvasWidget->repaint();
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

  m_pCanvasWidget->repaint();
  m_pVBorderWidget->repaint();
  m_pHBorderWidget->repaint();
}
*/

void KSpreadView::slotScrollHorz( int _value )
{
  if ( m_pTable == 0L )
    return;

  m_pTable->enableScrollBarUpdates( false );
  
  hideMarker();
  
  int dx = xOffset() - _value;
  m_iXOffset = _value;
  m_pCanvasWidget->scroll( dx, 0 );
  m_pHBorderWidget->scroll( dx, 0 );

  showMarker();

  m_pTable->enableScrollBarUpdates( true );
}

void KSpreadView::slotScrollVert( int _value )
{
  if ( m_pTable == 0L )
    return;

  m_pTable->enableScrollBarUpdates( false );

  hideMarker();
  
  int dy = yOffset() - _value;
  m_iYOffset = _value;
  m_pCanvasWidget->scroll( 0, dy );
  m_pVBorderWidget->scroll( 0, dy );

  showMarker();

  m_pTable->enableScrollBarUpdates( true );
}

void KSpreadView::setText( const char *_text )
{
  if ( m_pTable == 0L )
    return;
  
  KSpread::EventSetText event;
  event.text = CORBA::string_dup( _text );
  EMIT_EVENT( this, KSpread::eventSetText, event );
  
  // m_pTable->setText( m_iMarkerRow, m_iMarkerColumn, _text );
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadView::drawVisibleCells()
{
  hideMarker();

  QPainter painter;
  painter.begin( m_pCanvasWidget );
  painter.save();
    
  int xpos;
  int ypos;
  int left_col = m_pTable->leftColumn( 0, xpos, this );
  int right_col = m_pTable->rightColumn( m_pCanvasWidget->width(), this );
  int top_row = m_pTable->topRow( 0, ypos, this );
  int bottom_row = m_pTable->bottomRow( m_pCanvasWidget->height(), this );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
    
  QRect rect( 0, 0, m_pCanvasWidget->width(), m_pCanvasWidget->height() );
    
  int left = xpos;    
  for ( int y = top_row; y <= bottom_row; y++ )
  {
    RowLayout *row_lay = m_pTable->rowLayout( y );
    xpos = left;

    for ( int x = left_col; x <= right_col; x++ )
    {
      ColumnLayout *col_lay = m_pTable->columnLayout( x );
      KSpreadCell *cell = m_pTable->cellAt( x, y );
      cell->paintEvent( this, rect, painter, xpos, ypos, x, y, col_lay, row_lay );
      xpos += col_lay->width( this );
    }

    ypos += row_lay->height( this );
  }

  painter.end();

  showMarker();
}

void KSpreadView::drawMarker( QPainter * _painter )
{
  bool own_painter = FALSE;
    
  if ( _painter == 0L )
  {
    _painter = new QPainter();
    _painter->begin( m_pCanvasWidget );
    own_painter = TRUE;
  }
    
  int xpos;
  int ypos;    
  int w, h;
  QRect selection( m_pTable->selectionRect() );
    // printf("selection: %i %i %i\n",selection.left(), selection.right(), selection.bottom() );
    
  if ( selection.left() == 0 || selection.right() == 0x7fff || selection.bottom() == 0x7fff )
  {
    xpos = m_pTable->columnPos( m_iMarkerColumn, this );
    ypos = m_pTable->rowPos( m_iMarkerRow, this );
    KSpreadCell *cell = m_pTable->cellAt( m_iMarkerColumn, m_iMarkerRow );
    w = cell->width( m_iMarkerColumn, this );
    h = cell->height( m_iMarkerRow, this );
  }
  else
  {
    xpos = m_pTable->columnPos( selection.left(), this );
    ypos = m_pTable->rowPos( selection.top(), this );
    int x = m_pTable->columnPos( selection.right(), this );
    KSpreadCell *cell = m_pTable->cellAt( selection.right(), selection.top() );
    int tw = cell->width( selection.right(), this );
    w = ( x - xpos ) + tw;
    cell = m_pTable->cellAt( selection.left(), selection.bottom() );
    int y = m_pTable->rowPos( selection.bottom(), this );
    int th = cell->height( selection.bottom(), this );
    h = ( y - ypos ) + th;
  }
    
  RasterOp rop = _painter->rasterOp();
    
  _painter->setRasterOp( NotROP );
  QPen pen;
  pen.setWidth( 3 );
  _painter->setPen( pen );

    // _painter->drawRect( xpos - 1, ypos - 1, w + 2, h + 2 );
  _painter->drawLine( xpos - 2, ypos - 1, xpos + w + 2, ypos - 1 );
  _painter->drawLine( xpos - 1, ypos + 1, xpos - 1, ypos + h + 3 );
  _painter->drawLine( xpos + 1, ypos + h + 1, xpos + w - 3, ypos + h + 1 );
  _painter->drawLine( xpos + w, ypos + 1, xpos + w, ypos + h - 2 );
  _painter->fillRect( xpos + w - 2, ypos + h - 1, 5, 5, black );
  _painter->setRasterOp( rop );
  
  if ( own_painter )
  {
    _painter->end();
    delete _painter;
  }

  char buffer[ 20 ];
  sprintf( buffer, "%s%d", m_pTable->columnLabel( m_iMarkerColumn ), m_iMarkerRow );

  m_pPosWidget->setText(buffer);       
}

void KSpreadView::hideMarker( QPainter& _painter )
{
  if ( m_iMarkerVisible == 1 )
    drawMarker( &_painter );
  m_iMarkerVisible--;
}

void KSpreadView::showMarker( QPainter& _painter)
{
  if ( m_iMarkerVisible == 1 )
    return;
  m_iMarkerVisible++; 
  if ( m_iMarkerVisible == 1 )
    drawMarker( &_painter );
}

void KSpreadView::drawCell( KSpreadCell *_cell, int _col, int _row )
{
    QPainter painter;
    painter.begin( m_pCanvasWidget );

    drawCell( painter, _cell, _col, _row );

    painter.end();
}

void KSpreadView::drawCell( QPainter &painter, KSpreadCell *_cell, int _col, int _row )
{
    painter.save();

    hideMarker( painter );

    QRect rect( 0, 0, m_pCanvasWidget->width(), m_pCanvasWidget->height() );

    QRect r;
    _cell->paintEvent( this, rect, painter, _col, _row, &r );

    painter.restore();

    showMarker( painter );
}

/*
void KSpreadTable::drawCellList()
{    
    QPainter painter;
    painter.begin( pGui->canvasWidget() );
    painter.save();
    
    drawMarker( &painter );
    QRect rect( 0, 0, pGui->canvasWidget()->width(), pGui->canvasWidget()->height() );

    QRect r;
    QRect unite;
    
    QListIterator<KSpreadCell> it( objectList );
    for ( ; it.current(); ++it ) 
    {
	it.current()->paintEvent( rect, painter, it.current()->column(), it.current()->row(), false, &r );
	unite = unite.unite( r );
    }
    
    painter.restore();
    drawMarker( &painter );    

    painter.end();
}
*/

QPoint KSpreadView::marker()
{
  int row, column;
  
  if ( !isMarkerVisible() )
  {
    row = -1;
    column = -1;
  }
  else
  {
    row = m_iMarkerRow;
    column = m_iMarkerColumn;
  }

  return QPoint( column, row );
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

  m_pCanvasWidget->update();
}

void KSpreadView::slotUpdateView( KSpreadTable *_table, const QRect& _rect )
{
  printf("void KSpreadView::slotUpdateView( KSpreadTable *_table, const QRect& %i %i|%i %i )\n",_rect.left(),_rect.top(),_rect.right(),_rect.bottom());
  
  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  /* QPainter painter;
     painter.begin( m_pCanvasWidget ); */

  m_pCanvasWidget->updateCellRect( _rect );  
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
  
  m_pCanvasWidget->updateCellRect( uni );
  
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

  drawCell( _cell, _col, _row );

  if ( _col == m_iMarkerColumn && _row == m_iMarkerRow )
    editWidget()->setText( _cell->text() );
}

void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect& _old )
{
  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  printf("void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect &_old %i %i|%i %i\n",_old.left(),_old.top(),_old.right(),_old.bottom());

  hideMarker();

  int ypos;
  int xpos;
  int left_col = m_pTable->leftColumn( 0, xpos, this );
  int right_col = m_pTable->rightColumn( m_pCanvasWidget->width(), this );
  int top_row = m_pTable->topRow( 0, ypos, this );
  int bottom_row = m_pTable->bottomRow( m_pCanvasWidget->height(), this );

  for ( int x = left_col; x <= right_col; x++ )
    for ( int y = top_row; y <= bottom_row; y++ )
    {
      KSpreadCell *cell = m_pTable->cellAt( x, y );
      drawCell( cell, x, y );
    }
    
  // Are complete columns selected ?
  if ( _old.bottom() == 0x7FFF )
    m_pHBorderWidget->update();	
  // Are complete rows selected ?
  else if ( _old.right() == 0x7FFF )
    m_pVBorderWidget->update();

  showMarker();
}

/****************************************************************
 *
 * KSpreadEditWidget
 *
 ****************************************************************/

KSpreadEditWidget::KSpreadEditWidget( QWidget *_parent, KSpreadView *_view ) : QLineEdit( _parent, "KSpreadEditWidget" )
{
  m_pView = _view;
}

void KSpreadEditWidget::publicKeyPressEvent ( QKeyEvent* _ev ) 
{
  // QLineEdit::keyPressEvent( _ev );
  keyPressEvent( _ev );
}

void KSpreadEditWidget::slotAbortEdit()
{
  m_pView->canvasWidget()->setFocus();
  setText( "" );
}

void KSpreadEditWidget::slotDoneEdit()
{
  if ( !m_pView->activeTable() )
    return;
  
  m_pView->canvasWidget()->setFocus();
  m_pView->setText( text() );
  setText( "" );
}                    

void KSpreadEditWidget::keyPressEvent ( QKeyEvent* _ev ) 
{
  switch ( _ev->key() )
  {
    case Key_Down:
    case Key_Up:
    case Key_Return:
    case Key_Enter:

	if ( m_pView->activeTable() != 0L )
	{
	    m_pView->canvasWidget()->setFocus();
	    m_pView->setText( text() );
	    setText( "" );
	    QApplication::sendEvent( m_pView->canvasWidget(), _ev );
	}
	else
	    _ev->accept();
	
	break;
	
    default:
	QLineEdit::keyPressEvent( _ev );
    } 
}

/****************************************************************
 *
 * KSpreadCanvas
 *
 ****************************************************************/

KSpreadCanvas::KSpreadCanvas( QWidget *_parent, KSpreadView *_view ) : QWidget( _parent )
{
  QWidget::setFocusPolicy( QWidget::StrongFocus );

  m_pView = _view;
  m_eAction = DefaultAction;
  m_eMouseAction = NoAction;
  m_bGeometryStarted = false;
  m_bEditDirtyFlag = false;

  setBackgroundColor( white );
  setMouseTracking( TRUE );
}

void KSpreadCanvas::setAction( Actions _act )
{
  QRect selection( m_pView->activeTable()->selectionRect() );
  
  if ( _act == InsertChart )
  {    
    // Something must be selected
    if ( selection.right() == 0x7fff || selection.bottom() == 0x7fff || selection.left() == 0 )
    {
      QMessageBox::critical( this, i18n("KSpread Error" ), i18n("You must first select the cells\n"
								"which contain the data." ),
			     i18n( "Ok" ) );
      return;
    }
  }
  
  m_eAction = _act;
}

void KSpreadCanvas::setAction( Actions _act, KoDocumentEntry& _e )
{
  m_actionArgument = _e;
  setAction( _act );
}

void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  QRect selection( table->selectionRect() );
  
  if ( m_eMouseAction == ChildGeometry )
  {
    QPainter painter;
    painter.begin( this );
    painter.setRasterOp( NotROP );
      
    QPen pen;
    pen.setStyle( DashLine );
    painter.setPen( pen );
      
    if ( m_bGeometryStarted )
    {
      int x = m_ptGeometryStart.x();
      int y = m_ptGeometryStart.y();    
      if ( x > m_ptGeometryEnd.x() )
	x = m_ptGeometryEnd.x();
      if ( y > m_ptGeometryEnd.y() )
	    y = m_ptGeometryEnd.y();
      int w = m_ptGeometryEnd.x() - m_ptGeometryStart.x();
      if ( w < 0 ) w *= -1;
      int h = m_ptGeometryEnd.y() - m_ptGeometryStart.y();
	if ( h < 0 ) h *= -1;
	
	painter.drawRect( x, y, w, h );
    }
    else
      m_bGeometryStarted = TRUE;
    
    m_ptGeometryEnd = _ev->pos();
    
    int x = m_ptGeometryStart.x();
    int y = m_ptGeometryStart.y();    
    if ( x > m_ptGeometryEnd.x() )
      x = m_ptGeometryEnd.x();
    if ( y > m_ptGeometryEnd.y() )
      y = m_ptGeometryEnd.y();
    int w = m_ptGeometryEnd.x() - m_ptGeometryStart.x();
    if ( w < 0 ) w *= -1;
    int h = m_ptGeometryEnd.y() - m_ptGeometryStart.y();
    if ( h < 0 ) h *= -1;
    
    painter.drawRect( x, y, w, h );
    painter.end();
    
    return;
  }
    
  // if ( m_pView->isMarkerVisible() )
  {
    // Test wether we are in the lower right corner of the marker
    // if so => change the cursor
    int xpos;
    int ypos;
    int w, h;
    if ( selection.left() == 0 || selection.right() == 0x7fff || selection.bottom() == 0x7fff )
    {
      xpos = table->columnPos( m_pView->markerColumn(), m_pView );
      ypos = table->rowPos( m_pView->markerRow(), m_pView );
      KSpreadCell *cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
      w = cell->width( m_pView->markerColumn(), m_pView );
      h = cell->height( m_pView->markerRow(), m_pView );
    }
    else // if we have a rectangular selection ( not complete rows or columns )
    {
      xpos = table->columnPos( selection.left(), m_pView );
      ypos = table->rowPos( selection.top(), m_pView );
      int x = table->columnPos( selection.right(), m_pView );
      KSpreadCell *cell = table->cellAt( selection.right(), selection.top() );
      int tw = cell->width( selection.right(), m_pView );
      w = ( x - xpos ) + tw;
      cell = table->cellAt( selection.left(), selection.bottom() );
      int y = table->rowPos( selection.bottom(), m_pView );
      int th = cell->height( selection.bottom(), m_pView );
      h = ( y - ypos ) + th;
    }
    
    if ( _ev->pos().x() >= xpos + w - 2 && _ev->pos().x() <= xpos + w + 3 &&
	 _ev->pos().y() >= ypos + h - 1 && _ev->pos().y() <= ypos + h + 4 )
      setCursor( sizeAllCursor );
    else
      setCursor( arrowCursor );
  }
  
  if ( m_eMouseAction == NoAction )
    return;
    
  int ypos, xpos;
  int row = table->topRow( _ev->pos().y(), ypos, m_pView );
  int col = table->leftColumn( _ev->pos().x(), xpos, m_pView );

  if ( col < m_iMouseStartColumn )
	col = m_iMouseStartColumn;
  if ( row < m_iMouseStartRow )
    row = m_iMouseStartRow;

  if ( row == selection.bottom() && col == selection.right() )
    return;

  m_pView->hideMarker();
    
  // Set the new lower right corner of the selection
  selection.setRight( col );
  selection.setBottom( row );
  table->setSelection( selection, m_pView );
  
  // Scroll the table if neccessary
  if ( _ev->pos().x() < 0 )
    m_pView->horzScrollBar()->setValue( m_pView->xOffset() + xpos );
  else if ( _ev->pos().x() > width() )
  {
    ColumnLayout *cl = table->columnLayout( col + 1 );
    xpos = table->columnPos( col + 1, m_pView );
    m_pView->horzScrollBar()->setValue( m_pView->xOffset() + ( xpos + cl->width( m_pView ) - width() ) );
  }
  if ( _ev->pos().y() < 0 )
    m_pView->vertScrollBar()->setValue( m_pView->yOffset() + ypos );
  else if ( _ev->pos().y() > height() )
  {
    RowLayout *rl = table->rowLayout( row + 1 );
    ypos = table->rowPos( row + 1, m_pView );
    m_pView->vertScrollBar()->setValue( m_pView->yOffset() +
					( ypos + rl->height( m_pView ) - m_pView->canvasWidget()->height() ) );
  }
  
  m_pView->showMarker();
  
  // All visible cells
  /* int left_col = table->leftColumn( 0, xpos, m_pView );
  int right_col = table->rightColumn( width(), m_pView );
  int top_row = table->topRow( 0, ypos, m_pView );
  int bottom_row = table->bottomRow( height(), m_pView ); */
  
  // Redraw all cells that changed their marking mode.
  /* for ( int x = left_col; x <= right_col; x++ )
    for ( int y = top_row; y <= bottom_row; y++ )
    {
      bool b1 = ( x >= r.left() && x <= r.right() );
      bool b2 = ( x >= selection.left() && x <= selection.right() );
      bool b3 = ( y >= r.top() && y <= r.bottom() );
      bool b4 = ( y >= selection.top() && y <= selection.bottom() );
      
      if ( ( b1 && !b2 ) || ( !b1 && b2 ) || ( b3 && !b4 ) || ( !b3 && b4 ) )
      {
	KSpreadCell *cell = table->cellAt( x, y );
	m_pView->drawCell( cell, x, y );
	m_bMouseMadeSelection = TRUE;
      }
    } */

  m_bMouseMadeSelection = true;
}

void KSpreadCanvas::mouseReleaseEvent( QMouseEvent *_ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  if ( m_eMouseAction == ChildGeometry )
  {
    if ( !m_bGeometryStarted )
      return;

    m_bGeometryStarted = false;
      
    m_ptGeometryEnd = _ev->pos();
    
    int x = m_ptGeometryStart.x();
    int y = m_ptGeometryStart.y();    
    if ( x > m_ptGeometryEnd.x() )
      x = m_ptGeometryEnd.x();
    if ( y > m_ptGeometryEnd.y() )
      y = m_ptGeometryEnd.y();
    int w = m_ptGeometryEnd.x() - m_ptGeometryStart.x();
    if ( w < 0 ) w *= -1;
    int h = m_ptGeometryEnd.y() - m_ptGeometryStart.y();
    if ( h < 0 ) h *= -1;
    
    QPainter painter;
    painter.begin( this );
      
    QPen pen;
    pen.setStyle( DashLine );
    painter.setPen( pen );
      
    painter.setRasterOp( NotROP );
    painter.drawRect( x, y, w, h );
    painter.end();
      
    // TODO
    /*
    QRect r( x, y, w, h );
      PartFrame *_frame = pKSpread->createPart( "IDL:KChartFactory:1.0", r, m_pView->canvasWidget() );
      QRect r2;
      KSpreadTable::createChartCellBinding( _frame, r2 ); */

    QRect r( x, y, w, h );
    if ( m_eAction == InsertChart )
      m_pView->insertChart( r );
    else if ( m_eAction == InsertChild )
      m_pView->insertChild( r, m_actionArgument );
	      
    m_eMouseAction = NoAction;
    m_eAction = DefaultAction;
    return;
  }

  m_pView->hideMarker();
  
  QRect selection( table->selectionRect() );
  
  // m_pView->canvasWidget()->setMouseTracking( FALSE );
  // The user started the drag in the lower right corner of the marker ?
  if ( m_eMouseAction == ResizeCell )
  {
    KSpreadCell *cell = table->nonDefaultCell( m_iMouseStartColumn, m_iMouseStartRow );
    cell->forceExtraCells( m_iMouseStartColumn, m_iMouseStartRow, 
			   selection.right() - selection.left(), 
			   selection.bottom() - selection.top() );
	
    selection.setCoords( 0, 0, 0, 0 );
    table->setSelection( selection, m_pView );
    m_pView->doc()->setModified( TRUE );
  }
  else if ( m_eMouseAction == AutoFill )
  {
	/* Object *obj = getNonDefaultCell( mouseStartColumn, mouseStartRow );
	obj->forceExtraCells( mouseStartColumn, mouseStartRow, 
			      selection.right() - selection.left(), 
			      selection.bottom() - selection.top() ); */
    table->autofill( m_rctAutoFillSrc, table->selectionRect() );
	
    m_pView->doc()->setModified( TRUE );
    selection.setCoords( 0, 0, 0, 0 );
    table->setSelection( selection, m_pView );
  }
  // The user started the drag in the middle of a cell ?
  else if ( m_eMouseAction == Mark )
  {
    // Get the object in the lower right corner
    KSpreadCell *cell = table->cellAt( m_iMouseStartColumn, m_iMouseStartRow );
    // Did we mark only a single cell ?
    // Take care: One cell may obscure other cells ( extra size! ).
    if ( selection.left() + cell->extraXCells() == selection.right() &&
	 selection.top() + cell->extraYCells() == selection.bottom() )
    {
      // Delete the selection
      selection.setCoords( 0, 0, 0, 0 );
      table->setSelection( selection, m_pView );
    }
  }
    
  m_eMouseAction = NoAction;
  if ( m_bMouseMadeSelection )
    m_pView->drawVisibleCells();
  m_bMouseMadeSelection = FALSE;

  m_pView->showMarker();
}

void KSpreadCanvas::mousePressEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  // Do we have to create a new chart at this position ?
  if ( m_eAction == InsertChart || m_eAction == InsertChild )
  {
    m_ptGeometryStart = _ev->pos();
    m_ptGeometryEnd = _ev->pos();
    m_bGeometryStarted = FALSE;
    m_eMouseAction = ChildGeometry;
    return;
  }

#ifdef USE_PICTURE
  if ( _ev->button() == LeftButton )
  {
    QPoint p( _ev->pos().x() + m_pView->xOffset(), _ev->pos().y() + m_pView->yOffset() );
    
    QListIterator<KSpreadChildPicture> it = m_pView->pictures();
    for( ; it != 0L; ++it )
    {
      if ( it.current()->geometry().contains( p ) )
      {
	m_pView->markChildPicture( it.current() );
	return;
      }
    }
  }
#endif

  QRect selection( table->selectionRect() );
  
  // Get the position and size of the marker/marked-area
  int xpos;
  int ypos;
  int w, h;
  // No selection or complete rows/columns are selected
  if ( selection.left() == 0 ||
       selection.right() == 0x7fff || selection.bottom() == 0x7fff )
  {
    xpos = table->columnPos( m_pView->markerColumn(), m_pView );
    ypos = table->rowPos( m_pView->markerRow(), m_pView );
    KSpreadCell *cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
    w = cell->width( m_pView->markerColumn() );
    h = cell->height( m_pView->markerRow() );
  }
  else // if we have a rectangular selection ( not complete rows or columns )
  {
    xpos = table->columnPos( selection.left(), m_pView );
    ypos = table->rowPos( selection.top(), m_pView );
    int x = table->columnPos( selection.right(), m_pView );
    KSpreadCell *cell = table->cellAt( selection.right(), selection.top() );
    int tw = cell->width( selection.right(), m_pView );
    w = ( x - xpos ) + tw;
    cell = table->cellAt( selection.left(), selection.bottom() );
    int y = table->rowPos( selection.bottom(), m_pView );
    int th = cell->height( selection.bottom(), m_pView );
    h = ( y - ypos ) + th;
  }
    
  // Did we click in the lower right corner of the marker/marked-area ?
  if ( _ev->pos().x() >= xpos + w - 2 && _ev->pos().x() <= xpos + w + 3 &&
       _ev->pos().y() >= ypos + h - 1 && _ev->pos().y() <= ypos + h + 4 )
  {
    // Auto fill ?
    if ( _ev->button() == LeftButton )
    {
      m_eMouseAction = AutoFill;
      // Do we have a selection already ?
      if ( selection.left() != 0 && selection.right() != 0x7fff && selection.bottom() != 0x7fff )
      { /* Selection is ok */
      	m_rctAutoFillSrc = selection;
      }	    
      else // Select the current cell
      {
	KSpreadCell *cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(),
			     m_pView->markerColumn() + cell->extraXCells(),
			     m_pView->markerRow() + cell->extraYCells() );
	m_rctAutoFillSrc.setCoords( m_pView->markerColumn(), m_pView->markerRow(),
				    m_pView->markerColumn(), m_pView->markerRow() );
      }
      
      m_iMouseStartColumn = m_pView->markerColumn();
      m_iMouseStartRow = m_pView->markerRow();
    }
    // Resize a cell ?
    else if ( _ev->button() == MidButton && selection.left() == 0 )
    {
      m_eMouseAction = ResizeCell;
      KSpreadCell *cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
      selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(),
			   m_pView->markerColumn() + cell->extraXCells(),
			   m_pView->markerRow() + cell->extraYCells() );
      m_iMouseStartColumn = m_pView->markerColumn();
      m_iMouseStartRow = m_pView->markerRow();
    }

    table->setSelection( selection, m_pView );
    return;
  }

  m_pView->hideMarker();
    
  int row = table->topRow( _ev->pos().y(), ypos, m_pView );
  int col = table->leftColumn( _ev->pos().x(), xpos, m_pView );    

  if ( _ev->button() == LeftButton || !selection.contains( QPoint( col, row ) ) )
    table->unselect();
    
  m_pView->setMarkerColumn( col );
  m_pView->setMarkerRow( row );
  
  KSpreadCell *cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
  
  // Go to the upper left corner of the obscuring object
  if ( cell->isObscured() )
  {
    m_pView->setMarkerRow( cell->obscuringCellsRow() );
    m_pView->setMarkerColumn( cell->obscuringCellsColumn() );
    cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
  }

  if ( _ev->button() == LeftButton )
  {
    m_eMouseAction = Mark;
    selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(),
			 m_pView->markerColumn() + cell->extraXCells(),
			 m_pView->markerRow() + cell->extraYCells() );

    // if ( old_selection.left() != 0 || cell->extraXCells() != 0 || cell->extraYCells() != 0 )
    table->setSelection( selection, m_pView );
    m_iMouseStartColumn = m_pView->markerColumn();
    m_iMouseStartRow = m_pView->markerRow();
  }
  
  // Update the edit box
  if ( cell->text() != 0L )
    m_pView->editWidget()->setText( cell->text() );
  else
    m_pView->editWidget()->setText( "" );

  m_pView->showMarker();
    
  if ( _ev->button() == RightButton )
  {
    QPoint p = mapToGlobal( _ev->pos() );
    m_pView->openPopupMenu( p );
  }
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
  if ( m_pView->doc()->isLoading() )
    return;

  // HACK
  if ( x() == 5000 && y() == 5000 )
    return;
  
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  m_pView->hideMarker();
  
  QPainter painter;
  painter.begin( this );
  painter.save();
    
  int xpos;
  int ypos;
  int left_col = table->leftColumn( _ev->rect().x(), xpos, m_pView );
  int right_col = table->rightColumn( _ev->rect().right(), m_pView );
  int top_row = table->topRow( _ev->rect().y(), ypos, m_pView );
  int bottom_row = table->bottomRow( _ev->rect().bottom(), m_pView );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
    
  QRect r;
    
  int left = xpos;    
  for ( int y = top_row; y <= bottom_row; y++ )
  {
    RowLayout *row_lay = table->rowLayout( y );
    xpos = left;
    
    for ( int x = left_col; x <= right_col; x++ )
    {
      ColumnLayout *col_lay = table->columnLayout( x );
	    
      KSpreadCell *cell = table->cellAt( x, y );
      cell->paintEvent( m_pView, _ev->rect(), painter, xpos, ypos, x, y, col_lay, row_lay, &r );
	    
      xpos += col_lay->width( m_pView );
    }

    ypos += row_lay->height( m_pView );
  }

#ifdef USE_PICTURES
  QListIterator<KSpreadChildPicture> it = m_pView->pictures();
  for( ; it != 0L; ++it )
  {
    // if ( it.current()->geometry().intersects( _ev->rect() ) )
    {
      painter.translate( it.current()->geometry().left() - m_pView->xOffset(),
			 it.current()->geometry().top() - m_pView->yOffset() );
      painter.drawPicture( *(it.current()->picture() ) );
      painter.translate( - it.current()->geometry().left() - m_pView->xOffset(),
			 - it.current()->geometry().top() - m_pView->yOffset() );
    }
  }
#endif

  painter.end();

  m_pView->showMarker();
}

void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;
    
  switch( _ev->key() )
    {
      /**
       * Handle in KSpreadView event handler
       */
    case Key_Return:
    case Key_Enter:
    case Key_Down:
	if ( m_pView->markerRow() == 0xFFFF )
	  return;

	if ( editDirtyFlag() )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  setEditDirtyFlag( FALSE );
	}
	break;
	
    case Key_Up:
	if ( m_pView->markerRow() == 1 )
	    return;

	if ( editDirtyFlag() )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  setEditDirtyFlag( FALSE );
	}
	break;
	
    case Key_Left:
	if ( m_pView->markerColumn() == 1 )
	    return;

	if ( editDirtyFlag() )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  setEditDirtyFlag( FALSE );
	}
	break;

    case Key_Right:
	if ( m_pView->markerColumn() == 0xFFFF )
	    return;

	if ( editDirtyFlag() )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  setEditDirtyFlag( FALSE );
	}
	break;

	/**
	 * Handle here
	 */
    case Key_Escape:
      _ev->accept();
      if ( editDirtyFlag() )
      {
	KSpreadCell* cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	if ( cell->text() != 0L )
	  m_pView->editWidget()->setText( cell->text() );
	else
	  m_pView->editWidget()->setText( "" );
	m_bEditDirtyFlag = false;
      }
      return;
  
    default:
      char buffer[2];
      if ( _ev->ascii() == 0 )
      {
	_ev->accept();
	return;
      }
      
      buffer[0] = _ev->ascii();
      buffer[1] = 0;
      
      if ( !m_bEditDirtyFlag )
	m_pView->editWidget()->setText( "" );
      
      m_pView->editWidget()->publicKeyPressEvent( _ev );
      m_bEditDirtyFlag = TRUE;
      return;
    }

  /**
   * Tell the KSpreadView event handler and enable
   * makro recording by the way.
   */
  _ev->accept();

  KSpread::EventKeyPressed event;
  event.key = _ev->key();
  event.state = _ev->state();
  event.ascii = _ev->ascii();
  EMIT_EVENT( m_pView, KSpread::eventKeyPressed, event );
}

void KSpreadCanvas::updateCellRect( const QRect &_rect )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  QRect param( _rect );
  if ( param.left() <= 0 && param.top() <= 0 && param.right() <= 0 && param.bottom() <= 0 )
    return;
  
  // We want to repaint the border too => enlarge the rect
  if ( param.left() > 1 )
    param.setLeft( param.left() - 1 );
  if ( param.right() < 0x7fff )
    param.setRight( param.right() + 1 );
  if ( param.top() > 1 )
    param.setTop( param.top() - 1 );
  if ( param.bottom() < 0x7fff )
    param.setBottom( param.bottom() + 1 );
  
  if ( param.left() <= 0 )
    param.setLeft( 1 );
  if ( param.top() <= 0 )
    param.setTop( 1 );
  if ( param.right() < param.left() )
    return;
  if ( param.bottom() < param.top() )
    return;
  
  m_pView->hideMarker();
  
  QPainter painter;
  painter.begin( this );
  painter.save();
    
  int xpos = table->columnPos( param.left(), m_pView );
  int ypos = table->rowPos( param.top(), m_pView );

  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
    
  QRect r;
    
  int left = xpos;    
  for ( int y = param.top(); y <= param.bottom(); y++ )
  {
    RowLayout *row_lay = table->rowLayout( y );
    xpos = left;
    
    for ( int x = param.left(); x <= param.right(); x++ )
    {
      ColumnLayout *col_lay = table->columnLayout( x );
	    
      KSpreadCell *cell = table->cellAt( x, y );
      cell->paintEvent( m_pView, rect(), painter, xpos, ypos, x, y, col_lay, row_lay, &r );
	    
      xpos += col_lay->width( m_pView );

      if ( xpos > width() )
	break;
    }

    ypos += row_lay->height( m_pView );
    if ( ypos > height() )
      break;
  }
  
  painter.end();

  m_pView->showMarker();
}

/****************************************************************
 *
 * KSpreadVBorder
 *
 ****************************************************************/

KSpreadVBorder::KSpreadVBorder( QWidget *_parent, KSpreadView *_view ) : QWidget( _parent )
{
  m_pView = _view;
  
  setBackgroundColor( lightGray );
  setMouseTracking( TRUE );
  m_bResize = FALSE;
  m_bSelection = FALSE;
}

void KSpreadVBorder::mousePressEvent( QMouseEvent * _ev )
{
  m_bResize = FALSE;
  m_bSelection = FALSE;

  KSpreadTable *table = m_pView->activeTable();
  assert( table );
    
  // Find the first visible row and the y position of this row.
  int y = 0;
  int row = table->topRow( 0, y, m_pView );
    
  // Did the user click between two rows ?
  while ( y < m_pView->vBorderWidget()->height() )
  {
    int h = table->rowLayout( row )->height( m_pView );
    row++;
    if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1 )
      m_bResize = TRUE;
    y += h;
  }
    
  // So he clicked between two rows ?
  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pView->canvasWidget() );
    painter.setRasterOp( NotROP );
    painter.drawLine( 0, _ev->pos().y(), m_pView->canvasWidget()->width(), _ev->pos().y() );
    painter.end();

    int tmp;
    m_iResizeAnchor = table->topRow( _ev->pos().y() - 3, tmp, m_pView );
    m_iResizePos = _ev->pos().y();
  }
  else
  {
    m_bSelection = TRUE;
	
    table->unselect();
    int tmp;
    int hit_row = table->topRow( _ev->pos().y(), tmp, m_pView );
    m_iSelectionAnchor = hit_row;
    QRect selection( table->selectionRect() );
    selection.setCoords( 1, hit_row, 0x7FFF, hit_row );
    m_pView->vBorderWidget()->update();
    table->setSelection( selection, m_pView );
  }
}

void KSpreadVBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pView->canvasWidget() );
    painter.setRasterOp( NotROP );
    painter.drawLine( 0, m_iResizePos, m_pView->canvasWidget()->width(), m_iResizePos );
    painter.end();
	
    RowLayout *rl = table->nonDefaultRowLayout( m_iResizeAnchor );
    int y = table->rowPos( m_iResizeAnchor, m_pView );
    if ( m_pView->zoom() * (float)( _ev->pos().y() - y ) < m_pView->zoom() * 20.0 )
      rl->setHeight( 20, m_pView );
    else
      rl->setHeight( _ev->pos().y() - y, m_pView );
	
    m_pView->doc()->setModified( TRUE );
  }
  
  m_bSelection = FALSE;
  m_bResize = FALSE;
}

void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );

  // The button is pressed and we are resizing ?
  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pView->canvasWidget() );
    painter.setRasterOp( NotROP );
    painter.drawLine( 0, m_iResizePos, m_pView->canvasWidget()->width(), m_iResizePos );

    m_iResizePos = _ev->pos().y();
    // Dont make the row have a height < 20 pixel.
    int twenty = (int)( 20.0 * m_pView->zoom() );
    int y = table->rowPos( m_iResizeAnchor, m_pView );
    if ( m_iResizePos < y + twenty )
      m_iResizePos = y + twenty;
    painter.drawLine( 0, m_iResizePos, m_pView->canvasWidget()->width(), m_iResizePos );
    painter.end();
  }
  // The button is pressed and we are selecting ?
  else if ( m_bSelection )
  {
    int y = 0;
    int row = table->topRow( _ev->pos().y(), y, m_pView );
    QRect selection = table->selectionRect();
    
    if ( row < m_iSelectionAnchor )
    {
      selection.setTop( row );
      selection.setBottom( m_iSelectionAnchor );
    }
    else
    {
      selection.setBottom( row );
      selection.setTop( m_iSelectionAnchor );
    }
    table->setSelection( selection, m_pView );
    
    if ( _ev->pos().y() < 0 )
      m_pView->vertScrollBar()->setValue( m_pView->yOffset() + y );
    else if ( _ev->pos().y() > m_pView->canvasWidget()->height() )
    {
      RowLayout *rl = table->rowLayout( row + 1 );
      y = table->rowPos( row + 1, m_pView );
      m_pView->vertScrollBar()->setValue( m_pView->yOffset() + 
					  ( y + rl->height( m_pView ) - m_pView->canvasWidget()->height() ) );
    }	
  }
  // No button is pressed and the mouse is just moved
  else
  {
    int y = 0;
    int row = table->topRow( 0, y, m_pView );
	
    while ( y < m_pView->vBorderWidget()->height() )
    {
      int h = table->rowLayout( row )->height( m_pView );
      row++;
      if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1 )
      {
	m_pView->vBorderWidget()->setCursor( sizeAllCursor );
	return;
      }
      y += h;
    }
	
    m_pView->vBorderWidget()->setCursor( arrowCursor );
  }
}

void KSpreadVBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  QPainter painter;
  painter.begin( this );
  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  painter.eraseRect( _ev->rect() );

  QFontMetrics fm = painter.fontMetrics();

  painter.setClipRect( _ev->rect() );
    
  int ypos;
  int top_row = table->topRow( _ev->rect().y(), ypos, m_pView );
  int bottom_row = table->bottomRow( _ev->rect().bottom(), m_pView );

  QRect selection( table->selectionRect() );
  
  for ( int y = top_row; y <= bottom_row; y++ )
  {
    bool selected = ( selection.left() != 0 && selection.right() == 0x7FFF &&
		      y >= selection.top() && y <= selection.bottom() );

    RowLayout *row_lay = table->rowLayout( y );

    if ( selected )
    {
      static QColorGroup g2( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill2( black );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pView ), g2, FALSE, 1, &fill2 );
    }
    else
    {
      static QColorGroup g( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill( lightGray );
      qDrawShadePanel( &painter, 0, ypos, YBORDER_WIDTH, row_lay->height( m_pView ), g, FALSE, 1, &fill );
    }
	
    char buffer[ 20 ];
    sprintf( buffer, "%i", y );

    if ( selected )
      painter.setPen( white );
    else
      painter.setPen( black );
    
    painter.drawText( 3, ypos + ( row_lay->height( m_pView ) + fm.ascent() - fm.descent() ) / 2, buffer );

    ypos += row_lay->height( m_pView );
  }
  
  painter.end();
}

/****************************************************************
 *
 * KSpreadHBorder
 *
 ****************************************************************/

KSpreadHBorder::KSpreadHBorder( QWidget *_parent, KSpreadView *_view ) : QWidget( _parent )
{
  m_pView = _view;
  
  setBackgroundColor( lightGray );
  setMouseTracking( TRUE );
  m_bResize = FALSE;
  m_bSelection = FALSE;
}

void KSpreadHBorder::mousePressEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );
  
  m_bResize = FALSE;
  m_bSelection = FALSE;
    
  int x = 0;
  int col = table->leftColumn( 0, x, m_pView );
    
  while ( x < width() && !m_bResize )
  {
    int w = table->columnLayout( col )->width( m_pView );
    col++;
    if ( _ev->pos().x() >= x + w - 1 && _ev->pos().x() <= x + w + 1 )
      m_bResize = TRUE;
    x += w;
  }
    
  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pView->canvasWidget() );
    painter.setRasterOp( NotROP );
    painter.drawLine( _ev->pos().x(), 0, _ev->pos().x(), m_pView->canvasWidget()->height() );
    painter.end();

    int tmp;
    m_iResizeAnchor = table->leftColumn( _ev->pos().x() - 3, tmp, m_pView );
    m_iResizePos = _ev->pos().x();
  }
  else
  {
    m_bSelection = TRUE;

    table->unselect();
    int tmp;
    int hit_col = table->leftColumn( _ev->pos().x(), tmp, m_pView );
    m_iSelectionAnchor = hit_col;	
    QRect r;
    r.setCoords( hit_col, 1, hit_col, 0x7FFF );
    table->setSelection( r, m_pView );
  }
}

void KSpreadHBorder::mouseReleaseEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pView->canvasWidget() );
    painter.setRasterOp( NotROP );
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pView->canvasWidget()->height() );
    painter.end();
	
    ColumnLayout *cl = table->nonDefaultColumnLayout( m_iResizeAnchor );
    int x = table->columnPos( m_iResizeAnchor, m_pView );
    if ( ( m_pView->zoom() * (float)( _ev->pos().x() - x ) ) < 20.0 )
      cl->setWidth( 20, m_pView );	    
    else
      cl->setWidth( _ev->pos().x() - x, m_pView );
	
    m_pView->doc()->setModified( true );
  }
    
  m_bSelection = FALSE;
  m_bResize = FALSE;
}

void KSpreadHBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );

  if ( m_bResize )
  {
    QPainter painter;
    painter.begin( m_pView->canvasWidget() );
    painter.setRasterOp( NotROP );
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pView->canvasWidget()->height() );

    m_iResizePos = _ev->pos().x();
    int twenty = (int)( 20.0 * m_pView->zoom() );
    // Dont make the column have a width < 20 pixels.
    int x = table->columnPos( m_iResizeAnchor, m_pView );
    if ( m_iResizePos < x + twenty )
      m_iResizePos = x + twenty;
    painter.drawLine( m_iResizePos, 0, m_iResizePos, m_pView->canvasWidget()->height() );
    painter.end();
  }
  else if ( m_bSelection )
  {
    int x = 0;
    int col = table->leftColumn( _ev->pos().x(), x, m_pView );
    QRect r = table->selectionRect();

    if ( col < m_iSelectionAnchor )
    {
      r.setLeft( col );
      r.setRight( m_iSelectionAnchor );
    }
    else
    {
      r.setRight( col );
      r.setLeft( m_iSelectionAnchor );
    }
    table->setSelection( r, m_pView );

    if ( _ev->pos().x() < 0 )
      m_pView->horzScrollBar()->setValue( m_pView->xOffset() + x );
    else if ( _ev->pos().x() > m_pView->canvasWidget()->width() )
    {
      ColumnLayout *cl = table->columnLayout( col + 1 );
      x = table->columnPos( col + 1, m_pView );
      m_pView->horzScrollBar()->setValue( m_pView->xOffset() + 
					  ( x + cl->width( m_pView ) - m_pView->canvasWidget()->width() ) );
    }
  }
  // Perhaps we have to modify the cursor
  else
  {
    int x = 0;
    int col = table->leftColumn( 0, x, m_pView );
    
    while ( x < m_pView->hBorderWidget()->width() )
    {
      int w = table->columnLayout( col )->width( m_pView );
      col++;
      if ( _ev->pos().x() >= x + w - 1 && _ev->pos().x() <= x + w + 1 )
      {
	m_pView->hBorderWidget()->setCursor( sizeAllCursor );
	return;
      }
      x += w;
    }
    m_pView->hBorderWidget()->setCursor( arrowCursor );
  }
}

void KSpreadHBorder::paintEvent( QPaintEvent* _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );

  QPainter painter;
  painter.begin( this );
  QPen pen;
  pen.setWidth( 1 );
  painter.setPen( pen );
  painter.setBackgroundColor( white );

  painter.eraseRect( _ev->rect() );

  QFontMetrics fm = painter.fontMetrics();
    
  int xpos;
  int left_col = table->leftColumn( _ev->rect().x(), xpos, m_pView );
  int right_col = table->rightColumn( _ev->rect().right(), m_pView );

  QRect selection( table->selectionRect() );
  
  for ( int x = left_col; x <= right_col; x++ )
  {
    bool selected = ( selection.left() != 0 && selection.bottom() == 0x7FFF &&
		      x >= selection.left() && x <= selection.right() );
	
    ColumnLayout *col_lay = table->columnLayout( x );

    if ( selected )
    {
      static QColorGroup g2( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill2( black );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pView ), XBORDER_HEIGHT, g2, FALSE, 1, &fill2 );
    }
    else
    {
      static QColorGroup g( black, white, white, darkGray, lightGray, black, black );
      static QBrush fill( lightGray );
      qDrawShadePanel( &painter, xpos, 0, col_lay->width( m_pView ), XBORDER_HEIGHT, g, FALSE, 1, &fill );
    }

    int len = fm.width( table->columnLabel(x) );
    
    if ( selected )
      painter.setPen( white );
    else
      painter.setPen( black );
	
    painter.drawText( xpos + ( col_lay->width( m_pView ) - len ) / 2,
		      ( XBORDER_HEIGHT + fm.ascent() - fm.descent() ) / 2,
		      table->columnLabel(x) );

    xpos += col_lay->width( m_pView );
  }

  painter.end();
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

