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
#include "canvasview.h"
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

  QObject::connect( m_pDoc, SIGNAL( sigUpdateView(const QRect&) ), this, SLOT( slotUpdateView(const QRect&) ) );
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

  text = Q2C( i18n( "&Undo" ) );
  m_idMenuEdit_Undo = m_vMenuEdit->insertItem( text, this, "editUndo", 0 );

  text = Q2C( i18n( "&Redo" ) );
  m_idMenuEdit_Redo = m_vMenuEdit->insertItem( text, this, "editRedo", 0 );

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

  kdebug( KDEBUG_INFO, 0, "KImageShopView::mappingCreateMenubar : DONE" );

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

  // connect canvasview
  QObject::connect(m_pCanvasView, SIGNAL(sigPaint(QPaintEvent*)), this, SLOT(slotCVPaint(QPaintEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMousePress(QMouseEvent*)), this, SLOT(slotCVMousePress(QMouseEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMouseMove(QMouseEvent*)), this, SLOT(slotCVMouseMove(QMouseEvent*)));
  QObject::connect(m_pCanvasView, SIGNAL(sigMouseRelease(QMouseEvent*)), this, SLOT(slotCVMouseRelease(QMouseEvent*)));

  // create move tool
  m_pMoveTool = new MoveTool(m_pDoc);
  m_pTool = m_pMoveTool;

  // we have no brush -> create a default one
  QString _image = locate("data", "kimageshop/brushes/brush.jpg");
  m_pBrush = new Brush(_image);
  m_pBrush->setHotSpot(QPoint(25,25));
  
  // create brush tool
  m_pBrushTool = new BrushTool(m_pDoc, m_pBrush);

  // layerlist
  m_pLayerDialog = new LayerDialog(m_pDoc);
  m_pLayerDialog->show();
  m_pLayerDialog->resize(150,200);

  resizeEvent( 0L );
}

void KImageShopView::setupScrollbars()
{
  m_pVert = new QScrollBar( QScrollBar::Vertical, this );
  m_pHorz = new QScrollBar( QScrollBar::Horizontal, this );

  if (m_pCanvasView)
    m_pCanvasView->resize( widget()->width()-16, widget()->height()-16 );
  
  m_pVert->setGeometry( widget()->width()-16, 0, 16, widget()->height()-16 );
  m_pHorz->setGeometry( 0, widget()->height()-16, widget()->width()-16, 16 );
  m_pVert->show();
  m_pHorz->show();

  QObject::connect( m_pVert, SIGNAL( valueChanged( int ) ), this, SLOT( scrollV( int ) ) );
  QObject::connect( m_pHorz, SIGNAL( valueChanged( int ) ), this, SLOT( scrollH( int ) ) );

}

void KImageShopView::setupRulers()
{
  m_pHRuler = new KRuler( KRuler::horizontal, this );
  m_pVRuler = new KRuler( KRuler::vertical, this );
  m_pCanvasView->resize( m_pCanvasView->width() - 20, m_pCanvasView->height() - 20);
  m_pCanvasView->move(20, 20);
  m_pHRuler->setGeometry(20, 0, m_pCanvasView->width(), 20);
  m_pVRuler->setGeometry(0, 20, 20, m_pCanvasView->height());

  m_pVRuler->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
  m_pHRuler->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  m_pVRuler->setRulerStyle(KRuler::pixel);
  m_pHRuler->setRulerStyle(KRuler::pixel);
}

void KImageShopView::scrollH(int )
{
  slotCVPaint(0L);
  m_pHRuler->setOffset(m_pHorz->value());
}

void KImageShopView::scrollV(int )
{
  slotCVPaint(0L);
  m_pVRuler->setOffset(m_pVert->value());
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

void KImageShopView::resizeEvent(QResizeEvent*)
{
  if ((KoViewIf::hasFocus() || mode() == KOffice::View::RootMode))
    {
      int docHeight = m_pDoc->height();
      int docWidth = m_pDoc->width();

      m_pHRuler->show();
      m_pVRuler->show();

      m_pCanvasView->move(20, 20);

      if (docHeight <= (height() - 20) && docWidth <= (width() - 20))
	{
	  m_pVert->hide();
	  m_pHorz->hide();
	  m_pVert->setValue(0);
	  m_pHorz->setValue(0);
	}
      else if (docHeight <= (height() - 20))
	{
	  m_pVert->hide();
	  m_pVert->setValue(0);
	  m_pHorz->setRange(0, docWidth - width() + 20);
	  m_pHorz->show();
	}
      else if(docWidth <= (width() - 20))
	{
	  m_pHorz->hide();
	  m_pHorz->setValue(0);
	  m_pVert->setRange(0, docHeight - height() + 20);
	  m_pVert->show();
	}
      else
	{
	  m_pVert->setRange(0, docHeight - height() + 36);
	  m_pVert->show();
	  m_pHorz->setRange(0, docWidth - width() + 36);
	  m_pHorz->show();
	}
	
      if (m_pHorz->isVisible() && m_pVert->isVisible())
	m_pCanvasView->resize( width() - 36, height() - 36 );
      else if (m_pHorz->isVisible())
	m_pCanvasView->resize( width() - 20, height() - 36 );
      else if (m_pVert->isVisible())
	m_pCanvasView->resize( width() - 36, height() - 20 );
      else
	m_pCanvasView->resize( width() - 20, height() - 20 );

      m_pHRuler->setGeometry(20, 0, m_pCanvasView->width(), 20);
      m_pVRuler->setGeometry(0, 20, 20, m_pCanvasView->height());

      m_pVRuler->setRange(0, docHeight);
      m_pHRuler->setRange(0, docWidth);

      if(!m_pVert->isVisible())
	m_pVRuler->setOffset(-(m_pCanvasView->height() - docHeight)/2);
      else
	m_pVRuler->setOffset(0);

      if(!m_pHorz->isVisible())
	m_pHRuler->setOffset(-(m_pCanvasView->width() -  docWidth)/2);
      else
	m_pHRuler->setOffset(0);
  

      if (m_pHorz->isVisible())
	m_pVert->setGeometry(width() - 16, 0, 16, height() - 16);
      else
	m_pVert->setGeometry(width() - 16, 0, 16, height());

      if (m_pVert->isVisible())
	m_pHorz->setGeometry(0, height() - 16, width() - 16, 16);
      else
	m_pHorz->setGeometry(0, height() - 16, width(), 16);
    }
  else
    {
      m_pHorz->hide();
      m_pVert->hide();

      if ( m_pHRuler )
	m_pHRuler->hide();
      if ( m_pVRuler )
	m_pVRuler->hide();

      m_pCanvasView->move(0, 0);
      m_pCanvasView->resize(widget()->width(), widget()->height());
    }
}

void KImageShopView::slotActivateMoveTool()
{
  if (!m_pMoveTool)
    {
      m_pMoveTool = new MoveTool(m_pDoc);
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
}

void KImageShopView::slotUpdateView(const QRect &_area) // _area in canvas coordiantes
{
  // viewrect in canvas coordinates
  QRect viewRect(m_pHorz->value(), m_pVert->value(), m_pCanvasView->width(), m_pCanvasView->height());

  // does the area intersect the viewrect?
  if (!_area.intersects(viewRect)) return;

  // repaint only the visible part
  QRect area = viewRect & _area;

  // offset
  QPoint offset(m_pHorz->value(), m_pVert->value());

  // paint offset
  int x = (m_pCanvasView->width() > m_pDoc->width()) ? ((m_pCanvasView->width() -  m_pDoc->width())/2) : 0;
  int y = (m_pCanvasView->height() > m_pDoc->height()) ? ((m_pCanvasView->height() - m_pDoc->height())/2) : 0;
  
  // repaint
  m_pDoc->paintPixmap(m_pCanvasView, area, offset, QPoint(x,y));
}

void KImageShopView::slotCVPaint(QPaintEvent *)
{
  // repaint the whole canvasview
  slotUpdateView(QRect(m_pHorz->value(), m_pVert->value(), m_pCanvasView->width(), m_pCanvasView->height()));
}

void KImageShopView::slotCVMousePress(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  // paint offset
  int x = (m_pCanvasView->width() > m_pDoc->width()) ? ((m_pCanvasView->width() -  m_pDoc->width())/2) : 0;
  int y = (m_pCanvasView->height() > m_pDoc->height()) ? ((m_pCanvasView->height() - m_pDoc->height())/2) : 0;
 
  KImageShop::MouseEvent mouseEvent;
  // postion in canvas coordinates
  mouseEvent.posX = e->x() + m_pHorz->value() - x;
  mouseEvent.posY = e->y() + m_pVert->value() - y;
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

  // paint offset
  int x = (m_pCanvasView->width() > m_pDoc->width()) ? ((m_pCanvasView->width() -  m_pDoc->width())/2) : 0;
  int y = (m_pCanvasView->height() > m_pDoc->height()) ? ((m_pCanvasView->height() - m_pDoc->height())/2) : 0;
 
  KImageShop::MouseEvent mouseEvent;
  // postion in canvas coordinates
  mouseEvent.posX = e->x() + m_pHorz->value() - x;
  mouseEvent.posY = e->y() + m_pVert->value() - y;
  mouseEvent.globalPosX = e->globalX();
  mouseEvent.globalPosY = e->globalY();
  
  mouseEvent.leftButton = (e->button() & LeftButton) ? true : false;
  mouseEvent.rightButton = (e->button() & RightButton) ? true : false;
  mouseEvent.midButton = (e->button() & MidButton) ? true : false;
  
  mouseEvent.shiftButton = (e->state() & ShiftButton) ? true : false;
  mouseEvent.controlButton = (e->state() & ControlButton) ? true : false;
  mouseEvent.altButton = (e->state() & AltButton) ? true : false;

  m_pTool->mouseMove(mouseEvent);
  
  m_pHRuler->slotNewValue(e->x() - x);
  m_pVRuler->slotNewValue(e->y() - y);
}

void KImageShopView::slotCVMouseRelease(QMouseEvent *e)
{
  if(!m_pTool)
    return;

  // paint offset
  int x = (m_pCanvasView->width() > m_pDoc->width()) ? ((m_pCanvasView->width() -  m_pDoc->width())/2) : 0;
  int y = (m_pCanvasView->height() > m_pDoc->height()) ? ((m_pCanvasView->height() - m_pDoc->height())/2) : 0;
 
  KImageShop::MouseEvent mouseEvent;
  // postion in canvas coordinates
  mouseEvent.posX = e->x() + m_pHorz->value() - x;
  mouseEvent.posY = e->y() + m_pVert->value() - y;
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

void KImageShopView::editUndo()
{
}

void KImageShopView::editRedo()
{
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
    m_vMenuView->setItemChecked(m_idMenuView_LayerDialog, true);
  }
}

#include "kimageshop_view.moc"
