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
#ifndef STENCILBARMOVEMANAGER_H
#define STENCILBARMOVEMANAGER_H

#include <qobject.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class QTimer;
class QWidget;

class StencilBarMoveManager: public QObject
{ Q_OBJECT
public:
  StencilBarMoveManager();
  ~StencilBarMoveManager();

  bool isWorking() { return working; }

  void doXResize( QWidget*, bool mirror );
  void doYResize( QWidget*, bool mirror );
  void doXYResize( QWidget*, bool _mirrorX, bool _mirrorY );

  void doMove( QWidget* );
  void movePause( bool horizontal = true, bool vertical = true );
  void moveContinue();

  void setGeometry( const QRect& r );
  void setGeometry(int x, int y, int w, int h);

  QRect geometry();

  void resize( const QSize& s ) {setGeometry(xp, yp, s.width(), s.height()); }
  void resize(int rw, int rh) {setGeometry(xp, yp, rw, rh); }
  void move( int rx, int ry) {setGeometry(rx, ry, w, h); }
  void move( const QPoint& p ) {setGeometry(p.x(), p.y(), w, h); }

  int x() {return xp;}
  int y() {return yp;}
  int width() {return w;}
  int height() {return h;}

  bool isXMirror(){ return mirrorX; }
  bool isYMirror(){ return mirrorY; }

  void stop ();
  QWidget* getWidget(){ return widget; }

signals:
  void positionChanged();
  void sizeChanged();
  void fixPosition(int& x, int& y, int& w, int& h);
  void fixSize(int& x, int& y, int& w, int& h);

protected:
  void doResize( QWidget* );
  void setWidget( QWidget* );
  void drawRectangle (int x, int y, int w, int h);
  void paintProcess( bool onlyDeelete = true, int _x = 0, int _y = 0, int _w = 0, int _h = 0 );
  bool check(int& x, int& y, int& w, int& h, bool change =false);

private slots:
  void doMoveInternal();
  void doResizeInternal();

private:
  bool pauseMoveX;
  bool pauseMoveY;
  int xp, yp, w, h;
  int ox, oy, ow, oh;
  int orig_x, orig_y, orig_w, orig_h;
  bool noLast;
  bool working;

  QSize maxSize;
  QSize minSize;
  QWidget* widget;
  QTimer* timer;

  int rx, ry, sx, sy;
  int offX, offY;

  /* X-stuff */
  Window root;
  GC rootgc;
  int scr;
  XEvent ev;

  bool mirrorX;
  bool mirrorY;
  bool xOnly;
  bool yOnly;

  bool isDoMove;
  QRect rr;
  QPoint p;
};

#endif

