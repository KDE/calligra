/*
 *  kimageshop_view.cc - part of KImageShop
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


#include "kimageshop_view.h"
#include "kimageshop_global.h"
#include "kimageshop_doc.h"

#include "colordialog.h"
#include "layerdlg.h"
#include "gradientdlg.h"
#include "gradienteditordlg.h"
#include "brushdialog.h"
#include "brusheswidget.h"

#include "brush.h"

#include "tool.h"
#include "movetool.h"
#include "brushtool.h"
#include "zoomtool.h"
#include "gradienttool.h"

#include <kruler.h>

#include <qpainter.h>
#include <kaction.h>
#include <qscrollbar.h>

KImageShopView::KImageShopView( KImageShopDoc* doc, QWidget* parent, const char* name )
  : ContainerView( doc, parent, name )
  , m_pDoc(doc)
{
  QObject::connect( m_pDoc, SIGNAL( docUpdated( ) ),
                    this, SLOT( slotDocUpdated ( ) ) );
  QObject::connect( m_pDoc, SIGNAL( docUpdated( const QRect& ) ),
                    this, SLOT( slotDocUpdated ( const QRect& ) ) );

  setBackgroundMode( QWidget::NoBackground );
  setMouseTracking(true);

  // edit actions

  m_undo = new KAction( tr("&Undo"), KImageShopBarIcon("undo"), 0, this, SLOT( undo() ),
			actionCollection(), "undo");
  m_redo = new KAction( tr("&Redo"), KImageShopBarIcon("redo"), 0, this, SLOT( redo() ),
			actionCollection(), "redo");
  m_cut = new KAction( tr("C&ut"), KImageShopBarIcon("editcut"), 0, this, SLOT( cut() ),
		       actionCollection(), "cut");
  m_copy = new KAction( tr("&Copy"), KImageShopBarIcon("editcopy"), 0, this, SLOT( copy() ),
			actionCollection(), "copy");
  m_paste = new KAction( tr("&Paste"), KImageShopBarIcon("editpaste"), 0, this, SLOT( paste() ),
			 actionCollection(), "paste");

  // dialog actions

  m_dialog_layer = new KToggleAction( tr("&Layer Dialog"), KImageShopBarIcon("layer_dialog"), 0, this,
				SLOT( dialog_layer() ),actionCollection(), "dialog_layer");
  m_dialog_color = new KToggleAction( tr("&Color Dialog"), KImageShopBarIcon("color_dialog"), 0, this,
				SLOT( dialog_color() ),actionCollection(), "dialog_color");
  m_dialog_brush = new KToggleAction( tr("&Brush Dialog"), KImageShopBarIcon("brush_dialog"), 0, this,
				SLOT( dialog_brush() ),actionCollection(), "dialog_brush");
  m_dialog_gradient = new KToggleAction( tr("&Gradient Dialog"), KImageShopBarIcon("gradient_dialog"), 0, this,
				   SLOT( dialog_gradient() ),actionCollection(), "dialog_gradient");
  m_dialog_gradienteditor = new KToggleAction( tr("Gradient &Editor"), KImageShopBarIcon("gradienteditor_dialog"),
					 0, this, SLOT( dialog_gradienteditor() ),
					 actionCollection(), "dialog_gradienteditor");

  // tool actions

  m_tool_move = new KToggleAction( tr("&Move tool"), KImageShopBarIcon("move"), 0, this,
			     SLOT( tool_move() ),actionCollection(), "tool_move");
  ( (KToggleAction*) m_tool_move )->setExclusiveGroup( "tools" );
  m_tool_zoom = new KToggleAction( tr("&Zoom tool"), KImageShopBarIcon("viewmag"), 0, this,
			     SLOT( tool_zoom() ),actionCollection(), "tool_zoom");
  ( (KToggleAction*) m_tool_zoom )->setExclusiveGroup( "tools" );
  m_tool_brush = new KToggleAction( tr("&Brush tool"), KImageShopBarIcon("paintbrush"), 0, this,
			      SLOT( tool_brush() ),actionCollection(), "tool_brush");
  ( (KToggleAction*) m_tool_brush )->setExclusiveGroup( "tools" );
  m_tool_gradient = new KToggleAction( tr("&Gradient tool"), KImageShopBarIcon("gradient"), 0, this,
  				 SLOT( tool_gradient() ),actionCollection(), "tool_gradient");
  ( (KToggleAction*) m_tool_gradient )->setExclusiveGroup( "tools" );

  // layer actions

  m_layer_rotate180 = new KAction( tr("Rotate 1&80"), 0, this,
				   SLOT( layer_rotate180() ),actionCollection(), "layer_rotate180");

  m_layer_rotateleft90 = new KAction( tr("&Rotate &270"), 0, this,
				      SLOT( layer_rotateleft90() ),actionCollection(), "layer_rotateleft90");

  m_layer_rotateright90 = new KAction( tr("&Rotate &90"), 0, this,
				       SLOT( layer_rotateright90() ),actionCollection(), "layer_rotateright90");

  m_layer_mirrorX = new KAction( tr("Mirror &X"), 0, this,
				 SLOT( layer_mirrorX() ),actionCollection(), "layer_mirrorX");

  m_layer_mirrorY = new KAction( tr("Mirror &Y"), 0, this,
				 SLOT( layer_mirrorY() ),actionCollection(), "layer_mirrorY");

  // image actions
  m_merge_all_layers = new KAction( tr("Merge &all layers"), 0, this,
				    SLOT( merge_all_layers() ),actionCollection(), "merge_all_layers");

  m_merge_visible_layers = new KAction( tr("Merge &visible layers"), 0, this,
				    SLOT( merge_visible_layers() ),actionCollection(), "merge_visible_layers");

  m_merge_linked_layers = new KAction( tr("Merge &linked layers"), 0, this,
				    SLOT( merge_linked_layers() ),actionCollection(), "merge_linked_layers");

  // misc actions
  m_preferences = new KAction( tr("&Preferences"), 0, this,
			       SLOT( preferences() ),actionCollection(), "preferences");

  // scrollbars
  setupScrollBars();

  // rulers
  setupRulers();

  // color dialog
  m_pColorDialog = new ColorDialog( this );
  m_pColorDialog->move(100, 20);
  m_pColorDialog->hide();
  //addDialog(m_pColorDialog);

  // brush dialog
  m_pBrushDialog = new BrushDialog(this);
  m_pBrushDialog->resize(205, 267);
  m_pBrushDialog->move(405, 20);
  m_pBrushDialog->hide();
  //addDialog(m_pBrushDialog);

  // brush
  m_pBrushChooser = m_pBrushDialog->brushChooser();
  m_pBrush = m_pBrushChooser->currentBrush();
  QObject::connect(m_pBrushChooser, SIGNAL(selected(const Brush *)), this, SLOT(slotSetBrush(const Brush*)));

  // layer dialog
  m_pLayerDialog = new LayerDialog( m_pDoc, this );
  m_pLayerDialog->resize( 205, 267 );
  m_pLayerDialog->move( 200, 20 );
  m_pLayerDialog->show();
  m_pLayerDialog->setFocus();
  m_dialog_layer->setChecked(true);
  //addDialog(m_pLayerDialog);

  // create gradient dialog
  m_pGradientDialog = new GradientDialog( m_pDoc, this );
  m_pGradientDialog->resize( 206, 185 );
  m_pGradientDialog->move( 200, 290 );
  m_pGradientDialog->hide();
  //addDialog(m_pGradientDialog);

  // create gradient editor dialog
  m_pGradientEditorDialog = new GradientEditorDialog( m_pDoc, this );
  m_pGradientEditorDialog->resize( 400, 200 );
  m_pGradientEditorDialog->move( 100, 190 );
  m_pGradientEditorDialog->hide();
  //addDialog(m_pGradientEditorDialog);

  // create move tool
  m_pMoveTool = new MoveTool(m_pDoc);
  
  // create brush tool
  m_pBrushTool = new BrushTool(m_pDoc, m_pBrush);
  
  // create zoom tool
  m_pZoomTool = new ZoomTool(this);

  activateTool(m_pMoveTool);
}

void KImageShopView::setupScrollBars()
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

void KImageShopView::setupRulers()
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

void KImageShopView::resizeEvent(QResizeEvent*)
{
  qDebug("resize");
  m_pHRuler->show();
  m_pVRuler->show();
  m_pHorz->hide();
  m_pVert->hide();

  // KImageShopView heigth/width - ruler heigth/width

  int viewH = height() - 20;
  int viewW = width() - 20;

  // scrollbar geometry
  if (docHeight() <= viewH && docWidth() <= viewW) // we need no scrollbars
    {
      m_pVert->hide();
      m_pHorz->hide();
      m_pVert->setValue(0);
      m_pHorz->setValue(0);
    }
  else if (docHeight() <= viewH) // we need a horizontal scrollbar
    {
      m_pVert->hide();
      m_pVert->setValue(0);
      m_pHorz->setRange(0, docWidth() - viewW);
      m_pHorz->setGeometry(20, height()-16, width()-16, 16);
      m_pHorz->show();
    }
  else if(docWidth() <= viewW) // we need a vertical scrollbar
    {
      m_pHorz->hide();
      m_pHorz->setValue(0);
      m_pVert->setRange(0, docHeight() - viewH);
      m_pVert->setGeometry(width()-16, 20, 16, height()-16);
      m_pVert->show();
    }
  else // we need both scrollbars
    {
      m_pVert->setRange(0, docHeight() - viewH + 16);
      m_pVert->setGeometry(width()-16, 20, 16, height()-36);
      m_pVert->show();
      m_pHorz->setRange(0, docWidth() - viewW + 16);
      m_pHorz->setGeometry(20, height()-16, width()-36, 16);
      m_pHorz->show();
    }

  // ruler geometry
  m_pHRuler->setGeometry(20, 0, width()-20, 20);
  m_pVRuler->setGeometry(0, 20, 20, height()-20);

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

void KImageShopView::scrollH(int)
{
  m_pHRuler->setOffset(m_pHorz->value());
  repaint();
}

void KImageShopView::scrollV(int)
{
  m_pVRuler->setOffset(m_pVert->value());
  repaint();
}

void KImageShopView::slotDocUpdated()
{
  repaint();
}

void KImageShopView::slotDocUpdated(const QRect& r)
{
  repaint(r.left() + 20 + xPaintOffset(), r.top() + 20 + yPaintOffset() , r.width(), r.height()); 
}

void KImageShopView::paintEvent( QPaintEvent* e )
{
  QRect ur = e->rect();
  QPainter p;

  p.begin( this );

  // draw background ##### OPTIMIZE THIS TO REDUCE FLICKER
  p.eraseRect(0,0,20,20);
  p.eraseRect(20,20,xPaintOffset(),height());
  p.eraseRect(20 + xPaintOffset(),0, width(),20 + yPaintOffset());
  p.eraseRect(20 + xPaintOffset(),0, width(),20 + yPaintOffset());
  p.eraseRect(20 + xPaintOffset(), 20 + yPaintOffset() + docHeight(), width(), height());
  p.eraseRect(20 + xPaintOffset() + docWidth(), 20 + yPaintOffset(), width(), height());

  // draw the image
  ur.moveBy(-20 - xPaintOffset() + m_pHorz->value() , -20 - yPaintOffset() + m_pVert->value());

  ur.setWidth(ur.width()+1); // why do I need this?
  ur.setHeight(ur.height()+1); // why do I need this?
  
  int xt = 20 + xPaintOffset() + ur.x() - m_pHorz->value();
  int yt = 20 + yPaintOffset() + ur.y() - m_pVert->value();

  p.translate(xt, yt);

  // let the document draw the image
  part()->paintEverything( p, ur, FALSE, this );
  
  p.end();
}

void KImageShopView::mousePressEvent ( QMouseEvent *e )
{
  QPoint pos = e->pos();
  pos += QPoint(-xPaintOffset() - 20, -yPaintOffset() - 20);
  QMouseEvent event(QEvent::MouseButtonPress, pos, e->globalPos(), e->button(), e->state());

  emit mousePressed(&event);
}

void KImageShopView::mouseReleaseEvent ( QMouseEvent *e )
{
  QPoint pos = e->pos();
  pos += QPoint(-xPaintOffset() - 20, -yPaintOffset() - 20);
  QMouseEvent event(QEvent::MouseButtonRelease, pos, e->globalPos(), e->button(), e->state());

  emit mouseReleased(&event);
}

void KImageShopView::mouseMoveEvent ( QMouseEvent *e )
{
  m_pHRuler->slotNewValue(e->x() - xPaintOffset()-20 + m_pHorz->value());
  m_pVRuler->slotNewValue(e->y() - yPaintOffset()-20 + m_pVert->value());

  QPoint pos = e->pos();
  pos += QPoint(-xPaintOffset() - 20, -yPaintOffset() - 20);
  QMouseEvent event(QEvent::MouseMove, pos, e->globalPos(), e->button(), e->state());

  emit mouseMoved(&event);
}

void KImageShopView::activateTool(Tool* t)
{
  if (!t)
    return;
  
  if (m_pTool)
    QObject::disconnect(m_pTool);

  m_pTool = t;

  QObject::connect( this, SIGNAL( mousePressed( QMouseEvent* ) ),
                    m_pTool, SLOT( mousePress ( QMouseEvent* ) ) );

  QObject::connect( this, SIGNAL( mouseMoved( QMouseEvent* ) ),
                    m_pTool, SLOT( mouseMove ( QMouseEvent* ) ) );
  
  QObject::connect( this, SIGNAL( mouseReleased (QMouseEvent* ) ),
		    m_pTool, SLOT( mouseRelease ( QMouseEvent* ) ) );
}


/*
 * tool action slots
 */

void KImageShopView::tool_move()
{
  activateTool(m_pMoveTool);
}

void KImageShopView::tool_zoom()
{
  activateTool(m_pZoomTool);
}

void KImageShopView::tool_brush()
{
  activateTool(m_pBrushTool);
}

void KImageShopView::tool_gradient()
{
}

/*
 * edit action slots
 */

void KImageShopView::undo()
{
    qDebug("UNDO called");
}

void KImageShopView::redo()
{
    qDebug("REDO called");
}

void KImageShopView::copy()
{
    qDebug("COPY called");
}

void KImageShopView::cut()
{
    qDebug("CUT called");
}

void KImageShopView::paste()
{
    qDebug("PASTE called");
}

/*
 * dialog action slots
 */

void KImageShopView::dialog_layer()
{
  if (m_dialog_layer->isChecked())
    m_pLayerDialog->show();
  else
    m_pLayerDialog->hide();
}

void KImageShopView::dialog_color()
{
  if (m_dialog_color->isChecked())
    m_pColorDialog->show();
  else
    m_pColorDialog->hide();
}

void KImageShopView::dialog_brush()
{
  if (m_dialog_brush->isChecked())
    m_pBrushDialog->show();
  else
    m_pBrushDialog->hide();
}

void KImageShopView::dialog_gradient()
{
  if (m_dialog_gradient->isChecked())
    m_pGradientDialog->show();
  else
    m_pGradientDialog->hide();
}

void KImageShopView::dialog_gradienteditor()
{
  if (m_dialog_gradienteditor->isChecked())
    m_pGradientEditorDialog->show();
  else
    m_pGradientEditorDialog->hide();
}

/*
 * layer action slots
 */

void KImageShopView::layer_rotate180()
{
  m_pDoc->rotateLayer180(0);
}

void KImageShopView::layer_rotateleft90()
{
  m_pDoc->rotateLayerLeft90(0);
}

void KImageShopView::layer_rotateright90()
{
  m_pDoc->rotateLayerRight90(0);
}

void KImageShopView::layer_mirrorX()
{
  m_pDoc->mirrorLayerX(0);
}

void KImageShopView::layer_mirrorY()
{
  m_pDoc->mirrorLayerY(0);
}

/*
 * image action slots
 */

void KImageShopView::merge_all_layers()
{
  m_pDoc->mergeAllLayers();
}

void KImageShopView::merge_visible_layers()
{
  m_pDoc->mergeVisibleLayers();
}

void KImageShopView::merge_linked_layers()
{
  m_pDoc->mergeLinkedLayers();
}

/*
 * misc action slots
 */

void KImageShopView::preferences()
{
    qDebug("PREFERENCES called");
}

int KImageShopView::docWidth()
{
  return m_pDoc->width();
}

int KImageShopView::docHeight()
{
  return m_pDoc->height();
}

int KImageShopView::xPaintOffset()
{
  int s = 0;
  if (m_pVert->isVisible())
    s = 16;

  int v = static_cast<int>((width() - 20 - s - docWidth())/2);
  if (v < 0)
    v = 0;
  return v;
}

int KImageShopView::yPaintOffset()
{
  int s = 0;
  if (m_pHorz->isVisible())
    s = 16;

  int v = static_cast<int>((height() - 20 - s - docHeight())/2);
  if (v < 0)
    v = 0;
  return v;
}

void KImageShopView::slotSetBrush(const Brush* b)
{
  m_pBrush = b;
}

#include "kimageshop_view.moc"
