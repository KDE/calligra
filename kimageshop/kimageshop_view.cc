/*
 *  kimageshop_view.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <mkoch@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <qprinter.h>
#include <qmsgbox.h>
#include <qkeycode.h>
#include <qprndlg.h>

#include <kfiledialog.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kstdaccel.h>
#include <kiconloader.h>
#include <kruler.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "kimageshop_shell.h"
#include "canvasview.h"
#include "brush.h"
#include "layerdlg.h"
#include "colordialog.h"
#include "gradient.h"
#include "gradientdlg.h"
#include "gradienteditordlg.h"
#include "brushdialog.h"
#include "brusheswidget.h"
#include "tool.h"
#include "movetool.h"
#include "brushtool.h"
#include "zoomtool.h"
#include "gradienttool.h"
#include "preferencesdlg.h"

KImageShopView::KImageShopView( QWidget* _parent, const char* _name, KImageShopDoc* _doc )
  : QWidget( _parent, _name )
  , KoViewIf( _doc )
  , OPViewIf( _doc )
  , KImageShop::View_skel()
{
  setWidget( this );

  OPPartIf::setFocusPolicy( OpenParts::Part::ClickFocus );

  m_pDoc = _doc;
  m_pCanvasView = 0L;
  m_pLayerDialog = 0L;
  m_pBrushDialog = 0L;
  m_pGradientDialog = 0L;
  m_pColorDialog = 0L;
  m_pHorz = 0L;
  m_pVert = 0L;
  m_pHRuler = 0L;
  m_pVRuler = 0L;
  m_ZoomFactor = 1;
  m_fg = KColor(255,255,255);
  m_bg = KColor(0,0,0);

  QObject::connect(m_pDoc, SIGNAL(sigUpdateView(const QRect&)), this, SLOT(slotUpdateView(const QRect&)));
}

KImageShopView::~KImageShopView()
{
  kdebug(KDEBUG_INFO, 0, "KImageShopView::~KImageShopView() %li", _refcnt());
  cleanUp();
}

void KImageShopView::init()
{
  // register the view at the parent-parts UI managers
  kdebug( KDEBUG_INFO, 0, "Registering menu as %li", id() );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a menu bar manager" );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->registerClient( id(), this );
  else
    kdebug( KDEBUG_ERROR, 0, "Did not get a tool bar manager" );

  // setup scrollbars/rulers etc.
  createGUI();
}

void KImageShopView::cleanUp()
{
  kdebug( KDEBUG_INFO, 0, "void KImageShopView::cleanUp() " );

  // avoid loops
  if ( m_bIsClean ) { return; }

  // unregister the view from the parent parts UI managers
  kdebug( KDEBUG_INFO, 0, "1b) Unregistering menu and toolbar" );

  OpenParts::MenuBarManager_var menu_bar_manager = m_vMainWindow->menuBarManager();
  if ( !CORBA::is_nil( menu_bar_manager ) )
    menu_bar_manager->unregisterClient( id() );

  OpenParts::ToolBarManager_var tool_bar_manager = m_vMainWindow->toolBarManager();
  if ( !CORBA::is_nil( tool_bar_manager ) )
    tool_bar_manager->unregisterClient( id() );

  // let the document know that this view is gone
  m_pDoc->removeView(this);

  delete m_pHorz;
  delete m_pVert;
  delete m_pHRuler;
  delete m_pVRuler;
  delete m_pCanvasView;
  delete m_pLayerDialog;
  delete m_pBrushDialog;
  //  delete m_pColorDialog;
  delete m_pMoveTool;
  delete m_pBrushTool;
  delete m_pZoomTool;
  delete m_pGradientTool;

  KoViewIf::cleanUp();
}

bool KImageShopView::event( const QCString &_event, const CORBA::Any& _value )
{
  EVENT_MAPPER( _event, _value );

  // map OP events to member functions
  MAPPING( OpenPartsUI::eventCreateMenuBar, OpenPartsUI::typeCreateMenuBar_ptr, mappingCreateMenubar );
  MAPPING( OpenPartsUI::eventCreateToolBar, OpenPartsUI::typeCreateToolBar_ptr, mappingCreateToolbar );

  END_EVENT_MAPPER;

  return false;
}

#include <opMenuIf.h>

bool KImageShopView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  kdebug( KDEBUG_INFO, 0, "ImageShopView::mappingCreateToolbar" );

  if (CORBA::is_nil(_factory))
  {
    // ToolBarFactory is nil -> we lost control over the toolbar
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vToolBarEdit = 0L;
    m_vTBUndoMenu = 0L;
    m_vTBRedoMenu = 0L;
    m_vToolBarTools = 0L;
    return true;
  }

  OpenPartsUI::Pixmap_var pix;

  /*
  * edit toolbar
  */

  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarEdit->enable( OpenPartsUI::Show );
  m_vToolBarEdit->setFullWidth( false );
  m_vToolBarEdit->setBarPos( OpenPartsUI::Top );

  // undo
  pix = OPICON( "undo" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_UNDO, SIGNAL( clicked() ), this, "slotEditUndo", true, i18n( "Undo last action." ), -1 );
  m_vToolBarEdit->setDelayedPopup( TBEDIT_UNDO, m_vTBUndoMenu );
  m_vToolBarEdit->setItemEnabled( TBEDIT_UNDO, false );
  m_vTBUndoMenu->connect( "activated", this, "activatedUndoMenu" );

  // redo
  pix = OPICON( "redo" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_REDO, SIGNAL( clicked() ), this, "slotEditRedo", true, i18n( "Redo last action." ), -1 );
  m_vToolBarEdit->setDelayedPopup( TBEDIT_REDO, m_vTBRedoMenu );
  m_vToolBarEdit->setItemEnabled( TBEDIT_REDO, false );
  m_vTBRedoMenu->connect( "activated", this, "activatedRedoMenu" );

  m_vToolBarEdit->insertSeparator( -1 );

  // cut
  pix = OPICON( "editcut" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_CUT, SIGNAL( clicked() ), this, "slotEditCut", true, i18n( "Cut selected area." ), -1 );
  m_vToolBarEdit->setItemEnabled( TBEDIT_CUT, false );

  // copy
  pix = OPICON( "editcopy" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_COPY, SIGNAL( clicked() ), this, "slotEditCopy", true, i18n( "Copy selected area." ), -1 );
  m_vToolBarEdit->setItemEnabled( TBEDIT_COPY, false );

  // paste
  pix = OPICON( "editpaste" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_PASTE, SIGNAL( clicked() ), this, "slotEditPaste", true, i18n( "Paste clipboard into active layer." ), -1 );
  m_vToolBarEdit->setItemEnabled( TBEDIT_PASTE, false );

  /*
  * dialog toolbar
  */
  m_vToolBarDialogs = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarDialogs->enable( OpenPartsUI::Show );
  m_vToolBarDialogs->setFullWidth( false );

  // layer dialog
  pix = OPICON( "layer_dialog" );
  m_vToolBarDialogs->insertButton2( pix, TBDIALOGS_LAYER, SIGNAL( clicked() ), this, "slotLayerDialog", true, i18n( "Show/hide layer dialog." ), -1 );
  m_vToolBarDialogs->setToggle( TBDIALOGS_LAYER, true );
  m_vToolBarDialogs->toggleButton( TBDIALOGS_LAYER );

  // color dialog
  pix = OPICON( "color_dialog" );
  m_vToolBarDialogs->insertButton2( pix, TBDIALOGS_COLOR, SIGNAL( clicked() ), this, "slotColorDialog", true, i18n( "Show/hide color dialog." ), -1 );
  m_vToolBarDialogs->setToggle( TBDIALOGS_COLOR, true );

  // brush dialog
  pix = OPICON( "brush_dialog" );
  m_vToolBarDialogs->insertButton2( pix, TBDIALOGS_BRUSH, SIGNAL( clicked() ), this, "slotBrushDialog", true, i18n( "Show/hide brush dialog." ), -1 );
  m_vToolBarDialogs->setToggle( TBDIALOGS_BRUSH, true );

  // gradient dialog
  pix = OPICON( "gradient_dialog" );
  m_vToolBarDialogs->insertButton2( pix, TBDIALOGS_GRADIENT, SIGNAL( clicked() ), this, "slotGradientDialog", true, i18n( "Show/hide gradient dialog." ), -1 );
  m_vToolBarDialogs->setToggle( TBDIALOGS_GRADIENT, true );

  // gradient editor dialog
  pix = OPICON( "gradienteditor_dialog" );
  m_vToolBarDialogs->insertButton2( pix, TBDIALOGS_GRADIENTEDITOR, SIGNAL( clicked() ), this, "slotGradientEditorDialog", true, i18n( "Show/hide gradient editor dialog." ), -1 );
  m_vToolBarDialogs->setToggle( TBDIALOGS_GRADIENTEDITOR, true );

  /*
  * tools toolbar
  */
  m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarTools->enable( OpenPartsUI::Show );
  m_vToolBarTools->setFullWidth( false );
  m_vToolBarTools->setBarPos( OpenPartsUI::Left );

  // move tool
  pix = OPICON( "move" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_MOVETOOL, SIGNAL( clicked() ), this, "slotActivateMoveTool", true, i18n( "Move layers and selections." ), -1 );
  m_vToolBarTools->setToggle( TBTOOLS_MOVETOOL, true );
  m_vToolBarTools->toggleButton( TBTOOLS_MOVETOOL );
  m_toolButtons.push_back( TBTOOLS_MOVETOOL );

  // zoom tool
  pix = OPICON( "viewmag" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_ZOOMTOOL, SIGNAL(clicked()), this, "slotActivateZoomTool", true, i18n( "Zoom in/out." ), -1 );
  m_vToolBarTools->setToggle( TBTOOLS_ZOOMTOOL, true );
  m_toolButtons.push_back( TBTOOLS_ZOOMTOOL );

  // paint tool
  pix = OPICON( "paintbrush" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_BRUSHTOOL, SIGNAL(clicked()), this, "slotActivateBrushTool", true, i18n( "Paint using a brush." ), -1 );
  m_vToolBarTools->setToggle( TBTOOLS_BRUSHTOOL, true );
  m_toolButtons.push_back( TBTOOLS_BRUSHTOOL );

  // gradient tool
  pix = OPICON( "gradient" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_GRADIENTTOOL, SIGNAL(clicked()), this, "slotActivateGradientTool", true, i18n( "Draw a gradient." ), -1 );
  m_vToolBarTools->setToggle( TBTOOLS_GRADIENTTOOL, true );
  m_toolButtons.push_back( TBTOOLS_GRADIENTTOOL );

  kdebug( KDEBUG_INFO, 0, "KImageShopView::mappingCreateToolbar : done" );
  return true;
}

bool KImageShopView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr menubar )
{
  kdebug( KDEBUG_INFO, 0, "KImageShopView::mappingCreateMenubar" );

  if (CORBA::is_nil(menubar))
  {
    // MenuBar_ptr is nil -> we lost control over the menubar
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuImage = 0L;
    m_vMenuPlugIns = 0L;
    m_vMenuOptions = 0L;
    return true;
  }

  KStdAccel stdAccel;

  // don't use Q2C directly in arguments to avoid memory leaks!
  OpenPartsUI::Pixmap_var pix;

  // edit menu
  menubar->insertMenu( i18n( "&Edit" ), m_vMenuEdit, -1, -1 );

  pix = OPICON( "undo" );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem3( pix, i18n( "&Undo" ), this, "slotEditUndo", stdAccel.undo() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );

  pix = OPICON( "redo" );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem3( pix, i18n( "&Redo" ), this, "slotEditRedo", stdAccel.redo() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );

  m_vMenuEdit->insertSeparator( -1 );

  pix = OPICON( "editcut" );
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem3( pix, i18n( "&Cut" ), this, "slotEditCut", stdAccel.cut() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Cut, false );

  pix = OPICON( "editcopy" );
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem3( pix, i18n( "&Copy" ), this, "slotEditCopy", stdAccel.cut() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Copy, false );

  pix = OPICON( "editpaste" );
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem3( pix, i18n( "&Paste" ), this, "slotEditPaste", stdAccel.cut() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, false );

  // view menu
  menubar->insertMenu( i18n( "&View" ) , m_vMenuView, -1, -1 );

  m_idMenuView_LayerDialog = m_vMenuView->insertItem( i18n( "&Layer dialog" ), this, "slotLayerDialog", 0 );

  m_idMenuView_ColorDialog = m_vMenuView->insertItem( i18n( "&Color dialog" ), this, "slotColorDialog", 0 );

  m_idMenuView_BrushDialog = m_vMenuView->insertItem( i18n( "&Brushes dialog" ), this, "slotBrushDialog", 0 );

  m_idMenuView_GradientDialog = m_vMenuView->insertItem( i18n( "&Gradient dialog" ), this, "slotGradientDialog", 0 );

  m_idMenuView_GradientEditorDialog = m_vMenuView->insertItem( i18n( "Gradient &editor dialog" ), this, "slotGradientEditorDialog", 0 );

  m_vMenuView->insertSeparator( -1 );

  m_idMenuView_Preferences = m_vMenuView->insertItem( i18n( "&Preferences" ), this, "slotPreferences", 0 );

  m_vMenuView->setCheckable( true );
  m_vMenuView->setItemChecked( m_idMenuView_LayerDialog, true );

  // image menu
  menubar->insertMenu( i18n( "&Image" ), m_vMenuImage, -1, -1 );
  m_idMenuImage_Rotate180 = m_vMenuImage->insertItem( i18n( "&Rotate 180" ), this, "slotRotateLayer180", 0 );
  m_idMenuImage_RotateLeft90 = m_vMenuImage->insertItem( i18n( "&Rotate Left 90" ), this, "slotRotateLayerLeft90", 0 );
  m_idMenuImage_RotateRight90 = m_vMenuImage->insertItem( i18n( "&Rotate Right 90" ), this, "slotRotateLayerRight90", 0 );
  m_idMenuImage_MirrorX = m_vMenuImage->insertItem( i18n( "&Mirror X" ), this, "slotMirrorLayerX", 0 );
  m_idMenuImage_MirrorY = m_vMenuImage->insertItem( i18n( "&Mirror Y" ), this, "slotMirrorLayerY", 0 );
/*
  // plugins menu
  menubar->insertMenu( i18n( "&Plugins" ), m_vMenuPlugIns, -1, -1 );

  // options menu
  menubar->insertMenu( i18n( "&Options" ), m_vMenuOptions, -1, -1 );
*/

  return true;
}

void KImageShopView::createGUI()
{
  // create canvasview
  m_pCanvasView = new CanvasView(this);

  // setup GUI
  setupScrollbars();
  setupRulers();

  // connect canvasview
  QObject::connect(m_pCanvasView, SIGNAL(sigPaint(QPaintEvent*)), this, SLOT(slotCVPaint(QPaintEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMousePress(QMouseEvent*)), this, SLOT(slotCVMousePress(QMouseEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMouseMove(QMouseEvent*)), this, SLOT(slotCVMouseMove(QMouseEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMouseRelease(QMouseEvent*)), this, SLOT(slotCVMouseRelease(QMouseEvent*)));

  // create move tool
  m_pMoveTool = new MoveTool(m_pDoc);
  m_pTool = m_pMoveTool;

  // create brush dialog
  m_pBrushDialog = new BrushDialog(this);
  m_pBrushDialog->resize(205, 267);
  m_pBrushDialog->move(405, 20);
  m_pBrushDialog->hide();
  addDialog(m_pBrushDialog);

  m_pBrushChooser = m_pBrushDialog->brushChooser();
  m_pBrush = m_pBrushChooser->currentBrush();
  QObject::connect(m_pBrushChooser, SIGNAL(selected(const Brush *)), this, SLOT(slotSetBrush(const Brush*)));

  // create brush tool
  m_pBrushTool = new BrushTool(m_pDoc, m_pBrush);

  // create zoom tool
  m_pZoomTool = new ZoomTool(this);

  // create gradient tool
  m_pGradientTool = new GradientTool( m_pDoc, m_actGradient );

  // create layer dialog
  m_pLayerDialog = new LayerDialog( m_pDoc, this );
  m_pLayerDialog->resize( 205, 267 );
  m_pLayerDialog->move( 200, 20 );
  m_pLayerDialog->show();
  m_pLayerDialog->setFocus();
  addDialog(m_pLayerDialog);

  // create gradient dialog
  m_pGradientDialog = new GradientDialog( m_pDoc, this );
  m_pGradientDialog->resize( 206, 185 );
  m_pGradientDialog->move( 200, 290 );
  m_pGradientDialog->hide();

  // create gradient editor dialog
  m_pGradientEditorDialog = new GradientEditorDialog( m_pDoc, this );
  m_pGradientEditorDialog->resize( 400, 200 );
  m_pGradientEditorDialog->move( 100, 190 );
  m_pGradientEditorDialog->hide();

  // color dialog
  m_pColorDialog = new ColorDialog( this );
  m_pColorDialog->move(100, 20);
  m_pColorDialog->hide();
  addDialog(m_pColorDialog);
}

void KImageShopView::setupScrollbars()
{
  m_pVert = new QScrollBar( QScrollBar::Vertical, this );
  m_pHorz = new QScrollBar( QScrollBar::Horizontal, this );

  if (m_pCanvasView)
    m_pCanvasView->resize(widget()->width()-16, widget()->height()-16);

  m_pVert->setGeometry(widget()->width()-16, 0, 16, widget()->height()-16);
  m_pHorz->setGeometry(0, widget()->height()-16, widget()->width()-16, 16);
  m_pVert->show();
  m_pHorz->show();

  QObject::connect(m_pVert, SIGNAL(sliderMoved(int)), this, SLOT(scrollV(int)));
  QObject::connect(m_pHorz, SIGNAL(sliderMoved(int)), this, SLOT(scrollH(int)));

}

void KImageShopView::setupRulers()
{
  m_pHRuler = new KRuler(KRuler::horizontal, this);
  m_pVRuler = new KRuler(KRuler::vertical, this);
  m_pCanvasView->resize(m_pCanvasView->width() - 20, m_pCanvasView->height() - 20);
  m_pCanvasView->move(20, 20);
  m_pHRuler->setGeometry(20, 0, m_pCanvasView->width(), 20);
  m_pVRuler->setGeometry(0, 20, 20, m_pCanvasView->height());

  m_pVRuler->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
  m_pHRuler->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  m_pVRuler->setRulerStyle(KRuler::pixel);
  m_pHRuler->setRulerStyle(KRuler::pixel);
}

void KImageShopView::scrollH(int)
{
  m_pHRuler->setOffset(m_pHorz->value());
  resizeEvent(0L);
  slotCVPaint(0L);
}

void KImageShopView::scrollV(int)
{
  m_pVRuler->setOffset(m_pVert->value());
  resizeEvent(0L);
  slotCVPaint(0L);
}

void KImageShopView::scrollTo(const QPoint &pos)
{
  m_pHorz->setValue(pos.x());
  m_pVert->setValue(pos.y());
  resizeEvent(0L);
  m_pCanvasView->repaint();
}

int KImageShopView::hScrollValue()
{
  return m_pHorz->value();
}

int KImageShopView::vScrollValue()
{
  return m_pVert->value();
}

void KImageShopView::newView()
{
  ASSERT(m_pDoc != 0L);

  KImageShopShell* shell = new KImageShopShell;
  shell->show();
  shell->setDocument(m_pDoc);
}

KImageShopDoc* KImageShopView::doc()
{
  return m_pDoc;
}

void KImageShopView::helpUsing()
{
  kapp->invokeHTMLHelp("kimageshop/kimageshop.html", QString::null);
}

bool KImageShopView::printDlg()
{
  QPrinter prt;

  if(QPrintDialog::getPrinterSetup(&prt))
  {
    m_pDoc->print(&prt);
  }
  return true;
}

int KImageShopView::viewWidth()
{
  return m_pCanvasView->width();
}

int KImageShopView::viewHeight()
{
  return m_pCanvasView->height();
}

int KImageShopView::docWidth()
{
  return static_cast<int>(m_pDoc->width() * m_ZoomFactor);
}

int KImageShopView::docHeight()
{
  return static_cast<int>(m_pDoc->height() * m_ZoomFactor);
}

int KImageShopView::xPaintOffset()
{
  return 0;// (viewWidth() > docWidth()) ? static_cast<int>((viewWidth() -  docWidth())/2) : 0;
}

int KImageShopView::yPaintOffset()
{
  return 0;//(viewHeight() > docHeight()) ? static_cast<int>((viewHeight() - docHeight())/2) : 0;
}

void KImageShopView::resizeEvent(QResizeEvent*)
{
  if ((KoViewIf::hasFocus() || mode() == KOffice::View::RootMode))
    {
      m_pHRuler->show();
      m_pVRuler->show();
      m_pCanvasView->move(20, 20);

      // KImageShopView heigth/width - ruler heigth/width
      int viewH = height() - 20;
      int viewW = width() - 20;

      // scrollbar and canvasview geometry
      if (docHeight() <= viewH && docWidth() <= viewW) // we need no scrollbars
	{
	  m_pVert->hide();
	  m_pHorz->hide();
	  m_pVert->setValue(0);
	  m_pHorz->setValue(0);
	  m_pCanvasView->resize(width() - 20, height() - 20);
	}
      else if (docHeight() <= viewH) // we need a horizontal scrollbar
	{
	  m_pVert->hide();
	  m_pVert->setValue(0);
	  m_pHorz->setRange(0, docWidth() - viewW);
	  m_pHorz->setGeometry(0, height() - 16, width(), 16);
	  m_pHorz->show();
	  m_pCanvasView->resize(width() - 20, height() - 36);

	}
      else if(docWidth() <= viewW) // we need a vertical scrollbar
	{
	  m_pHorz->hide();
	  m_pHorz->setValue(0);
	  m_pVert->setRange(0, docHeight() - viewH);
	  m_pVert->setGeometry(width() - 16, 0, 16, height());
	  m_pVert->show();
	  m_pCanvasView->resize(width() - 36, height() - 20);
	}
      else // we need both scrollbars
	{
	  m_pVert->setRange(0, docHeight() - viewH + 16);
	  m_pVert->setGeometry(width() - 16, 0, 16, height() - 16);
	  m_pVert->show();
	  m_pHorz->setRange(0, docWidth() - viewW + 16);
	  m_pHorz->setGeometry(0, height() - 16, width() - 16, 16);
	  m_pHorz->show();
	  m_pCanvasView->resize(width() - 36, height() - 36);
	}

      // ruler geometry
      m_pHRuler->setGeometry(20, 0, viewWidth(), 20);
      m_pVRuler->setGeometry(0, 20, 20, viewHeight());

      // ruler ranges
      m_pVRuler->setRange(0, docHeight());
      m_pHRuler->setRange(0, docWidth());

      // ruler offset
      if(m_pVert->isVisible())
	m_pVRuler->setOffset(m_pVert->value());
      else
	m_pVRuler->setOffset(-yPaintOffset());

      if(m_pHorz->isVisible())
	m_pHRuler->setOffset(m_pHorz->value());
      else
	m_pHRuler->setOffset(-xPaintOffset());
    }
  else
    {
      m_pHorz->hide();
      m_pVert->hide();
      m_pHRuler->hide();
      m_pVRuler->hide();

      m_pCanvasView->move(0, 0);
      m_pCanvasView->resize(widget()->width(), widget()->height());
    }
}

void KImageShopView::activateTool( int toolID )
{
    // implement RadioButton behaviour for the Tool toolbar
    vector<int>::iterator it;
    for( it = m_toolButtons.begin(); it != m_toolButtons.end(); it++ )
        if( m_vToolBarTools->isButtonOn( *it ) != ( *it == toolID ) )
            m_vToolBarTools->setButton( *it, *it == toolID );
}

void KImageShopView::slotActivateMoveTool()
{
  if (!m_pMoveTool)
      m_pMoveTool = new MoveTool(m_pDoc);

  m_pTool = m_pMoveTool;

  activateTool( TBTOOLS_MOVETOOL );
}

void KImageShopView::slotActivateBrushTool()
{
  if (!m_pBrush)
    {
      if ( m_pBrushChooser )
	m_pBrush = m_pBrushChooser->currentBrush();

      else
	{
	  // FIXME this should go away
	  // the chooser should be the only one dealing with brushes
	  QString _image = locate("appdata", "brushes/brush.jpg");
	  m_pBrush = new Brush(_image);
	  // m_pBrush->setHotSpot(QPoint(25,25));
	}
    }

  if (!m_pBrushTool)
    m_pBrushTool = new BrushTool(m_pDoc, m_pBrush);

  m_pTool = m_pBrushTool;

  activateTool( TBTOOLS_BRUSHTOOL);
}

void KImageShopView::slotActivateZoomTool()
{
  if (!m_pZoomTool)
    m_pZoomTool = new ZoomTool(this);

  m_pTool = m_pZoomTool;

  activateTool( TBTOOLS_ZOOMTOOL );
}

void KImageShopView::slotActivateGradientTool()
{
  if( !m_pGradientTool )
    m_pGradientTool = new GradientTool( m_pDoc, m_actGradient );

  m_pTool = m_pGradientTool;

  activateTool( TBTOOLS_GRADIENTTOOL );
}

void KImageShopView::slotUpdateView(const QRect &_area) // _area in canvas coordiantes
{
  // viewrect in canvas coordinates
  QRect viewRect(static_cast<int>(m_pHorz->value()/m_ZoomFactor), static_cast<int>(m_pVert->value()/m_ZoomFactor), m_pCanvasView->width()/m_ZoomFactor, m_pCanvasView->height()/m_ZoomFactor);

  // does the area intersect the viewrect?
  if (!_area.intersects(viewRect)) return;

  // repaint only the visible part
  QRect area = viewRect & _area;

  // offset
  QPoint offset(static_cast<int>(m_pHorz->value()/m_ZoomFactor), static_cast<int>(m_pVert->value()/m_ZoomFactor));

  // repaint
  m_pDoc->paintPixmap(m_pCanvasView, area, offset, QPoint(xPaintOffset(),yPaintOffset()), m_ZoomFactor);
}

void KImageShopView::slotCVPaint(QPaintEvent *)
{
  // viewrect in canvas coordinates
  QRect viewRect(static_cast<int>(m_pHorz->value()/m_ZoomFactor), static_cast<int>(m_pVert->value()/m_ZoomFactor), m_pCanvasView->width()/m_ZoomFactor, m_pCanvasView->height()/m_ZoomFactor);

  slotUpdateView(viewRect);
}

void KImageShopView::slotCVMousePress(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  KImageShop::MouseEvent mouseEvent;
  // postion in canvas coordinates
  mouseEvent.posX = static_cast<long>((e->x() - xPaintOffset() + m_pHorz->value()) / m_ZoomFactor);
  mouseEvent.posY = static_cast<long>((e->y() - yPaintOffset() + m_pVert->value()) / m_ZoomFactor);
  mouseEvent.globalPosX = e->globalX();
  mouseEvent.globalPosY = e->globalY();

  mouseEvent.leftButton = (e->button() & LeftButton) ? true : false;
  mouseEvent.rightButton = (e->button() & RightButton) ? true : false;
  mouseEvent.midButton = (e->button() & MidButton) ? true : false;

  mouseEvent.shiftButton = (e->state() & ShiftButton) ? true : false;
  mouseEvent.controlButton = (e->state() & ControlButton) ? true : false;
  mouseEvent.altButton = (e->state() & AltButton) ? true : false;

  m_pTool->mousePress(mouseEvent);
}

void KImageShopView::slotCVMouseMove(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  KImageShop::MouseEvent mouseEvent;
  // postion in canvas coordinates
  mouseEvent.posX = static_cast<long>((e->x() - xPaintOffset() + m_pHorz->value()) / m_ZoomFactor);
  mouseEvent.posY = static_cast<long>((e->y() - yPaintOffset() + m_pVert->value()) / m_ZoomFactor);
  mouseEvent.globalPosX = e->globalX();
  mouseEvent.globalPosY = e->globalY();

  mouseEvent.leftButton = (e->button() & LeftButton) ? true : false;
  mouseEvent.rightButton = (e->button() & RightButton) ? true : false;
  mouseEvent.midButton = (e->button() & MidButton) ? true : false;

  mouseEvent.shiftButton = (e->state() & ShiftButton) ? true : false;
  mouseEvent.controlButton = (e->state() & ControlButton) ? true : false;
  mouseEvent.altButton = (e->state() & AltButton) ? true : false;

  m_pTool->mouseMove(mouseEvent);

  m_pHRuler->slotNewValue(e->x() - xPaintOffset());
  m_pVRuler->slotNewValue(e->y() - yPaintOffset());
}

void KImageShopView::slotCVMouseRelease(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  KImageShop::MouseEvent mouseEvent;
  // postion in canvas coordinates
  mouseEvent.posX = static_cast<long>((e->x() - xPaintOffset() + m_pHorz->value()) / m_ZoomFactor);
  mouseEvent.posY = static_cast<long>((e->y() - yPaintOffset() + m_pVert->value()) / m_ZoomFactor);
  mouseEvent.globalPosX = e->globalX();
  mouseEvent.globalPosY = e->globalY();

  mouseEvent.leftButton = (e->button() & LeftButton) ? true : false;
  mouseEvent.rightButton = (e->button() & RightButton) ? true : false;
  mouseEvent.midButton = (e->button() & MidButton) ? true : false;

  mouseEvent.shiftButton = (e->state() & ShiftButton) ? true : false;
  mouseEvent.controlButton = (e->state() & ControlButton) ? true : false;
  mouseEvent.altButton = (e->state() & AltButton) ? true : false;

  m_pTool->mouseRelease(mouseEvent);
}

void KImageShopView::slotSetZoomFactor(float zoomFactor)
{
  // avoid divide by zero
  if (zoomFactor == 0)
    m_ZoomFactor = 1;

  // min == 1/32
  if (zoomFactor < 0.03125)
    zoomFactor = 0.03125;

  // max == 32/1
  if (zoomFactor > 32)
    zoomFactor = 32;

  if (zoomFactor == m_ZoomFactor)
    return;

  m_ZoomFactor = zoomFactor;
}

void KImageShopView::slotSetBrush(const Brush *brush )
{
  m_pBrush = brush;
}

void KImageShopView::slotEditUndo()
{
  undo( 1 );
}

void KImageShopView::slotEditRedo()
{
  redo( 1 );
}

void KImageShopView::undo( int _number )
{
  cout << "KImageShop::undo()" << endl;

  for( int i = 0; i < _number; i++ )
    m_pDoc->commandHistory()->undo();
}

void KImageShopView::redo( int _number )
{
  cout << "KImageShop::redo()" << endl;

  for( int i = 0; i < _number; i++ )
    m_pDoc->commandHistory()->redo();
}

void KImageShopView::activatedUndoMenu( long _id )
{
  undo( _id );
}

void KImageShopView::activatedRedoMenu( long _id )
{
  redo( _id );
}

void KImageShopView::slotEditCut()
{
}

void KImageShopView::slotEditCopy()
{
}

void KImageShopView::slotEditPaste()
{
}

void KImageShopView::slotLayerDialog()
{
  if( m_pLayerDialog )
  {
      bool vis = m_pLayerDialog->isVisible();
    if( vis )
      m_pLayerDialog->hide();
    else
       m_pLayerDialog->show();

    // TODO: make this working
    m_vMenuView->setItemChecked( m_idMenuView_LayerDialog, true );
    m_vToolBarDialogs->setButton( TBDIALOGS_LAYER, !vis );
  }
}

void KImageShopView::slotBrushDialog()
{
  if ( m_pBrushDialog ) {
      bool vis = m_pBrushDialog->isVisible();
    if ( vis )
      m_pBrushDialog->hide();
    else
      m_pBrushDialog->show();

    // TODO: make this working
    m_vMenuView->setItemChecked( m_idMenuView_BrushDialog, true );
    m_vToolBarDialogs->setButton( TBDIALOGS_BRUSH, !vis );
  }
}

void KImageShopView::slotGradientDialog()
{
  if( m_pGradientDialog )
  {
      bool vis = m_pGradientDialog->isVisible();
    if( vis )
      m_pGradientDialog->hide();
    else
      m_pGradientDialog->show();

    // TODO: make this working
    m_vMenuView->setItemChecked( m_idMenuView_GradientDialog, true );
    m_vToolBarDialogs->setButton( TBDIALOGS_GRADIENT, !vis );
  }
}

void KImageShopView::slotGradientEditorDialog()
{
  if( m_pGradientEditorDialog )
      {
      bool vis = m_pGradientEditorDialog->isVisible();
    if( vis )
      m_pGradientEditorDialog->hide();
    else
      m_pGradientEditorDialog->show();

    // TODO: make this working
    m_vMenuView->setItemChecked( m_idMenuView_GradientEditorDialog, true );
    m_vToolBarDialogs->setButton( TBDIALOGS_GRADIENTEDITOR, !vis );
  }
}

void KImageShopView::slotColorDialog()
{
  if( m_pColorDialog )
      {
          bool vis = m_pColorDialog->isVisible();
	  if( vis )
		m_pColorDialog->hide();
	  else
		m_pColorDialog->show();
	
	  // TODO: make this working
	  m_vMenuView->setItemChecked( m_idMenuView_ColorDialog, !vis );
          m_vToolBarDialogs->setButton( TBDIALOGS_COLOR, !vis );

  }
}

void KImageShopView::slotPreferences()
{
  PreferencesDialog::editPreferences();
}

void KImageShopView::changeUndo( QString _text, bool _enable )
{
  cout << "KImageShopView::changeUndo : " << endl;

  if( _enable )
  {
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, true );
    QString str;
    str.sprintf( i18n( "Undo: %s" ), _text.data() );
    m_vMenuEdit->changeItemText( str, m_idMenuEdit_Undo );
    m_vToolBarEdit->setItemEnabled( TBEDIT_UNDO, true );
  }
  else
  {
    m_vMenuEdit->changeItemText( i18n( "No Undo possible" ), m_idMenuEdit_Undo );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );
    m_vToolBarEdit->setItemEnabled( TBEDIT_UNDO, false );
  }
}

void KImageShopView::changeRedo( QString _text, bool _enable )
{
  cout << "KImageShopView::changeRedo : " << endl;

  if( _enable )
  {
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, true );
    QString str;
    str.sprintf( i18n( "Redo: %s" ), _text.data() );
    m_vMenuEdit->changeItemText( str, m_idMenuEdit_Redo );
    m_vToolBarEdit->setItemEnabled( TBEDIT_REDO, true );
  }
  else
  {
    m_vMenuEdit->changeItemText( i18n( "No Redo possible" ), m_idMenuEdit_Redo );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
    m_vToolBarEdit->setItemEnabled( TBEDIT_REDO, false );
  }
}

void  KImageShopView::slotSetFGColor(const KColor& c)
{
  m_fg = c;
}

void  KImageShopView::slotSetBGColor(const KColor& c)
{
  m_bg = c;
}

void KImageShopView::slotRotateLayer180()
{
	m_pDoc->rotateLayer180(0);
}

void KImageShopView::slotRotateLayerLeft90()
{
	m_pDoc->rotateLayerLeft90(0);
}

void KImageShopView::slotRotateLayerRight90()
{
	m_pDoc->rotateLayerRight90(0);
}

void KImageShopView::slotMirrorLayerX()
{
	m_pDoc->mirrorLayerX(0);
}

void KImageShopView::slotMirrorLayerY()
{
	m_pDoc->mirrorLayerY(0);
}

#include "kimageshop_view.moc"


