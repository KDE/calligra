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

#include <kimgio.h>
#include <kfiledialog.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kstddirs.h>
#include <kstdaccel.h>
#include <kiconloader.h>
#include <kruler.h>
#include <kpixmapcache.h>

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
#include "tool.h"
#include "movetool.h"
#include "brushtool.h"
#include "zoomtool.h"

// ugly hack around none working macro ICON()
#undef OPICON
#define OPICON( name ) OPUIUtils::convertPixmap( *KPixmapCache::toolbarPixmap( name ) );

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
  m_pHorz = 0L;
  m_pVert = 0L;
  m_pHRuler = 0L;
  m_pVRuler = 0L;
  m_ZoomFactor = 1;
  m_fg = QColor(255,255,255);
  m_bg = QColor(0,0,0);

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
  delete m_pMoveTool;
  delete m_pBrushTool;
  delete m_pZoomTool;
    
  KoViewIf::cleanUp();
}

bool KImageShopView::event( const char* _event, const CORBA::Any& _value )
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

  CORBA::WString_var text;
  OpenPartsUI::Pixmap_var pix;

  // edit toolbar
  m_vToolBarEdit = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarEdit->enable( OpenPartsUI::Show );
  m_vToolBarEdit->setFullWidth( false );
  m_vToolBarEdit->setBarPos( OpenPartsUI::Top );

  // undo
  text = Q2C( i18n( "Undo last action." ) );
  pix = OPICON( "undo.xpm" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_UNDO, SIGNAL( clicked() ), this, "slotEditUndo", true, text, -1 );
  m_vToolBarEdit->setDelayedPopup( TBEDIT_UNDO, m_vTBUndoMenu );
  m_vToolBarEdit->setItemEnabled( TBEDIT_UNDO, false );

  // redo
  text = Q2C( i18n( "Redo last action." ) );
  pix = OPICON( "redo.xpm" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_REDO, SIGNAL( clicked() ), this, "slotEditRedo", true, text, -1 );
  m_vToolBarEdit->setDelayedPopup( TBEDIT_REDO, m_vTBRedoMenu );
  m_vToolBarEdit->setItemEnabled( TBEDIT_REDO, false );

  m_vToolBarEdit->insertSeparator( -1 );

  // cut
  text = Q2C( i18n( "Cut selected area." ) );
  pix = OPICON( "editcut.xpm" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_CUT, SIGNAL( clicked() ), this, "slotEditCut", true, text, -1 );
  m_vToolBarEdit->setItemEnabled( TBEDIT_CUT, false );

  // copy
  text = Q2C( i18n( "Copy selected area." ) );
  pix = OPICON( "editcopy.xpm" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_COPY, SIGNAL( clicked() ), this, "slotEditCopy", true, text, -1 );
  m_vToolBarEdit->setItemEnabled( TBEDIT_COPY, false );

  // paste
  text = Q2C( i18n( "Paste clipboard into active layer." ) );
  pix = OPICON( "editpaste.xpm" );
  m_vToolBarEdit->insertButton2( pix, TBEDIT_PASTE, SIGNAL( clicked() ), this, "slotEditPaste", true, text, -1 );
  m_vToolBarEdit->setItemEnabled( TBEDIT_PASTE, false );

  // tools toolbar
  m_vToolBarTools = _factory->create( OpenPartsUI::ToolBarFactory::Transient );
  m_vToolBarTools->enable( OpenPartsUI::Show );
  m_vToolBarTools->setFullWidth( false );
  m_vToolBarTools->setBarPos( OpenPartsUI::Left );

  // move tool
  text = Q2C( i18n( "Move layers and selections." ) );
  pix = OPICON( "move.xpm" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_MOVETOOL, SIGNAL( clicked() ), this, "slotActivateMoveTool", true, text, -1 );
  m_vToolBarTools->setToggle( TBTOOLS_MOVETOOL, true );
  m_vToolBarTools->toggleButton( TBTOOLS_MOVETOOL );

  // zoom tool
  text = Q2C( i18n( "Zoom in/out." ) );
  pix = OPICON( "viewmag.xpm" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_ZOOMTOOL, SIGNAL(clicked()), this, "slotActivateZoomTool", true, text, -1 );
  m_vToolBarTools->setToggle( TBTOOLS_ZOOMTOOL, true );

  // paint tool
  text = Q2C( i18n( "Paint using a brush." ) );
  pix = OPICON( "paintbrush.xpm" );
  m_vToolBarTools->insertButton2( pix, TBTOOLS_BRUSHTOOL, SIGNAL(clicked()), this, "slotActivateBrushTool", true, text, -1 );
  m_vToolBarTools->setToggle( TBTOOLS_BRUSHTOOL, true );

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
  CORBA::WString_var text;
  OpenPartsUI::Pixmap_var pix;

  // edit menu
  text = Q2C( i18n( "&Edit" ) );
  menubar->insertMenu( text , m_vMenuEdit, -1, -1 );

  text = Q2C( i18n( "&Undo" ) );
  pix = OPICON( "undo.xpm" );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem3( pix, text, this, "slotEditUndo", stdAccel.undo() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );

  text = Q2C( i18n( "&Redo" ) );
  pix = OPICON( "redo.xpm" );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem3( pix, text, this, "slotEditRedo", stdAccel.redo() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );

  m_vMenuEdit->insertSeparator( -1 );

  text = Q2C( i18n( "&Cut" ) );
  pix = OPICON( "editcut.xpm" );
  m_idMenuEdit_Cut = m_vMenuEdit->insertItem3( pix, text, this, "slotEditCut", stdAccel.cut() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Cut, false );

  text = Q2C( i18n( "&Copy" ) );
  pix = OPICON( "editcopy.xpm" );
  m_idMenuEdit_Copy = m_vMenuEdit->insertItem3( pix, text, this, "slotEditCopy", stdAccel.cut() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Copy, false );

  text = Q2C( i18n( "&Paste" ) );
  pix = OPICON( "editpaste.xpm" );
  m_idMenuEdit_Paste = m_vMenuEdit->insertItem3( pix, text, this, "slotEditPaste", stdAccel.cut() );
  m_vMenuEdit->setItemEnabled( m_idMenuEdit_Paste, false );

  // view menu
  text = Q2C( i18n( "&View" ) );
  menubar->insertMenu( text , m_vMenuView, -1, -1 );
  m_vMenuView->setCheckable( true );

  text = Q2C( i18n( "&Layer dialog" ) );
  m_idMenuView_LayerDialog = m_vMenuView->insertItem( text, this, "viewLayerDialog", 0 );
  m_vMenuEdit->setItemChecked( m_idMenuView_LayerDialog, false );

  text = Q2C( i18n( "&Color dialog" ) );
  m_idMenuView_ColorDialog = m_vMenuView->insertItem( text, this, "viewColorDialog", 0 );

  m_vMenuView->insertSeparator( -1 );

  text = Q2C( i18n( "&Preferences" ) );
  m_idMenuView_Preferences = m_vMenuView->insertItem( text, this, "viewPreferences", 0 );

  // image menu
  text = Q2C( i18n( "&Image" ) );
  menubar->insertMenu( text , m_vMenuImage, -1, -1 );

  // plugins menu
  text = Q2C( i18n( "&Plugins" ) );
  menubar->insertMenu( text , m_vMenuPlugIns, -1, -1 );

  // options menu
  text = Q2C( i18n( "&Options" ) );
  menubar->insertMenu( text , m_vMenuOptions, -1, -1 );

  kdebug( KDEBUG_INFO, 0, "KImageShopView::mappingCreateMenubar : DONE" );

  return true;
}

void KImageShopView::createGUI()
{
  kimgioRegister();
  
  // create canvasview
  m_pCanvasView = new CanvasView(this);

  // m_pColorDialog = new ColorDialog(m_pCanvasView);
  //m_pColorDialog->move(100, 100);
  //m_pColorDialog->show();
  
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

  // create a default brush
  QString _image = locate("data", "kimageshop/brushes/brush.jpg");
  m_pBrush = new Brush(_image);
  m_pBrush->setHotSpot(QPoint(25,25));
  
  // create brush tool
  m_pBrushTool = new BrushTool(m_pDoc, m_pBrush);

  // create zoom tool
  m_pZoomTool = new ZoomTool(this);

  // layerlist
  m_pLayerDialog = new LayerDialog(m_pDoc, this);
  m_pLayerDialog->resize(205,267);
  m_pLayerDialog->move(200,20);
  m_pLayerDialog->show();
  
  resizeEvent(0L);
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

CORBA::Boolean KImageShopView::printDlg()
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
  return (viewWidth() > docWidth()) ? static_cast<int>((viewWidth() -  docWidth())/2) : 0;
}

int KImageShopView::yPaintOffset()
{
  return (viewHeight() > docHeight()) ? static_cast<int>((viewHeight() - docHeight())/2) : 0;
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

void KImageShopView::slotActivateMoveTool()
{
  if (!m_pMoveTool)
      m_pMoveTool = new MoveTool(m_pDoc);

  m_pTool = m_pMoveTool;
  
  if(m_vToolBarTools->isButtonOn(TBTOOLS_MOVETOOL))
    {
      // move tool is already on but will automatically be toggled by
      // ktoolbar code -> toggle it by hand to keep it on.
      m_vToolBarTools->isButtonOn(TBTOOLS_MOVETOOL);
    }

  // shut off brushtool (move this to a function as soon as
  // we have more tools.
  if(m_vToolBarTools->isButtonOn(TBTOOLS_BRUSHTOOL))
    m_vToolBarTools->toggleButton(TBTOOLS_BRUSHTOOL);
  if(m_vToolBarTools->isButtonOn(TBTOOLS_ZOOMTOOL))
    m_vToolBarTools->toggleButton(TBTOOLS_ZOOMTOOL);
}

void KImageShopView::slotActivateBrushTool()
{
  if (!m_pBrush)
    {
      // we have no brush -> create a default one
      QString _image = locate("data", "kimageshop/brushes/brush.jpg");
      m_pBrush = new Brush(_image);
      m_pBrush->setHotSpot(QPoint(25,25));
    }
  
  if (!m_pBrushTool)
    m_pBrushTool = new BrushTool(m_pDoc, m_pBrush);

  m_pTool = m_pBrushTool;

  if(m_vToolBarTools->isButtonOn(TBTOOLS_BRUSHTOOL))
    {
      // brush tool is already on but will automatically be toggled by
      // ktoolbar code -> toggle it by hand to keep it on.
      m_vToolBarTools->isButtonOn(TBTOOLS_BRUSHTOOL);
    }
  
  // shut off movetool (move this to a function as soon as
  // we have more tools.
  if(m_vToolBarTools->isButtonOn(TBTOOLS_MOVETOOL))
    m_vToolBarTools->toggleButton(TBTOOLS_MOVETOOL);
  if(m_vToolBarTools->isButtonOn(TBTOOLS_ZOOMTOOL))
    m_vToolBarTools->toggleButton(TBTOOLS_ZOOMTOOL);
}

void KImageShopView::slotActivateZoomTool()
{
  if (!m_pZoomTool)
    m_pZoomTool = new ZoomTool(this);

  m_pTool = m_pZoomTool;
  
  if(m_vToolBarTools->isButtonOn(TBTOOLS_ZOOMTOOL))
    {
      // brush tool is already on but will automatically be toggled by
      // ktoolbar code -> toggle it by hand to keep it on.
      m_vToolBarTools->isButtonOn(TBTOOLS_ZOOMTOOL);
    }
  
  // shut off movetool (move this to a function as soon as
  // we have more tools.
  if(m_vToolBarTools->isButtonOn(TBTOOLS_MOVETOOL))
    m_vToolBarTools->toggleButton(TBTOOLS_MOVETOOL);
  if(m_vToolBarTools->isButtonOn(TBTOOLS_BRUSHTOOL))
    m_vToolBarTools->toggleButton(TBTOOLS_BRUSHTOOL);
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

void KImageShopView::slotEditUndo()
{
  m_pDoc->commandHistory()->undo();
}

void KImageShopView::slotEditUndo2() {}
void KImageShopView::slotEditUndo3() {}
void KImageShopView::slotEditUndo4() {}
void KImageShopView::slotEditUndo5() {}
void KImageShopView::slotEditUndo6() {}
void KImageShopView::slotEditUndo7() {}
void KImageShopView::slotEditUndo8() {}
void KImageShopView::slotEditUndo9() {}
void KImageShopView::slotEditUndo10() {}
 
void KImageShopView::slotEditRedo()
{
  m_pDoc->commandHistory()->redo();
}

void KImageShopView::slotEditRedo2() {}
void KImageShopView::slotEditRedo3() {}
void KImageShopView::slotEditRedo4() {}
void KImageShopView::slotEditRedo5() {}
void KImageShopView::slotEditRedo6() {}
void KImageShopView::slotEditRedo7() {}
void KImageShopView::slotEditRedo8() {}
void KImageShopView::slotEditRedo9() {}
void KImageShopView::slotEditRedo10() {}
 
void KImageShopView::slotEditCut()
{
}
 
void KImageShopView::slotEditCopy()
{
}
 
void KImageShopView::slotEditPaste()
{
}

void KImageShopView::viewLayerDialog()
{
  if( m_pLayerDialog )
  {
    if( m_pLayerDialog->isVisible() )
      m_pLayerDialog->hide();
    else
       m_pLayerDialog->show();
      
    // TODO: make this working
    m_vMenuView->setItemChecked( m_idMenuView_LayerDialog, true );
  }
}

void KImageShopView::viewColorDialog()
{
}

void KImageShopView::viewPreferences()
{
}

void KImageShopView::changeUndo( QString _text, bool _enable )
{
  cout << "ImageShopView::changeUndo : ";

  CORBA::WString_var text;
 
  if( _enable )
  {
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, true );
    QString str;
    str.sprintf( i18n( "Undo: %s" ), _text.data() );
    text = Q2C( str );
    m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
    m_vToolBarEdit->setItemEnabled( TBEDIT_UNDO, true );

    cout << "UNDO enabled" << endl;
  }
  else
  {
    text = Q2C( i18n( "No Undo possible" ) );
    m_vMenuEdit->changeItemText( text, m_idMenuEdit_Undo );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Undo, false );
    m_vToolBarEdit->setItemEnabled( TBEDIT_UNDO, false );

    cout << "UNDO not enabled" << endl;
  }
}

void KImageShopView::changeRedo( QString _text, bool _enable )
{
  cout << "KImageShopView::changeRedo : ";

  CORBA::WString_var text;
 
  if( _enable )
  {
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, true );
    QString str;
    str.sprintf( i18n( "Redo: %s" ), _text.data() );
    text = Q2C( str );
    m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
    m_vToolBarEdit->setItemEnabled( TBEDIT_REDO, true );

    cout << "REDO enabled" << endl;
  }
  else
  {
    text = Q2C( i18n( "No Redo possible" ) );
    m_vMenuEdit->changeItemText( text, m_idMenuEdit_Redo );
    m_vMenuEdit->setItemEnabled( m_idMenuEdit_Redo, false );
    m_vToolBarEdit->setItemEnabled( TBEDIT_REDO, false );

    cout << "REDO not enabled" << endl;
  }
}

void  KImageShopView::slotSetFGColor(const QColor& c)
{
  m_fg = c;
}

void  KImageShopView::slotSetBGColor(const QColor& c)
{
  m_bg = c;
}

#include "kimageshop_view.moc"
