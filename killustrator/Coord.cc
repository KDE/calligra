/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <qdatastream.h>
#include <qwmatrix.h>

#include <Coord.h>

Coord Coord::transform (const QWMatrix& m) const {

    double x, y;
    m.map ((double) x_, (double) y_, &x, &y);
    return Coord (x, y);
}

void Coord::translate (float dx, float dy) {
  x_ += dx;
  y_ += dy;
}

bool Coord::isNear (const Coord& p, int range) const {
  return (p.x () >= x_ - range && p.x () <= x_ + range &&
          p.y () >= y_ - range && p.y () <= y_ + range);

}

QDataStream& operator<< (QDataStream& s, const Coord& c) {
  return s << c.x () << c.y ();
}

QDataStream& operator>> (QDataStream& s, Coord& c) {
  float x, y;

  s >> x >> y;
  c.x (x); c.y (y);
  return s;
}

Rect& Rect::operator= (const Rect& r) {
  x1_ = r.x1_; y1_ = r.y1_;
  x2_ = r.x2_; y2_ = r.y2_;
  return *this;
}

Rect Rect::normalize () const {
  Rect r;
  if (x2_ < x1_) {
    r.x1_ = x2_; r.x2_ = x1_;
  }
  else {
    r.x1_ = x1_; r.x2_ = x2_;
  }
  if (y2_ < y1_) {
    r.y1_ = y2_; r.y2_ = y1_;
  }
  else {
    r.y1_ = y1_; r.y2_ = y2_;
  }
  return r;
}

Rect Rect::unite (const Rect& r) const {
  if (isValid ()) {
    if (r.isValid ()) {
      Rect tmp;
      tmp.x1_ = QMIN (x1_, r.x1_);
      tmp.x2_ = QMAX (x2_, r.x2_);
      tmp.y1_ = QMIN (y1_, r.y1_);
      tmp.y2_ = QMAX (y2_, r.y2_);
      return tmp;
    }
    else
      return *this;
  }
  else
    return r;
}

bool Rect::contains (const Coord& p) const {
  return p.x () >= x1_ && p.x () <= x2_ && p.y () >= y1_ && p.y () <= y2_;
}

bool Rect::contains (const Rect& r) const {
  return r.x1_ >= x1_ && r.x2_ <= x2_ && r.y1_ >= y1_ && r.y2_ <= y2_;
}

bool Rect::intersects (const Rect& r) const {
  float x1, x2, y1, y2;
  x1 = QMAX(x1_, r.x1_);
  y1 = QMAX(y1_, r.y1_);
  x2 = QMIN(x2_, r.x2_);
  y2 = QMIN(y2_, r.y2_);
  return (x2 > x1 && y2 > y1);
}

void Rect::enlarge (float v) {
  x1_ -= v; y1_ -= v;
  x2_ += v; y2_ += v;
}

Rect Rect::transform (const QWMatrix& m) const {
  Rect result;
  if (m.m12 () == 0.0F && m.m21 () == 0.0F) {
    result = Rect (topLeft ().transform (m), bottomRight ().transform (m));
  }
  else {
    int i;
    Coord p[4] = { Coord (x1_, y1_), Coord (x1_, y2_),
                   Coord (x2_, y2_), Coord (x2_, y1_) };
    for (i = 0; i < 4; i++)
      p[i] = p[i].transform (m);

    result.left (p[0].x ());
    result.top (p[0].y ());
    result.right (p[0].x ());
    result.bottom (p[0].y ());

    for (int i = 1; i < 4; i++) {
      result.left (QMIN(p[i].x (), result.left ()));
      result.top (QMIN(p[i].y (), result.top ()));
      result.right (QMAX(p[i].x (), result.right ()));
      result.bottom (QMAX(p[i].y (), result.bottom ()));
    }
  }
  return result;
}

Rect Rect::translate (float dx, float dy) const {
  return Rect (left () + dx, top () + dy, width (), height ());
}

bool Rect::operator== (const Rect& r) const {
  return (x1_ == r.x1_ && x2_ == r.x2_ && y1_ == r.y1_ && y2_ == r.y2_);
}
