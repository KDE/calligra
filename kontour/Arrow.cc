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

#include <Arrow.h>

#include <qpixmap.h>
#include <qpainter.h>
#include <qcolor.h>

#include <kstaticdeleter.h>

#include <Painter.h>

#if 0
static QCOORD sysArrow_1[] = { -5, -3, 10, 0, -5, 3 };
static QCOORD sysArrow_2[] = { -5, -3, 10, 0, -5, 3, 0, 0, -5, -3 };
static QCOORD sysArrow_3[] = { -2, -2, 4, 0, -2, 2 };
#else
// Ben Skelton <skeltobc@elec.canterbury.ac.nz> has proposed this:
// make the first point the point where the line meets the arrow head
static QCOORD sysArrow_1[] = { -15, 0, -15, -3, 0, 0, -15, 3 };
static QCOORD sysArrow_2[] = { -10, 0, -15, -3, 0, 0, -15, 3 };
static QCOORD sysArrow_3[] = { -6, 0, -6, -2, 0, 0, -6, 2 };
// and some more arrows
static QCOORD sysArrow_4[] = { -6, 0, -6, -3, 0, -3, 0, 3, -6, 3};
static QCOORD sysArrow_5[] = { -10, 0, -10, -2, 0, 0, -10, 2 };
static QCOORD sysArrow_6[] = { -15, 0, -10, -3, 0, 0, -10, 3 };
static QCOORD sysArrow_7[] = { -10, 0, -10, -4, -1, 0, -1, -6, 0, -6,
                               0, 6, -1, 6, -1, 0, -10, 4 };
#endif

QIntDict<Arrow> *Arrow::arrows=0L;
namespace KIlluDeleter {
static KStaticDeleter< QIntDict<Arrow> > sd;
};

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
    QPainter p;
    p.begin (lpreview);
    p.scale (2, 2);
    p.drawLine (5, 5, 35, 5);
    //    draw (p, Coord (10, 5), black, 1, 180);
    draw (p, Coord (5, 5), Qt::black, 1, 180);
    p.end ();
  }
  return *lpreview;
}

QPixmap& Arrow::rightPixmap () {
  if (rpreview == 0L) {
    rpreview = new QPixmap (50, 20);
    rpreview->fill ();
    QPainter p;
    p.begin (rpreview);
    p.scale (2, 2);
    //    p.drawLine (0, 5, 10, 5);
    //    draw (p, Coord (10, 5), black, 1, 0);
    p.drawLine (0, 5, 20, 5);
    draw (p, Coord (20, 5), Qt::black, 1, 0);
    p.end ();
  }
  return *rpreview;
}

void Arrow::draw (QPainter& p, const Coord& c, const QColor& color,
                  float width, float angle) {
  p.save ();
  p.translate (c.x (), c.y ());
  p.rotate (angle);
  if (width == 0)
    width = 1.0;
  p.scale (width, width);
  //p.scale (1.0, width);
  if (fill)
    p.setBrush (color);
  else
    p.setBrush (Qt::white);
  p.setPen (color);
  p.drawPolygon (points);
  p.restore ();
}

Rect Arrow::boundingBox (const Coord& c, float width, float angle) {
  Rect box, r;

  QPoint p = points.at (0);
  r.left (p.x ());
  r.top (p.y ());
  r.right (p.x ());
  r.bottom (p.y ());

  for (unsigned int i = 1; i < points.size (); i++) {
    p = points.at (i);

    r.left (QMIN(p.x (), r.left ()));
    r.top (QMIN(p.y (), r.top ()));
    r.right (QMAX(p.x (), r.right ()));
    r.bottom (QMAX(p.y (), r.bottom ()));
  }

  QWMatrix m;
  if (width == 0)
    width = 1.0;
  m.translate (c.x (), c.y ());
  m.rotate (angle);
  m.scale (width, width);
  box = r.transform (m);
  return box;
}

void Arrow::install (Arrow* arrow) {
    if(arrows==0L)
        arrows=KIlluDeleter::sd.setObject(new QIntDict<Arrow>);
    arrows->insert (arrow->arrowID (), arrow);
}

Arrow* Arrow::getArrow (long id) {
    if(arrows==0L)
        arrows=KIlluDeleter::sd.setObject(new QIntDict<Arrow>);
    if (arrows->isEmpty ())
        Arrow::initialize ();
    return arrows->find (id);
}

QIntDictIterator<Arrow> Arrow::getArrows () {
    if(arrows==0L)
        arrows=KIlluDeleter::sd.setObject(new QIntDict<Arrow>);
    if (arrows->isEmpty ())
        Arrow::initialize ();

    return QIntDictIterator<Arrow> (*arrows);
}

int Arrow::length () {
  return points.point(0).x();
}

void Arrow::initialize () {
  // initialize system arrows
  Arrow::install (new Arrow (1, 4, sysArrow_1));
  Arrow::install (new Arrow (2, 4, sysArrow_5));
  Arrow::install (new Arrow (3, 4, sysArrow_2));
  Arrow::install (new Arrow (4, 4, sysArrow_3));
  Arrow::install (new Arrow (5, 5, sysArrow_4));
  Arrow::install (new Arrow (6, 4, sysArrow_6));
  Arrow::install (new Arrow (7, 9, sysArrow_7));
  Arrow::install (new Arrow (8, 4, sysArrow_1, false));
  Arrow::install (new Arrow (9, 4, sysArrow_5, false));
  Arrow::install (new Arrow (10, 4, sysArrow_2, false));
  Arrow::install (new Arrow (11, 4, sysArrow_3, false));
  Arrow::install (new Arrow (12, 5, sysArrow_4, false));
  Arrow::install (new Arrow (13, 4, sysArrow_6, false));
}
