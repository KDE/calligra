/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#include "stencilbarmovemanager.h"

#include <qcursor.h>
#include <qapplication.h>
#include <qwindowdefs.h>
#include <qtimer.h>
#include <qrect.h>
#include <qframe.h>
#include <qpainter.h>

StencilBarMoveManager::StencilBarMoveManager()
: QObject()
{
  XGCValues gv;

  working=false;
  noLast=true;

  scr = qt_xscreen();
  root = qt_xrootwin();

  gv.function = GXxor;
  gv.line_width = 2;
  gv.foreground = WhitePixel(qt_xdisplay(), scr)^BlackPixel(qt_xdisplay(), scr);
  gv.subwindow_mode = IncludeInferiors;
  long mask = GCForeground | GCFunction | GCLineWidth | GCSubwindowMode;
  rootgc = XCreateGC(qt_xdisplay(), qt_xrootwin(), mask, &gv);

  timer = new QTimer(this);
}

StencilBarMoveManager::~StencilBarMoveManager()
{
  stop();
}

void StencilBarMoveManager::doMove( QWidget* _w )
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

void StencilBarMoveManager::doMoveInternal()
{
  if ( !working )
    return;

  if (!pauseMoveX)
    rx = QCursor::pos().x();

  if (!pauseMoveY)
    ry = QCursor::pos().y();

  xp = rx-offX;
  yp = ry-offY;

  emit positionChanged();

  if (check(xp, yp, w, h)) {
    paintProcess(false,xp, yp, w, h);

    XFlush(qt_xdisplay());
    XSync(qt_xdisplay(),false);
  }
}

void StencilBarMoveManager::stop()
{
  if (!working)
    return;

  timer->stop();
  QApplication::restoreOverrideCursor();

  paintProcess();
  XFlush(qt_xdisplay());

  working = false;
}

void StencilBarMoveManager::setGeometry( const QRect& r )
{
  setGeometry(r.x(),r.y(),r.width(),r.height());
}

void StencilBarMoveManager::setGeometry(int _x, int _y, int _w, int _h)
{
  xp=_x;
  yp=_y;
  w=_w;
  h=_h;

  check(_x, _y, _w, _h, true);
  paintProcess(false,_x, _y, _w, _h);

  XFlush(qt_xdisplay());
  XSync(qt_xdisplay(),false);
}

void StencilBarMoveManager::drawRectangle( int _x, int _y, int _w, int _h)
{
  if (!noLast)
    return;

  ox = _x;
  oy = _y;
  ow = _w;
  oh = _h;

  XDrawRectangle(qt_xdisplay(), root, rootgc, _x, _y, _w, _h);
  noLast = false;
}

void StencilBarMoveManager::paintProcess( bool onlyDeelete, int _x, int _y, int _w, int _h )
{
  if (noLast && onlyDeelete)
    return;

  if ( ox == _x && oy == _y && ow ==_w && oh == _h )
    return;

  XDrawRectangle(qt_xdisplay(), root, rootgc, ox, oy, ow, oh);
  noLast = true;

  drawRectangle(_x,_y,_w,_h);
}

void StencilBarMoveManager::movePause( bool horizontal, bool vertical )
{
  pauseMoveX = horizontal;
  pauseMoveY = vertical;
}

void StencilBarMoveManager::moveContinue()
{
  pauseMoveX = false;
  pauseMoveY = false;
}

void StencilBarMoveManager::doXResize( QWidget* w, bool mirror )
{
  if (working)
    return;

  mirrorX = mirror;
  mirrorY = false;

  yOnly = false;
  xOnly = true;

  doResize(w);
}

void StencilBarMoveManager::doYResize( QWidget* w, bool mirror )
{
  if (working)
    return;

  mirrorX = false;
  mirrorY = mirror;

  yOnly = true;
  xOnly = false;

  doResize(w);
}

void StencilBarMoveManager::doXYResize( QWidget* w, bool _mirrorX, bool _mirrorY )
{
  if (working)
    return;

  mirrorX = _mirrorX;
  mirrorY = _mirrorY;

  yOnly = false;
  xOnly = false;

  doResize(w);
}

void StencilBarMoveManager::doResizeInternal()
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

    XFlush(qt_xdisplay());
    XSync(qt_xdisplay(),false);
  }
}

void StencilBarMoveManager::setWidget( QWidget* _w )
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

void StencilBarMoveManager::doResize( QWidget* _w )
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

bool StencilBarMoveManager::check(int& x, int& y, int& w, int& h, bool change)
{
  int w1 = QMIN(QMAX(minSize.width(),w),maxSize.width());
  int h1 = QMIN(QMAX(minSize.height(),h),maxSize.height());

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

QRect StencilBarMoveManager::geometry()
{
  int x0 = xp;
  int y0 = yp;
  int w0 = w;
  int h0 = h;
  check(x0,y0,w0,h0,true);

  return QRect(x0,y0,w0,h0);
}
#include "stencilbarmovemanager.moc"
