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
#include "kivio_doc.h"
#include "stencilbardockmanager.h"
#include "stencilbarbutton.h"
#include "kivio_stackbar.h"
#include "kivio_view.h"

#include <qcursor.h>
#include <qvaluelist.h>
#include <qapplication.h>
#include <qwindowdefs.h>
#include <qtimer.h>
#include <qrect.h>
#include <qframe.h>
#include <qpainter.h>

#include <klocale.h>
#include <kdebug.h>
#include <QX11Info>

StencilBarDockManager::StencilBarDockManager( KivioView* parent, const char* name )
: QWidget(parent,name)
{
  dragButton = 0L;
  dragWidget = 0L;
  m_pView = parent;
  m_destinationBar = 0L;
  dragPos = OnDesktop;

  m_pBars.setAutoDelete(true);

  m_pDoc = m_pView->doc();
  connect(m_pDoc, SIGNAL(sig_deleteStencilSet(DragBarButton*, QWidget*, KivioStackBar*)), this,
          SLOT(slotDeleteStencilSet(DragBarButton*, QWidget*, KivioStackBar*)));

  moveManager = new KoToolDockMoveManager();
  connect(moveManager,SIGNAL(positionChanged()),SLOT(slotMoving()));
}

StencilBarDockManager::~StencilBarDockManager()
{
  delete moveManager;
}

void StencilBarDockManager::insertStencilSet( QWidget* w, const QString& caption,
                                              BarPos pos, QRect /*r*/, KivioStackBar* destinationBar )
{
  KivioStackBar* bar = 0L;
  
  if(destinationBar) {
    bar = destinationBar;
  } else {
    Qt::Dock position = Qt::DockLeft;

    if ( pos == AutoSelect ) {
      pos = Left;
    }

    switch (pos) {
      case OnDesktop:
        position = Qt::DockTornOff;
        break;
      case Left:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockLeft);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockLeft;
        break;
      }
      case Top:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockTop);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockTop;
        break;
      }
      case Right:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockRight);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockRight;
        break;
      }
      case Bottom:
      {
        QPtrList<QDockWindow> dockList = m_pView->mainWindow()->dockWindows(Qt::DockBottom);
        
        for(QDockWindow* dock = dockList.first(); dock; dock = dockList.next()) {
          if(::qt_cast<KivioStackBar*>(dock) && (static_cast<KivioStackBar*>(dock)->view() == m_pView)) {
            bar = static_cast<KivioStackBar*>(dock);
            break;
          }
        }

        position = Qt::DockBottom;
        break;
      }
      case OnTopLevelBar: // hmm this shouldn't happen :)
        position = Qt::DockTornOff;
        break;
      default:
        break;
    }

    if (!bar) {
      bar = new KivioStackBar(m_pView, m_pView->mainWindow());
      m_pView->mainWindow()->moveDockWindow(bar, position);
      m_pBars.append(bar);
      connect(bar,SIGNAL(beginDragPage(DragBarButton*)),SLOT(slotBeginDragPage(DragBarButton*)));
      connect(bar,SIGNAL(finishDragPage(DragBarButton*)),SLOT(slotFinishDragPage(DragBarButton*)));
      connect(bar,SIGNAL(deleteButton(DragBarButton*,QWidget*,KivioStackBar*)),
              m_pDoc, SLOT(slotDeleteStencilSet(DragBarButton*,QWidget*,KivioStackBar*)));
    }
  }

  bar->insertPage(w,caption);
  bar->showPage(w);
  bar->show();
}

void StencilBarDockManager::slotBeginDragPage( DragBarButton* w )
{
  m_destinationBar = 0L;
  dragButton = w;
  dragWidget = ((KivioStackBar*)sender())->findPage(dragButton);
  dragButton->setUpdatesEnabled(false);
  dragWidget->setUpdatesEnabled(false);
  dragPos = OnDesktop;
  moveManager->doMove(dragButton);
}

void StencilBarDockManager::slotFinishDragPage( DragBarButton* )
{
  moveManager->stop();
  dragButton->setUpdatesEnabled(true);
  dragButton->update();

  dragWidget->setUpdatesEnabled(true);
  dragWidget->update();

  QString caption(dragButton->text());
  KivioStackBar* bar = (KivioStackBar*)sender();
  bar->removePage(dragWidget);

  insertStencilSet(dragWidget,caption,dragPos,moveManager->geometry(),m_destinationBar);

  // remove KivioStackBar if no more pages
  if (!bar->visiblePage()) {
    int k = m_pBars.findRef(bar);

    if ( k!= -1 ) {
      m_pBars.remove(k);
      m_pBars.insert(k,0L);
    }
  }

  dragButton = 0L;
  dragWidget = 0L;
  m_destinationBar = 0L;
}


void StencilBarDockManager::slotMoving()
{
  m_destinationBar = 0L;
  QPoint p = QCursor::pos();
  QPoint globalPos;

  // check existing bars
  for ( KivioStackBar* bar = m_pBars.first(); bar; bar = m_pBars.next() ) {
    if ( bar->geometry().contains(p) ) {
      if(bar->place() == QDockWindow::OutsideDock) {
        dragPos = OnTopLevelBar;
        globalPos = bar->geometry().topLeft();
      } else {
        dragPos = AutoSelect;
        globalPos = bar->mapToGlobal(bar->geometry().topLeft());
      }
      
      m_destinationBar = bar;
      moveManager->movePause(true,false);
      moveManager->setGeometry(globalPos.x(),
                               moveManager->y(),
                               bar->width(),
                               moveManager->getWidget()->sizeHint().height());
      return;
    }
  }

  // check "on mainView"
  QRect mr(m_pView->mapToGlobal(QPoint(0,0)),m_pView->size());

  if ( mr.contains(p) ) {
    QRect r(mr);
    r.setWidth(r.width()/4);
    if ( r.contains(p) && !m_pBars.at(Left) ) {
      moveManager->movePause();
      moveManager->setGeometry(r);
      dragPos = Left;
      return;
    }

    r.moveBy(r.width()*3,0);

    if ( r.contains(p) && !m_pBars.at(Right) ) {
      moveManager->movePause();
      moveManager->setGeometry(r);
      dragPos = Right;
      return;
    }

    QRect t(mr);

    t.setHeight(t.height()/4);
    if ( t.contains(p) && !m_pBars.at(Top) ) {
      moveManager->movePause();
      moveManager->setGeometry(t);
      dragPos = Top;
      return;
    }

    t.moveBy(0,t.height()*3);

    if ( t.contains(p) && !m_pBars.at(Bottom) ) {
      moveManager->movePause();
      moveManager->setGeometry(t);
      dragPos = Bottom;
      return;
    }
  }

  // on desktop
  moveManager->resize(dragWidget->size());
  moveManager->movePause(false,false);
  dragPos = OnDesktop;
}

void StencilBarDockManager::slotDeleteStencilSet( DragBarButton* pBtn, QWidget *, KivioStackBar *pBar )
{
  pBar->deletePageAndButton(pBtn);

  // remove KivioStackBar if no more pages
  if (!pBar->visiblePage()) {
    int k = m_pBars.findRef(pBar);

    if ( k!= -1 ) {
      m_pBars.remove(k);
      pBar = 0;
    } else {
      delete pBar;
      pBar = 0;
    }
  }
}

void StencilBarDockManager::setAllStackBarsShown(bool shown)
{
  for(KivioStackBar* bar = m_pBars.first(); bar; bar = m_pBars.next()) {
    if(shown) {
      bar->show();
    } else {
      bar->hide();
    }
  }
}

//
// KoToolDockMoveManager
//

KoToolDockMoveManager::KoToolDockMoveManager()
  : QObject()
{
#if defined Q_WS_X11 && !defined K_WS_QTONLY
    XGCValues gv;
#endif

    working=false;
    noLast=true;

#if defined Q_WS_X11 && !defined K_WS_QTONLY
    scr = QX11Info::appScreen();
    root = QX11Info::appRootWindow();

    gv.function = GXxor;
    gv.line_width = 2;
    gv.foreground = WhitePixel(QX11Info::display(), scr)^BlackPixel(qt_xdisplay(), scr);
    gv.subwindow_mode = IncludeInferiors;
    long mask = GCForeground | GCFunction | GCLineWidth | GCSubwindowMode;
    rootgc = XCreateGC(QX11Info::display(), QX11Info::appRootWindow(), mask, &gv);
#endif

    timer = new QTimer(this);
}

KoToolDockMoveManager::~KoToolDockMoveManager()
{
  stop();
  delete timer;
}

void KoToolDockMoveManager::doMove( QWidget* _w )
{
  if (working)
    return;

  working=true;
  isDoMove = true;
  mirrorX=false;
  mirrorY=false;

  setWidget(_w);

  offX = rx - p.x();
  offY = ry - p.y();

  orig_x = p.x();
  orig_y = p.y();
  orig_w = w;
  orig_h = h;

  QApplication::setOverrideCursor(QCursor(sizeAllCursor));

  movePause(false,false);

  drawRectangle(xp, yp, w, h);

  timer->disconnect();
  connect(timer,SIGNAL(timeout()),SLOT(doMoveInternal()));
  timer->start(0);
}

void KoToolDockMoveManager::doMoveInternal()
{
  if ( !working )
    return;

  if (!pauseMoveX)
    rx = QCursor::pos().x();

  if (!pauseMoveY)
    ry = QCursor::pos().y();

  xp = rx - offX;
  yp = ry - offY;

  emit positionChanged();

  if (check(xp, yp, w, h)) {
    paintProcess(false,xp, yp, w, h);

#if defined Q_WS_X11 && !defined K_WS_QTONLY
        XFlush(QX11Info::display());
        XSync(QX11Info::display(),false);
#endif
  }
}

void KoToolDockMoveManager::stop()
{
  if (!working)
    return;

  timer->stop();
  QApplication::restoreOverrideCursor();

  paintProcess();
#if defined Q_WS_X11 && !defined K_WS_QTONLY
    XFlush(QX11Info::display());
#endif

    working = false;
}

void KoToolDockMoveManager::setGeometry( const QRect& r )
{
  setGeometry(r.x(),r.y(),r.width(),r.height());
}

void KoToolDockMoveManager::setGeometry(int _x, int _y, int _w, int _h)
{
  xp=_x;
  yp=_y;
  w=_w;
  h=_h;

  check(_x, _y, _w, _h, true);
  paintProcess(false,_x, _y, _w, _h);

#if defined Q_WS_X11 && !defined K_WS_QTONLY
    XFlush(QX11Info::display());
    XSync(QX11Info::display(),false);
#endif
}

void KoToolDockMoveManager::drawRectangle( int _x, int _y, int _w, int _h)
{
  if (!noLast)
    return;

  ox = _x;
  oy = _y;
  ow = _w;
  oh = _h;

#if defined Q_WS_X11 && !defined K_WS_QTONLY
    XDrawRectangle(QX11Info::display(), root, rootgc, _x, _y, _w, _h);
#endif
    noLast = false;
}

void KoToolDockMoveManager::paintProcess( bool onlyDelete, int _x, int _y, int _w, int _h )
{
  if (noLast && onlyDelete)
    return;

  if ( ox == _x && oy == _y && ow ==_w && oh == _h )
    return;

#if defined Q_WS_X11 && !defined K_WS_QTONLY
    XDrawRectangle(QX11Info::display(), root, rootgc, ox, oy, ow, oh);
#endif
    noLast = true;

    drawRectangle(_x,_y,_w,_h);
}

void KoToolDockMoveManager::movePause( bool horizontal, bool vertical )
{
  pauseMoveX = horizontal;
  pauseMoveY = vertical;
}

void KoToolDockMoveManager::moveContinue()
{
  pauseMoveX = false;
  pauseMoveY = false;
}

void KoToolDockMoveManager::doXResize( QWidget* w, bool mirror )
{
  if (working)
    return;

  mirrorX = mirror;
  mirrorY = false;

  yOnly = false;
  xOnly = true;

  doResize(w);
}

void KoToolDockMoveManager::doYResize( QWidget* w, bool mirror )
{
  if (working)
    return;

  mirrorX = false;
  mirrorY = mirror;

  yOnly = true;
  xOnly = false;

  doResize(w);
}

void KoToolDockMoveManager::doXYResize( QWidget* w, bool _mirrorX, bool _mirrorY )
{
  if (working)
    return;

  mirrorX = _mirrorX;
  mirrorY = _mirrorY;

  yOnly = false;
  xOnly = false;

  doResize(w);
}

void KoToolDockMoveManager::doResizeInternal()
{
  if (!yOnly)
    rx = QCursor::pos().x();

  if (!xOnly)
    ry = QCursor::pos().y();

  int dx = rx - sx;
  int dy = ry - sy;

  if ( mirrorX ){
    w = rr.width() - dx;
    xp = rr.x() + dx;
  } else {
    w = rr.width() + dx;
  }

  if ( mirrorY ){
    h = rr.height() - dy;
    yp = rr.y() + dy;
  } else {
    h = rr.height() + dy;
  }

  emit sizeChanged();

  if (check(xp, yp, w, h)) {
    paintProcess(false,xp, yp, w, h);

#if defined Q_WS_X11 && !defined K_WS_QTONLY
        XFlush(QX11Info::display());
        XSync(QX11Info::display(),false);
#endif
  }
}

void KoToolDockMoveManager::setWidget( QWidget* _w )
{
  widget = _w;
  minSize = widget->minimumSize();
  maxSize = widget->maximumSize();

  rr = QRect(widget->mapToGlobal(QPoint(0,0)),widget->size());
  p =  rr.topLeft();

  xp = rr.x();
  yp = rr.y();
  w  = rr.width();
  h  = rr.height();

  rx = QCursor::pos().x();
  ry = QCursor::pos().y();
}

void KoToolDockMoveManager::doResize( QWidget* _w )
{
  setWidget(_w);

  working=true;
  isDoMove = false;

  QPoint curPos = QCursor::pos();
  sx = curPos.x();
  sy = curPos.y();

  offX = sx - p.x();
  offY = sy - p.y();

  drawRectangle(xp, yp, w, h);

  timer->disconnect();
  connect(timer,SIGNAL(timeout()),SLOT(doResizeInternal()));
  timer->start(0);
}

bool KoToolDockMoveManager::check(int& x, int& y, int& w, int& h, bool change)
{

  int w1 = qMin(qMax(minSize.width(), w), maxSize.width());
  int h1 = qMin(qMax(minSize.height(), h), maxSize.height());

  bool f1 = (w1-w)+(h1-h) == 0;

  if (change) {
    if (mirrorX)
      x += w - w1;
    w = w1;
    if (mirrorY)
      y += h - h1;
    h = h1;
  }

  int x0 = x;
  int y0 = y;
  int w0 = w;
  int h0 = h;

  if (isDoMove)
    emit fixPosition(x0,y0,w0,h0);
  else
    emit fixSize(x0,y0,w0,h0);

  bool f2 = (x0==x)&&(y0==y)&&(w0==w)&&(h0==h);

  if (change) {
    x = x0;
    y = y0;
    w = w0;
    h = h0;
  }

  return f1&&f2;
}

QRect KoToolDockMoveManager::geometry()
{
  int x0 = xp;
  int y0 = yp;
  int w0 = w;
  int h0 = h;
  check(x0,y0,w0,h0,true);

  return QRect(x0,y0,w0,h0);
}

#include "stencilbardockmanager.moc"
