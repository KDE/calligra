/*
 *  kimageshop_view.cc - part of KImageShop
 *
 *  Copyright (c) 1999 The KImageShop team (see file AUTHORS)
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
#include <qwmatrix.h>
#include <kimgio.h>

#include <kfiledialog.h>
#include <kcolordlg.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kiconloader.h>
#include <klineeditdlg.h>
#include <kruler.h>
#include <kpixmapcache.h>

#include <opUIUtils.h>
#include <opMainWindow.h>
#include <opMainWindowIf.h>

#include <koPartSelectDia.h>
#include <koAboutDia.h>

#include "zoomfactordlg.h"
#include "preferencesdlg.h"

#include "kimageshop_doc.h"
#include "kimageshop_view.h"
#include "kimageshop_shell.h"
#include "canvas.h"
#include "brush.h"
#include "layerdlg.h"
#include "tool.h"
#include "movetool.h"
#include "brushtool.h"

#define CHECK_DOCUMENT \
	if( m_pDoc->isEmpty() ) \
	{ \
	  kdebug( KDEBUG_INFO, 0, "Document is empty. Action won't be executed." ); \
	  return; \
	}

#define CHECK_RUNNING \
	if( m_pDoc->m_executeCommand ) \
	{ \
	  kdebug( KDEBUG_INFO, 0, "Document is empty. External command won't be executed." ); \
	  return; \
	}
	
#define CHECK_ALL \
	CHECK_DOCUMENT \
	CHECK_RUNNING


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

  QObject::connect( m_pDoc, SIGNAL( sigUpdateView() ), this, SLOT( slotUpdateView() ) );

  slotUpdateView();
}

KImageShopView::~KImageShopView()
{
  kdebug( KDEBUG_INFO, 0, "KImageShopView::~KImageShopView() %li", _refcnt() );
  cleanUp();
}

void KImageShopView::init()
{
  // register the view at the parent parts UI managers
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
  m_pDoc->removeView( this );

  delete m_pHorz;
  delete m_pVert;
  delete m_pHRuler;
  delete m_pVRuler;
  delete m_pCanvasView;
  delete m_pLayerDialog;
  
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

bool KImageShopView::mappingCreateToolbar( OpenPartsUI::ToolBarFactory_ptr _factory )
{
  kdebug( KDEBUG_INFO, 0, "ImageShopView::mappingCreateToolbar" );

  if ( CORBA::is_nil( _factory ) )
  {
    // ToolBarFactory is nil -> we lost control over the toolbar
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vToolBarEdit = 0L;
    m_vToolBarTools = 0L;
    return true;
  }

  // create and enable edit toolbar
  m_vToolBarEdit  = _factory->create(OpenPartsUI::ToolBarFactory::Transient);
  m_vToolBarEdit->enable(OpenPartsUI::Show);
 
  // create and enable tools toolbar
  m_vToolBarTools  = _factory->create(OpenPartsUI::ToolBarFactory::Transient);
  m_vToolBarTools->enable(OpenPartsUI::Show);
  m_vToolBarTools->setFullWidth(false);
  m_vToolBarTools->setBarPos(OpenPartsUI::Left);

  CORBA::WString_var text;
  OpenPartsUI::Pixmap_var pix;
  
  // move tool
  text = Q2C(i18n("Move tool"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("move.xpm"));
  m_vToolBarTools->insertButton2(pix, TBTOOLS_MOVETOOL, SIGNAL(clicked()), this, "slotActivateMoveTool", true, text, -1);
  m_vToolBarTools->setToggle(TBTOOLS_MOVETOOL, true);
  m_vToolBarTools->toggleButton(TBTOOLS_MOVETOOL);

  // paint brush
  text = Q2C(i18n("Paint brush"));
  pix = OPUIUtils::convertPixmap(*KPixmapCache::toolbarPixmap("paintbrush.xpm"));
  m_vToolBarTools->insertButton2(pix, TBTOOLS_BRUSHTOOL, SIGNAL(clicked()), this, "slotActivateBrushTool", true, text, -1);
  m_vToolBarTools->setToggle(TBTOOLS_BRUSHTOOL, true);

  kdebug( KDEBUG_INFO, 0, "KImageShopView::mappingCreateToolbar : done" );
  return true;
}

bool KImageShopView::mappingCreateMenubar( OpenPartsUI::MenuBar_ptr menubar )
{
  kdebug( KDEBUG_INFO, 0, "KImageShopView::mappingCreateMenubar" );

  if ( CORBA::is_nil( menubar ) )
  {
    // MenuBar_ptr is nil -> we lost control over the menubar
    kdebug( KDEBUG_INFO, 0, "Setting to nil" );
    m_vMenuEdit = 0L;
    m_vMenuView = 0L;
    m_vMenuTransform = 0L;
    m_vMenuPlugIns = 0L;
    m_vMenuOptions = 0L;
    return true;
  }
  
  // don't use Q2C directly in arguments to avoid memory leaks!
  CORBA::WString_var text;

  // edit menu
  text = Q2C( i18n( "&Edit" ) );
  menubar->insertMenu( text , m_vMenuEdit, -1, -1 );

  // view menu
  text = Q2C( i18n( "&View" ) );
  menubar->insertMenu( text , m_vMenuView, -1, -1 );
m_vMenuView->setCheckable( true );

  text = Q2C( i18n( "&Layer dialog" ) );
  m_idMenuView_LayerDialog = m_vMenuView->insertItem( text, this, "viewLayerDialog", 0 );

  // transform menu
  text = Q2C( i18n( "&Transform" ) );
  menubar->insertMenu( text , m_vMenuTransform, -1, -1 );

  // plugins menu
  text = Q2C( i18n( "&Plugins" ) );
  menubar->insertMenu( text , m_vMenuPlugIns, -1, -1 );

  // options menu
  text = Q2C( i18n( "&Options" ) );
  menubar->insertMenu( text , m_vMenuOptions, -1, -1 );

  return true;
}

void KImageShopView::createGUI()
{
  kimgioRegister();
  
  // create canvasview
  m_pCanvasView = new CanvasView(this);
  
  // setup GUI
  setupScrollbars();
  setupRulers();
  setRanges();

  // register canvasview at the canvas
  m_pDoc->canvas_()->addView(m_pCanvasView);

  // connect canvasview
  QObject::connect(m_pCanvasView, SIGNAL(sigPaint(QPaintEvent*)), this, SLOT(slotCVPaint(QPaintEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMousePress(QMouseEvent*)), this, SLOT(slotCVMousePress(QMouseEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMouseMove(QMouseEvent*)), this, SLOT(slotCVMouseMove(QMouseEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMouseRelease(QMouseEvent*)), this, SLOT(slotCVMouseRelease(QMouseEvent*)));

  // create default (move) tool
  m_pMoveTool = new MoveTool(m_pDoc->canvas_());
  m_pTool = m_pMoveTool;
  
  // layerlist
  m_pLayerDialog = new LayerDialog(m_pDoc->canvas_());
  m_pLayerDialog->show();
  m_pLayerDialog->resize(150,200);

  resizeEvent( 0L );
}

void KImageShopView::setupScrollbars()
{
  m_pVert = new QScrollBar( QScrollBar::Vertical, this );
  m_pHorz = new QScrollBar( QScrollBar::Horizontal, this );
  m_pVert->show();
  m_pHorz->show();
  //QObject::connect( m_pVert, SIGNAL( valueChanged( int ) ), this, SLOT( scrollV( int ) ) );
  //QObject::connect( m_pHorz, SIGNAL( valueChanged( int ) ), this, SLOT( scrollH( int ) ) );
  m_pVert->setValue(m_pVert->maxValue());
  m_pHorz->setValue(m_pHorz->maxValue());
  m_pVert->setValue(m_pVert->minValue());
  m_pHorz->setValue(m_pHorz->minValue());
  
  if ( m_pCanvasView ) 
    m_pCanvasView->resize( widget()->width()-16, widget()->height()-16 );
  
  m_pVert->setGeometry( widget()->width()-16, 0, 16, widget()->height()-16 );
  m_pHorz->setGeometry( 0, widget()->height()-16, widget()->width()-16, 16 );
}

void KImageShopView::setupRulers()
{
  m_pHRuler = new KRuler( KRuler::horizontal, this );
  m_pVRuler = new KRuler( KRuler::vertical, this );
  m_pCanvasView->resize( m_pCanvasView->width() - 20, m_pCanvasView->height() - 20 );
  m_pCanvasView->move( 20, 20 );
  m_pHRuler->setGeometry( 20, 0, m_pCanvasView->width(), 20 );
  m_pVRuler->setGeometry( 0, 20, 20, m_pCanvasView->height() );

  m_pVRuler->showTinyMarks(true);
  m_pVRuler->showLittleMarks(true);
  m_pVRuler->showMediumMarks(true);
  m_pVRuler->showBigMarks(true);
  m_pVRuler->showEndMarks(true);

  m_pHRuler->showTinyMarks(true);
  m_pHRuler->showLittleMarks(true);
  m_pHRuler->showMediumMarks(true);
  m_pHRuler->showBigMarks(true);
  m_pHRuler->showEndMarks(true);
}

void KImageShopView::setRanges()
{
  m_pVRuler->setRange(0, m_pCanvasView->height());
  m_pHRuler->setRange(0, m_pCanvasView->width());

  m_pVert->setRange(0, m_pCanvasView->height());
  m_pHorz->setRange(0, m_pCanvasView->width());
}

void KImageShopView::scrollH( int _value )
{
  m_pCanvasView->scroll( _value, 0 );
}

void KImageShopView::scrollV( int _value )
{
  m_pCanvasView->scroll( 0, _value );
}

void KImageShopView::newView()
{
  ASSERT( m_pDoc != 0L );

  KImageShopShell* shell = new KImageShopShell;
  shell->show();
  shell->setDocument( m_pDoc );
}

KImageShopDoc* KImageShopView::doc()
{
  return m_pDoc;
}

void KImageShopView::helpUsing()
{
  kapp->invokeHTMLHelp( "kimageshop/kimage.html", QString::null );
}

CORBA::Boolean KImageShopView::printDlg()
{
  QPrinter prt;

  if( QPrintDialog::getPrinterSetup( &prt ) )
  {
    m_pDoc->print( &prt );
  }
  return true;
}

void KImageShopView::resizeEvent( QResizeEvent* )
{
  if ( ( KoViewIf::hasFocus() || mode() == KOffice::View::RootMode ) )
    {
      m_pHorz->show();
      m_pVert->show();
      
      if ( m_pHRuler )
	m_pHRuler->show();
      if ( m_pVRuler )
	m_pVRuler->show();
	
      m_pCanvasView->resize( widget()->width() - 36, widget()->height() - 36 );
      m_pCanvasView->move( 20, 20 );
      m_pVert->setGeometry( widget()->width() - 16, 0, 16, widget()->height() - 16 );
      m_pHorz->setGeometry( 0, widget()->height() - 16, widget()->width() - 16, 16 );
      
      if ( m_pHRuler )
	m_pHRuler->setGeometry( 20, 0, m_pCanvasView->width(), 20 );
      if ( m_pVRuler )
	m_pVRuler->setGeometry( 0, 20, 20, m_pCanvasView->height() );
    }
  else
    {
      m_pHorz->hide();
      m_pVert->hide();

      if ( m_pHRuler )
	m_pHRuler->hide();
      if ( m_pVRuler )
	m_pVRuler->hide();

      m_pCanvasView->move( 0, 0 );
      m_pCanvasView->resize( widget()->width(), widget()->height() );
    }

  slotUpdateView();
}

void KImageShopView::slotActivateMoveTool()
{
  if (!m_pMoveTool)
    {
      m_pMoveTool = new MoveTool(m_pDoc->canvas_());
    }

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
}

void KImageShopView::slotActivateBrushTool()
{
  if (!m_pBrush)
    {
      // we have no brush -> create a default one
      QString _image = locate("data", "kimageshop/brushes/brush.jpg");
      m_pBrush = new brush(_image);
      m_pBrush->setHotSpot(QPoint(25,25));
    }
  
  if (!m_pBrushTool)
    m_pBrushTool = new BrushTool(m_pDoc->canvas_(), m_pBrush);

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
}

void KImageShopView::slotUpdateView()
{
  // if( m_pDoc->image().isNull() )
  //{
  //  return;
  // }

  //m_pixmap.convertFromImage( m_pDoc->image() );
  QWidget::update();
}

void KImageShopView::slotCVPaint(QPaintEvent *e)
{
  m_pDoc->canvas_()->repaintView(m_pCanvasView, e->rect());
}

void KImageShopView::slotCVMousePress(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  KImageShop::MouseEvent mouseEvent;
  mouseEvent.posX = e->x();
  mouseEvent.posY = e->y();
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
  mouseEvent.posX = e->x();
  mouseEvent.posY = e->y();
  mouseEvent.globalPosX = e->globalX();
  mouseEvent.globalPosY = e->globalY();
  
  mouseEvent.leftButton = (e->button() & LeftButton) ? true : false;
  mouseEvent.rightButton = (e->button() & RightButton) ? true : false;
  mouseEvent.midButton = (e->button() & MidButton) ? true : false;
  
  mouseEvent.shiftButton = (e->state() & ShiftButton) ? true : false;
  mouseEvent.controlButton = (e->state() & ControlButton) ? true : false;
  mouseEvent.altButton = (e->state() & AltButton) ? true : false;

  m_pTool->mouseMove(mouseEvent);
}

void KImageShopView::slotCVMouseRelease(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  KImageShop::MouseEvent mouseEvent;
  mouseEvent.posX = e->x();
  mouseEvent.posY = e->y();
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

void KImageShopView::paintEvent( QPaintEvent * )
{
  // FIME: do we need this? The child widgets handle the paintevent.
}

void KImageShopView::viewLayerDialog()
{
  if( m_pLayerDialog )
  {
    if( m_pLayerDialog->isVisible() )
    {
      m_pLayerDialog->hide();
    }
    else
    {
      m_pLayerDialog->show();
    }
//  m_vMenuView->setItemChecked( m_idMenuView_LayerDialog, m_pLayerDialog->isVisible() );
    m_vMenuView->setItemChecked( m_idMenuView_LayerDialog, TRUE );
  }
}

#include "kimageshop_view.moc"
