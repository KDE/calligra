/*
 *  kis_view.cc - part of KImageShop
 *
 *  Copyright (c) 1999 Matthias Elter  <me@kde.org>
 *                1999 Michael Koch    <koch@kde.org>
 *                1999 Carsten Pfeiffer <pfeiffer@kde.org>
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

#include <qevent.h>
#include <qpainter.h>
#include <qbutton.h>
#include <qscrollbar.h>
#include <qstringlist.h>

#include <kdebug.h>
#include <kruler.h>
#include <kaction.h>
#include <klocale.h>
#include <khelpmenu.h>
#include <kaboutdata.h>
#include <kstdaction.h>
#include <kfiledialog.h>
#include <kiconloader.h>

#include "kis_view.h"
#include "kis_doc.h"
#include "kis_util.h"
#include "kis_canvas.h"
#include "kis_sidebar.h"
#include "kis_tabbar.h"
#include "kis_brush.h"
#include "kis_tool.h"
#include "kis_factory.h"
#include "kis_gradient.h"
#include "kis_pluginserver.h"

#include "kis_brushchooser.h"
#include "kis_layerview.h"
#include "kis_channelview.h"

#include "kis_dlg_gradient.h"
#include "kis_dlg_gradienteditor.h"
#include "kis_dlg_preferences.h"
#include "kis_dlg_new.h"
#include "kis_tool_select.h"
#include "kis_tool_move.h"
#include "kis_tool_zoom.h"
#include "kis_tool_brush.h"
#include "kis_tool_airbrush.h"
#include "kis_tool_pen.h"
#include "kis_tool_gradient.h"
#include "kis_tool_colorpicker.h"
#include "kis_tool_eraser.h"

//#define KISBarIcon( x ) BarIcon( x, KisFactory::global() )

KisView::KisView( KisDoc* doc, QWidget* parent, const char* name )
    : KoView( doc, parent, name )
    , m_pDoc( doc )
    , m_zoomFactor( 1.0 )
{
  setInstance( KisFactory::global() );
  setXMLFile( "kimageshop.rc" );

  QObject::connect( m_pDoc, SIGNAL( docUpdated( ) ),
                    this, SLOT( slotDocUpdated ( ) ) );
  QObject::connect( m_pDoc, SIGNAL( docUpdated( const QRect& ) ),
                    this, SLOT( slotDocUpdated ( const QRect& ) ) );

  m_fg = KisColor::black();
  m_bg = KisColor::white();

  setupCanvas();
  setupScrollBars();
  setupRulers();
  setupTabBar();
  setupActions();
  setupDialogs();
  setupSideBar();
  setupTools();
}

void KisView::setupCanvas()
{
  m_pCanvas = new KisCanvas(this, "kis_canvas");

  QObject::connect( m_pCanvas, SIGNAL( mousePressed( QMouseEvent* ) ),
                    this, SLOT( canvasGotMousePressEvent ( QMouseEvent* ) ) );

  QObject::connect( m_pCanvas, SIGNAL( mouseMoved( QMouseEvent* ) ),
                    this, SLOT( canvasGotMouseMoveEvent ( QMouseEvent* ) ) );

  QObject::connect( m_pCanvas, SIGNAL( mouseReleased (QMouseEvent* ) ),
		    this, SLOT( canvasGotMouseReleaseEvent ( QMouseEvent* ) ) );

  QObject::connect( m_pCanvas, SIGNAL( gotPaintEvent (QPaintEvent* ) ),
		    this, SLOT( canvasGotPaintEvent ( QPaintEvent* ) ) );

}

void KisView::setupSideBar()
{
  m_pSideBar = new KisSideBar(this, "kis_sidebar");

  // brush chooser
  m_pBrushChooser = new KisBrushChooser(this);
  m_pBrush = m_pBrushChooser->currentBrush();
  QObject::connect(m_pBrushChooser, SIGNAL(selected(const KisBrush *)),
				   this, SLOT(slotSetBrush(const KisBrush*)));

  m_pBrushChooser->setCaption("Brushes");
  m_pSideBar->plug(m_pBrushChooser);

  // layer view
  m_pLayerView = new KisLayerView(m_pDoc, this);
  m_pLayerView->setCaption("Layers");
  m_pSideBar->plug(m_pLayerView);

  // channel view
  m_pChannelView = new KisChannelView(m_pDoc, this);
  m_pChannelView->setCaption("Channels");
  m_pSideBar->plug(m_pChannelView);

  // activate brushes tab
  m_pSideBar->slotActivateTab("Brushes");

  // init sidebar
  m_pSideBar->slotSetBrush(*m_pBrush);
  m_pSideBar->slotSetFGColor(m_fg);
  m_pSideBar->slotSetBGColor(m_bg);

  connect(m_pSideBar, SIGNAL(fgColorChanged(const KisColor&)), this,
		  SLOT(slotSetFGColor(const KisColor&)));
  connect(m_pSideBar, SIGNAL(bgColorChanged(const KisColor&)), this,
		  SLOT(slotSetBGColor(const KisColor&)));
}

void KisView::setupScrollBars()
{
  m_pVert = new QScrollBar( QScrollBar::Vertical, this );
  m_pHorz = new QScrollBar( QScrollBar::Horizontal, this );

  m_pVert->setGeometry(width()-16, 20, 16, height()-36);
  m_pHorz->setGeometry(20, height()-16, width()-36, 16);
  m_pHorz->setValue(0);
  m_pVert->setValue(0);
  m_pVert->show();
  m_pHorz->show();

  QObject::connect(m_pVert, SIGNAL(valueChanged(int)), this, SLOT(scrollV(int)));
  QObject::connect(m_pHorz, SIGNAL(valueChanged(int)), this, SLOT(scrollH(int)));

}

void KisView::setupRulers()
{
  m_pHRuler = new KRuler(Qt::Horizontal, this);
  m_pVRuler = new KRuler(Qt::Vertical, this);

  m_pHRuler->setGeometry(20, 0, width()-20, 20);
  m_pVRuler->setGeometry(0, 20, 20, height()-20);

  m_pVRuler->setRulerMetricStyle(KRuler::Pixel);
  m_pHRuler->setRulerMetricStyle(KRuler::Pixel);

  m_pVRuler->setFrameStyle(QFrame::Panel | QFrame::Raised);
  m_pHRuler->setFrameStyle(QFrame::Panel | QFrame::Raised);
  m_pHRuler->setLineWidth(1);
  m_pVRuler->setLineWidth(1);
}

void KisView::setupTabBar()
{
  // tabbar
  m_pTabBar = new KisTabBar(this, m_pDoc);
  m_pTabBar->slotImageListUpdated();

  QObject::connect( m_pTabBar, SIGNAL( tabSelected( const QString& ) ),
		    m_pDoc, SLOT( setCurrentImage( const QString& ) ) );

  QObject::connect( m_pDoc, SIGNAL( imageListUpdated() ),
		    m_pTabBar, SLOT( slotImageListUpdated( ) ) );


  // tabbar control buttons
  m_pTabFirst = new QPushButton( this );
  m_pTabFirst->setPixmap( QPixmap( BarIcon( "tab_first" ) ) );
  QObject::connect( m_pTabFirst, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollFirst() ) );

  m_pTabLeft = new QPushButton( this );
  m_pTabLeft->setPixmap( QPixmap( BarIcon( "tab_left" ) ) );
  QObject::connect( m_pTabLeft, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollLeft() ) );

  m_pTabRight = new QPushButton( this );
  m_pTabRight->setPixmap( QPixmap( BarIcon( "tab_right" ) ) );
  QObject::connect( m_pTabRight, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollRight() ) );

  m_pTabLast = new QPushButton( this );
  m_pTabLast->setPixmap( QPixmap( BarIcon( "tab_last" ) ) );
  QObject::connect( m_pTabLast, SIGNAL( clicked() ), m_pTabBar, SLOT( slotScrollLast() ) );
}

void KisView::setupTools()
{
  // select tool
  m_pSelectTool = new SelectTool( m_pDoc, m_pCanvas );

  // move tool
  m_pMoveTool = new MoveTool(m_pDoc);

  // brush tool
  m_pBrushTool = new BrushTool(m_pDoc, this, m_pBrush);

  // airbrush tool
  m_pAirBrushTool = new AirBrushTool(m_pDoc, this, m_pBrush);

  // pen tool
  m_pPenTool = new PenTool(m_pDoc, this, m_pBrush);

  // eraser tool
  m_pEraserTool = new EraserTool(m_pDoc, this, m_pBrush);

  // color picker
  m_pColorPicker = new ColorPicker(m_pDoc, this);

  // zoom tool
  m_pZoomTool = new ZoomTool(this);

  // gradient tool
  m_pGradientTool = new GradientTool( m_pDoc, this, m_pCanvas, m_pGradient );

  m_tool_brush->setChecked( true );
  activateTool(m_pBrushTool);
}

void KisView::setupDialogs()
{
  // gradient dialog
  m_pGradientDialog = new GradientDialog( m_pDoc, this );
  m_pGradientDialog->resize( 206, 185 );
  m_pGradientDialog->move( 200, 290 );
  m_pGradientDialog->hide();
  connect( m_pGradientDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );

  // gradient editor dialog
  m_pGradientEditorDialog = new GradientEditorDialog( m_pDoc, this );
  m_pGradientEditorDialog->resize( 400, 200 );
  m_pGradientEditorDialog->move( 100, 190 );
  m_pGradientEditorDialog->hide();
  connect( m_pGradientEditorDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );

  updateToolbarButtons();
}

void KisView::setupActions()
{
  // edit actions
  
  // jwc - no undo - redo yet
  //m_undo = KStdAction::undo( this, SLOT( undo() ), actionCollection(), "undo");
  //m_redo = KStdAction::redo( this, SLOT( redo() ), actionCollection(), "redo");
  m_cut = KStdAction::cut( this, SLOT( cut() ), actionCollection(), "cut");
  m_copy = KStdAction::copy( this, SLOT( copy() ), actionCollection(), "copy");
  m_paste = KStdAction::paste( this, SLOT( paste() ), actionCollection(), "paste");

  // view actions

  new KAction( i18n("Zoom &in"), "viewmag+", 0, this,
               SLOT( zoom_in() ), actionCollection(), "zoom_in" );

  new KAction( i18n("Zoom &out"), "viewmag-", 0, this,
               SLOT( zoom_out() ), actionCollection(), "zoom_out" );

  m_dialog_gradient = new KToggleAction( i18n("&Gradient Dialog"),
										 "gradient_dialog", 0, this,
				   SLOT( dialog_gradient() ),actionCollection(), "dialog_gradient");
  m_dialog_gradienteditor = new KToggleAction( i18n("Gradient &Editor"),
											   "gradienteditor_dialog",
											   0, this, SLOT( dialog_gradienteditor() ),
											   actionCollection(), "dialog_gradienteditor");

  // tool actions

  m_tool_select_rect = new KToggleAction( i18n( "&Rectangular select" ),
										  "rectangular", 0, this,
										  SLOT( tool_select_rect() ), actionCollection(), "tool_select_rect" );
  m_tool_select_rect->setExclusiveGroup( "tools" );

  m_tool_select_polygon = new KToggleAction( i18n( "&Polygon select" ), "rectangular" , 0, this,
                             SLOT( tool_select_rect() ), actionCollection(), "tool_select_polygon" );
  m_tool_select_polygon->setExclusiveGroup( "tools" );

  m_tool_move = new KToggleAction( i18n("&Move tool"), "move", 0, this,
			     SLOT( tool_move() ),actionCollection(), "tool_move");
  m_tool_move->setExclusiveGroup( "tools" );

  m_tool_zoom = new KToggleAction( i18n("&Zoom tool"), "zoom", 0, this,
			     SLOT( tool_zoom() ),actionCollection(), "tool_zoom");
  m_tool_zoom->setExclusiveGroup( "tools" );

  m_tool_draw = new KToggleAction( i18n("&Draw simple figure"), "pencil", 0, this,
			      SLOT( tool_pen() ),actionCollection(), "tool_draw_figure");
  m_tool_draw->setExclusiveGroup( "tools" );

  m_tool_pen = new KToggleAction( i18n("&Pen tool"), "pencil", 0, this,
			      SLOT( tool_pen() ),actionCollection(), "tool_pen");
  m_tool_pen->setExclusiveGroup( "tools" );

  m_tool_brush = new KToggleAction( i18n("&Brush tool"), "paintbrush", 0, this,
			      SLOT( tool_brush() ),actionCollection(), "tool_brush");
  m_tool_brush->setExclusiveGroup( "tools" );

  m_tool_airbrush = new KToggleAction( i18n("&Airbrush tool"), "airbrush", 0, this,
				       SLOT( tool_airbrush() ),actionCollection(), "tool_airbrush");
  m_tool_airbrush->setExclusiveGroup( "tools" );
  m_tool_airbrush->setEnabled( false );

  m_tool_fill = new KToggleAction( i18n("&Filler tool"), "airbrush", 0, this,
				       SLOT( tool_airbrush() ),actionCollection(), "tool_fill");
  m_tool_fill->setExclusiveGroup( "tools" );

  m_tool_eraser = new KToggleAction( i18n("&Eraser tool"), "eraser", 0, this,
			      SLOT( tool_eraser() ),actionCollection(), "tool_eraser");
  m_tool_eraser->setExclusiveGroup( "tools" );

  m_tool_colorpicker = new KToggleAction( i18n("&Color picker"), "colorpicker", 0, this,
			      SLOT( tool_colorpicker() ),actionCollection(), "tool_colorpicker");
  m_tool_colorpicker->setExclusiveGroup( "tools" );

  m_tool_gradient = new KToggleAction( i18n("&Gradient tool"), "gradient", 0, this,
  				 SLOT( tool_gradient() ),actionCollection(), "tool_gradient");
  m_tool_gradient->setExclusiveGroup( "tools" );

  // layer actions

  (void) new KAction( i18n("&Insert layer..."), 0, this,
                      SLOT( insert_layer() ), actionCollection(), "insert_layer" );

  (void) new KAction( i18n("I&nsert image as layer..."), 0, this,
                      SLOT( insert_layer_image() ), actionCollection(), "insert_layer_image" );

  (void) new KAction( i18n("Rotate &180"), 0, this,
				   SLOT( layer_rotate180() ),actionCollection(), "layer_rotate180");

  (void) new KAction( i18n("Rotate &270"), 0, this,
				      SLOT( layer_rotateleft90() ),actionCollection(), "layer_rotateleft90");

  (void) new KAction( i18n("Rotate &90"), 0, this,
				       SLOT( layer_rotateright90() ),actionCollection(), "layer_rotateright90");

  (void) new KAction( i18n("Mirror &X"), 0, this,
				 SLOT( layer_mirrorX() ),actionCollection(), "layer_mirrorX");

  (void) new KAction( i18n("Mirror &Y"), 0, this,
				 SLOT( layer_mirrorY() ),actionCollection(), "layer_mirrorY");

  // image actions

  (void) new KAction( i18n("Merge &all layers"), 0, this,
				    SLOT( merge_all_layers() ),actionCollection(), "merge_all_layers");

  (void) new KAction( i18n("Merge &visible layers"), 0, this,
				    SLOT( merge_visible_layers() ),actionCollection(), "merge_visible_layers");

  (void) new KAction( i18n("Merge &linked layers"), 0, this,
				    SLOT( merge_linked_layers() ),actionCollection(), "merge_linked_layers");

  // setting actions

  (void) KStdAction::showMenubar( this, SLOT( showMenubar() ), actionCollection(), "show_menubar" );

  (void) KStdAction::showToolbar( this, SLOT( showToolbar() ), actionCollection(), "show_toolbar" );

  (void) KStdAction::showStatusbar( this, SLOT( showStatusbar() ), actionCollection(), "show_statusbar" );

  (void) new KRadioAction( i18n("Show &Sidebar"), 0, this,
                           SLOT( showSidebar() ), actionCollection(), "show_sidebar" );

  (void) KStdAction::saveOptions( this, SLOT( saveOptions() ), actionCollection(), "save_options" );

  (void) KStdAction::preferences( this, SLOT( preferences() ), actionCollection(), "configure");

  // help actions

  m_helpMenu = new KHelpMenu( this );

  (void) KStdAction::helpContents( m_helpMenu, SLOT( appHelpActivated() ), actionCollection(), "help_contents" );
  (void) KStdAction::whatsThis( m_helpMenu, SLOT( contextHelpActivated() ), actionCollection(), "help_whatsthis" );
  (void) KStdAction::reportBug( m_helpMenu, SLOT( reportBug() ), actionCollection(), "help_bugreport" );
  (void) KStdAction::aboutApp( m_helpMenu, SLOT( aboutApplication() ), actionCollection(), "help_about" );

  // disable at startup unused actions

  // jwc - no undo - redo yet
  //m_undo->setEnabled( false );
  //m_redo->setEnabled( false );
  m_cut->setEnabled( false );
  m_copy->setEnabled( false );
  m_paste->setEnabled( false );
}

void KisView::slotTabSelected(const QString& name)
{
  m_pDoc->setCurrentImage(name);
  resizeEvent(0L);
}

void KisView::resizeEvent(QResizeEvent*)
{
  int sideW = m_pSideBar->width();

  // sidebar geometry
  m_pSideBar->setGeometry(width()-sideW, 0, sideW, height());

  // ruler geometry
  m_pHRuler->setGeometry(20, 0, width()-20-sideW, 20);
  m_pVRuler->setGeometry(0, 20, 20, height()-36);

  // tabbar control buttons
  m_pTabFirst->setGeometry(0, height()-16, 16, 16);
  m_pTabLeft->setGeometry(16, height()-16, 16, 16);
  m_pTabRight->setGeometry(32, height()-16, 16, 16);
  m_pTabLast->setGeometry(48, height()-16, 16, 16);

  // KisView heigth/width - ruler heigth/width
  int drawH = height() - 20 - 16;
  int drawW = width() - 20 - sideW;
  int docH = docHeight();
  int docW = docWidth();

  // scrollbar geometry
  if (docH <= drawH && docW <= drawW) // we need no scrollbars
    {
      m_pVert->hide();
      m_pHorz->hide();
      m_pVert->setValue(0);
      m_pHorz->setValue(0);
      m_pCanvas->setGeometry(20, 20, drawW, drawH);
      m_pTabBar->setGeometry(64, height() - 16 , width() - sideW - 64, 16);
    }
  else if (docH <= drawH) // we need a horizontal scrollbar
    {
      m_pVert->hide();
      m_pVert->setValue(0);
      m_pHorz->setRange(0, docW - drawW);
      m_pHorz->setGeometry(64  + (width()-sideW-64)/2, height()-16, (width()-sideW-64)/2, 16);
      m_pHorz->show();
      m_pCanvas->setGeometry(20, 20, drawW, drawH);
      m_pTabBar->setGeometry(64, height() - 16 , (width()-sideW-64)/2, 16);
    }
  else if(docW <= drawW) // we need a vertical scrollbar
    {
      m_pHorz->hide();
      m_pHorz->setValue(0);
      m_pVert->setRange(0, docH - drawH);
      m_pVert->setGeometry(width()-16-sideW, 20, 16, height()-36);
      m_pVert->show();
      m_pCanvas->setGeometry(20, 20, drawW-16, drawH);
      m_pTabBar->setGeometry(64, height() - 16 , width() - sideW - 64, 16);
    }
  else // we need both scrollbars
    {
      m_pVert->setRange(0, docH - drawH);
      m_pVert->setGeometry(width()-16-sideW, 20, 16, height()-36);
      m_pVert->show();
      m_pHorz->setRange(0, docW - drawW);
      m_pHorz->setGeometry(64  + (width()-sideW-64)/2, height()-16, (width()-sideW-64)/2, 16);
      m_pHorz->show();
      m_pCanvas->setGeometry(20, 20, drawW-16, drawH);
      m_pTabBar->setGeometry(64, height() - 16 , (width()-sideW-64)/2, 16);
    }

  // ruler ranges
  m_pVRuler->setRange(0, docH);
  m_pHRuler->setRange(0, docW);

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

void KisView::updateReadWrite( bool /*readwrite*/ )
{
#ifdef __GNUC__
#warning TODO
#endif
}

void KisView::scrollH(int)
{
  m_pHRuler->setOffset(m_pHorz->value());
  m_pCanvas->repaint();
}

void KisView::scrollV(int)
{
  m_pVRuler->setOffset(m_pVert->value());
  m_pCanvas->repaint();
}

void KisView::slotDocUpdated()
{
  //kdDebug() << "KisView::slotDocUpdated" << endl;
  m_pCanvas->repaint();
}

void KisView::slotDocUpdated(const QRect& rect)
{
  KisImage* img = m_pDoc->current();
  if (!img)
	return;

  QRect r = rect;

  r = r.intersect(img->imageExtents());
  r.setBottom(r.bottom()+1);
  r.setRight(r.right()+1);

  int xt = xPaintOffset() + r.x() - m_pHorz->value();
  int yt = yPaintOffset() + r.y() - m_pVert->value();

  QPainter p;

  p.begin( m_pCanvas );
  p.translate(xt, yt);

  // let the document draw the image
  koDocument()->paintEverything( p, r, FALSE, this );
  p.end();
}

void KisView::canvasGotMousePressEvent( QMouseEvent *e )
{
  QMouseEvent ev( QEvent::MouseButtonPress
		  , QPoint(e->pos().x() - xPaintOffset() + m_pHorz->value(),
			   e->pos().y() - yPaintOffset() + m_pVert->value())
		  , e->globalPos(), e->button(), e->state() );

  emit canvasMousePressEvent( &ev );
}

void KisView::canvasGotMouseMoveEvent ( QMouseEvent *e )
{
  int x = e->pos().x() - xPaintOffset() + m_pHorz->value();
  int y = e->pos().y() - yPaintOffset() + m_pVert->value();

  QMouseEvent ev( QEvent::MouseMove
		  , QPoint(x, y)
		  , e->globalPos(), e->button(), e->state() );

  emit canvasMouseMoveEvent( &ev );
}

void KisView::canvasGotMouseReleaseEvent ( QMouseEvent *e )
{
  QMouseEvent ev( QEvent::MouseButtonRelease
		  , QPoint(e->pos().x() - xPaintOffset() + m_pHorz->value(),
			   e->pos().y() - yPaintOffset() + m_pVert->value())
		  , e->globalPos(), e->button(), e->state() );

  emit canvasMouseReleaseEvent( &ev );
}

void KisView::canvasGotPaintEvent( QPaintEvent*e )
{
  KisImage* img = m_pDoc->current();
  if (!img)
	{
	  QPainter p (m_pCanvas);
	  p.eraseRect(e->rect());
	  return;
	}

  QPainter p;
  QRect ur = e->rect();
  p.begin( m_pCanvas );

  // FIXME: Michael, you scale the whole image, that makes it dog slow, scale just the are you need.
  p.scale( zoomFactor(), zoomFactor() );

  // draw background
  p.eraseRect( 0, 0, xPaintOffset(), height() );
  p.eraseRect( xPaintOffset(), 0, width(), yPaintOffset() );
  p.eraseRect( xPaintOffset(), yPaintOffset() + docHeight(), width(), height() );
  p.eraseRect( xPaintOffset() + docWidth(), yPaintOffset(), width(), height() );

  // draw the image
  ur.moveBy( - xPaintOffset() + m_pHorz->value() , - yPaintOffset() + m_pVert->value());
  ur = ur.intersect(img->imageExtents());

  ur.setBottom(ur.bottom()+1);
  ur.setRight(ur.right()+1);

  if (ur.top() > img->height()
      || ur.left() > img->width())
    {
       p.end();
       return;
    }

  int xt = xPaintOffset() + ur.x() - m_pHorz->value();
  int yt = yPaintOffset() + ur.y() - m_pVert->value();

  p.translate(xt, yt);
  koDocument()->paintEverything( p, ur, FALSE, this );

  p.end();
}

void KisView::activateTool(KisTool* t)
{
  if (!t)
    return;

  if (m_pTool)
    QObject::disconnect(m_pTool);

  m_pTool = t;

  QObject::connect( this, SIGNAL( canvasMousePressEvent( QMouseEvent* ) ),
                    m_pTool, SLOT( mousePress ( QMouseEvent* ) ) );

  QObject::connect( this, SIGNAL( canvasMouseMoveEvent( QMouseEvent* ) ),
                    m_pTool, SLOT( mouseMove ( QMouseEvent* ) ) );

  QObject::connect( this, SIGNAL( canvasMouseReleaseEvent (QMouseEvent* ) ),
		    m_pTool, SLOT( mouseRelease ( QMouseEvent* ) ) );

  if (m_pCanvas)
    m_pCanvas->setCursor(m_pTool->cursor());
}


/*
 * tool action slots
 */

void KisView::tool_select_rect()
{
  activateTool(m_pSelectTool);
}

void KisView::tool_move()
{
  activateTool(m_pMoveTool);
}

void KisView::tool_zoom()
{
  activateTool(m_pZoomTool);
}

void KisView::tool_brush()
{
  activateTool(m_pBrushTool);
}

void KisView::tool_airbrush()
{
  activateTool(m_pAirBrushTool);
}

void KisView::tool_eraser()
{
  activateTool(m_pEraserTool);
}

void KisView::tool_pen()
{
  activateTool(m_pPenTool);
}

void KisView::tool_colorpicker()
{
  activateTool(m_pColorPicker);
}

void KisView::tool_gradient()
{
  activateTool( m_pGradientTool );
}

/*
 * edit action slots
 */

void KisView::undo()
{
    kdDebug() << "UNDO called" << endl;

    //m_pDoc->commandHistory()->undo(); //jwc
}

void KisView::redo()
{
    kdDebug() << "REDO called" << endl;

    //m_pDoc->commandHistory()->redo(); //jwc
}

void KisView::copy()
{
    kdDebug() << "COPY called" << endl;
}

void KisView::cut()
{
    kdDebug() << "CUT called" << endl;
}

void KisView::paste()
{
    kdDebug() << "PASTE called" << endl;
}

/*
 * dialog action slots
 */

void KisView::zoom( int _x, int _y, float zf )
{
  if (zf == 0) zf = 1;

  setZoomFactor( zf );

  int x = static_cast<int> (_x * zf - docWidth() / 2);
  int y = static_cast<int> (_y * zf - docHeight() / 2);

  if (x < 0) x = 0;
  if (y < 0) y = 0;

  scrollTo( QPoint( x, y ) );

  m_pCanvas->update();
}

void KisView::zoom_in( int x, int y )
{
  float zf = zoomFactor();

  zf *= 2;

  zoom( x, y, zf);
}

void KisView::zoom_out( int x, int y )
{
  float zf = zoomFactor();

  zf /= 2;

  zoom( x, y, zf);
}

void KisView::zoom_in()
{
    zoom_in( 0, 0 );
}

void KisView::zoom_out()
{
    zoom_out( 0, 0 );
}

void KisView::dialog_gradient()
{
  if (m_dialog_gradient->isChecked())
  {
    m_pGradientDialog->show();
    m_pGradientDialog->setFocus();
  }
  else
    m_pGradientDialog->hide();
}


void KisView::dialog_gradienteditor()
{
  if (m_dialog_gradienteditor->isChecked())
  {
    m_pGradientEditorDialog->show();
    m_pGradientEditorDialog->setFocus();
  }
  else
    m_pGradientEditorDialog->hide();
}

void KisView::updateToolbarButtons()
{
  kdDebug() << "KisView::updateToolbarButtons" << endl;

  m_dialog_gradient->setChecked( m_pGradientDialog->isVisible() );
  m_dialog_gradienteditor->setChecked( m_pGradientEditorDialog->isVisible() );
}

/*
 * layer action slots
 */

void KisView::insert_layer()
{
}

void KisView::insert_layer_image()
{
  KURL url = KFileDialog::getOpenURL( getenv("HOME"),KisUtil::readFilters(),0,i18n("Image file for layer") );

  if( !url.isEmpty() )
  {
    // TODO : insert layer
  }
}

void KisView::layer_rotate180()
{

}

void KisView::layer_rotateleft90()
{

}

void KisView::layer_rotateright90()
{

}

void KisView::layer_mirrorX()
{

}

void KisView::layer_mirrorY()
{

}

/*
 * image action slots
 */

void KisView::merge_all_layers()
{
  if (m_pDoc->current())
	m_pDoc->current()->mergeAllLayers();
}

void KisView::merge_visible_layers()
{
  if (m_pDoc->current())
	m_pDoc->current()->mergeVisibleLayers();
}

void KisView::merge_linked_layers()
{
  if (m_pDoc->current())
	m_pDoc->current()->mergeLinkedLayers();
}

/*
 * misc action slots
 */

void KisView::showMenubar()
{
}

void KisView::showToolbar()
{
}

void KisView::showStatusbar()
{
}

void KisView::showSidebar()
{
}

void KisView::saveOptions()
{
}

void KisView::preferences()
{
  kdDebug() << "PREFERENCES called" << endl;

  PreferencesDialog::editPreferences();
}

int KisView::docWidth()
{
  if (m_pDoc->current())
	return m_pDoc->current()->width();
  else
	return 0;
}

int KisView::docHeight()
{
  if (m_pDoc->current())
	return m_pDoc->current()->height();
  else
	return 0;
}

int KisView::xPaintOffset()
{
  // FIXME : make this configurable
  return 0;

  int v = static_cast<int>((m_pCanvas->width() - docWidth())/2);
  if (v < 0)
    v = 0;
  return v;
}

int KisView::yPaintOffset()
{
  // FIXME : make this configurable
  return 0;

  int v = static_cast<int>((m_pCanvas->height() - docHeight())/2);
  if (v < 0)
    v = 0;
  return v;
}

void KisView::scrollTo( QPoint )
{
}

float KisView::zoomFactor()
{
  return m_zoomFactor;
}

void KisView::setZoomFactor( float zf )
{
    m_zoomFactor = zf;
}

void KisView::slotSetBrush(const KisBrush* b)
{
  m_pBrush = b;
  if (m_pBrushTool)
    m_pBrushTool->setBrush(b);
  if (m_pPenTool)
    m_pPenTool->setBrush(b);
  if (m_pAirBrushTool)
    m_pAirBrushTool->setBrush(b);
  if (m_pEraserTool)
    m_pEraserTool->setBrush(b);

  m_pSideBar->slotSetBrush(*b);
}

void KisView::slotSetFGColor(const KisColor& c)
{
  m_fg = c;
}

void KisView::slotSetBGColor(const KisColor& c)
{
  m_bg = c;
}

void KisView::slotUndoRedoChanged( QString undo, QString redo )
{
  //####### FIXME

  m_undo->setEnabled( !undo.isEmpty() );
  m_redo->setEnabled( !redo.isEmpty() );
}

void KisView::slotUndoRedoChanged( QStringList undo, QStringList redo )
{
  //####### FIXME

#if 0
  if( undo.count() )
  {
    // enable undo action
    m_undo->setEnabled( true );
  }
  else
  {
    // disable undo action
    m_undo->setEnabled( false );
  }

  if( redo.count() )
  {
    // enable redo action
    m_redo->setEnabled( true );
  }
  else
  {
    // disable redo action
    m_redo->setEnabled( false );
  }
#endif

}

#include "kis_view.moc"

