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
#include <kaction.h>
#include <kruler.h>
#include <klocale.h>

#include "kis_view.h"
#include "kis_doc.h"
#include "kis_canvas.h"
#include "kis_tabbar.h"
#include "kis_brush.h"
#include "kis_tool.h"
#include "kis_util.h"

#include "kis_dlg_color.h"
#include "kis_dlg_layer.h"
#include "kis_dlg_gradient.h"
#include "kis_dlg_gradienteditor.h"
#include "kis_dlg_brush.h"
#include "kis_dlg_new.h"
#include "brusheswidget.h"

#include "kis_tool_move.h"
#include "kis_tool_zoom.h"
#include "kis_tool_brush.h"
#include "kis_tool_airbrush.h"
#include "kis_tool_pen.h"
#include "kis_tool_gradient.h"
#include "kis_tool_colorpicker.h"
#include "kis_tool_eraser.h"

kisView::kisView( kisDoc* doc, QWidget* parent, const char* name )
  : ContainerView( doc, parent, name )
  , m_pDoc(doc)
{
  QObject::connect( m_pDoc, SIGNAL( docUpdated( ) ),
                    this, SLOT( slotDocUpdated ( ) ) );
  QObject::connect( m_pDoc, SIGNAL( docUpdated( const QRect& ) ),
                    this, SLOT( slotDocUpdated ( const QRect& ) ) );

  m_fg = KColor::black();
  m_bg = KColor::white();

  setupCanvas();
  setupScrollBars();
  setupRulers();
  setupTabBar();
  setupActions();
  setupDialogs();
  setupTools();
}

void kisView::setupCanvas()
{
  m_pCanvas = new kisCanvas(this, "kis_canvas");

  QObject::connect( m_pCanvas, SIGNAL( mousePressed( QMouseEvent* ) ),
                    this, SLOT( canvasGotMousePressEvent ( QMouseEvent* ) ) );

  QObject::connect( m_pCanvas, SIGNAL( mouseMoved( QMouseEvent* ) ),
                    this, SLOT( canvasGotMouseMoveEvent ( QMouseEvent* ) ) );
  
  QObject::connect( m_pCanvas, SIGNAL( mouseReleased (QMouseEvent* ) ),
		    this, SLOT( canvasGotMouseReleaseEvent ( QMouseEvent* ) ) );

  QObject::connect( m_pCanvas, SIGNAL( gotPaintEvent (QPaintEvent* ) ),
		    this, SLOT( canvasGotPaintEvent ( QPaintEvent* ) ) );
  
}

void kisView::setupScrollBars()
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

void kisView::setupRulers()
{
  m_pHRuler = new KRuler(KRuler::horizontal, this);
  m_pVRuler = new KRuler(KRuler::vertical, this);

  m_pHRuler->setGeometry(20, 0, width()-20, 20);
  m_pVRuler->setGeometry(0, 20, 20, height()-20);

  m_pVRuler->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
  m_pHRuler->setFrameStyle(QFrame::WinPanel | QFrame::Raised);

  m_pVRuler->setRulerStyle(KRuler::pixel);
  m_pHRuler->setRulerStyle(KRuler::pixel);
}

void kisView::setupTabBar()
{
  // tabbar
  m_pTabBar = new kisTabBar(this, m_pDoc);

  QStringList images = m_pDoc->images();
  if (!images.isEmpty())
    {
      QStringList::Iterator it;

      for ( it = images.begin(); it != images.end(); ++it )
	  m_pTabBar->addTab(*it);

      m_pTabBar->setActiveTab(*images.begin());
    }

  QObject::connect( m_pTabBar, SIGNAL( tabChanged( const QString& ) ),
		    this, SLOT( slotTabSelected( const QString& ) ) );

  QObject::connect( m_pDoc, SIGNAL( imageAdded( const QString& ) ),
		    this, SLOT( slotImageAdded( const QString& ) ) );

 
  // tabbar control buttons
  m_pTabFirst = new QPushButton( this );
  m_pTabFirst->setPixmap( QPixmap( KISBarIcon( "tab_first" ) ) );
  QObject::connect( m_pTabFirst, SIGNAL( clicked() ), this, SLOT( slotScrollToFirstTab() ) );

  m_pTabLeft = new QPushButton( this );
  m_pTabLeft->setPixmap( QPixmap( KISBarIcon( "tab_left" ) ) );
  QObject::connect( m_pTabLeft, SIGNAL( clicked() ), this, SLOT( slotScrollToLeftTab() ) );

  m_pTabRight = new QPushButton( this );
  m_pTabRight->setPixmap( QPixmap( KISBarIcon( "tab_right" ) ) );
  QObject::connect( m_pTabRight, SIGNAL( clicked() ), this, SLOT( slotScrollToRightTab() ) );

  m_pTabLast = new QPushButton( this );
  m_pTabLast->setPixmap( QPixmap( KISBarIcon( "tab_last" ) ) );
  QObject::connect( m_pTabLast, SIGNAL( clicked() ), this, SLOT( slotScrollToLastTab() ) );
}

void kisView::setupTools()
{
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
  //connect(m_pColorPicker, SIGNAL(fgColorPicked(const KColor&)), this, SLOT(slotSetFGColor(const KColor&)));
  //connect(m_pColorPicker, SIGNAL(bgColorPicked(const KColor&)), this, SLOT(slotSetBGColor(const KColor&)));
  
  // zoom tool
  m_pZoomTool = new ZoomTool(this);
  
  m_tool_brush->setChecked( true );
  activateTool(m_pBrushTool);
}

void kisView::setupDialogs()
{
  // color dialog
  m_pColorDialog = new ColorDialog( this );
  m_pColorDialog->resize(254, 150);
  m_pColorDialog->move(521, 380);
  m_pColorDialog->show();
  m_dialog_color->setChecked(true);
  //addDialog(m_pColorDialog);
  connect( m_pColorDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );

  connect(m_pColorDialog, SIGNAL(fgColorChanged(const KColor&)), this, SLOT(slotSetFGColor(const KColor&)));
  connect(m_pColorDialog, SIGNAL(bgColorChanged(const KColor&)), this, SLOT(slotSetBGColor(const KColor&)));

  // layer dialog
  m_pLayerDialog = new LayerDialog( m_pDoc, this );
  m_pLayerDialog->resize( 205, 267 );
  m_pLayerDialog->move( 560, 22 );
  m_pLayerDialog->show();
  m_pLayerDialog->setFocus();
  m_dialog_layer->setChecked(true);
  //addDialog(m_pLayerDialog);
  connect( m_pLayerDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );

  // brush dialog
  m_pBrushDialog = new BrushDialog(this);
  m_pBrushDialog->resize(185, 158);
  m_pBrushDialog->move(523, 220);
  m_pBrushDialog->show();
  //addDialog(m_pBrushDialog);
  connect( m_pBrushDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );

  // brush
  m_pBrushChooser = m_pBrushDialog->brushChooser();
  m_pBrush = m_pBrushChooser->currentBrush();
  QObject::connect(m_pBrushChooser, SIGNAL(selected(const Brush *)), this, SLOT(slotSetBrush(const Brush*)));

  // gradient dialog
  m_pGradientDialog = new GradientDialog( m_pDoc, this );
  m_pGradientDialog->resize( 206, 185 );
  m_pGradientDialog->move( 200, 290 );
  m_pGradientDialog->hide();
  //addDialog(m_pGradientDialog);
  connect( m_pGradientDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );

  // gradient editor dialog
  m_pGradientEditorDialog = new GradientEditorDialog( m_pDoc, this );
  m_pGradientEditorDialog->resize( 400, 200 );
  m_pGradientEditorDialog->move( 100, 190 );
  m_pGradientEditorDialog->hide();
  //addDialog(m_pGradientEditorDialog);
  connect( m_pGradientEditorDialog, SIGNAL( sigClosed() ), SLOT( updateToolbarButtons() ) );
}

void kisView::setupActions()
{
  // edit actions
  
  m_undo = new KAction( i18n("&Undo"), KISBarIcon("undo"), 0, this, SLOT( undo() ),
			actionCollection(), "undo");
  m_redo = new KAction( i18n("&Redo"), KISBarIcon("redo"), 0, this, SLOT( redo() ),
			actionCollection(), "redo");
  m_cut = new KAction( i18n("C&ut"), KISBarIcon("editcut"), 0, this, SLOT( cut() ),
		       actionCollection(), "cut");
  m_copy = new KAction( i18n("&Copy"), KISBarIcon("editcopy"), 0, this, SLOT( copy() ),
			actionCollection(), "copy");
  m_paste = new KAction( i18n("&Paste"), KISBarIcon("editpaste"), 0, this, SLOT( paste() ),
			 actionCollection(), "paste");

  // dialog actions

  m_dialog_layer = new KToggleAction( i18n("&Layer Dialog"), KISBarIcon("layer_dialog"), 0, this,
				SLOT( dialog_layer() ),actionCollection(), "dialog_layer");
  m_dialog_color = new KToggleAction( i18n("&Color Dialog"), KISBarIcon("color_dialog"), 0, this,
				SLOT( dialog_color() ),actionCollection(), "dialog_color");
  m_dialog_brush = new KToggleAction( i18n("&Brush Dialog"), KISBarIcon("brush_dialog"), 0, this,
				SLOT( dialog_brush() ),actionCollection(), "dialog_brush");
  m_dialog_gradient = new KToggleAction( i18n("&Gradient Dialog"), KISBarIcon("gradient_dialog"), 0, this,
				   SLOT( dialog_gradient() ),actionCollection(), "dialog_gradient");
  m_dialog_gradienteditor = new KToggleAction( i18n("Gradient &Editor"), KISBarIcon("gradienteditor_dialog"),
					 0, this, SLOT( dialog_gradienteditor() ),
					 actionCollection(), "dialog_gradienteditor");

  // tool actions

  m_tool_move = new KToggleAction( i18n("&Move tool"), KISBarIcon("move"), 0, this,
			     SLOT( tool_move() ),actionCollection(), "tool_move");
  m_tool_move->setExclusiveGroup( "tools" );
  m_tool_zoom = new KToggleAction( i18n("&Zoom tool"), KISBarIcon("viewmag"), 0, this,
			     SLOT( tool_zoom() ),actionCollection(), "tool_zoom");
  m_tool_zoom->setExclusiveGroup( "tools" );
  m_tool_pen = new KToggleAction( i18n("&Pen tool"), KISBarIcon("pen"), 0, this,
			      SLOT( tool_pen() ),actionCollection(), "tool_pen");
  m_tool_pen->setExclusiveGroup( "tools" );
  m_tool_brush = new KToggleAction( i18n("&Brush tool"), KISBarIcon("paintbrush"), 0, this,
			      SLOT( tool_brush() ),actionCollection(), "tool_brush");
  m_tool_brush->setExclusiveGroup( "tools" );
  m_tool_airbrush = new KToggleAction( i18n("&Airbrush tool"), KISBarIcon("airbrush"), 0, this,
				       SLOT( tool_airbrush() ),actionCollection(), "tool_airbrush");
  m_tool_airbrush->setExclusiveGroup( "tools" );
  m_tool_eraser = new KToggleAction( i18n("&Eraser tool"), KISBarIcon("eraser"), 0, this,
			      SLOT( tool_eraser() ),actionCollection(), "tool_eraser");
  m_tool_eraser->setExclusiveGroup( "tools" );
  m_tool_colorpicker = new KToggleAction( i18n("&Color picker"), KISBarIcon("colorpicker"), 0, this,
			      SLOT( tool_colorpicker() ),actionCollection(), "tool_colorpicker");
  m_tool_colorpicker->setExclusiveGroup( "tools" );
  m_tool_gradient = new KToggleAction( i18n("&Gradient tool"), KISBarIcon("gradient"), 0, this,
  				 SLOT( tool_gradient() ),actionCollection(), "tool_gradient");
  m_tool_gradient->setExclusiveGroup( "tools" );

  // layer actions

  m_layer_rotate180 = new KAction( i18n("Rotate 1&80"), 0, this,
				   SLOT( layer_rotate180() ),actionCollection(), "layer_rotate180");

  m_layer_rotateleft90 = new KAction( i18n("&Rotate &270"), 0, this,
				      SLOT( layer_rotateleft90() ),actionCollection(), "layer_rotateleft90");

  m_layer_rotateright90 = new KAction( i18n("&Rotate &90"), 0, this,
				       SLOT( layer_rotateright90() ),actionCollection(), "layer_rotateright90");

  m_layer_mirrorX = new KAction( i18n("Mirror &X"), 0, this,
				 SLOT( layer_mirrorX() ),actionCollection(), "layer_mirrorX");

  m_layer_mirrorY = new KAction( i18n("Mirror &Y"), 0, this,
				 SLOT( layer_mirrorY() ),actionCollection(), "layer_mirrorY");

  // image actions
  m_merge_all_layers = new KAction( i18n("Merge &all layers"), 0, this,
				    SLOT( merge_all_layers() ),actionCollection(), "merge_all_layers");

  m_merge_visible_layers = new KAction( i18n("Merge &visible layers"), 0, this,
				    SLOT( merge_visible_layers() ),actionCollection(), "merge_visible_layers");

  m_merge_linked_layers = new KAction( i18n("Merge &linked layers"), 0, this,
				    SLOT( merge_linked_layers() ),actionCollection(), "merge_linked_layers");

  // misc actions
  m_preferences = new KAction( i18n("&Preferences"), 0, this,
			       SLOT( preferences() ),actionCollection(), "preferences");
}

void kisView::slotTabSelected(const QString& name)
{
  m_pDoc->setCurrentImage(name);
  resizeEvent(0L);
}

void kisView::slotImageAdded(const QString& name)
{
  m_pTabBar->addTab(name);
  m_pTabBar->setActiveTab(name);
}

void kisView::resizeEvent(QResizeEvent*)
{
  // ruler geometry
  m_pHRuler->setGeometry(20, 0, width()-20, 20);
  m_pVRuler->setGeometry(0, 20, 20, height()-36);

  // tabbar control buttons
  m_pTabFirst->setGeometry(0, height()-16, 16, 16);
  m_pTabLeft->setGeometry(16, height()-16, 16, 16);
  m_pTabRight->setGeometry(32, height()-16, 16, 16);
  m_pTabLast->setGeometry(48, height()-16, 16, 16);

  // kisView heigth/width - ruler heigth/width
  int canH = m_pCanvas->height();
  int canW = m_pCanvas->width();
  int viewH = height() - 20 - 16;
  int viewW = width() - 20;
  int docH = docHeight();
  int docW = docWidth();

  // scrollbar geometry
  if (docH <= canH && docW <= canW) // we need no scrollbars
    {
      m_pVert->hide();
      m_pHorz->hide();
      m_pVert->setValue(0);
      m_pHorz->setValue(0);
      m_pCanvas->setGeometry(20, 20, viewW, viewH);
      m_pTabBar->setGeometry(64, height() - 16 , width() - 64, 16);
    }
  else if (docH <= canH) // we need a horizontal scrollbar
    {
      m_pVert->hide();
      m_pVert->setValue(0);
      m_pHorz->setRange(0, docW - canW);
      m_pHorz->setGeometry(64  + (width()-64)/2, height()-16, (width()-64)/2, 16);
      m_pHorz->show();
      m_pCanvas->setGeometry(20, 20, viewW, viewH);
      m_pTabBar->setGeometry(64, height() - 16 , (width()-64)/2, 16);
    }
  else if(docW <= canW) // we need a vertical scrollbar
    {
      m_pHorz->hide();
      m_pHorz->setValue(0);
      m_pVert->setRange(0, docH - canH);
      m_pVert->setGeometry(width()-16, 20, 16, height()-36);
      m_pVert->show();
      m_pCanvas->setGeometry(20, 20, viewW-16, viewH);
      m_pTabBar->setGeometry(64, height() - 16 , width() - 64, 16);
    }
  else // we need both scrollbars
    {
      m_pVert->setRange(0, docH - canH);
      m_pVert->setGeometry(width()-16, 20, 16, height()-36);
      m_pVert->show();
      m_pHorz->setRange(0, docW - canW);
      m_pHorz->setGeometry(64  + (width()-80)/2, height()-16, (width()-80)/2, 16);
      m_pHorz->show();
      m_pCanvas->setGeometry(20, 20, viewW-16, viewH);
      m_pTabBar->setGeometry(64, height() - 16 , (width()-80)/2, 16);
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

void kisView::scrollH(int)
{
  m_pHRuler->setOffset(m_pHorz->value());
  m_pCanvas->repaint();
}

void kisView::scrollV(int)
{
  m_pVRuler->setOffset(m_pVert->value());
  m_pCanvas->repaint();
}

void kisView::slotDocUpdated()
{
  //qDebug("kisView::slotDocUpdated");
  m_pCanvas->repaint();
}

void kisView::slotDocUpdated(const QRect& rect)
{
  QRect r = rect;

  r = r.intersect(m_pDoc->imageExtents());
  r.setBottom(r.bottom()+1);
  r.setRight(r.right()+1);

  int xt = xPaintOffset() + r.x() - m_pHorz->value();
  int yt = yPaintOffset() + r.y() - m_pVert->value();

  //qDebug("kisView::slotDocUpdated l: %d; t: %d; r: %d; b: %d"
  //	 ,r.left(), r.top(), r.right(), r.bottom());

  QPainter p;

  p.begin( m_pCanvas );
  p.translate(xt, yt);
  
  // let the document draw the image
  part()->paintEverything( p, r, FALSE, this );
  p.end();
}

void kisView::canvasGotMousePressEvent( QMouseEvent *e )
{
  QMouseEvent ev( QEvent::MouseButtonPress
		  , QPoint(e->pos().x() - xPaintOffset() + m_pHorz->value(),
			   e->pos().y() - yPaintOffset() + m_pVert->value())
		  , e->globalPos(), e->button(), e->state() );

  emit canvasMousePressEvent( &ev );
}

void kisView::canvasGotMouseMoveEvent ( QMouseEvent *e )
{
  QMouseEvent ev( QEvent::MouseMove
		  , QPoint(e->pos().x() - xPaintOffset() + m_pHorz->value(),
			   e->pos().y() - yPaintOffset() + m_pVert->value())
		  , e->globalPos(), e->button(), e->state() );

  emit canvasMouseMoveEvent( &ev );
}

void kisView::canvasGotMouseReleaseEvent ( QMouseEvent *e )
{
  QMouseEvent ev( QEvent::MouseButtonRelease
		  , QPoint(e->pos().x() - xPaintOffset() + m_pHorz->value(),
			   e->pos().y() - yPaintOffset() + m_pVert->value())
		  , e->globalPos(), e->button(), e->state() );

  emit canvasMouseReleaseEvent( &ev );
}

void kisView::canvasGotPaintEvent( QPaintEvent*e )
{
  QRect ur = e->rect();
  QPainter p;

  //qDebug("kisView::canvasGotPaintEvent l: %d; t: %d; r: %d; b: %d"
  //	 , e->rect().left(), e->rect().top(), e->rect().right(), e->rect().bottom());

  p.begin( m_pCanvas );

  // draw background
  p.eraseRect(0, 0, xPaintOffset(), height());
  p.eraseRect(xPaintOffset(), 0, width(), yPaintOffset());
  p.eraseRect(xPaintOffset(), yPaintOffset() + docHeight(), width(), height());
  p.eraseRect(xPaintOffset() + docWidth(), yPaintOffset(), width(), height());

  // draw the image
  ur.moveBy( -xPaintOffset() + m_pHorz->value() , -yPaintOffset() + m_pVert->value());
  ur = ur.intersect(m_pDoc->imageExtents());

  ur.setBottom(ur.bottom()+1);
  ur.setRight(ur.right()+1);

  if (ur.top() > m_pDoc->height()
      || ur.left() > m_pDoc->width())
    {
       p.end();
       return;
    }

  int xt = xPaintOffset() + ur.x() - m_pHorz->value();
  int yt = yPaintOffset() + ur.y() - m_pVert->value();

  p.translate(xt, yt);
  part()->paintEverything( p, ur, FALSE, this );
    
  p.end();
}

void kisView::activateTool(Tool* t)
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

void kisView::tool_move()
{
  activateTool(m_pMoveTool);
}

void kisView::tool_zoom()
{
  activateTool(m_pZoomTool);
}

void kisView::tool_brush()
{
  activateTool(m_pBrushTool);
}

void kisView::tool_airbrush()
{
  activateTool(m_pAirBrushTool);
}

void kisView::tool_eraser()
{
  activateTool(m_pEraserTool);
}

void kisView::tool_pen()
{
  activateTool(m_pPenTool);
}

void kisView::tool_colorpicker()
{
  activateTool(m_pColorPicker);
}

void kisView::tool_gradient()
{
}

/*
 * edit action slots
 */

void kisView::undo()
{
    qDebug("UNDO called");
}

void kisView::redo()
{
    qDebug("REDO called");
}

void kisView::copy()
{
    qDebug("COPY called");
}

void kisView::cut()
{
    qDebug("CUT called");
}

void kisView::paste()
{
    qDebug("PASTE called");
}

/*
 * dialog action slots
 */

void kisView::dialog_layer()
{
  if (m_dialog_layer->isChecked())
    m_pLayerDialog->show();
  else
    m_pLayerDialog->hide();
}

void kisView::dialog_color()
{
  if (m_dialog_color->isChecked())
    m_pColorDialog->show();
  else
    m_pColorDialog->hide();
}

void kisView::dialog_brush()
{
  if (m_dialog_brush->isChecked())
    m_pBrushDialog->show();
  else
    m_pBrushDialog->hide();
}

void kisView::dialog_gradient()
{
  if (m_dialog_gradient->isChecked())
    m_pGradientDialog->show();
  else
    m_pGradientDialog->hide();
}


void kisView::dialog_gradienteditor()
{
  if (m_dialog_gradienteditor->isChecked())
    m_pGradientEditorDialog->show();
  else
    m_pGradientEditorDialog->hide();
}

void kisView::updateToolbarButtons()
{
  kdebug( KDEBUG_INFO, 0, "kisView::updateToolvarButtons" );
  
  m_dialog_layer->setChecked( m_pLayerDialog->isVisible() );
  m_dialog_color->setChecked( m_pColorDialog->isVisible() );
  m_dialog_brush->setChecked( m_pBrushDialog->isVisible() );
  m_dialog_gradient->setChecked( m_pGradientDialog->isVisible() );
  m_dialog_gradienteditor->setChecked( m_pGradientEditorDialog->isVisible() );
}

/*
 * layer action slots
 */

void kisView::layer_rotate180()
{
  m_pDoc->rotateLayer180(0);
}

void kisView::layer_rotateleft90()
{
  m_pDoc->rotateLayerLeft90(0);
}

void kisView::layer_rotateright90()
{
  m_pDoc->rotateLayerRight90(0);
}

void kisView::layer_mirrorX()
{
  m_pDoc->mirrorLayerX(0);
}

void kisView::layer_mirrorY()
{
  m_pDoc->mirrorLayerY(0);
}

/*
 * image action slots
 */

void kisView::merge_all_layers()
{
  m_pDoc->mergeAllLayers();
}

void kisView::merge_visible_layers()
{
  m_pDoc->mergeVisibleLayers();
}

void kisView::merge_linked_layers()
{
  m_pDoc->mergeLinkedLayers();
}

/*
 * misc action slots
 */

void kisView::preferences()
{
    qDebug("PREFERENCES called");
}

int kisView::docWidth()
{
  return m_pDoc->width();
}

int kisView::docHeight()
{
  return m_pDoc->height();
}

int kisView::xPaintOffset()
{
  // FIXME : make this configurable
  return 0;

  int v = static_cast<int>((m_pCanvas->width() - docWidth())/2);
  if (v < 0)
    v = 0;
  return v;
}

int kisView::yPaintOffset()
{
  // FIXME : make this configurable
  return 0;

  int v = static_cast<int>((m_pCanvas->height() - docHeight())/2);
  if (v < 0)
    v = 0;
  return v;
}

float kisView::zoomFactor()
{
  return 2.0; // FIXME
}

void kisView::slotSetBrush(const Brush* b)
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
}

void kisView::slotSetFGColor(const KColor& c)
{
  m_fg = c;
}

void kisView::slotSetBGColor(const KColor& c)
{
  m_bg = c;
}

void kisView::slotScrollToFirstTab()
{
  m_pTabBar->scrollFirst();
}

void kisView::slotScrollToLeftTab()
{
  m_pTabBar->scrollLeft();
}

void kisView::slotScrollToRightTab()
{
  m_pTabBar->scrollRight();
}

void kisView::slotScrollToLastTab()
{
  m_pTabBar->scrollLast();
}

#include "kis_view.moc"
