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

#ifndef Painter_h_
#define Painter_h_

#include <qlist.h>

class QPainter;
class Coord;

class Painter {
public:
  static void drawLine (QPainter& p, float x1, float y1, float x2, float y2);
  static void drawRect (QPainter& p, float x, float y, float w, float h);
  static void drawEllipse (QPainter& p, float x, float y, float w, float h);
  static void drawArc (QPainter& p, float x, float y, float w, float h,
                       float a, float alen);
  static void drawPie (QPainter& p, float x, float y, float w, float h,
                       float a, float alen);
  static void drawBezier (QPainter& p, QList<Coord>& points, int index);

private:
  static inline int round (float f) {
    return f > 0.0 ? int (f + 0.5) : int (f - 0.5);
  }

};

#endif
