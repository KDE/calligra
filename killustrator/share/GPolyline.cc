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

#include <assert.h>
#include <stdlib.h>
#include <iostream.h>
#include <math.h>
#include "GPolyline.h"
#include "GPolyline.moc"

#include <klocale.h>
#include <kapp.h>

#define RAD_FACTOR 180.0 / M_PI

GPolyline::GPolyline () {
  connect (this, SIGNAL(propertiesChanged ()), this, 
           SLOT(updateProperties ()));
  points.setAutoDelete (true);
  sArrow = eArrow = 0L;
  sAngle = eAngle = 0;
  //  outlineInfo.ckind = GObject::OutlineInfo::Custom_Line;
}

GPolyline::GPolyline (const list<XmlAttribute>& attribs) : GObject (attribs) {
  connect (this, SIGNAL(propertiesChanged ()), this, 
           SLOT(updateProperties ()));
  points.setAutoDelete (true);
  sArrow = eArrow = 0L;
  sAngle = eAngle = 0;
  //  outlineInfo.ckind = GObject::OutlineInfo::Custom_Line;

  list<XmlAttribute>::const_iterator first = attribs.begin ();
	
  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    if (attr == "arrow1")
      outlineInfo.startArrowId = (*first).intValue ();
    else if (attr == "arrow2")
      outlineInfo.endArrowId = (*first).intValue ();
    first++;
  }
  sArrow = (outlineInfo.startArrowId > 0 ? 
	    Arrow::getArrow (outlineInfo.startArrowId) : 0L);
  eArrow = (outlineInfo.endArrowId > 0 ? 
	    Arrow::getArrow (outlineInfo.endArrowId) : 0L);
  calcBoundingBox ();
}
  
GPolyline::GPolyline (const GPolyline& obj) : GObject (obj) {
  connect (this, SIGNAL(propertiesChanged ()), this, 
           SLOT(updateProperties ()));
  points.setAutoDelete (true);
  QListIterator<Coord> it (obj.points);
  for (; it.current (); ++it) 
    points.append (new Coord (* (it.current ())));
  sArrow = obj.sArrow;
  eArrow = obj.eArrow;
  calcBoundingBox ();
}
  
float GPolyline::calcArrowAngle (const Coord& p1, const Coord& p2, 
				 int direction) {
  float angle = 0.0;

  if (p1.x () == p2.x ()) {
    if (p1.y () < p2.y ())
      angle = (direction == 0 ? 270.0 : 90);
    else
      angle = (direction == 0 ? 90.0 : 270);
  }
  else {
    float x1, x2, y1, y2;

    if (p1.x () <= p2.x ()) {
      x1 = p1.x (); y1 = p1.y ();
      x2 = p2.x (); y2 = p2.y ();
    }
    else {
      x2 = p1.x (); y2 = p1.y ();
      x1 = p2.x (); y1 = p2.y ();
    }
    float m = -(y2 - y1) / (x2 - x1);
    angle = atan (m) * RAD_FACTOR;
    if (p1.x () < p2.x ()) {
      if (direction == 0)
	angle = 180.0 - angle;
      else
	angle = -angle;
    }
    else {
      if (direction == 1)
	angle = 180.0 - angle;
      else
	angle = -angle;
    }
  }
  return angle;
}

const char* GPolyline::typeName () {
  return i18n ("Polyline");
}

void GPolyline::draw (Painter& p, bool withBasePoints) {
  unsigned int i;
  QPen pen (outlineInfo.color, (uint) outlineInfo.width, 
            outlineInfo.style);
  p.save ();
  p.setPen (pen);
  p.setWorldMatrix (tmpMatrix, true);

  unsigned int num = points.count ();
  for (i = 1; i < num; i++) {
    p.drawLine (points.at (i - 1)->x (), points.at (i - 1)->y (),
		points.at (i)->x (), points.at (i)->y ());
  }
  p.restore ();
  float w = outlineInfo.width == 0 ? 1.0 : outlineInfo.width;
  if (sArrow != 0L) {
    Coord p1 = points.at (0)->transform (tmpMatrix);
    sArrow->draw (p, p1, outlineInfo.color, w, sAngle);
  }
  if (eArrow != 0L) {
    Coord p2 = points.at (num - 1)->transform (tmpMatrix);
    eArrow->draw (p, p2, outlineInfo.color, w, eAngle);
  }
  p.save ();
  if (withBasePoints) {
    p.setPen (black);
    p.setBrush (white);
    for (i = 0; i < num; i++) {
      Coord c = points.at (i)->transform (tmpMatrix);
      int x = (int) c.x ();
      int y = (int) c.y ();
      p.drawRect (x - 2, y - 2, 4, 4);
    }
  }
  p.restore ();
}

void GPolyline::writeToPS (ostream& os) {
  GObject::writeToPS (os);
  os << '[';
  for (int i = points.count () - 1; i >= 0; i--) {
    Coord* c = points.at (i);
    os << ' ' << c->x () << ' ' << c->y ();
  }
  os << "] DrawPolyline\n";
  float w = outlineInfo.width == 0 ? 1.0 : outlineInfo.width;
  if (sArrow != 0L) {
    Coord p1 = points.at (0)->transform (tmpMatrix);
    sArrow->writeToPS (os, p1, outlineInfo.color, w, sAngle);
  }
  if (eArrow != 0L) {
    Coord p2 = points.at (points.count () - 1)->transform (tmpMatrix);
    eArrow->writeToPS (os, p2, outlineInfo.color, w, eAngle);
  }
}

bool GPolyline::contains (const Coord& p) {
  float x1, x2, y1, y2, m, n, yp;

  if (box.contains (p)) {
    QWMatrix mi = tMatrix.invert ();
    QPoint pp = mi.map (QPoint ((int) p.x (), (int) p.y ()));

    for (unsigned int i = 1; i < points.count (); i++) {
      x1 = points.at (i - 1)->x ();
      x2 = points.at (i)->x ();
      if (x2 <= x1) {
        // swap the points
        x2 = x1;
        x1 = points.at (i)->x ();
        y2 = points.at (i - 1)->y ();
        y1 = points.at (i)->y ();
      }
      else {
        y1 = points.at (i - 1)->y ();
        y2 = points.at (i)->y ();
      }

      if (x1 - 3 <= pp.x () && pp.x () <= x2 + 3) {
        if (x1 == x2) {
          if ((y1 <= pp.y () && pp.y () <= y2) ||
              (y2 <= pp.y () && pp.y () <= y1))
          return true;
        }
        else {
          // y = m * x + n;
          m = (y2 - y1) / (x2 - x1);
          n = y1 - m * x1;
          yp = m * (float) pp.x () + n;

          if (yp - 3 <= pp.y () && pp.y () <= yp + 3) 
            return true;
        }
      }
    }
  }
  return false;
}

void GPolyline::setPoint (int idx, const Coord& p) {
  QWMatrix mi = tMatrix.invert ();
  Coord np = p.transform (mi);

  points.at (idx)->x (np.x ());
  points.at (idx)->y (np.y());
  calcBoundingBox ();
  emit changed ();
}

void GPolyline::addPoint (int idx, const Coord& p, bool update) {
  QWMatrix mi = tMatrix.invert ();
  Coord np = p.transform (mi);

  points.insert (idx, new Coord (np));

  if (update) {
    calcBoundingBox ();
    emit changed ();
  }
}

void GPolyline::_addPoint (int idx, const Coord& p) {
  points.insert (idx, new Coord (p));
  calcBoundingBox ();
  emit changed ();
}

const Coord& GPolyline::getPoint (int idx) {
  return *(points.at (idx));
}

QList<Coord>& GPolyline::getPoints () {
  return points;
}

void GPolyline::movePoint (int idx, float dx, float dy) {
  float x = points.at (idx)->x ();
  float y = points.at (idx)->y ();
  float ndx, ndy;

  QWMatrix mi = tMatrix.invert ();
  ndx = dx * mi.m11 () + dy * mi.m21 ();
  ndy = dy * mi.m22 () + dx * mi.m12 ();

  points.at (idx)->x (x + ndx);
  points.at (idx)->y (y + ndy);
  calcBoundingBox ();
  emit changed ();
}


unsigned int GPolyline::numOfPoints () const {
  return points.count ();
}

int GPolyline::getNeighbourPoint (const Coord& p) {
  Coord c;

  // for speedup in add line segments check first point #0 and #num-1
  c = points.at (0)->transform (tMatrix);
  if (c.isNear (p, NEAR_DISTANCE))
    return 0;
  unsigned int last = points.count () - 1;
  c = points.at (last)->transform (tMatrix);
  if (c.isNear (p, NEAR_DISTANCE))
    return last;

  // and now the remaining
  for (unsigned int i = 1; i < last; i++) { 
    c = points.at (i)->transform (tMatrix);
    if (c.isNear (p, NEAR_DISTANCE))
      return i;
  }
  return -1;
}

void GPolyline::removePoint (int idx, bool update) {
  points.remove (idx);
  if (update) {
    calcBoundingBox ();
    emit changed ();
  }
}

GObject* GPolyline::copy () {
  return new GPolyline (*this);
}

void GPolyline::calcBoundingBox () {
  Rect r;
  unsigned int num = points.count ();

  if (num == 0)
    return;
  
  Coord p = points.at (0)->transform (tmpMatrix);
  
  r.left (p.x ());
  r.top (p.y ());
  r.right (p.x ());
  r.bottom (p.y ());

  for (unsigned int i = 1; i < num; i++) {
    Coord p = points.at (i)->transform (tmpMatrix);

    r.left (QMIN(p.x (), r.left ()));
    r.top (QMIN(p.y (), r.top ()));
    r.right (QMAX(p.x (), r.right ()));
    r.bottom (QMAX(p.y (), r.bottom ()));
  }

  if (num < 2)
    return;

  if (sArrow != 0L) {
    Coord p1 = points.at (0)->transform (tmpMatrix);
    Coord p2 = points.at (1)->transform (tmpMatrix);
    sAngle = calcArrowAngle (p1, p2, 0);
  }
  if (eArrow != 0L) {
    Coord p1 = points.at (num - 2)->transform (tmpMatrix);
    Coord p2 = points.at (num - 1)->transform (tmpMatrix);
    eAngle = calcArrowAngle (p1, p2, 1);
  }

  updateBoundingBox (r);
}

void GPolyline::updateProperties () {
  //  if (outlineInfo.ckind != GObject::OutlineInfo::Custom_Line)
  //    return;

  if ((sArrow == 0L && outlineInfo.startArrowId > 0) ||
      (sArrow && sArrow->arrowID () != outlineInfo.startArrowId) ||
      (eArrow == 0L && outlineInfo.endArrowId > 0) ||
      (eArrow && eArrow->arrowID () != outlineInfo.endArrowId)) {
    sArrow = (outlineInfo.startArrowId > 0 ? 
	      Arrow::getArrow (outlineInfo.startArrowId) : 0L);
    eArrow = (outlineInfo.endArrowId > 0 ? 
	      Arrow::getArrow (outlineInfo.endArrowId) : 0L);
    calcBoundingBox (); // for computing angles of arrows
    emit changed ();
  }
}

void GPolyline::writeToXml (XmlWriter& xml) {
  xml.startTag ("polyline", false);
  writePropertiesToXml (xml);
  xml.addAttribute ("arrow1", outlineInfo.startArrowId);
  xml.addAttribute ("arrow2", outlineInfo.endArrowId);
  xml.closeTag (false);

  for (QListIterator<Coord> it (points); it.current (); ++it) {
    xml.startTag ("point", false);
    xml.addAttribute ("x", it.current ()->x ());
    xml.addAttribute ("y", it.current ()->y ());
    xml.closeTag (true);
  }
  xml.endTag ();
}
