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

#include <qcolor.h>
#include "Arrow.h"

static QCOORD sysArrow_1[] = { -5, -3, 10, 0, -5, 3 };
static QCOORD sysArrow_2[] = { -5, -3, 10, 0, -5, 3, 0, 0, -5, -3 };
static QCOORD sysArrow_3[] = { -2, -2, 4, 0, -2, 2 };

QIntDict<Arrow> Arrow::arrows;

Arrow::Arrow (long aid, int npts, const QCOORD* pts, bool fillIt) : 
  points (npts, pts) {
  id = aid;
  lpreview = 0L;
  rpreview = 0L;
  fill = fillIt;
}

Arrow::~Arrow () {
  if (lpreview)
    delete lpreview;
  if (rpreview)
    delete rpreview;
}

long Arrow::arrowID () const {
  return id;
}

QPixmap& Arrow::leftPixmap () {
  if (lpreview == 0L) {
    lpreview = new QPixmap (50, 20);
    lpreview->fill ();
    Painter p;
    p.begin (lpreview);
    p.scale (2, 2);
    p.drawLine (5, 5, 35, 5);
    draw (p, Coord (10, 5), black, 1, 180);
    p.end ();
  }
  return *lpreview;
}

QPixmap& Arrow::rightPixmap () {
  if (rpreview == 0L) {
    rpreview = new QPixmap (50, 20);
    rpreview->fill ();
    Painter p;
    p.begin (rpreview);
    p.scale (2, 2);
    p.drawLine (0, 5, 10, 5);
    draw (p, Coord (10, 5), black, 1, 0);
    p.end ();
  }
  return *rpreview;
}

void Arrow::draw (Painter& p, const Coord& c, const QColor& color,
		  float width, float angle) {
  p.save ();
  p.translate ((int) c.x (), (int) c.y ());
  p.rotate (angle);
  p.scale (width, width);
  if (fill)
    p.setBrush (color);
  else
    p.setBrush (white);
  p.drawPolygon (points);
  p.restore ();
}

void Arrow::install (Arrow* arrow) {
  arrows.insert (arrow->arrowID (), arrow);
}

Arrow* Arrow::getArrow (long id) {
  if (arrows.isEmpty ())
    Arrow::initialize ();
  return arrows.find (id);
}

QIntDictIterator<Arrow> Arrow::getArrows () {
  if (arrows.isEmpty ())
    Arrow::initialize ();
  
  return QIntDictIterator<Arrow> (arrows);
}

void Arrow::initialize () {
  // initialize system arrows
  Arrow::install (new Arrow (1, 3, sysArrow_1));
  Arrow::install (new Arrow (2, 5, sysArrow_2));
  Arrow::install (new Arrow (3, 3, sysArrow_3));
  Arrow::install (new Arrow (4, 3, sysArrow_1, false));
  Arrow::install (new Arrow (5, 3, sysArrow_3, false));
}
