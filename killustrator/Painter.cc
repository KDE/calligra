/* -*- C++ -*-

  $Id$
  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
                     Steffen Thorhauer (thorhaue@iti.cs.uni-magdeburg.de)
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as
  published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <Painter.h>
#include <qpainter.h>
#include <Coord.h>

#define DELTA 0.05

void Painter::drawLine (QPainter& p, float x1, float y1, float x2, float y2) {
  p.drawLine (round (x1), round (y1), round (x2), round (y2));
}

void Painter::drawRect (QPainter& p, float x, float y, float w, float h) {
  p.drawRect (round (x), round (y), round (w), round (h));
}

void Painter::drawEllipse (QPainter& p, float x, float y, float w, float h) {
  p.drawEllipse (round (x), round (y), round (w), round (h));
}

void Painter::drawArc (QPainter& p, float x, float y, float w, float h,
                       float a, float alen) {
  p.drawArc (round (x), round (y), round (w), round (h),
                     round (a), round (alen));
}

void Painter::drawPie (QPainter& p, float x, float y, float w, float h,
                       float a, float alen) {
  p.drawPie (round (x), round (y), round (w), round (h),
             round (a), round (alen));
}

void Painter::drawBezier (QPainter& p, QList<Coord>& points, int index) {
    float th,th2,th3;
    float t,t2,t3;
    int x0,y0,x1,y1;
    int i0=index;
    int i1=index+1;
    int i2=index+2;
    int i3=index+3;

      x1 = (int)points.at (i0)->x();
      y1 = (int)points.at (i0)->y();
      for(t = 0; t < 1.01; t += DELTA) {
        x0 = x1;
        y0 = y1;
        th = 1 - t;
        t2= t*t;
        t3 = t2*t;
        th2 = th*th;
        th3 = th2*th;
        x1 = (int) (th3* +(int)points.at (i0)->x() +
           3*t*th2*(int)points.at (i1)->x() +
           3*t2*th*(int)points.at (i2)->x() +
           t3*(int)points.at (i3)->x());
       y1 = (int) (th3* +(int)points.at (i0)->y() +
           3*t*th2*(int)points.at (i1)->y() +
           3*t2*th*(int)points.at (i2)->y() +
           t3*(int)points.at (i3)->y());
        p.drawLine(x0, y0, x1, y1);
      }
}
