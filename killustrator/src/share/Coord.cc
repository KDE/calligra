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

#include <qglobal.h>
#include "Coord.h"

Coord Coord::transform (const QWMatrix& m) const {
  float x, y;

  m.map (x_, y_, &x, &y);
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
