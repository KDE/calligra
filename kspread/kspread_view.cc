#include "kspread_view.h"

#include <utils.h>
#include <kapp.h>
#include <qpushbt.h>
#include <qmsgbox.h>
#include <iostream.h>
#include <stdlib.h>
#include <qobjcoll.h>
#include <qkeycode.h>
#include <kbutton.h>
#include <klocale.h>

#include <koPartSelectDia.h>
#include <koPrintDia.h>
#include <koAboutDia.h>
#include "kspread_map.h"
#include "kspread_table.h"
#include "kspread_dlg_scripts.h"
#include "kspread_doc.h"
#include "kspread_shell.h"

/*****************************************************************************
 *
 * KSpreadView
 *
 *****************************************************************************/

KSpreadScripts* KSpreadView::m_pGlobalScriptsDialog = 0L;

KSpreadView::KSpreadView( QWidget *_parent, const char *_name, KSpreadDoc* _doc ) :
  QWidget( _parent, _name ), View_impl(), KSpread::View_skel()
{
  setWidget( this );

  Control_impl::setFocusPolicy( OPControls::Control::ClickFocus ); 

  m_lstFrames.setAutoDelete( true );  

  m_pDoc = _doc;

  m_bShowGUI = true;
  m_bMarkerVisible = false;

  m_iXOffset = 0;
  m_iYOffset = 0;

  m_bUndo = false;
  m_bRedo = false;
    
  m_pTable = 0L;
  m_fZoom = 1.0;

  m_iMarkerColumn = 1;
  m_iMarkerRow = 1;

  m_bMarkerVisible = true;

  m_pPopupMenu = 0L;
}

void KSpreadView::createGUI()
{      
  m_defaultGridPen.setColor( lightGray );
  m_defaultGridPen.setWidth( 1 );
  m_defaultGridPen.setStyle( SolidLine );
  
  // Toolbar
  m_vToolBarFactory = m_vPartShell->toolBarFactory();
  if ( !CORBA::is_nil( m_vToolBarFactory ) )
  {
    /* m_rToolBarFile = m_vToolBarFactory->createToolBar( this, CORBA::string_dup( "File" ) );
    m_rToolBarFile->setFileToolBar( true );

    QString tmp = kapp->kde_toolbardir().copy();
    tmp += "/fileopen.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonFile_Open = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( "Open" ) );
  
    tmp = kapp->kde_toolbardir().copy();
    tmp += "/filefloppy.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFile_Save = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( "Save" ) );

    tmp = kapp->kde_toolbardir().copy();
    tmp += "/fileprint.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFile_Print = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( "Print" ) );
  
    m_rToolBarFile->insertSeparator();

    tmp = kapp->kde_toolbardir().copy();
    tmp += "/viewmag-.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFile_ZoomOut = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( "Zoom Out" ) );

    tmp = kapp->kde_toolbardir().copy();
    tmp += "/viewmag+.xpm";
    pix = loadPixmap( tmp );
    m_idButtonFile_ZoomIn = m_rToolBarFile->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( "Zoom In" ) );
    */
    m_rToolBarEdit = m_vToolBarFactory->createToolBar( this, CORBA::string_dup( "Edit" ) );

    QString tmp = kapp->kde_toolbardir().copy();
    tmp += "/editcopy.xpm";
    QString pix = loadPixmap( tmp );
    m_idButtonEdit_Copy = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Copy" ) ),
							this, "copySelection" );

    tmp = kapp->kde_toolbardir().copy();
    tmp += "/editcut.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_Cut = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Cut" ) ),
						       this, "cutSelection" );

    tmp = kapp->kde_toolbardir().copy();
    tmp += "/editpaste.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_Paste = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Paste" ) ),
							 this, "paste" );

    m_rToolBarEdit->insertSeparator();

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/rowout.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_DelRow = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Delete Row" ) ),
							  this, "deleteRow" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/colout.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_DelCol = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Delete Column" ) ),
							  this, "deleteColumn" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/rowin.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_InsRow = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Insert Row" ) ),
							  this, "insertRow" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/colin.xpm";
    pix = loadPixmap( tmp );
    m_idButtonEdit_InsCol = m_rToolBarEdit->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Insert Column" ) ),
							  this, "insertColumn" );

    m_rToolBarLayout = m_vToolBarFactory->createToolBar( this, CORBA::string_dup( i18n( "Layout" ) ) );

    m_idComboLayout_Font = m_rToolBarLayout->insertCombo( false, CORBA::string_dup( i18n( "Font" ) ), 120, this, "fontSelected" );
    m_rToolBarLayout->insertComboItem ( m_idComboLayout_Font, CORBA::string_dup( "Courier" ), -1 );
    m_rToolBarLayout->insertComboItem ( m_idComboLayout_Font, CORBA::string_dup( "Helvetica" ), -1 );
    m_rToolBarLayout->insertComboItem ( m_idComboLayout_Font, CORBA::string_dup( "Symbol" ), -1 );
    m_rToolBarLayout->insertComboItem ( m_idComboLayout_Font, CORBA::string_dup( "Times" ), -1 );
  
    m_idComboLayout_FontSize = m_rToolBarLayout->insertCombo( false, CORBA::string_dup( i18n( "Font Size" ) ), 50,
							      this, "fontSizeSelected" );
    int sizes[24] = { 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 22, 24, 26, 28, 32, 48, 64 };
    for( int i = 0; i < 24; i++ )
    {
      char buffer[ 10 ];
      sprintf( buffer, "%i", sizes[i] );
      m_rToolBarLayout->insertComboItem ( m_idComboLayout_FontSize, CORBA::string_dup( buffer ), -1 );
    }

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/bold.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Bold = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Bold" ) ),
							    this, "bold" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/italic.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Italic = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Italic" ) ),
							      this, "italic" );

    m_rToolBarLayout->insertSeparator();

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/money.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Money = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Money Format" ) ),
							     this, "moneyFormat" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/percent.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Percent = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Percent Format" ) ),
							       this, "percent" );

    m_rToolBarLayout->insertSeparator();

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/left.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Left = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Align Left" ) ),
							    this, "alignLeft" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/center.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Center = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Align Center" ) ),
							      this, "alignCenter" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/right.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Right = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Align Right" ) ),
							     this, "alignRight" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/multirow.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_MultiRows = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ),
								 CORBA::string_dup( i18n( "Allow multiple lines" ) ),
								 this, "multiRow" );

    m_rToolBarLayout->insertSeparator();

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/precminus.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_PrecMinus = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ),
								 CORBA::string_dup( i18n( "Lower Precision" ) ),
								 this, "precisionMinus" );

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/precplus.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_PrecPlus = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), 
								CORBA::string_dup( i18n( "Higher Precision" ) ),
								this, "precisionPlus" );

    m_rToolBarLayout->insertSeparator();

    tmp = kapp->kde_datadir().copy();
    tmp += "/kspread/pics/chart.xpm";
    pix = loadPixmap( tmp );
    m_idButtonLayout_Chart = m_rToolBarLayout->insertButton( CORBA::string_dup( pix ), CORBA::string_dup( i18n( "Insert Chart" ) ),
							     this, "insertChart" );  
  }
  
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
  m_pTabBarFirst = newIconButton( "/kspread/pics/tab_first.xpm" );
  QObject::connect( m_pTabBarFirst, SIGNAL( clicked() ), SLOT( slotScrollToFirstTable() ) );
  m_pTabBarLeft = newIconButton( "/kspread/pics/tab_left.xpm" );    
  QObject::connect( m_pTabBarLeft, SIGNAL( clicked() ), SLOT( slotScrollToLeftTable() ) );
  m_pTabBarRight = newIconButton( "/kspread/pics/tab_right.xpm" );    
  QObject::  connect( m_pTabBarRight, SIGNAL( clicked() ), SLOT( slotScrollToRightTable() ) );
  m_pTabBarLast = newIconButton( "/kspread/pics/tab_last.xpm" );    
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
  
  m_pCancelButton = newIconButton( "/kspread/pics/abort.xpm", TRUE, m_pToolWidget );
  m_pCancelButton->setGeometry( 60, 2, 26, 26 );
  m_pOkButton = newIconButton( "/kspread/pics/done.xpm", TRUE, m_pToolWidget );     
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

  /******************************************************
   * Menu
   ******************************************************/

  m_vMenuBarFactory = m_vPartShell->menuBarFactory();
  if ( !CORBA::is_nil( m_vMenuBarFactory ) )
  {
    // Menubar
    m_rMenuBar = m_vMenuBarFactory->createMenuBar( this );

    // Edit
    m_idMenuEdit = m_rMenuBar->insertMenu( CORBA::string_dup( i18n( "&Edit" ) ) );
    m_idMenuEdit_Undo = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "Un&do" ) ), m_idMenuEdit,
						       this, CORBA::string_dup( "undo" ) );
    m_idMenuEdit_Redo = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "&Redo" ) ), m_idMenuEdit,
						       this, CORBA::string_dup( "redo" ) );

    m_rMenuBar->insertSeparator( m_idMenuEdit );

    QString path = kapp->kde_toolbardir().copy();
    path += "/editcut.xpm";
    QString pix = loadPixmap( path );
    m_idMenuEdit_Cut = m_rMenuBar->insertItemP( CORBA::string_dup( pix ), i18n( "C&ut" ), m_idMenuEdit, this,
						CORBA::string_dup( "cutSelection" ) );

    path = kapp->kde_toolbardir().copy();
    path += "/editcopy.xpm";
    pix = loadPixmap( path );
    m_idMenuEdit_Copy = m_rMenuBar->insertItemP( CORBA::string_dup( pix ), i18n( "&Copy" ), m_idMenuEdit, this,
						 CORBA::string_dup( "copySelection" ) );

    path = kapp->kde_toolbardir().copy();
    path += "/editpaste.xpm";
    pix = loadPixmap( path );
    m_idMenuEdit_Paste = m_rMenuBar->insertItemP( CORBA::string_dup( pix ), i18n( "&Paste" ), m_idMenuEdit, this,
						  CORBA::string_dup( "paste" ) );

    m_rMenuBar->insertSeparator( m_idMenuEdit );

    m_idMenuEdit_Insert = m_rMenuBar->insertSubMenu( i18n( "&Insert" ), m_idMenuEdit );
    m_idMenuEdit_Insert_Table = m_rMenuBar->insertItem( i18n( "&Table" ), m_idMenuEdit_Insert, this, "insertTable" );
    m_idMenuEdit_Insert_Table = m_rMenuBar->insertItem( i18n( "&Image" ), m_idMenuEdit_Insert, this, "insertImage" );
    m_idMenuEdit_Insert_Table = m_rMenuBar->insertItem( i18n( "&Chart" ), m_idMenuEdit_Insert, this, "insertChart" );
    m_idMenuEdit_Insert_Table = m_rMenuBar->insertItem( i18n( "&Object ..." ), m_idMenuEdit_Insert, this, "insertObject" );
    
    m_rMenuBar->insertSeparator( m_idMenuEdit );

    m_idMenuEdit_Cell = m_rMenuBar->insertItem( "C&ell", m_idMenuEdit, this, CORBA::string_dup( "editCell" ) );
	
    m_rMenuBar->insertSeparator( m_idMenuEdit );

    m_idMenuEdit_Layout = m_rMenuBar->insertItem( "Paper &Layout", m_idMenuEdit, this, CORBA::string_dup( "paperLayoutDlg" ) );

  /* insertMenu = new QPopupMenu;
     CHECK_PTR( insertMenu );
     QList<KPartEntry>& list = xclPart->shell()->availableParts();
     KPartEntry *e;
	for ( e = list.first(); e != 0L; e = list.next() )
	    if ( !e->isSeamless() )
	    {
		lstToolbarParts.append( e );
		insertMenu->insertItem( e->partName() );
	    }
	
	connect( insertMenu, SIGNAL( activated( int ) ), this, SLOT( slotInsertPart( int ) ) );
	*/
    
    // View
    m_idMenuView = m_rMenuBar->insertMenu( CORBA::string_dup( i18n( "&View" ) ) );

    m_rMenuBar->setCheckable( m_idMenuView, true );
    m_idMenuView_NewView = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "New View" ) ), m_idMenuView, this,
							 CORBA::string_dup( "newView" ) );

    m_rMenuBar->insertSeparator( m_idMenuView );
    m_idMenuView_ShowPageBorders = m_rMenuBar->insertItem( CORBA::string_dup( i18n( "Show Page Borders" ) ), m_idMenuView, this,
							 CORBA::string_dup( "togglePageBorders" ) );
	
    // Folder
    m_idMenuFolder = m_rMenuBar->insertMenu( CORBA::string_dup( i18n( "F&older" ) ) );

    m_idMenuFolder_NewTable = m_rMenuBar->insertItem( i18n( "New Table" ), m_idMenuFolder, this, CORBA::string_dup( "insertNewTable" ) );

    // Format
    m_idMenuFormat = m_rMenuBar->insertMenu( CORBA::string_dup( i18n( "Fo&rmat" ) ) );

    m_idMenuFormat_AutoFill = m_rMenuBar->insertItem( i18n( "&Auto Fill ..." ), m_idMenuFormat, this, CORBA::string_dup( "autoFill" ) );

    // Scripts
    m_idMenuScripts = m_rMenuBar->insertMenu( CORBA::string_dup( i18n( "&Scripts" ) ) );

    m_idMenuScripts_EditGlobal = m_rMenuBar->insertItem( i18n( "Edit &global scripts..." ), m_idMenuScripts, this,
							 CORBA::string_dup( "editGlobalScripts" ) );
    m_idMenuScripts_EditLocal = m_rMenuBar->insertItem( i18n( "Edit &local scripts..." ), m_idMenuScripts, this,
							 CORBA::string_dup( "editLocalScripts" ) );
    m_idMenuScripts_Reload = m_rMenuBar->insertItem( i18n( "&Reload scripts" ), m_idMenuScripts, this,
							 CORBA::string_dup( "reloadScripts" ) );

    // Help
    m_idMenuHelp = m_rMenuBar->insertMenu( CORBA::string_dup( i18n( "&Help" ) ) );

    m_idMenuHelp_About = m_rMenuBar->insertItem( i18n( "&About" ), m_idMenuHelp, this, CORBA::string_dup( "helpAbout" ) );
    m_idMenuHelp_Using = m_rMenuBar->insertItem( i18n( "&Using KSpread" ), m_idMenuHelp, this, CORBA::string_dup( "helpUsing" ) );
	
    enableUndo( false );
    enableRedo( false );

    KSpreadTable *tbl;
    for ( tbl = m_pDoc->map()->firstTable(); tbl != 0L; tbl = m_pDoc->map()->nextTable() )
      addTable( tbl );
  }      

  QObject::connect( m_pDoc, SIGNAL( sig_addTable( KSpreadTable* ) ), SLOT( slotAddTable( KSpreadTable* ) ) );
}

KSpreadView::~KSpreadView()
{
  m_pDoc->removeView( this );
}

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

void KSpreadView::helpAbout()
{
  KoAboutDia::about( KoAboutDia::KSpread, "0.0.1" );
}

void KSpreadView::helpUsing()
{
  kapp->invokeHTMLHelp( "kspread/kspread.html", 0 );
}

QButton * KSpreadView::newIconButton( const char *_file, bool _kbutton, QWidget *_parent )
{
  if ( _parent == 0L )
    _parent = this;
    
  QPixmap *pixmap = 0L;
    
  QString filename = kapp->kde_datadir().copy();
  filename += _file;        
  pixmap = new QPixmap();
  pixmap->load( filename.data() );
    
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
  m_rMenuBar->setItemEnabled( m_idMenuEdit_Undo, _b );
  m_bUndo = _b;
}

void KSpreadView::enableRedo( bool _b )
{
  m_rMenuBar->setItemEnabled( m_idMenuEdit_Redo, _b );
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
  m_pDoc->reloadScripts();
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

   m_lstFrames.clear();
   QListIterator<KSpreadChild> it = m_pTable->childIterator();
   for( ; it.current(); ++it )
     slotInsertChild( it.current() );

   m_pVBorderWidget->repaint();
   m_pHBorderWidget->repaint();
   m_pCanvasWidget->repaint();
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

void KSpreadView::newView()
{
  assert( (m_pDoc != 0L) );

  KSpreadShell_impl* shell = new KSpreadShell_impl;
  shell->enableMenuBar();
  shell->PartShell_impl::enableStatusBar();
  shell->enableToolBars();
  shell->show();
  shell->setDocument( m_pDoc );
  
  CORBA::release( shell );
}

CORBA::Boolean KSpreadView::printDlg()
{
  // Open some printer dialog
  QPrinter prt;
  if ( !KoPrintDia::print( prt, m_pDoc, SLOT( paperLayoutDlg() ) ) )
    return true;
  
  QPainter painter;
  painter.begin( &prt );
  // Print the table and tell that m_pDoc is NOT embedded.
  m_pTable->print( painter, FALSE, &prt );
  painter.end();

  return true;
}

void KSpreadView::insertChart( const QRect& _geometry )
{
}

void KSpreadView::insertChild( const QRect& _geometry, const char *_arg )
{
  m_pTable->insertChild( _geometry, _arg );
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

  OPParts::Document_var doc = _child->document();
  OPParts::View_var v;

  try
  { 
    /* CORBA::Object_var obj;
    obj = doc->createView();
    if( !CORBA::is_nil( obj ) )
    {
      // Narrow by hand
      CORBA::Object_ptr p = obj;
      OPParts::View_ptr view_stub = new OPParts::View_stub;
      view_stub->CORBA::Object::operator=( *p );
      v = view_stub;
    }
    else
      cerr << "Shit! We did not get a view!!!" << endl; */
    cout << "SYNC1\n" << endl;
    v = doc->createView();
    cout << "SYNC2\n" << endl;
  }
  catch ( OPParts::Document::MultipleViewsNotSupported &_ex )
  {
    // HACK
    printf("void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("Could not create view\n");
    exit(1);
  }

  if ( CORBA::is_nil( v ) )
  {
    printf("void KSpreadView::slotInsertObject( const QRect& _rect, OPParts::Document_ptr _doc )\n");
    printf("return value is 0L\n");
    exit(1);
  }

  v->setMode( OPParts::Part::ChildMode );
  v->setPartShell( partShell() );

  cout << "SYNC3\n" << endl;
  KSpreadChildFrame *p = new KSpreadChildFrame( this, _child );
  cout << "SYNC3b\n" << endl;
  p->show();
  cout << "SYNC4\n" << endl;
  p->setGeometry( _child->geometry() );
  cout << "SYNC5\n" << endl;
  p->attach( v );
  cout << "SYNC6\n" << endl;

  m_lstFrames.append( p );
  
  QObject::connect( p, SIGNAL( sig_geometryEnd( PartFrame_impl* ) ),
		    this, SLOT( slotChildGeometryEnd( PartFrame_impl* ) ) );
  QObject::connect( p, SIGNAL( sig_moveEnd( PartFrame_impl* ) ),
		    this, SLOT( slotChildMoveEnd( PartFrame_impl* ) ) );  
} 

void KSpreadView::slotChildGeometryEnd( PartFrame_impl* _frame )
{
  // ATTENTION: This is an upcast
  KSpreadChildFrame *f = (KSpreadChildFrame*)_frame;
  // TODO zooming
  m_pTable->changeChildGeometry( f->child(), _frame->partGeometry() );
}

void KSpreadView::slotChildMoveEnd( PartFrame_impl* _frame )
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
   
   m_rMenuBar->setItemChecked( m_idMenuView_ShowPageBorders, !m_pTable->isShowPageBorders() );
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

void KSpreadView::resizeEvent( QResizeEvent * )
{
  // HACK
  if ( x() == 5000 && y() == 5000 )
    return;
  
  if ( m_bShowGUI )
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
    if ( m_pPopupMenu != 0L )
	delete m_pPopupMenu;
    
    m_pPopupMenu = new QPopupMenu();

    m_pPopupMenu->insertItem( "Copy", this, SLOT( slotCopy() ) );
    m_pPopupMenu->insertItem( "Cut", this, SLOT( slotCut() ) );
    m_pPopupMenu->insertItem( "Paste", this, SLOT( slotPaste() ) );
    m_pPopupMenu->insertItem( "Delete", this, SLOT( slotDelete() ) );
    m_pPopupMenu->insertSeparator();
    m_pPopupMenu->insertItem( "Layout", this, SLOT( slotLayoutDlg() ) );

    m_pPopupMenu->popup( _point );
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
  QRect selection( m_pTable->selection() );
  
  if ( selection.contains( QPoint( m_iMarkerColumn, m_iMarkerRow ) ) )
    CellLayoutDlg dlg( this, m_pTable, selection.left(), selection.top(),
		       selection.right(), selection.bottom() );
  else
    CellLayoutDlg dlg( this, m_pTable, m_iMarkerColumn, m_iMarkerRow, m_iMarkerColumn, m_iMarkerRow );

  m_pDoc->setModified( true );
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
  m_pCanvasWidget->setAction( KSpreadCanvas::InsertChild, "KSpread" );
}

void KSpreadView::insertImage()
{
  m_pCanvasWidget->setAction( KSpreadCanvas::InsertChild, "KImage" );
}

void KSpreadView::insertObject()
{
  KoPartEntry* pe = KoPartSelectDia::selectPart();
  if ( !pe )
    return;
  
  m_pCanvasWidget->setAction( KSpreadCanvas::InsertChild, pe->name() );
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
  
  int dx = xOffset() - _value;
  m_iXOffset = _value;
  m_pCanvasWidget->scroll( dx, 0 );
  m_pHBorderWidget->scroll( dx, 0 );
}

void KSpreadView::slotScrollVert( int _value )
{
  if ( m_pTable == 0L )
    return;
  
  int dy = yOffset() - _value;
  m_iYOffset = _value;
  m_pCanvasWidget->scroll( 0, dy );
  m_pVBorderWidget->scroll( 0, dy );
}

/* void KSpreadView::setText( const char *_text )
{
  assert( m_pTable );
  
  if ( !isMarkerVisible() )
    return;
  
  m_pDoc->setModified( true );
  
  KSpreadCell *cell = nonDefaultCell( markerColumn, markerRow );
  
  KSpreadUndoSetText *undo;
  if ( !m_pDoc->undoBuffer()->isLocked() )
  {
    undo = new KSpreadUndoSetText( m_pDoc, m_pTable, cell->text(), markerColumn, markerRow );
    m_pDoc->undoBuffer()->appendUndo( undo );
  }
  
  cell->setText( _text );
  
  drawVisibleCells();
} */

void KSpreadView::setText( const char *_text )
{
  if ( m_pTable == 0L )
    return;
  
  m_pTable->setText( m_iMarkerRow, m_iMarkerColumn, _text );
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

void KSpreadView::drawVisibleCells()
{
  if ( isMarkerVisible() )
    drawMarker();

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
  
  if ( isMarkerVisible() )
  {
    painter.restore();
    drawMarker( &painter );
  }

  painter.end();
}

void KSpreadView::drawMarker( QPainter * _painter )
{
  if ( !isMarkerVisible() )
    return;
    
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
  QRect selection( m_pTable->selection() );
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
    
    if ( isMarkerVisible() )
      drawMarker( &painter );
    QRect rect( 0, 0, m_pCanvasWidget->width(), m_pCanvasWidget->height() );

    QRect r;
    _cell->paintEvent( this, rect, painter, _col, _row, &r );

    painter.restore();
    if ( isMarkerVisible() )
	drawMarker( &painter );    
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

void KSpreadView::marker( int &row, int &column)
{
  if ( !isMarkerVisible() )
  {
    row=-1;
    column=-1;
  }
  else
  {
    row = m_iMarkerRow;
    column=m_iMarkerColumn;
  }
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

  QPainter painter;
  painter.begin( m_pCanvasWidget );
  
  int x,y;
  for( y = _rect.top(); y <= _rect.bottom(); y++ )
    for( x = _rect.left(); x <= _rect.right(); x++ )
      drawCell( painter, m_pTable->cellAt( x, y ), x, y );

  painter.end();
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
  printf("void KSpreadView::slotChangeSelection( KSpreadTable *_table, const QRect &_old %i %i|%i %i, const QRect &_new %i %i|%i %i )\n",_old.left(),_old.top(),_old.right(),_old.bottom(),_new.left(),_new.top(),_new.right(),_new.bottom());
  
  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  // TODO: This is VERY slow
  m_pCanvasWidget->update();
}

void KSpreadView::slotUpdateCell( KSpreadTable *_table, KSpreadCell *_cell, int _col, int _row )
{
  printf("void KSpreadView::slotUpdateCell( KSpreadTable *_table, KSpreadCell *_cell, _col=%i, _row=%i )\n",_col,_row);
  
  // Do we display this table ?
  if ( _table != m_pTable )
    return;

  drawCell( _cell, _col, _row );
}

void KSpreadView::slotUnselect( KSpreadTable *_table, const QRect& _old )
{
  // Do we display this table ?
  if ( _table != m_pTable )
    return;
  
  bool marker = isMarkerVisible();
  if ( marker )
  {
    drawMarker();
    // TODO
    // markerIsVisible = FALSE;
  } 

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
    m_pHBorderWidget->repaint();	
    // Are complete rows selected ?
  if ( _old.right() == 0x7FFF )
    m_pHBorderWidget->repaint();

  if ( marker )
  {
    // TODO
    // markerIsVisible = TRUE;
    drawMarker();
  }
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
  QRect selection( m_pView->activeTable()->selection() );
  
  if ( _act == InsertChart )
  {    
    // Something must be selected
    if ( selection.right() == 0x7fff || selection.bottom() == 0x7fff || selection.left() == 0 )
    {
      QMessageBox::critical( this, i18n("KSpread Error" ), i18n("You must first select the cells\nwhich contain the data." ),
			     i18n( "Ok" ) );
      return;
    }
  }
  
  m_eAction = _act;
}

void KSpreadCanvas::setAction( Actions _act, const char *_arg )
{
  m_strActionArgument = _arg;
  setAction( _act );
}

void KSpreadCanvas::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  QRect selection( table->selection() );
  
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
    
  if ( m_pView->isMarkerVisible() )
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

  if ( m_pView->isMarkerVisible() )
  {
    m_pView->drawMarker();
    // TODO
    // markerIsVisible = FALSE;
  }
    
  // Set the new lower right corner of the selection
  selection.setRight( col );
  selection.setBottom( row );
  table->setSelection( selection );
  
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
      m_pView->insertChild( r, m_strActionArgument );
	      
    m_eMouseAction = NoAction;
    m_eAction = DefaultAction;
    return;
  }
  
  QRect selection( table->selection() );
  
  // m_pView->canvasWidget()->setMouseTracking( FALSE );
  // The user started the drag in the lower right corner of the marker ?
  if ( m_eMouseAction == ResizeCell )
  {
    KSpreadCell *cell = table->nonDefaultCell( m_iMouseStartColumn, m_iMouseStartRow );
    cell->forceExtraCells( m_iMouseStartColumn, m_iMouseStartRow, 
			   selection.right() - selection.left(), 
			   selection.bottom() - selection.top() );
	
    selection.setCoords( 0, 0, 0, 0 );
    table->setSelection( selection );
    m_pView->doc()->setModified( TRUE );
  }
  else if ( m_eMouseAction == AutoFill )
  {
	/* Object *obj = getNonDefaultCell( mouseStartColumn, mouseStartRow );
	obj->forceExtraCells( mouseStartColumn, mouseStartRow, 
			      selection.right() - selection.left(), 
			      selection.bottom() - selection.top() ); */
    table->autofill( m_rctAutoFillSrc, table->selection() );
	
    m_pView->doc()->setModified( TRUE );
    selection.setCoords( 0, 0, 0, 0 );
    table->setSelection( selection );
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
      table->setSelection( selection );
    }
  }
    
  m_eMouseAction = NoAction;
  if ( m_bMouseMadeSelection )
    m_pView->drawVisibleCells();
  m_bMouseMadeSelection = FALSE;

  if ( !m_pView->isMarkerVisible() )
  {
    // TODO
    // markerIsVisible = TRUE;
    m_pView->drawMarker();
  }
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
    // m_eAction = DefaultAction;
    m_eMouseAction = ChildGeometry;
      
      /* KPart *bp = pKSpread->shell()->newPart( "kchart", m_pView->canvasWidget() );

	ChartPart *cp = (ChartPart*)bp;
	cp->visual()->hide();

	cp->setKSpreadTable( this );
	QString r = "A1:D3";
	if ( selection.left() != 0 && selection.right() != 0x7fff && selection.bottom() != 0x7fff )
	{
	    QString t = table->columnLabel( selection.left() );
	    QString t2 = table->columnLabel( selection.right() );
	    r.sprintf( "%s%i:%s%i", t.data(), selection.top(), t2.data(), selection.bottom() );
	}
	
	if ( cp->wizard( r ) )
	{
	    cp->visual()->setGeometry( _ev->pos().x(), _ev->pos().y(), 200, 200 );
	    cp->visual()->show();
	    appendPart( cp );
	    unselect();
	}
	else
	    delete bp;
	    */
	return;
  }
  
  QRect selection( table->selection() );
  
  // Get the position and size of the marker
  int xpos;
  int ypos;
  int w, h;
  if ( selection.left() == 0 || selection.right() == 0x7fff || selection.bottom() == 0x7fff )
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
    
  // Did we click in the lower right corner of the marker ?
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

    table->setSelection( selection );
    return;
  }

  if ( m_pView->isMarkerVisible() )
  {
    m_pView->drawMarker();
    // TODO
    // markerIsVisible = FALSE;
  }
    
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
    // TODO
    // markerIsVisible = TRUE;
    m_pView->drawMarker();
	
    m_eMouseAction = Mark;
    selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(),
			 m_pView->markerColumn() + cell->extraXCells(),
			 m_pView->markerRow() + cell->extraYCells() );
    table->setSelection( selection );
    m_iMouseStartColumn = m_pView->markerColumn();
    m_iMouseStartRow = m_pView->markerRow();
    // m_pView->canvasWidget()->setMouseTracking( TRUE );
    return;
  }
  
  // Update the edit box
  if ( cell->text() != 0L )
    m_pView->editWidget()->setText( cell->text() );
  else
    m_pView->editWidget()->setText( "" );
    
  // TODO
  // markerIsVisible = TRUE;
  m_pView->drawMarker();
    
  if ( _ev->button() == RightButton )
  {
    QPoint p = mapToGlobal( _ev->pos() );
    m_pView->openPopupMenu( p );
  }
}

void KSpreadCanvas::paintEvent( QPaintEvent* _ev )
{
  // HACK
  if ( x() == 5000 && y() == 5000 )
    return;

  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  if ( m_pView->isMarkerVisible() )
    m_pView->drawMarker();

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
  
  if ( m_pView->isMarkerVisible() )
    m_pView->drawMarker( &painter );

  painter.end();
}

void KSpreadCanvas::keyPressEvent ( QKeyEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  if ( !table )
    return;

  int x, y;
  RowLayout *rl;
  ColumnLayout *cl;
  KSpreadCell *cell;
  // Flag that indicates wether we make a selection right now
  bool make_select = FALSE;
  
  QRect selection( table->selection() );
  
  // Are we making a selection right now ? Go thru this only if no selection is made
  // or if we neither selected complete rows nor columns.
  if ( ( _ev->state() & ShiftButton ) == ShiftButton &&
       ( selection.left() == 0 || ( selection.right() != 0 && selection.bottom() != 0 ) ) && 
       ( _ev->key() == Key_Down || _ev->key() == Key_Up || _ev->key() == Key_Left || _ev->key() == Key_Right ) )
    make_select = TRUE;

  // Do we have an old selection ? If yes, unselect everything
  if ( selection.left() != 0 && !make_select )
    table->unselect();
    
  switch( _ev->key() )
    {
    case Key_Return:
    case Key_Enter:
    case Key_Down:
	_ev->accept();
	if ( m_pView->markerRow() == 0xFFFF )
	  return;

	if ( m_bEditDirtyFlag )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  m_bEditDirtyFlag = FALSE;
	}
	
	m_pView->drawMarker();
	// TODO
	// markerIsVisible = FALSE;

	if ( selection.left() == 0 && make_select )
	{
	    selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(), m_pView->markerColumn(), m_pView->markerRow() );
	    // cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	    // m_pView->drawCell( cell, m_pView->markerColumn(), m_pView->markerRow(), TRUE );
	}

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	    m_pView->setMarkerRow( m_pView->markerRow() + 1 + cell->extraYCells() );
	else
	    m_pView->setMarkerRow( m_pView->markerRow() + 1 );

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	    m_pView->setMarkerRow( cell->obscuringCellsRow() );
	    m_pView->setMarkerColumn( cell->obscuringCellsColumn() );
	}

	y = table->rowPos( m_pView->markerRow(), m_pView );
	rl = table->rowLayout( m_pView->markerRow() );
	if ( y + rl->height( m_pView ) > height() )
	  m_pView->vertScrollBar()->setValue( m_pView->yOffset() + ( y + rl->height( m_pView ) - height() ) );

	// If we have been at the top of the selection ...
	if ( selection.top() == m_pView->markerRow() - 1 && selection.top() != selection.bottom() && make_select )
	{
	    selection.setTop( m_pView->markerRow() );
	    /* for ( int i = selection.left(); i <= selection.right(); i++ )
	    {
	      cell = table->cellAt( i, m_pView->markerRow() - 1 );
	      m_pView->drawCell( cell, i, m_pView->markerRow() - 1, TRUE );
	    } */
	}
	else if ( make_select )
	{
	    selection.setBottom( m_pView->markerRow() );
	    /* for ( int i = selection.left(); i <= selection.right(); i++ )
	    {
		cell = table->cellAt( i, m_pView->markerRow() );
		m_pView->drawCell( cell, i, m_pView->markerRow(), TRUE );
	    } */
	}

	if ( make_select )
	  table->setSelection( selection );
	
	// TODO
	// markerIsVisible = TRUE;
	m_pView->drawMarker();

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	if ( cell->text() != 0L )
	    m_pView->editWidget()->setText( cell->text() );
	else
	    m_pView->editWidget()->setText( "" );

	break;

    case Key_Up:
	_ev->accept();
	if ( m_pView->markerRow() == 1 )
	    return;

	if ( m_bEditDirtyFlag )
	{
	    m_pView->setText( m_pView->editWidget()->text() );
	    m_bEditDirtyFlag = FALSE;
	}
	
	m_pView->drawMarker();
	// TODO
	// markerIsVisible = FALSE;

	if ( selection.left() == 0 && make_select )
	{
	    selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(), m_pView->markerColumn(), m_pView->markerRow() );
	    // cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	    // m_pView->drawCell( cell, m_pView->markerColumn(), m_pView->markerRow(), TRUE );
	}

	m_pView->setMarkerRow( m_pView->markerRow() - 1 );
	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	    m_pView->setMarkerRow( cell->obscuringCellsRow() );
	    m_pView->setMarkerColumn( cell->obscuringCellsColumn() );
	}
	
	y = table->rowPos( m_pView->markerRow(), m_pView );
	rl = table->rowLayout( m_pView->markerRow() );
	if ( y < 0 )
	    m_pView->vertScrollBar()->setValue( m_pView->yOffset() + y );

	// If we have been at the top of the selection ...
	if ( selection.top() == m_pView->markerRow() + 1 && make_select )
	{
	    selection.setTop( m_pView->markerRow() );
	    /* for ( int i = selection.left(); i <= selection.right(); i++ )
	    {
		obj = table->cellAt( i, m_pView->markerRow() );
		drawObject( obj, i, m_pView->markerRow(), TRUE );
	    } */
	}
	else if ( make_select )
	{
	    selection.setBottom( m_pView->markerRow() );
	    /* for ( int i = selection.left(); i <= selection.right(); i++ )
	    {
		obj = table->cellAt( i, m_pView->markerRow() + 1 );
		drawObject( obj, i, m_pView->markerRow() + 1, TRUE );
	    } */
	}

	if ( make_select )
	  table->setSelection( selection );

	// TODO
	// markerIsVisible = TRUE;
	m_pView->drawMarker();

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	if ( cell->text() != 0L )
	    m_pView->editWidget()->setText( cell->text() );
	else
	    m_pView->editWidget()->setText( "" );

	break;

    case Key_Left:
	_ev->accept();
	if ( m_pView->markerColumn() == 1 )
	    return;

	if ( m_bEditDirtyFlag )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  m_bEditDirtyFlag = FALSE;
	}

	m_pView->drawMarker();
	// TODO
	// markerIsVisible = FALSE;
	
	if ( selection.left() == 0 && make_select )
	{
	    selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(), m_pView->markerColumn(), m_pView->markerRow() );
	    // obj = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	    // drawObject( obj, m_pView->markerColumn(), m_pView->markerRow(), TRUE );
	}

	m_pView->setMarkerColumn( m_pView->markerColumn() - 1 );
	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	// Go to the upper left corner of the obscuring object
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	    m_pView->setMarkerRow( cell->obscuringCellsRow() );
	    m_pView->setMarkerColumn( cell->obscuringCellsColumn() );
	}

	x = table->columnPos( m_pView->markerColumn(), m_pView );
	cl = table->columnLayout( m_pView->markerColumn() );
	if ( x < 0 )
	  m_pView->horzScrollBar()->setValue( m_pView->xOffset() + x );

	// If we have been at the left side of the selection ...
	if ( selection.left() == m_pView->markerColumn() + 1 && make_select )
	{
	    selection.setLeft( m_pView->markerColumn() );
	    /* for ( int i = selection.top(); i <= selection.bottom(); i++ )
	    {
		obj = table->cellAt( m_pView->markerColumn(), i );
		drawObject( obj, m_pView->markerColumn(), i, TRUE );
	    } */
	}
	else if ( make_select )
	{
	    selection.setRight( m_pView->markerColumn() );
	    /* for ( int i = selection.top(); i <= selection.bottom(); i++ )
	    {
		obj = table->cellAt( m_pView->markerColumn() + 1, i );
		drawObject( obj, m_pView->markerColumn() + 1, i, TRUE );
	    } */
	}

	if ( make_select )
	  table->setSelection( selection );

	// TODO
	// markerIsVisible = TRUE;
	m_pView->drawMarker();

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	if ( cell->text() != 0L )
	    m_pView->editWidget()->setText( cell->text() );
	else
	    m_pView->editWidget()->setText( "" );

	break;

    case Key_Right:
	_ev->accept();
	if ( m_pView->markerColumn() == 0xFFFF )
	    return;
	
	if ( m_bEditDirtyFlag )
	{
	  m_pView->setText( m_pView->editWidget()->text() );
	  m_bEditDirtyFlag = FALSE;
	}

	m_pView->drawMarker();
	// TODO
	// markerIsVisible = FALSE;

	if ( selection.left() == 0 && make_select )
	{
	    selection.setCoords( m_pView->markerColumn(), m_pView->markerRow(), m_pView->markerColumn(), m_pView->markerRow() );
	    // obj = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	    // drawObject( obj, m_pView->markerColumn(), m_pView->markerRow(), TRUE );
	}

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	// Are we leaving a cell with extra size ?
	if ( cell->isForceExtraCells() )
	    m_pView->setMarkerColumn( m_pView->markerColumn() + 1 + cell->extraXCells() );
	else
	    m_pView->setMarkerColumn( m_pView->markerColumn() + 1 );

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	// Go to the upper left corner of the obscuring object ( if there is one )
	if ( cell->isObscured() && cell->isObscuringForced() )
	{
	  m_pView->setMarkerRow( cell->obscuringCellsRow() );
	  m_pView->setMarkerColumn( cell->obscuringCellsColumn() );
	}

	x = table->columnPos( m_pView->markerColumn(), m_pView );
	cl = table->columnLayout( m_pView->markerColumn() );
	if ( x + cl->width( m_pView ) > width() )
	    m_pView->horzScrollBar()->setValue( m_pView->xOffset() + ( x + cl->width( m_pView ) - width() ) );

	// If we have been at the right side of the selection ...
	if ( selection.right() == m_pView->markerColumn() - 1 && make_select )
	{
	    selection.setRight( m_pView->markerColumn() );
	    /* for ( int i = selection.top(); i <= selection.bottom(); i++ )
	    {
		obj = table->cellAt( m_pView->markerColumn(), i );
		drawObject( obj, m_pView->markerColumn(), i, TRUE );
	    } */
	}
	else if ( make_select )
	{
	    selection.setLeft( m_pView->markerColumn() );
	    /* for ( int i = selection.top(); i <= selection.bottom(); i++ )
	    {
		obj = table->cellAt( m_pView->markerColumn() - 1, i );
		drawObject( obj, m_pView->markerColumn() - 1, i, TRUE );
	    } */
	}

	if ( make_select )
	  table->setSelection( selection );

	// TODO
	// markerIsVisible = TRUE;
	m_pView->drawMarker();

	cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	if ( cell->text() != 0L )
	    m_pView->editWidget()->setText( cell->text() );
	else
	    m_pView->editWidget()->setText( "" );

	break;

    case Key_Escape:
	_ev->accept();

	if ( m_bEditDirtyFlag )
	{
	  cell = table->cellAt( m_pView->markerColumn(), m_pView->markerRow() );
	  if ( cell->text() != 0L )
	    m_pView->editWidget()->setText( cell->text() );
	  else
	    m_pView->editWidget()->setText( "" );
	}
	break;
	
    default:
	char buffer[2];
	if ( _ev->ascii() == 0 )
	{
	  _ev->accept();
	  // _ev->ignore();
	  return;
	}
	
	buffer[0] = _ev->ascii();
	buffer[1] = 0;
	
	// m_pView->editWidget()->setText( buffer );
	// m_pView->editWidget()->setFocus();
	if ( !m_bEditDirtyFlag )
	    m_pView->editWidget()->setText( "" );
	
	m_pView->editWidget()->publicKeyPressEvent( _ev );
	m_bEditDirtyFlag = TRUE;
	break;
    }
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
}

void KSpreadVBorder::mousePressEvent( QMouseEvent * _ev )
{
  m_bResize = FALSE;
  m_bSelection = FALSE;

  KSpreadTable *table = m_pView->activeTable();
  assert( table );
    
  int y = 0;
  int row = table->topRow( 0, y, m_pView );
    
  while ( y < m_pView->vBorderWidget()->height() )
  {
    int h = table->rowLayout( row )->height( m_pView );
    row++;
    if ( _ev->pos().y() >= y + h - 1 && _ev->pos().y() <= y + h + 1 )
      m_bResize = TRUE;
    y += h;
  }
    
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
	QRect selection( table->selection() );
	selection.setCoords( 1, hit_row, 0x7FFF, hit_row );
	// m_pView->vBorderWidget()->update();

	/*
	int xpos;
	int left_col = getLeftColumn( 0, xpos );
	int right_col = getBottomRow( m_pView->canvasWidget()->width() );
	for ( int i = left_col; i <= right_col; i++ )
	{
	    Object *obj = table->cellAt( i, hit_row );
	    drawObject( obj, i, hit_row, TRUE );
	}*/
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
	
    // Redraw all columns right from the changed column, including the changed one.
    /* int ypos;
    int left_col = table->leftColumn( 0, ypos, m_pView );
    int right_col = table->rightColumn( m_pView->canvasWidget()->width(), m_pView );
    int bottom_row = table->bottomRow( m_pView->canvasWidget()->height(), m_pView );
    for ( y = m_iResizeAnchor; y <= bottom_row; y++ )
      for ( int x = left_col; x <= right_col; x++ )
      {
	KSpreadCell *cell = table->cellAt( x, y );
	cell->setLayoutDirtyFlag();
      }

    m_pView->drawVisibleCells();
    m_pView->vBorderWidget()->update(); */

    m_pView->doc()->setModified( TRUE );
  }
  
  m_bSelection = FALSE;
  m_bResize = FALSE;
}

void KSpreadVBorder::mouseMoveEvent( QMouseEvent * _ev )
{
  KSpreadTable *table = m_pView->activeTable();
  assert( table );

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
  else if ( m_bSelection )
  {
    int y = 0;
    int row = table->topRow( _ev->pos().y(), y, m_pView );
    QRect selection = table->selection();
    
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
    table->setSelection( selection );
    
    if ( _ev->pos().y() < 0 )
      m_pView->vertScrollBar()->setValue( m_pView->yOffset() + y );
    else if ( _ev->pos().y() > m_pView->canvasWidget()->height() )
    {
      RowLayout *rl = table->rowLayout( row + 1 );
      y = table->rowPos( row + 1, m_pView );
      m_pView->vertScrollBar()->setValue( m_pView->yOffset() + 
					  ( y + rl->height( m_pView ) - m_pView->canvasWidget()->height() ) );
    }
	
    /*
    // Do we have to redraw ?
    if ( r != selection )
    {
      m_pView->vBorderWidget()->update();

      int ypos;
      int xpos;
      int left_col = table->leftColumn( 0, xpos );
      int right_col = table->rightColumn( m_pView->canvasWidget()->width() );
      int top_row = table->topRow( 0, ypos );
      int bottom_row = table->bottomRow( m_pView->canvasWidget()->height() );
      
      for ( y = top_row; y <= bottom_row; y++ )
      {
	// Did this column change its selection state ?
	bool b1 = ( y >= r.top() && y <= r.bottom() );
	bool b2 = ( y >= selection.top() && y <= selection.bottom() );
	
	if ( ( b1 && !b2 ) || ( !b1 && b2 ) )
	{
	  for ( int i = left_col; i <= right_col; i++ )
	  {
	    Object *obj = table->cellAt( i, y );
	    drawObject( obj, i, y, TRUE );
	  }
	}
      }
    } */
  }
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
  assert( table );

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

  QRect selection( table->selection() );
  
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
    table->setSelection( r );
    // update();
    
    /* 
    int ypos;
    int top_row = topRow( 0, ypos );
    int bottom_row = bottomRow( pView->canvasWidget()->height() );
    for ( int i = top_row; i <= bottom_row; i++ )
    {
      KSpreadCell *obj = table->cellAt( hit_col, i );
      m_pView->drawObject( cell, hit_col, i, TRUE );
    }
    */
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
	
    // Redraw all columns right from the changed column, including the changed one.
    /* int ypos;
    int top_row = table->topRow( 0, ypos, m_pView );
    int bottom_row = table->bottomRow( m_pView->canvasWidget()->height(), m_pView );
    int right_col = table->rightColumn( m_pView->canvasWidget()->width(), m_pView ); */

    /* for ( x = m_iResizeAnchor; x <= right_col; x++ )
      for ( int y = top_row; y <= bottom_row; y++ )
      {
	KSpreadCell *cell = table->cellAt( x, y );
	cell->setLayoutDirtyFlag();
      }

    m_pView->hBorderWidget()->update();
    m_pView->drawVisibleCells(); */

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
    QRect r = table->selection();

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
    table->setSelection( r );    

    if ( _ev->pos().x() < 0 )
      m_pView->horzScrollBar()->setValue( m_pView->xOffset() + x );
    else if ( _ev->pos().x() > m_pView->canvasWidget()->width() )
    {
      ColumnLayout *cl = table->columnLayout( col + 1 );
      x = table->columnPos( col + 1, m_pView );
      m_pView->horzScrollBar()->setValue( m_pView->xOffset() + 
					  ( x + cl->width( m_pView ) - m_pView->canvasWidget()->width() ) );
    }
    /*
	// Do we have to redraw ?
	if ( r != selection )
	{
	    m_pView->hBorderWidget()->update();

	    int ypos;
	    int xpos;
	    int left_col = table->leftColumn( 0, xpos );
	    int right_col = getRightColumn( m_pView->canvasWidget()->width() );
	    int top_row = getTopRow( 0, ypos );
	    int bottom_row = table->bottomRow( m_pView->canvasWidget()->height() );

	    for ( x = left_col; x <= right_col; x++ )
	    {
		// Did this column change its selection state ?
		bool b1 = ( x >= r.left() && x <= r.right() );
		bool b2 = ( x >= selection.left() && x <= selection.right() );
		
		if ( ( b1 && !b2 ) || ( !b1 && b2 ) )
		{
		    for ( int i = top_row; i <= bottom_row; i++ )
		    {
			Object *obj = table->cellAt( x, i );
			drawObject( obj, x, i, TRUE );
		    }
		}
	    }
	} */
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

  QRect selection( table->selection() );
  
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
  PartFrame_impl( _view->canvasWidget() )
{
  m_pView = _view;
  m_pChild = _child;
}

#include "kspread_view.moc"
