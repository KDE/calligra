/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "kivio_canvas.h"
#include "kivio_guidelines.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_view.h"
#include "kivio_doc.h"
#include "kivio_tabbar.h"
#include "kivio_ruler.h"

#include "kivio_icon_view.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stackbar.h"
#include "kivio_screen_painter.h"
#include "kivio_grid_data.h"
#include "kivio_guidelines.h"

#include "tool_controller.h"
#include "tool.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcursor.h>

#include <assert.h>
#include <stdio.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qsize.h>


KivioCanvas::KivioCanvas( QWidget *par, KivioView* view, KivioDoc* doc, ToolController* tc, QScrollBar* vs, QScrollBar* hs, KivioRuler* vr, KivioRuler* hr )
: QWidget(par, "KivioCanvas", WResizeNoErase | WRepaintNoErase),
  m_pView(view),
  m_pDoc(doc),
  m_pToolsController(tc),
  m_pVertScrollBar(vs),
  m_pHorzScrollBar(hs),
  m_pVRuler(vr),
  m_pHRuler(hr)
{
  setBackgroundMode(NoBackground);
  setAcceptDrops(true);
  setMouseTracking(true);
  setFocusPolicy(StrongFocus);
  setFocus();

  delegateThisEvent = true;
  storedCursor = 0;
  pressGuideline = 0;

  m_pVRuler->installEventFilter(this);
  m_pHRuler->installEventFilter(this);

  m_pVertScrollBar->setLineStep(1);
  m_pHorzScrollBar->setLineStep(1);


  m_pVertScrollBar->setPageStep(10);
  m_pHorzScrollBar->setPageStep(10);

  connect(m_pVertScrollBar, SIGNAL(valueChanged(int)), SLOT(scrollV(int)));
  connect( m_pHorzScrollBar, SIGNAL(valueChanged(int)), SLOT(scrollH(int)));

  m_pZoom = 0.5;

  m_iXOffset = 0;
  m_iYOffset = 0;

  m_pScrollX = 0;
  m_pScrollY = 0;

  m_buffer = new QPixmap();

  m_pDragStencil = 0L;
  unclippedSpawnerPainter = 0L;
  unclippedPainter = 0L;

  m_borderTimer = new QTimer(this);
  connect(m_borderTimer,SIGNAL(timeout()),SLOT(borderTimerTimeout()));

  m_guideLinesTimer = new QTimer(this);
  connect(m_guideLinesTimer,SIGNAL(timeout()),SLOT(guideLinesTimerTimeout()));
}

KivioCanvas::~KivioCanvas()
{
  delete m_buffer;
}

KivioPage* KivioCanvas::findPage( const QString& _name )
{
  return m_pDoc->map()->findPage( _name );
}

const KivioPage* KivioCanvas::activePage() const
{
  return m_pView->activePage();
}

KivioPage* KivioCanvas::activePage()
{
  return m_pView->activePage();
}

void KivioCanvas::scrollH( int value )
{
  eraseGuides();

  // Relative movement
  int dx = m_iXOffset - value;
  // New absolute position
  m_iXOffset = value;

  bitBlt(m_buffer, dx, 0, m_buffer);
  scroll(dx, 0);

  updateRulers(true,false);
  emit visibleAreaChanged();
}

void KivioCanvas::scrollV( int value )
{
  eraseGuides();

  // Relative movement
  int dy = m_iYOffset - value;
  // New absolute position
  m_iYOffset = value;

  bitBlt(m_buffer, 0, dy, m_buffer);
  scroll(0, dy);

  updateRulers(false,true);
  emit visibleAreaChanged();
}

void KivioCanvas::scrollDx( int dx )
{
  if ( dx == 0 )
    return;

  int value = m_iXOffset - dx;
  m_pHorzScrollBar->setValue(value);
}

void KivioCanvas::scrollDy( int dy )
{
  if ( dy == 0 )
    return;

  int value = m_iYOffset - dy;
  m_pVertScrollBar->setValue(value);
}

void KivioCanvas::updateRulers( bool horiz, bool vert )
{
  if (!isUpdatesEnabled())
    return;

  if (horiz) {
    int pw = (int)actualPaperSizePt().w;
    int x0 = (int)((width()-pw)/2) - xOffset();
    m_pHRuler->setZoom(m_pZoom);
    m_pHRuler->updateVisibleArea(-x0,0);
  }

  if (vert) {
    int ph = (int)actualPaperSizePt().h;
    int y0 = (int)((height()-ph)/2) - yOffset();
    m_pVRuler->setZoom(m_pZoom);
    m_pVRuler->updateVisibleArea(0,-y0);
  }
}

void KivioCanvas::resizeEvent( QResizeEvent* )
{
  KivioGuideLines::resize(size(),m_pDoc);
  m_buffer->resize(size());
  updateRulers(true,true);
  updateScrollBars();

  emit visibleAreaChanged();
}

void KivioCanvas::wheelEvent( QWheelEvent* ev )
{
  ev->accept();
/*
  QPoint p = ev->pos();
  if ((ev->delta()<0)) {
    zoomIn(p);
  } else {
    zoomOut(p);
  }
*/
  QPoint p = ev->pos();
  if( (ev->delta()<0))
  {
     m_pVertScrollBar->subtractPage();
  }
  else
  {
     m_pVertScrollBar->addPage();
  }
}

void KivioCanvas::setUpdatesEnabled( bool isUpdate )
{
  static int i = 0;

  QWidget::setUpdatesEnabled(isUpdate);
  if (isUpdate) {
    --i;
    if (i == 0) {
      update();
      updateRulers(true,true);
      updateScrollBars();

      blockSignals(false);

      emit zoomChanges(m_pZoom);
      emit visibleAreaChanged();
    }
  } else {
    i++;
    blockSignals(true);
  }
}

void KivioCanvas::zoomIn(QPoint p)
{
  setUpdatesEnabled(false);
  TKPoint p0 = mapFromScreen(p);
  setZoom(m_pZoom*(100.0f+25.0f)/100.0f);
  QPoint p1 = mapToScreen(p0);
  scrollDx(-p1.x()+p.x());
  scrollDy(-p1.y()+p.y());
  setUpdatesEnabled(true);
}

void KivioCanvas::zoomOut(QPoint p)
{
  setUpdatesEnabled(false);
  TKPoint p0 = mapFromScreen(p);
  setZoom(m_pZoom/(100.0f+25.0f)*100.0f);
  QPoint p1 = mapToScreen(p0);
  scrollDx(-p1.x()+p.x());
  scrollDy(-p1.y()+p.y());
  setUpdatesEnabled(true);
}

void KivioCanvas::paintEvent( QPaintEvent* ev )
{
  if ( m_pDoc->isLoading() || !activePage() )
    return;

  KivioPage* page = activePage();
  eraseGuides();

  QPainter painter;
  painter.begin(m_buffer);

  QRect rect( ev->rect() );
  painter.fillRect(rect,white);

  // Draw Grid
  if (m_pDoc->grid().isShow) {
    int x = rect.x();
    int y = rect.y();
    int w = x + rect.width();
    int h = y + rect.height();

    TKSize dxy = actualGridFrequency();
    dxy.convertToPt();

    TKPoint p;
    p.set(0,0,UnitPoint);

    painter.setPen(m_pDoc->grid().color);

    QPoint p0 = mapToScreen(p);
    while (p0.x()<x) {
      p.x += dxy.w;
      p0 = mapToScreen(p);
    }
    while (p0.x()<=w) {
      painter.drawLine(p0.x(),y,p0.x(),h);

      p.x += dxy.w;
      p0 = mapToScreen(p);
    }

    p.set(0,0,UnitPoint);
    p0 = mapToScreen(p);
    while (p0.x()>w) {
      p.x -= dxy.w;
      p0 = mapToScreen(p);
    }
    while (p0.x()>=x) {
      painter.drawLine(p0.x(),y,p0.x(),h);

      p.x -= dxy.w;
      p0 = mapToScreen(p);
    }

    p.set(0,0,UnitPoint);
    p0 = mapToScreen(p);
    while (p0.y()<y) {
      p.y += dxy.h;
      p0 = mapToScreen(p);
    }
    while (p0.y()<=h) {
      painter.drawLine(x,p0.y(),w,p0.y());

      p.y += dxy.h;
      p0 = mapToScreen(p);
    }

    p.set(0,0,UnitPoint);
    p0 = mapToScreen(p);
    while (p0.y()>h) {
      p.y -= dxy.h;
      p0 = mapToScreen(p);
    }
    while (p0.y()>=y) {
      painter.drawLine(x,p0.y(),w,p0.y());

      p.y -= dxy.h;
      p0 = mapToScreen(p);
    }
  }

  painter.translate(-m_iXOffset,-m_iYOffset);

  QPoint p0 = actualPaperOrigin();
  int pw = (int)actualPaperSizePt().w;
  int ph = (int)actualPaperSizePt().h;
  int px0 = p0.x();
  int py0 = p0.y();

  if (m_pView->isShowPageMargins()) {
    float zf = m_pZoom;
    TKPageLayout pl = page->paperLayout();
    int ml = (int)(pl.ptLeft()*zf);
    int mt = (int)(pl.ptTop()*zf);
    int mr = (int)(pl.ptRight()*zf);
    int mb = (int)(pl.ptBottom()*zf);

    painter.save();
    painter.setPen(QPen(blue,1,DotLine));
    painter.drawRect(px0+ml,py0+mt,pw-ml-mr,ph-mt-mb);
    painter.restore();
  }

  if (m_pView->isShowPageBorders()) {
    painter.setPen(black);
    painter.fillRect(px0+pw,py0+3,5,ph,gray);
    painter.fillRect(px0+3,py0+ph,pw,3,gray);
    painter.drawRect(px0,py0,pw,ph);
  }

  // Draw content
  KivioScreenPainter kpainter;
  kpainter.start( m_buffer );
  kpainter.painter()->translate( -m_iXOffset, -m_iYOffset );
  kpainter.painter()->translate( px0, py0 );
  m_pDoc->paintContent(kpainter, rect, false, page, p0, m_pZoom, true);
  kpainter.stop();

  paintGuides(false);
  painter.end();

  bitBlt(this,rect.left(),rect.top(),m_buffer,rect.left(),rect.top(),rect.width(),rect.height());
}

void KivioCanvas::centerPage()
{
  m_pHorzScrollBar->setValue(0);
  m_pVertScrollBar->setValue(0);
}

void KivioCanvas::updateScrollBars()
{
  m_pScrollX = (int)actualPaperSizePt().w/2;
  m_pScrollY = (int)actualPaperSizePt().h/2;

  m_pHorzScrollBar->setRange(-m_pScrollX,m_pScrollX);
  if ( m_pHorzScrollBar->value() > m_pHorzScrollBar->maxValue() ||
       m_pHorzScrollBar->value() < m_pHorzScrollBar->minValue() )
  {
    m_pHorzScrollBar->setValue(0);
  }

  m_pVertScrollBar->setRange(-m_pScrollY,m_pScrollY);
  if ( m_pVertScrollBar->value() > m_pVertScrollBar->maxValue() ||
       m_pVertScrollBar->value() < m_pVertScrollBar->minValue() )
  {
    m_pVertScrollBar->setValue(0);
  }

  m_pVertScrollBar->setPageStep( height() );
  m_pHorzScrollBar->setPageStep( width() );
}

QSize KivioCanvas::actualSize()
{
  return QSize(2*m_pScrollX + width(), 2*m_pScrollY + height());
}

QPoint KivioCanvas::actualPaperOrigin()
{
  int pw = (int)actualPaperSizePt().w;
  int ph = (int)actualPaperSizePt().h;
  int px0 = (width()-pw)/2;
  int py0 = (height()-ph)/2;

  return QPoint(px0,py0);
}

float KivioCanvas::zoom()
{
  return m_pZoom;
}


void KivioCanvas::setZoom(float zoom)
{
  m_pZoom = QMAX(0.05f,QMIN(100.0f,zoom));

  updateScrollBars();
  updateRulers(true,true);

  erase();
  repaint();

  emit zoomChanges(m_pZoom);
  emit visibleAreaChanged();
}

TKSize KivioCanvas::actualGridFrequency()
{
  TKSize actual;
  int f = 0;
  do {
    f++;
    actual = m_pDoc->grid().freq;
    actual.w *= f;
    actual.h *= f;
    actual.convertToPt(m_pZoom);
  } while ( actual.w < 10 || actual.h < 10 );

  actual = m_pDoc->grid().freq;
  actual.w *= f;
  actual.h *= f;

  return actual;
}

TKSize KivioCanvas::actualPaperSizePt()
{
  TKSize ps;
  TKPageLayout pl = activePage()->paperLayout();

  float w = pl.ptWidth()*m_pZoom;
  float h = pl.ptHeight()*m_pZoom;
  ps.set(w,h,UnitPoint);

  return ps;
}

void KivioCanvas::toggleShowRulers( bool b )
{
  if (b) {
    m_pHRuler->show();
    m_pVRuler->show();
  } else {
    m_pHRuler->hide();
    m_pVRuler->hide();
  }
}

bool KivioCanvas::event( QEvent* e )
{
  bool f = QWidget::event(e);
  if (m_pToolsController && delegateThisEvent)
    m_pToolsController->delegateEvent(e,this);

  delegateThisEvent = true;
  return f;
}

void KivioCanvas::enterEvent( QEvent* )
{
}

void KivioCanvas::leaveEvent( QEvent* )
{
  m_pVRuler->updatePointer(-1,-1);
  m_pHRuler->updatePointer(-1,-1);
}

void KivioCanvas::mousePressEvent(QMouseEvent* e)
{
    if(!m_pDoc->isReadWrite())
        return;
    if(m_pView->isShowGuides())
    {
        lastPoint = e->pos();
        TKPoint p = mapFromScreen(e->pos());
        KivioGuideLines* gl = activePage()->guideLines();

        bool unselectAllGuideLines = true;
        pressGuideline = 0;

        if ((e->state() & ~ShiftButton) == NoButton) {
            KivioGuideLineData* gd = gl->find(p.x,p.y,2.0/m_pZoom);
            if (gd) {
                pressGuideline = gd;
                if ((e->button() == RightButton) || ((e->button() & ShiftButton) == ShiftButton)) {
                    if (gd->isSelected())
                        gl->unselect(gd);
                    else
                        gl->select(gd);
                } else {
                    if (!gd->isSelected()) {
                        gl->unselectAll();
                        gl->select(gd);
                    }
                }
                unselectAllGuideLines = false;
                delegateThisEvent = false;
                updateGuides();
                m_guideLinesTimer->start(500,true);
            }
        }

        if (unselectAllGuideLines && gl->hasSelected()) {
            gl->unselectAll();
            updateGuides();
        }
    }
}

void KivioCanvas::mouseReleaseEvent(QMouseEvent* e)
{
    if(!m_pDoc->isReadWrite())
        return;
  if (pressGuideline) {
    m_guideLinesTimer->stop();
    TKPoint p = mapFromScreen(e->pos());
    KivioGuideLines* gl = activePage()->guideLines();
    KivioGuideLineData* gd = gl->find(p.x,p.y,2.0/m_pZoom);
    if (gd) {
      setCursor(gd->orientation()==Qt::Vertical ? sizeHorCursor:sizeVerCursor);
    } else {
      updateGuidesCursor();
    }
    delegateThisEvent = false;
    pressGuideline = 0;
  }
}

void KivioCanvas::mouseMoveEvent(QMouseEvent* e)
{
    if(!m_pDoc->isReadWrite())
        return;
    if(m_pView->isShowGuides())
    {
        m_pVRuler->updatePointer(e->pos().x(),e->pos().y());
        m_pHRuler->updatePointer(e->pos().x(),e->pos().y());

        TKPoint p = mapFromScreen(e->pos());
        KivioGuideLines* gl = activePage()->guideLines();

        if ((e->state() & LeftButton == LeftButton) && gl->hasSelected()) {
            if (m_guideLinesTimer->isActive()) {
                m_guideLinesTimer->stop();
                guideLinesTimerTimeout();
            }
            delegateThisEvent = false;
            eraseGuides();
            QPoint p = e->pos();
            p -= lastPoint;
            if (p.x() != 0)
                gl->moveSelectedByX(p.x()/m_pZoom);
            if (p.y() != 0)
                gl->moveSelectedByY(p.y()/m_pZoom);
            paintGuides();
        } else {
            if ((e->state() & ~ShiftButton) == NoButton) {
                KivioGuideLineData* gd = gl->find(p.x,p.y,2.0/m_pZoom);
                if (gd) {
                    delegateThisEvent = false;
                    if (!storedCursor)
                        storedCursor = new QCursor(cursor());
                    setCursor(gd->orientation()==Qt::Vertical ? sizeHorCursor:sizeVerCursor);
                } else {
                    updateGuidesCursor();
                }
            }
        }
    }
//  float xf = p.xToUnit(UnitMillimeter);
//  float yf = p.yToUnit(UnitMillimeter);

//  debug("%s %s",(const char*)QString::number(xf,'f',2),(const char*)QString::number(yf,'f',2));
    lastPoint = e->pos();
}

QPoint KivioCanvas::mapToScreen( TKPoint pos )
{
  QPoint p;
  QPoint p0 = actualPaperOrigin();
  int x = (int)(pos.x*m_pZoom);
  int y = (int)(pos.y*m_pZoom);

  p.setX( x + p0.x() - m_iXOffset );
  p.setY( y + p0.y() - m_iYOffset );

  return p;
}

TKPoint KivioCanvas::mapFromScreen( QPoint pos )
{
  QPoint p0 = actualPaperOrigin();
  int x = pos.x() + m_iXOffset - p0.x();
  int y = pos.y() + m_iYOffset - p0.y();

  float xf = x/m_pZoom;
  float yf = y/m_pZoom;

  TKPoint p;
  p.set(xf,yf,UnitPoint);

  return p;
}

void KivioCanvas::startRectDraw( const QPoint &p, RectType )
{
  currRect = QRect( 0, 0, -1, -1 );

  QPoint pos( p );
  oldRectValid = false;
  beginUnclippedPainter();
  rectAnchor = pos;
  currRect = QRect( rectAnchor, QPoint(0,0) );

  m_borderTimer->start(100);
}

void KivioCanvas::continueRectDraw( const QPoint &p, RectType )
{
  QPoint pos = p;
  QPoint p2 = pos;
  QRect r( rectAnchor, p2 );
  r = r.normalize();

  if ( oldRectValid )
    unclippedPainter->drawRect( currRect );
  if ( r.width() > 1 || r.height() > 1 ) {
    oldRectValid = true;
    currRect = r;
    unclippedPainter->drawRect( currRect );
  } else {
    oldRectValid = false;
  }
}

void KivioCanvas::endRectDraw()
{
  m_borderTimer->stop();

  if ( !unclippedPainter )
    return;

  if ( oldRectValid )
    unclippedPainter->drawRect( currRect );

  endUnclippedPainter();
}

/**
 * Starts a new drag & draw (called from the drag enter event)
 *
 * @param p The point to begin at
 *
 * This will allocate a new KivioStencil for drawing with and
 * set some class variables used during redraws.
 */
void KivioCanvas::startSpawnerDragDraw( const QPoint &p )
{
    currRect = QRect( 0, 0, -1, -1 );

    KivioStencilSpawner *pSpawner = KivioIconView::curDragSpawner();
    if( !pSpawner )
        return;

    // If we for some reason didn't delete an old drag stencil,
    // do so now.
    if( m_pDragStencil )
    {
       kdDebug() << "KivioCanvas::startSpawnerDragDraw() - m_pDragStencil still exists.  BUG!" << endl;
        delete m_pDragStencil;
        m_pDragStencil = 0L;
    }

    // Map the point from screenspace to page space
    TKPoint qp = mapFromScreen( p );
    qp = snapToGrid(qp);

    // Allocate a new stencil for dragging around
    m_pDragStencil = pSpawner->newStencil();
    m_pDragStencil->setPosition( qp.x, qp.y );
//    m_pDragStencil->setDimensions( pSpawner->defWidth(), pSpawner->defHeight() );

    // Invalidate the rectangle
    oldRectValid = true;

    // Create a new painter object
    beginUnclippedSpawnerPainter();

    // Calculate the zoom value
    float zf = m_pZoom;

    // Translate the painter so that 0,0 means where the page starts on the canvas
    QPoint paperOrigin = actualPaperOrigin();
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate( paperOrigin.x() - m_iXOffset, paperOrigin.y() - m_iYOffset );

    // Assign the painter object to the intra-stencil data object, as well
    // as the zoom factor
    m_dragStencilData.painter = unclippedSpawnerPainter;
    m_dragStencilData.scale = zf;

    // Draw the outline of the stencil
    m_pDragStencil->paintOutline( &m_dragStencilData );

    unclippedSpawnerPainter->painter()->restore();

}

/**
 * Undraws the old stencil outline, draws the new one
 */
void KivioCanvas::continueSpawnerDragDraw( const QPoint &p )
{
   bool snappedX, snappedY;

    QPoint pos = p;
    QPoint p2 = pos;

    // Translate the painter so that 0,0 means where the page starts on the canvas
    QPoint paperOrigin = actualPaperOrigin();
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate( paperOrigin.x() - m_iXOffset, paperOrigin.y() - m_iYOffset );

    // Undraw the old outline
    if( oldRectValid )
    {
        m_pDragStencil->paintOutline( &m_dragStencilData );
    }

    // Map the new point from screenspace to page space
    TKPoint orig = mapFromScreen(p2);
    TKPoint qp = snapToGrid( orig );

    // First snap to screen
    qp = snapToGrid(qp);
    m_pDragStencil->setPosition( qp.x, qp.y );

    // Now snap to the guides
    qp.set( orig.x + m_pDragStencil->w(), orig.y + m_pDragStencil->h(), UnitPoint );
    qp = snapToGuides(qp, snappedX, snappedY);
    if( snappedX==true ) {
       m_pDragStencil->setX(qp.x - m_pDragStencil->w());
    }
    if( snappedY==true ) {
       m_pDragStencil->setY(qp.y - m_pDragStencil->h());
    }

    qp.set( orig.x, orig.y, UnitPoint );
    qp = snapToGuides(qp, snappedX, snappedY);
    if( snappedX==true ) {
       m_pDragStencil->setX(qp.x);
    }
    if( snappedY==true ) {
       m_pDragStencil->setY(qp.y);
    }

    // Redraw the new outline
    oldRectValid = true;
    m_pDragStencil->paintOutline( &m_dragStencilData );
    unclippedSpawnerPainter->painter()->restore();

}


/**
 * Ends the ability to draw a drag & drop spawner object
 */
void KivioCanvas::endSpawnerDragDraw()
{
    // Avoid the noid
    if ( !unclippedSpawnerPainter )
        return;

    // If we have a valid old drawing spot, undraw it
    if ( oldRectValid )
    {
        QPoint paperOrigin = actualPaperOrigin();
        unclippedSpawnerPainter->painter()->save();
        unclippedSpawnerPainter->painter()->translate( paperOrigin.x() - m_iXOffset, paperOrigin.y() - m_iYOffset );
        m_pDragStencil->paintOutline( &m_dragStencilData );
        unclippedSpawnerPainter->painter()->restore();
    }

    // Smack the painter around a bit
    endUnclippedSpawnerPainter();

    // If we have a stencil we were dragging around, delete it.
    if( m_pDragStencil )
    {
        delete m_pDragStencil;
        m_pDragStencil = 0L;
    }

}


/**
 * Creates a new spawner drawing object
 */
void KivioCanvas::beginUnclippedSpawnerPainter()
{
    // End any previous attempts
    endUnclippedSpawnerPainter();

    // I have no idea what this does.  Max?
    bool unclipped = testWFlags( WPaintUnclipped );
    setWFlags( WPaintUnclipped );


    // Allocate a new painter object for use in drawing
    unclippedSpawnerPainter = new KivioScreenPainter();

    // Tell it to start (allocates a Qpainter object)
    unclippedSpawnerPainter->start(this);

    // Uhhhhh??
    if( !unclipped )
        clearWFlags( WPaintUnclipped );


    // Make sure it's doing NOT drawing.
    unclippedSpawnerPainter->painter()->setRasterOp( NotROP );
    unclippedSpawnerPainter->painter()->setPen( QColor(0,0,250) );

}


/**
 * Deletes the current spawner drawing object
 */
void KivioCanvas::endUnclippedSpawnerPainter()
{
    if( unclippedSpawnerPainter )
    {
        unclippedSpawnerPainter->stop();
        delete unclippedSpawnerPainter;
        unclippedSpawnerPainter = 0L;
    }
}

void KivioCanvas::beginUnclippedPainter()
{
    endUnclippedPainter();
    bool unclipped = testWFlags( WPaintUnclipped );

    setWFlags( WPaintUnclipped );
    unclippedPainter = new QPainter;
    unclippedPainter->begin( this );

    if ( !unclipped )
        clearWFlags( WPaintUnclipped );

    unclippedPainter->setRasterOp( NotROP );
    unclippedPainter->setPen( QPen(blue,1,DotLine) );
}

void KivioCanvas::endUnclippedPainter()
{
    if ( unclippedPainter )
    {
        unclippedPainter->end();
        delete unclippedPainter;
        unclippedPainter = 0;
    }
}

void KivioCanvas::borderTimerTimeout()
{
  QPoint p = mapFromGlobal(QCursor::pos());
  int dx = 0;
  int dy = 0;
  int d = 10;

  QRect r(currRect);
  int vpos = m_pVertScrollBar->value();
  int vmax = m_pVertScrollBar->maxValue();
  int vmin = m_pVertScrollBar->minValue();

  int hpos = m_pHorzScrollBar->value();
  int hmax = m_pHorzScrollBar->maxValue();
  int hmin = m_pHorzScrollBar->minValue();

  if ( p.x() < 0 && hpos > hmin ) {
    dx = QMIN(d,hpos-hmin);
    r.setRight(r.right()+dx);
    rectAnchor.setX(rectAnchor.x()+dx);
  }

  if ( p.y() < 0 && vpos > vmin ) {
    dy = QMIN(d,vpos-vmin);
    r.setBottom(r.bottom()+dy);
    rectAnchor.setY(rectAnchor.y()+dy);
  }

  if ( p.x() > width() && hpos < hmax ) {
    dx = -QMIN(d,hmax-hpos);
    r.setLeft(r.left()+dx);
    rectAnchor.setX(rectAnchor.x()+dx);
  }

  if ( p.y() > height() && vpos < vmax  ) {
    dy = -QMIN(d,vmax-vpos);
    r.setTop(r.top()+dy);
    rectAnchor.setY(rectAnchor.y()+dy);
  }

  if ( dx != 0 || dy != 0 ) {
    unclippedPainter->drawRect( currRect );
    scrollDx(dx);
    scrollDy(dy);
    unclippedPainter->drawRect( r );
    currRect = r;
  }
}


/**
 * Handles the initial drag event
 *
 * @param e The event
 *
 * This will check to make sure the drag object is of the correct mimetype.
 * If it is, it accepts it, and the calls startSpawnerDragDraw which will
 * allocate a new drawing object and set some class variables for future
 * drawing.
 */
void KivioCanvas::dragEnterEvent( QDragEnterEvent *e )
{
    if( e->provides("kivio/stencilSpawner") )
    {
        e->accept();
        startSpawnerDragDraw( e->pos() );
    }
}



/**
 * Handles drag-move events
 *
 * @param e The event
 *
 * This makes sure the drag object is of type kivio/stencilSpawner since these
 * are currently the only type of mime-types accepted.  If so, it accepts the
 * event, and then tells the drawing object to update itself with a new position.
 */
void KivioCanvas::dragMoveEvent( QDragMoveEvent *e )
{
    // Does it speak our language?
    if( e->provides("kivio/stencilSpawner") )
    {
        e->accept();
        continueSpawnerDragDraw( e->pos() );
    }
}


/**
 * Handles drops for this object
 *
 * @param e The drop event object
 *
 * This function takes care of handling the final drop event of this object.  It will
 * allocate a new object of the currently dragged Spawner type (stencil), and add it
 * to the active page of the document (which actually adds it to the active layer
 * of the active page).
 */
void KivioCanvas::dropEvent( QDropEvent *e )
{
    // Terminate the drawing object
    endSpawnerDragDraw();

//    // Get as handle on the icon view
//    KivioIconView *pIconView = (KivioIconView *)m_pView->stackBar()->visiblePage();

//    // Get a pointer to the currently dragged KivioStencilSpawner object
//    KivioStencilSpawner *pSpawner = pIconView->curDragSpawner();
    KivioStencilSpawner *pSpawner = KivioIconView::curDragSpawner();
    if( !pSpawner )
        return;


    // Get a pointer to the current KivioPage
    KivioPage *pPage = activePage();
    if( !pPage )
    {
       kdDebug() << "KivioCanvas::dropEvent() - No active page for stencil to drop on" << endl;
        return;
    }

    // Allocate a new stencil
    KivioStencil *pNewStencil = pSpawner->newStencil();

    // Set the current stencil settings
    QPoint pos = e->pos();
    TKPoint pagePoint = snapToGrid(mapFromScreen( pos ));
    pNewStencil->setX( pagePoint.x );
    pNewStencil->setY( pagePoint.y );
    pNewStencil->setW( pSpawner->defWidth() );
    pNewStencil->setH( pSpawner->defHeight() );

    // Only set these properties if we held ctrl down
    // FIXME: Make this happen!
//    pNewStencil->setFGColor( m_pView->fgColor() );
//    pNewStencil->setBGColor( m_pView->bgColor() );
//    pNewStencil->setLineWidth( (float)m_pView->lineWidth() );


    // Add the new stencil to the page
    pPage->addStencil( pNewStencil );

    pPage->unselectAllStencils();
    pPage->selectStencil( pNewStencil );

    // Select the "selection tool" in case it's not done
    Tool *t = m_pToolsController->findTool("Select");
    if( t )
    {
        m_pToolsController->selectTool(t);
    }

    m_pDoc->updateView(activePage());
}


/**
 * Handles when a drag leaves this object
 *
 * @param e The event object
 *
 * This will call endSpawnerDragDraw() which terminates the drawing
 * object and ends interaction with the drag.
 */
void KivioCanvas::dragLeaveEvent( QDragLeaveEvent * )
{
    endSpawnerDragDraw();
}



void KivioCanvas::drawSelectedStencilsXOR()
{
    // This should never happen, but check just in case
    if ( !unclippedSpawnerPainter )
        return;

    float zf = m_pZoom;

    // Translate the painter so that 0,0 means where the page starts on the canvas
    QPoint paperOrigin = actualPaperOrigin();
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate( paperOrigin.x() - m_iXOffset, paperOrigin.y() - m_iYOffset );

    // Assign the painter object to the intra-stencil data object, as well
    // as the zoom factor
    m_dragStencilData.painter = unclippedSpawnerPainter;
    m_dragStencilData.scale = zf;

    KivioStencil *pStencil = activePage()->selectedStencils()->first();
    while( pStencil )
    {
        pStencil->paintOutline( &m_dragStencilData );

        pStencil = activePage()->selectedStencils()->next();
    }

    unclippedSpawnerPainter->painter()->restore();
}

void KivioCanvas::drawStencilXOR( KivioStencil *pStencil )
{
    // This should never happen, but check just in case
    if ( !unclippedSpawnerPainter )
        return;

    float zf = m_pZoom;

    // Translate the painter so that 0,0 means where the page starts on the canvas
    QPoint paperOrigin = actualPaperOrigin();
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate( paperOrigin.x() - m_iXOffset, paperOrigin.y() - m_iYOffset );

    // Assign the painter object to the intra-stencil data object, as well
    // as the zoom factor
    m_dragStencilData.painter = unclippedSpawnerPainter;
    m_dragStencilData.scale = zf;

    pStencil->paintOutline( &m_dragStencilData );

    unclippedSpawnerPainter->painter()->restore();
}

void KivioCanvas::keyReleaseEvent( QKeyEvent *e )
{
    switch( e->key() )
    {
        case Key_Delete: {
            KivioGuideLines* gl = activePage()->guideLines();
            if (gl->hasSelected()) {
              eraseGuides();
              gl->removeSelected();
              paintGuides();
              updateGuidesCursor();
            } else {
              activePage()->deleteSelectedStencils();
              m_pDoc->updateView(activePage());
            }
            break;
        }
    }
}

TKPoint KivioCanvas::snapToGridAndGuides(TKPoint point)
{
    TKPoint p = point;

    TKSize dist = m_pDoc->grid().snap;
    TKSize dxy = m_pDoc->grid().freq;

    dxy.convertToPt();
    dist.convertToPt();

    int dx = (int)(p.x/dxy.w);
    int dy = (int)(p.y/dxy.h);

    float distx = QMIN(QABS(p.x-dxy.w*dx),QABS(p.x-dxy.w*(dx+1)));
    float disty = QMIN(QABS(p.y-dxy.h*dy),QABS(p.y-dxy.h*(dy+1)));

    if( m_pDoc->grid().isSnap)
    {
       if ( distx < dist.w) {
	  if ( QABS(p.x-dxy.w*dx) < QABS(p.x-dxy.w*(dx+1)) )
	     p.x = dxy.w*dx;
	  else
	     p.x = dxy.w*(dx+1);
       }

       if ( disty < dist.h) {
	  if ( QABS(p.y-dxy.h*dy) < QABS(p.y-dxy.h*(dy+1)) )
	     p.y = dxy.h*dy;
	  else
	     p.y = dxy.h*(dy+1);
       }
    }

    /*
     * Now if the point is within 4 pixels of a gridline, snap
     * to the grid line.
     */
    if (m_pView->isSnapGuides())
    {
       float four = 4.0f / m_pZoom;
       KivioGuideLines *pGuides = activePage()->guideLines();
       KivioGuideLineData *pData = pGuides->findHorizontal( point.y, four );
       if( pData )
       {
	  p.y = (float)pData->position();
       }

       pData = pGuides->findVertical( point.x, four );
       if( pData )
       {
	  p.x = (float)pData->position();
       }
    }

    return p;
}

TKPoint KivioCanvas::snapToGrid(TKPoint point)
{
    if (!m_pDoc->grid().isSnap)
      return point;

    TKPoint p = point;

    TKSize dist = m_pDoc->grid().snap;
    TKSize dxy = m_pDoc->grid().freq;

    dxy.convertToPt();
    dist.convertToPt();

    int dx = (int)(p.x/dxy.w);
    int dy = (int)(p.y/dxy.h);

    float distx = QMIN(QABS(p.x-dxy.w*dx),QABS(p.x-dxy.w*(dx+1)));
    float disty = QMIN(QABS(p.y-dxy.h*dy),QABS(p.y-dxy.h*(dy+1)));

    if ( distx < dist.w) {
      if ( QABS(p.x-dxy.w*dx) < QABS(p.x-dxy.w*(dx+1)) )
        p.x = dxy.w*dx;
      else
        p.x = dxy.w*(dx+1);
    }

    if ( disty < dist.h) {
      if ( QABS(p.y-dxy.h*dy) < QABS(p.y-dxy.h*(dy+1)) )
        p.y = dxy.h*dy;
      else
        p.y = dxy.h*(dy+1);
    }

    return p;
}

TKPoint KivioCanvas::snapToGuides(TKPoint point, bool &snappedX, bool &snappedY)
{
    snappedX = false;
    snappedY = false;
    TKPoint p = point;

    if (m_pView->isSnapGuides())
    {
       float four = 4.0f / m_pZoom;
       KivioGuideLines *pGuides = activePage()->guideLines();
       KivioGuideLineData *pData = pGuides->findHorizontal( point.y, four );
       if( pData )
       {
	  snappedY = true;
	  p.y = (float)pData->position();
       }

       pData = pGuides->findVertical( point.x, four );
       if( pData )
       {
	  snappedX = true;
	  p.x = (float)pData->position();
       }
    }

    return p;
}

float KivioCanvas::snapToGridX(float z)
{
  TKPoint p;
  p.set(z,0,UnitPoint);
  return snapToGrid(p).x;
}

float KivioCanvas::snapToGridY(float z)
{
  TKPoint p;
  p.set(0,z,UnitPoint);
  return snapToGrid(p).y;
}

void KivioCanvas::updateGuides()
{
  eraseGuides();
  paintGuides();
}

void KivioCanvas::guideLinesTimerTimeout()
{
  if (!storedCursor)
    storedCursor = new QCursor(cursor());
  setCursor(sizeAllCursor);
}

void KivioCanvas::updateGuidesCursor()
{
  if (storedCursor) {
    setCursor(*storedCursor);
    delete storedCursor;
    storedCursor = 0;
  }
}

bool KivioCanvas::eventFilter(QObject* o, QEvent* e)
{
  if ((o == m_pVRuler || o == m_pHRuler) && (e->type() == QEvent::MouseMove || e->type() == QEvent::MouseButtonRelease) && m_pView->isShowGuides()) {

    QMouseEvent* me = (QMouseEvent*)e;
    QPoint p = mapFromGlobal(me->globalPos());
    KivioGuideLines* gl = activePage()->guideLines();

    if (e->type() == QEvent::MouseMove) {
      bool f = geometry().contains(p);
      if (!pressGuideline && f) {
        enterEvent(0);

        eraseGuides();
        gl->unselectAll();
        KivioGuideLineData* gd;
        TKPoint tp = mapFromScreen(p);
        if (o == m_pVRuler)
          gd = gl->add(tp.x,Qt::Vertical);
        else
          gd = gl->add(tp.y,Qt::Horizontal);

        pressGuideline = gd;
        gl->select(gd);
        paintGuides();

        updateGuidesCursor();
        QWidget* w = (QWidget*)o;
        storedCursor = new QCursor(w->cursor());
        w->setCursor(sizeAllCursor);

        lastPoint = p;

      } else {
        if (pressGuideline && !f) {
          leaveEvent(0);

          eraseGuides();
          gl->remove(pressGuideline);
          paintGuides();

          if (storedCursor) {
            QWidget* w = (QWidget*)o;
            w->setCursor(*storedCursor);
            delete storedCursor;
            storedCursor = 0;
          }

          pressGuideline = 0;
        } else {
          if (pressGuideline && f) {
            QMouseEvent* m = new QMouseEvent(QEvent::MouseMove, p, me->globalPos(), me->button(), me->state());
            mouseMoveEvent(m);
            delete m;
            delegateThisEvent = true;
          }
        }
      }
    }
    if (e->type() == QEvent::MouseButtonRelease && pressGuideline) {

      eraseGuides();
      gl->unselect(pressGuideline);
      paintGuides();

      pressGuideline = 0;
      if (storedCursor) {
        QWidget* w = (QWidget*)o;
        w->setCursor(*storedCursor);
        delete storedCursor;
        storedCursor = 0;
      }
      enterEvent(0);
      QMouseEvent* m = new QMouseEvent(QEvent::MouseMove, p, me->globalPos(), NoButton, NoButton);
      mouseMoveEvent(m);
      delete m;
      delegateThisEvent = true;
    }
  }

  return QWidget::eventFilter(o,e);
}

void KivioCanvas::eraseGuides()
{
  KivioGuideLines* gl = activePage()->guideLines();
  gl->erase(m_buffer,this);
}

void KivioCanvas::paintGuides(bool show)
{
  if (!m_pView->isShowGuides())
    return;

  KivioGuideLines* gl = activePage()->guideLines();
  gl->paint(m_buffer,this);
  if (show)
    bitBlt(this,0,0,m_buffer);
}

void KivioCanvas::setViewCenterPoint(KivioPoint p)
{
  setUpdatesEnabled(false);

  TKPageLayout pl = activePage()->paperLayout();
  float w = pl.ptWidth()/2.0;
  float h = pl.ptHeight()/2.0;

  w = w - p.x();
  h = h - p.y();

  centerPage();

  scrollDx((int)(w*m_pZoom));
  scrollDy((int)(h*m_pZoom));

  setUpdatesEnabled(true);
}

KivioRect KivioCanvas::visibleArea()
{
  TKPoint p0 = mapFromScreen(QPoint(0,0));
  TKPoint p1 = mapFromScreen(QPoint(width()-1,height()-1));

  return KivioRect(p0.x, p0.y, p1.x - p0.x, p1.y - p0.y);
}

void KivioCanvas::setVisibleArea(KivioRect r, int margin)
{
  setUpdatesEnabled(false);

  int cw = width() - 2*margin;
  int ch = height() - 2*margin;

  float zw = cw/r.w();
  float zh = ch/r.h();
  float z = QMIN(zw,zh);

  setZoom(z);

  KivioPoint c = r.center();

  setViewCenterPoint(c);
  setUpdatesEnabled(true);
}

void KivioCanvas::setVisibleAreaByWidth(KivioRect r, int margin)
{
  setUpdatesEnabled(false);

  int cw = width() - 2*margin;
  float z = cw/r.w();

  setZoom(z);

  KivioPoint c = r.center();

  setViewCenterPoint(c);
  setUpdatesEnabled(true);
}

void KivioCanvas::setVisibleAreaByHeight(KivioRect r, int margin)
{
  setUpdatesEnabled(false);

  int ch = height() - 2*margin;
  float z = ch/r.h();

  setZoom(z);

  KivioPoint c = r.center();

  setViewCenterPoint(c);
  setUpdatesEnabled(true);
}
#include "kivio_canvas.moc"
