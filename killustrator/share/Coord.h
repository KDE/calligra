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

#ifndef Coord_h_
#define Coord_h_

class QDataStream;
class QWMatrix;

class Coord {
public:
  Coord () : x_ (0), y_ (0) {}
  Coord (float x, float y) : x_ (x), y_ (y) {}
  Coord (const Coord& c) : x_ (c.x_), y_ (c.y_) {}

  void x (float f) { x_ = f; }
  void y (float f) { y_ = f; }
  float x () const { return x_; }
  float y () const { return y_; }

  Coord& operator= (const Coord& c) {
    x_ = c.x_; y_ = c.y_;
    return *this;
  }

  bool operator== (const Coord& c) const {
    return x_ == c.x_ && y_ == c.y_;
  }

  bool operator!= (const Coord& c) const {
    return !(*this == c);
  }

  Coord transform (const QWMatrix& m) const;
  void translate (float dx, float dy);

  bool isNear (const Coord& p, int range) const;

private:
  float x_, y_;
};

QDataStream& operator<< (QDataStream&, const Coord&);
QDataStream& operator>> (QDataStream&, Coord&);

class Rect {
public:
  Rect () : x1_ (0), y1_ (0), x2_ (0), y2_ (0) {}
  Rect (const Coord& p1, const Coord& p2) :
    x1_ (p1.x ()), y1_ (p1.y ()), x2_ (p2.x ()), y2_ (p2.y ()) {}
  Rect (float l, float t, float w, float h) :
    x1_ (l), y1_ (t), x2_ (l + w), y2_ (t + h) {}
  Rect (const Rect& r) :
    x1_ (r.x1_), y1_ (r.y1_), x2_ (r.x2_), y2_ (r.y2_) {}

  float left () const { return x1_; }
  float top () const { return y1_; }
  float right () const { return x2_; }
  float bottom () const { return y2_; }
  float x () const { return x1_; }
  float y () const { return y1_; }
  float width () const { return x2_ - x1_; }
  float height () const { return y2_ - y1_; }

  void left (float f) { x1_ = f; }
  void top (float f) { y1_ = f; }
  void right (float f) { x2_ = f; }
  void bottom (float f) { y2_ = f; }

  bool isValid () const { return x1_ <= x2_ && y1_ <= y2_; }

  Rect& operator= (const Rect& r);
  Rect normalize () const;
  Rect unite (const Rect& r) const;
  bool contains (const Coord& p) const;
  bool contains (const Rect& r) const;

  Rect transform (const QWMatrix& m) const;
  Rect translate (float dx, float dy) const;
  bool intersects (const Rect& r) const;

  bool empty () const { return x1_ == 0 && x2_ == 0 && y1_ == 0 && y2_ == 0; }

  Coord topLeft () const { return Coord (x1_, y1_); }
  Coord bottomRight () const { return Coord (x2_, y2_); }
  Coord topRight () const { return Coord (x2_, y1_); }
  Coord bottomLeft () const { return Coord (x1_, y2_); }

  Coord center () const { return Coord ((x1_ + x2_) / 2, (y1_ + y2_) / 2); }

  void enlarge (float v);

  bool operator== (const Rect& r) const;

private:
  float x1_, y1_, x2_, y2_;
};

#endif
