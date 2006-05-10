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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include "kivio_canvas.h"
#include "kivio_page.h"
#include "kivio_map.h"
#include "kivio_view.h"
#include "kivio_doc.h"

#include "kivio_icon_view.h"
#include "kivio_stencil.h"
#include "kivio_stencil_spawner.h"
#include "kivio_stencil_spawner_info.h"
#include "kivio_stackbar.h"
#include "kivio_screen_painter.h"
#include "kivio_grid_data.h"
#include "kivio_layer.h"

#include "kivio_pluginmanager.h"

#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kcursor.h>
#include <KoGlobal.h>
#include <KoZoomHandler.h>
#include <KoSize.h>
#include <KoRuler.h>
#include <KoPoint.h>
#include <KoTabBar.h>
#include <kapplication.h>

#include <assert.h>
#include <stdio.h>
#include <QLabel>
#include <qpixmap.h>
#include <qscrollbar.h>
#include <qtimer.h>
#include <qsize.h>

using namespace Kivio;

KivioCanvas::KivioCanvas( QWidget *par, KivioView* view, KivioDoc* doc, QScrollBar* vs, QScrollBar* hs)
: QWidget(par, "KivioCanvas", WResizeNoErase | WRepaintNoErase),
  m_pView(view),
  m_pDoc(doc),
  m_pVertScrollBar(vs),
  m_pHorzScrollBar(hs),
  m_guides(view, view->zoomHandler())
{
  setBackgroundMode(NoBackground);
  setAcceptDrops(true);
  setMouseTracking(true);
  setFocusPolicy(StrongFocus);
  setFocus();

  m_showConnectorTargets = false;
  delegateThisEvent = true;

  m_pVertScrollBar->setLineStep(1);
  m_pHorzScrollBar->setLineStep(1);


  m_pVertScrollBar->setPageStep(10);
  m_pHorzScrollBar->setPageStep(10);

  connect(m_pVertScrollBar, SIGNAL(valueChanged(int)), SLOT(scrollV(int)));
  connect( m_pHorzScrollBar, SIGNAL(valueChanged(int)), SLOT(scrollH(int)));

  m_iXOffset = 0;
  m_iYOffset = 0;

  m_pScrollX = 0;
  m_pScrollY = 0;

  m_pageOffsetX = 0;
  m_pageOffsetY = 0;

  m_pasteMoving = false;

  m_buffer = new QPixmap();

  m_pDragStencil = 0L;
  unclippedSpawnerPainter = 0L;
  unclippedPainter = 0L;

  m_borderTimer = new QTimer(this);
  connect(m_borderTimer,SIGNAL(timeout()),SLOT(borderTimerTimeout()));
}

KivioCanvas::~KivioCanvas()
{
  delete m_buffer;
  delete m_borderTimer;
  delete unclippedPainter;
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
  // Relative movement
  int dx = m_iXOffset - value;
  // New absolute position
  m_iXOffset = value;

  bitBlt(m_buffer, dx, 0, m_buffer);
  scroll(dx, 0);

  emit visibleAreaChanged();
}

void KivioCanvas::scrollV( int value )
{
  // Relative movement
  int dy = m_iYOffset - value;
  // New absolute position
  m_iYOffset = value;

  bitBlt(m_buffer, 0, dy, m_buffer);
  scroll(0, dy);

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

void KivioCanvas::resizeEvent( QResizeEvent* )
{
  m_buffer->resize(size());
  updateScrollBars();

  emit visibleAreaChanged();
}

void KivioCanvas::wheelEvent( QWheelEvent* ev )
{
  ev->accept();
  
  if( (ev->delta()>0))
  {
     if(ev->state() == Qt::ControlButton) {
       zoomIn(ev->pos());
     } else if(ev->state() == Qt::ShiftButton) {
       m_pVertScrollBar->setValue(m_pVertScrollBar->value() - height());
     } else {
       m_pVertScrollBar->setValue(m_pVertScrollBar->value() - 30);
     }
  }
  else
  {
     if(ev->state() == Qt::ControlButton) {
       zoomOut(ev->pos());
     } else if(ev->state() == Qt::ShiftButton) {
       m_pVertScrollBar->setValue(m_pVertScrollBar->value() + height());
     } else {
       m_pVertScrollBar->setValue(m_pVertScrollBar->value() + 30);
     }
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
      updateScrollBars();

      blockSignals(false);

      emit visibleAreaChanged();
    }
  } else {
    i++;
    blockSignals(true);
  }
}

void KivioCanvas::zoomIn(const QPoint &p)
{
  setUpdatesEnabled(false);
  KoPoint p0 = mapFromScreen(p);
  m_pView->viewZoom(m_pView->zoomHandler()->zoom() + 25);
  QPoint p1 = mapToScreen(p0);
  scrollDx(-p1.x()+p.x());
  scrollDy(-p1.y()+p.y());
  setUpdatesEnabled(true);
}

void KivioCanvas::zoomOut(const QPoint &p)
{
  setUpdatesEnabled(false);
  KoPoint p0 = mapFromScreen(p);
  int newZoom = m_pView->zoomHandler()->zoom() - 25;

  if(newZoom > 0) {
    m_pView->viewZoom(newZoom);
    QPoint p1 = mapToScreen(p0);
    scrollDx(-p1.x()+p.x());
    scrollDy(-p1.y()+p.y());
  }
  setUpdatesEnabled(true);
}

void KivioCanvas::paintEvent( QPaintEvent* ev )
{
  if ( m_pDoc->isLoading() || !activePage() )
    return;

  KivioPage* page = activePage();

  QPainter painter;
  painter.begin(m_buffer);

  KoPageLayout pl = page->paperLayout();
  int pw = m_pView->zoomHandler()->zoomItX(pl.ptWidth);
  int ph = m_pView->zoomHandler()->zoomItY(pl.ptHeight);
  QRect fillRect(0, 0, pw, ph);
  QRect paintRect = ev->rect();
  QRegion grayRegion(paintRect);
  grayRegion.translate(m_iXOffset - m_pageOffsetX, m_iYOffset - m_pageOffsetY);
  grayRegion -= fillRect;
  grayRegion.translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);

  // This code comes from KPresenter's kprcanvas.cpp
  // Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
  painter.save();
  painter.setClipRegion(grayRegion, QPainter::CoordPainter);
  painter.setPen(Qt::NoPen);
  painter.fillRect(grayRegion.boundingRect(), KApplication::palette().active().brush(QColorGroup::Mid));
  painter.restore();
  // end of copy...

  painter.translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset +m_pageOffsetY);
  painter.fillRect(fillRect, white);

  // Draw Grid
  if(m_pDoc->grid().isShow) {
    KoPoint topLeft(0, 0);
    KoPoint bottomRight(pl.ptWidth, pl.ptHeight);
    QPoint zoomedTL = m_pView->zoomHandler()->zoomPoint(topLeft);
    QPoint zoomedBR = m_pView->zoomHandler()->zoomPoint(bottomRight);

    KoSize freq = m_pDoc->grid().freq;

    painter.setPen(m_pDoc->grid().color);

    double x = qRound(topLeft.x() / freq.width()) * freq.width();
    int zoomed = 0;

    while(x <= bottomRight.x()) {
      zoomed = m_pView->zoomHandler()->zoomItX(x);
      painter.drawLine(zoomed, zoomedTL.y(), zoomed, zoomedBR.y());
      x += freq.width();
    }

    double y = qRound(topLeft.y() / freq.height()) * freq.height();

    while(y <= bottomRight.y()) {
      zoomed = m_pView->zoomHandler()->zoomItY(y);
      painter.drawLine(zoomedTL.x(), zoomed, zoomedBR.x(), zoomed);
      y += freq.height();
    }
  }

  if(m_pView->isShowPageMargins()) {
    int ml = m_pView->zoomHandler()->zoomItX(pl.ptLeft);
    int mt = m_pView->zoomHandler()->zoomItY(pl.ptTop);
    int mr = m_pView->zoomHandler()->zoomItX(pl.ptRight);
    int mb = m_pView->zoomHandler()->zoomItY(pl.ptBottom);

    painter.save();
    painter.setPen(QPen("#C7C7C7",1,SolidLine));
    painter.drawRect(ml,mt,pw-ml-mr,ph-mt-mb);
    painter.restore();
  }

  // Draw page borders
  painter.setPen(black);
  painter.fillRect(pw, 3, 5, ph, gray);
  painter.fillRect(3, ph, pw, 3, gray);
  painter.drawRect(0, 0, pw, ph);

  // Draw content
  KivioScreenPainter kpainter;
  kpainter.start(m_buffer);
  kpainter.translateBy(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);
  m_pDoc->paintContent(kpainter, paintRect, false, page, QPoint(0, 0), m_pView->zoomHandler(), showConnectorTargets(), true);
  kpainter.stop();

  if(m_pView->isShowGuides()) {
    m_guides.paintGuides(painter);
  }

  painter.end();

  bitBlt(this, paintRect.left(), paintRect.top(), m_buffer,
         paintRect.left(), paintRect.top(), paintRect.width(), paintRect.height());
}

void KivioCanvas::updateScrollBars()
{
  if(!activePage()) {
    return;
  }

  KoPageLayout pl = activePage()->paperLayout();
  int pw = m_pView->zoomHandler()->zoomItX(pl.ptWidth);
  int ph = m_pView->zoomHandler()->zoomItY(pl.ptHeight);

  m_pScrollX = qMax(pw - width(), 0);
  m_pScrollY = qMax(ph - height(), 0);

  m_pHorzScrollBar->setRange(0, m_pScrollX);
  if ( m_pHorzScrollBar->value() > m_pHorzScrollBar->maxValue() ||
       m_pHorzScrollBar->value() < m_pHorzScrollBar->minValue() )
  {
    m_pHorzScrollBar->setValue(0);
  }

  m_pVertScrollBar->setRange(0, m_pScrollY);
  if ( m_pVertScrollBar->value() > m_pVertScrollBar->maxValue() ||
       m_pVertScrollBar->value() < m_pVertScrollBar->minValue() )
  {
    m_pVertScrollBar->setValue(0);
  }

  m_pVertScrollBar->setPageStep( height() );
  m_pHorzScrollBar->setPageStep( width() );

  if(pw >= width()) {
    m_pageOffsetX = 0;
  } else {
    m_pageOffsetX = (width() - pw) / 2;
  }

  if(ph >= height()) {
    m_pageOffsetY = 0;
  } else {
    m_pageOffsetY = (height() - ph) / 2;
  }
}

QSize KivioCanvas::actualSize() const
{
  return QSize(m_pScrollX, m_pScrollY);
}

bool KivioCanvas::event( QEvent* e )
{
  bool f = QWidget::event(e);

  if (m_pView->pluginManager() && delegateThisEvent) {
    f = m_pView->pluginManager()->delegateEvent(e);
  }

  delegateThisEvent = true;
  return f;
}

void KivioCanvas::enterEvent( QEvent* )
{
}

void KivioCanvas::leaveEvent( QEvent* )
{
  m_pView->setMousePos(-1, -1);
}

void KivioCanvas::mousePressEvent(QMouseEvent* e)
{
    if(!m_pDoc->isReadWrite())
        return;

    if(m_pasteMoving) {
      endPasteMoving();
      return;
    }

    Kivio::PluginManager* pluginManager = view()->pluginManager();

    if(m_pView->isShowGuides() && (!pluginManager || (pluginManager->defaultTool() == pluginManager->activeTool()))) {
      if(m_guides.mousePressEvent(e)) {
        delegateThisEvent = false;
        return;
      }
    }
}

void KivioCanvas::mouseReleaseEvent(QMouseEvent* e)
{
  if(!m_pDoc->isReadWrite())
    return;

  Kivio::PluginManager* pluginManager = view()->pluginManager();

  if(view()->isShowGuides() && (!pluginManager || (pluginManager->defaultTool() == pluginManager->activeTool()))) {
    if(m_guides.mouseReleaseEvent(e)) {
      delegateThisEvent = false;
      return;
    }
  }
}

void KivioCanvas::mouseMoveEvent(QMouseEvent* e)
{
  if(!m_pDoc->isReadWrite())
    return;

  if(m_pasteMoving) {
    continuePasteMoving(e->pos());
  } else {
    Kivio::PluginManager* pluginManager = view()->pluginManager();

    if(m_pView->isShowGuides() && (!pluginManager || (pluginManager->defaultTool() == pluginManager->activeTool()))) {
      delegateThisEvent = !m_guides.mouseMoveEvent(e);
    }
  }

  lastPoint = e->pos();
  view()->setMousePos(lastPoint.x(), lastPoint.y());
}

QPoint KivioCanvas::mapToScreen(const KoPoint& pos)
{
  QPoint p;
  int x = m_pView->zoomHandler()->zoomItX(pos.x());
  int y = m_pView->zoomHandler()->zoomItY(pos.y());

  p.setX( x - m_iXOffset + m_pageOffsetX);
  p.setY( y - m_iYOffset + m_pageOffsetY);

  return p;
}

KoPoint KivioCanvas::mapFromScreen( const QPoint & pos )
{
  int x = pos.x() + m_iXOffset - m_pageOffsetX;
  int y = pos.y() + m_iYOffset - m_pageOffsetY;
  double xf = m_pView->zoomHandler()->unzoomItX(x);
  double yf = m_pView->zoomHandler()->unzoomItY(y);

  KoPoint p(xf, yf);
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
    kDebug(43000) << "KivioCanvas::startSpawnerDragDraw() - m_pDragStencil still exists.  BUG!" << endl;
    delete m_pDragStencil;
    m_pDragStencil = 0L;
  }

  // Map the point from screenspace to page space
  KoPoint qp = mapFromScreen( p );
  qp = snapToGrid(qp);

  // Allocate a new stencil for dragging around
  m_pDragStencil = pSpawner->newStencil();
  m_pDragStencil->setPosition( qp.x(), qp.y() );

  // Invalidate the rectangle
  oldRectValid = true;

  // Create a new painter object
  beginUnclippedSpawnerPainter();

  // Translate the painter so that 0,0 means where the page starts on the canvas
  unclippedSpawnerPainter->painter()->save();
  unclippedSpawnerPainter->painter()->translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);

  // Assign the painter object to the intra-stencil data object, as well
  // as the zoom factor
  m_dragStencilData.painter = unclippedSpawnerPainter;
  m_dragStencilData.zoomHandler = m_pView->zoomHandler();

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

  // Translate the painter so that 0,0 means where the page starts on the canvas
  unclippedSpawnerPainter->painter()->save();
  unclippedSpawnerPainter->painter()->translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);

  // Undraw the old outline
  if( oldRectValid )
  {
    m_pDragStencil->paintOutline( &m_dragStencilData );
  }

  // Map the new point from screenspace to page space
  KoPoint orig = mapFromScreen(p);
  KoPoint qp = snapToGrid( orig );

  // First snap to screen
  qp = snapToGrid(qp);
  m_pDragStencil->setPosition( qp.x(), qp.y() );

  // Now snap to the guides
  qp.setCoords(orig.x() + m_pDragStencil->w(), orig.y() + m_pDragStencil->h());
  qp = snapToGuides(qp, snappedX, snappedY);

  if(snappedX) {
    m_pDragStencil->setX(qp.x() - m_pDragStencil->w());
  }

  if(snappedY) {
    m_pDragStencil->setY(qp.y() - m_pDragStencil->h());
  }

  qp.setCoords(orig.x() + (m_pDragStencil->w() / 2.0), orig.y() + (m_pDragStencil->h() / 2.0));
  qp = snapToGuides(qp, snappedX, snappedY);

  if(snappedX) {
    m_pDragStencil->setX(qp.x() - (m_pDragStencil->w() / 2.0));
  }

  if(snappedY) {
    m_pDragStencil->setY(qp.y() - (m_pDragStencil->h() / 2.0));
  }

  qp.setCoords(orig.x(), orig.y());
  qp = snapToGuides(qp, snappedX, snappedY);

  if(snappedX) {
    m_pDragStencil->setX(qp.x());
  }

  if(snappedY) {
    m_pDragStencil->setY(qp.y());
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
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);
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

  setFocus();
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
    dx = qMin(d,hpos-hmin);
    r.setRight(r.right()+dx);
    rectAnchor.setX(rectAnchor.x()+dx);
  }

  if ( p.y() < 0 && vpos > vmin ) {
    dy = qMin(d,vpos-vmin);
    r.setBottom(r.bottom()+dy);
    rectAnchor.setY(rectAnchor.y()+dy);
  }

  if ( p.x() > width() && hpos < hmax ) {
    dx = -qMin(d,hmax-hpos);
    r.setLeft(r.left()+dx);
    rectAnchor.setX(rectAnchor.x()+dx);
  }

  if ( p.y() > height() && vpos < vmax  ) {
    dy = -qMin(d,vmax-vpos);
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
        activePage()->unselectAllStencils();
        updateAutoGuideLines();
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

    // Get a pointer to the currently dragged KivioStencilSpawner object
    KivioStencilSpawner *pSpawner = KivioIconView::curDragSpawner();
    
    if( !pSpawner )
        return;
        
    QPoint pos = e->pos();
    KoPoint pagePoint = snapToGrid(mapFromScreen( pos ));
    view()->addStencilFromSpawner(pSpawner, pagePoint.x(), pagePoint.y());

    // FIXME Select the "selection tool" in case it's not done
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

    // Translate the painter so that 0,0 means where the page starts on the canvas
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);

    // Assign the painter object to the intra-stencil data object, as well
    // as the zoom factor
    m_dragStencilData.painter = unclippedSpawnerPainter;
    m_dragStencilData.zoomHandler = m_pView->zoomHandler();

    KivioStencil *pStencil = activePage()->selectedStencils()->first();
    while( pStencil )
    {
        pStencil->paintOutline( &m_dragStencilData );
        pStencil->paintSelectionHandles( &m_dragStencilData );

        pStencil = activePage()->selectedStencils()->next();
    }

    unclippedSpawnerPainter->painter()->restore();
}

void KivioCanvas::drawStencilXOR( KivioStencil *pStencil )
{
    // This should never happen, but check just in case
    if ( !unclippedSpawnerPainter )
        return;

    // Translate the painter so that 0,0 means where the page starts on the canvas
    unclippedSpawnerPainter->painter()->save();
    unclippedSpawnerPainter->painter()->translate(-m_iXOffset + m_pageOffsetX, -m_iYOffset + m_pageOffsetY);

    // Assign the painter object to the intra-stencil data object, as well
    // as the zoom factor
    m_dragStencilData.painter = unclippedSpawnerPainter;
    m_dragStencilData.zoomHandler = m_pView->zoomHandler();

    pStencil->paintOutline( &m_dragStencilData );
    pStencil->paintSelectionHandles( &m_dragStencilData );

    unclippedSpawnerPainter->painter()->restore();
}

void KivioCanvas::keyPressEvent( QKeyEvent *e )
{
  if(view()->isShowGuides() && m_guides.keyPressEvent(e)) {
    return;
  }
}

KoPoint KivioCanvas::snapToGridAndGuides(const KoPoint& point)
{
  KoPoint p = point;

  p = snapToGrid(p);

  bool snappedX, snappedY;
  KoPoint guidePoint = snapToGuides(point, snappedX, snappedY);

  if(snappedX) {
    p.setX(guidePoint.x());
  }

  if(snappedY) {
    p.setY(guidePoint.y());
  }

  return p;
}

KoPoint KivioCanvas::snapToGrid(const KoPoint& point)
{
  if (!m_pDoc->grid().isSnap)
    return point;

  KoPoint p = point;

  KoSize dist = m_pDoc->grid().snap;
  KoSize freq = m_pDoc->grid().freq;

  double dx = qRound(p.x() / freq.width());
  double dy = qRound(p.y() / freq.height());

  double distx = QABS(p.x() - (freq.width() * dx));
  double disty = QABS(p.y() - (freq.height() * dy));

  if(distx < dist.width()) {
    p.setX(freq.width() * dx);
  }

  if(disty < dist.height()) {
    p.setY(freq.height() * dy);
  }

  return p;
}

KoPoint KivioCanvas::snapToGuides(const KoPoint& point, bool &snappedX, bool &snappedY)
{
  snappedX = false;
  snappedY = false;
  KoPoint p = point;

  if (m_pView->isSnapGuides())
  {
    KoGuides::SnapStatus status = KoGuides::SNAP_NONE;
    KoPoint diff;
    m_guides.snapToGuideLines(p, 4, status, diff);
    p += diff;

    if(status & KoGuides::SNAP_HORIZ) {
      snappedY = true;
    }
    if(status & KoGuides::SNAP_VERT) {
      snappedX = true;
    }

    m_guides.repaintSnapping(p, status);
  }

  return p;
}

void KivioCanvas::setViewCenterPoint(const KoPoint &p)
{
  setUpdatesEnabled(false);

  KoRect va = visibleArea();

  double x = qMax(0.0, p.x() - (va.width() / 2.0));
  double y = qMax(0.0, p.y() - (va.height() / 2.0));

  m_pVertScrollBar->setValue(m_pView->zoomHandler()->zoomItY(y));
  m_pHorzScrollBar->setValue(m_pView->zoomHandler()->zoomItX(x));

  setUpdatesEnabled(true);
}

KoRect KivioCanvas::visibleArea()
{
  KoPoint p0 = mapFromScreen(QPoint(0,0));
  KoPoint p1 = mapFromScreen(QPoint(width()-1,height()-1));

  return KoRect(p0.x(), p0.y(), p1.x() - p0.x(), p1.y() - p0.y());
}

void KivioCanvas::setVisibleArea(KoRect r, int margin)
{
  setUpdatesEnabled(false);
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());

  float cw = width() - 2 * margin;
  float ch = height() - 2 * margin;

  float zw = cw / (float)zoom.zoomItX(r.width());
  float zh = ch / (float)zoom.zoomItY(r.height());
  float z = qMin(zw, zh);

  m_pView->viewZoom(qRound(z * 100));

  KoPoint c = r.center();

  setViewCenterPoint(KoPoint(c.x(), c.y()));
  setUpdatesEnabled(true);
}

void KivioCanvas::setVisibleAreaByWidth(KoRect r, int margin)
{
  setUpdatesEnabled(false);
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());

  float cw = width() - 2*margin;
  float z = cw / (float)zoom.zoomItX(r.width());

  m_pView->viewZoom(qRound(z * 100));

  KoPoint c = r.center();

  setViewCenterPoint(KoPoint(c.x(), c.y()));
  setUpdatesEnabled(true);
}

void KivioCanvas::setVisibleAreaByHeight(KoRect r, int margin)
{
  setUpdatesEnabled(false);
  KoZoomHandler zoom;
  zoom.setZoomAndResolution(100, KoGlobal::dpiX(),
    KoGlobal::dpiY());

  float ch = height() - 2*margin;
  float z = ch / (float)zoom.zoomItY(r.height());

  m_pView->viewZoom(qRound(z * 100));

  KoPoint c = r.center();

  setViewCenterPoint(KoPoint(c.x(), c.y()));
  setUpdatesEnabled(true);
}

void KivioCanvas::startPasteMoving()
{
  setEnabled(false);
  KoPoint p = activePage()->getRectForAllSelectedStencils().center();
  m_origPoint.setCoords(p.x(), p.y());

  // Create a new painter object
  beginUnclippedSpawnerPainter();
  drawSelectedStencilsXOR();

  // Build the list of old geometry
  KoRect *pData;
  m_lstOldGeometry.clear();
  KivioStencil* pStencil = activePage()->selectedStencils()->first();

  while( pStencil )
  {
    pData = new KoRect;
    *pData = pStencil->rect();
    m_lstOldGeometry.append(pData);

    pStencil = activePage()->selectedStencils()->next();
  }

  continuePasteMoving(lastPoint);
  m_pasteMoving = true;
  setEnabled(true);
}

void KivioCanvas::continuePasteMoving(const QPoint &pos)
{
  KoPoint pagePoint = mapFromScreen( pos );

  double dx = pagePoint.x() - m_origPoint.x();
  double dy = pagePoint.y() - m_origPoint.y();

  bool snappedX;
  bool snappedY;

  double newX, newY;

  // Undraw the old stencils
  drawSelectedStencilsXOR();

  // Translate to the new position
  KoPoint p;
  KoRect selectedRect = activePage()->getRectForAllSelectedStencils();

  newX = selectedRect.x() + dx;
  newY = selectedRect.y() + dy;

  // First attempt a snap-to-grid
  p.setCoords(newX, newY);
  p = snapToGrid(p);

  newX = p.x();
  newY = p.y();

  // Now the guides override the grid so we attempt to snap to them
  p.setCoords(selectedRect.x() + dx + selectedRect.width(), selectedRect.y() + dy + selectedRect.height());
  p = snapToGuides(p, snappedX, snappedY);

  if(snappedX) {
    newX = p.x() - selectedRect.width();
  }

  if(snappedY) {
    newY = p.y() - selectedRect.height();
  }

  p.setCoords(selectedRect.x() + dx + (selectedRect.width() / 2.0), selectedRect.y() + dy + (selectedRect.height() / 2.0));
  p = snapToGuides(p, snappedX, snappedY);

  if(snappedX) {
    newX = p.x() - (selectedRect.width() / 2.0);
  }

  if(snappedY) {
    newY = p.y() - (selectedRect.height() / 2.0);
  }

  p.setCoords(selectedRect.x() + dx, selectedRect.y() + dy);
  p = snapToGuides(p, snappedX, snappedY);

  if(snappedX) {
    newX = p.x();
  }

  if(snappedY) {
    newY = p.y();
  }

  dx = newX - selectedRect.x();
  dy = newY - selectedRect.y();

  // Translate to the new position
  KivioStencil *pStencil = activePage()->selectedStencils()->first();
  KoRect* pData = m_lstOldGeometry.first();
  // bool move = true;

  while( pStencil && pData )
  {
    newX = pData->x() + dx;
    newY = pData->y() + dy;

    if( pStencil->protection()->at( kpX ) == false ) {
      pStencil->setX(newX);
    }
    if( pStencil->protection()->at( kpY ) == false ) {
      pStencil->setY(newY);
    }

    pData = m_lstOldGeometry.next();
    pStencil = activePage()->selectedStencils()->next();
  }

  // Draw the stencils
  drawSelectedStencilsXOR();
  m_pView->updateToolBars();
}

void KivioCanvas::endPasteMoving()
{
  KivioStencil *pStencil = activePage()->selectedStencils()->first();
  KoRect *pData = m_lstOldGeometry.first();

  while( pStencil && pData )
  {
    if(pStencil->type() == kstConnector) {
      pStencil->searchForConnections(m_pView->activePage(), m_pView->zoomHandler()->unzoomItY(4));
    }

    pData = m_lstOldGeometry.next();
    pStencil = activePage()->selectedStencils()->next();
  }

  drawSelectedStencilsXOR();

  endUnclippedSpawnerPainter();

  // Clear the list of old geometry
  m_lstOldGeometry.clear();
  m_pasteMoving = false;
}

void KivioCanvas::updateAutoGuideLines()
{
  QValueList<double> hGuideLines;
  QValueList<double> vGuideLines;
  QPtrList<KivioLayer> layerList = *(activePage()->layers());
  QPtrListIterator<KivioLayer> layerIt(layerList);
  KivioLayer* layer = 0;
  QPtrList<KivioStencil> stencilList;
  QPtrListIterator<KivioStencil> stencilIt(stencilList);
  KivioStencil* stencil = 0;

  while((layer = layerIt.current()) != 0) {
    ++layerIt;

    if(layer->visible()) {
      stencilList = *(layer->stencilList());
      stencilIt.toFirst();

      while((stencil = stencilIt.current()) != 0) {
        ++stencilIt;

        if(!stencil->isSelected()) {
          hGuideLines << stencil->y() << (stencil->y() + (stencil->h() / 2.0)) << (stencil->y() + stencil->h());
          vGuideLines << stencil->x() << (stencil->x() + (stencil->w() / 2.0)) << (stencil->x() + stencil->w());
        }
      }
    }
  }

  KoPageLayout pl = activePage()->paperLayout();

  // Add the middle of the page and the margins
  hGuideLines << (pl.ptHeight / 2.0) << pl.ptTop << pl.ptBottom;
  vGuideLines << (pl.ptWidth / 2.0) << pl.ptLeft << pl.ptRight;

  guideLines().setAutoGuideLines(hGuideLines, vGuideLines);
}

#include "kivio_canvas.moc"
