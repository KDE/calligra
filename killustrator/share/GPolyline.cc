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

#include <GPolyline.h>

#include <qdom.h>
#include <qpainter.h>
#include <klocale.h>

#include <GCurve.h>
#include <Arrow.h>
#include <Painter.h>
#include <stdlib.h>

GPolyline::GPolyline () {
  connect (this, SIGNAL(propertiesChanged (GObject::Property, int)), this,
           SLOT(updateProperties (GObject::Property, int)));
  points.setAutoDelete (true);
  sArrow = (outlineInfo.startArrowId > 0 ?
            Arrow::getArrow (outlineInfo.startArrowId) : 0L);
  eArrow = (outlineInfo.endArrowId > 0 ?
            Arrow::getArrow (outlineInfo.endArrowId) : 0L);
  sAngle = eAngle = 0;
  sdx = sdy = 0;
  edx = edy = 0;
}

GPolyline::GPolyline (const QDomElement &element) : GObject (element.namedItem("gobject").toElement()) {

    connect (this, SIGNAL(propertiesChanged (GObject::Property, int)), this,
             SLOT(updateProperties (GObject::Property, int)));
    points.setAutoDelete (true);
    sArrow = eArrow = 0L;
    sAngle = eAngle = 0;

    outlineInfo.startArrowId = element.attribute("arrow1").toInt();
    outlineInfo.endArrowId = element.attribute("arrow2").toInt();
    sArrow = (outlineInfo.startArrowId > 0 ?
              Arrow::getArrow (outlineInfo.startArrowId) : 0L);
    eArrow = (outlineInfo.endArrowId > 0 ?
              Arrow::getArrow (outlineInfo.endArrowId) : 0L);

    QDomElement p = element.firstChild().toElement();
    Coord *point;
    int i=0;
    for( ; !p.isNull(); p = p.nextSibling().toElement() ) {
        if(p.tagName()=="point") {
            point=new Coord();
            point->x(p.attribute("x").toFloat());
            point->y(p.attribute("y").toFloat());
            points.insert(i, point);
            ++i;
        }
    }
    calcBoundingBox ();
}

GPolyline::GPolyline (const GPolyline& obj) : GObject (obj) {
    connect (this, SIGNAL(propertiesChanged (GObject::Property, int)), this,
             SLOT(updateProperties (GObject::Property, int)));
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

QString GPolyline::typeName () const {
  return i18n("Polyline");
}

void GPolyline::draw (QPainter& p, bool withBasePoints, bool /*outline*/) {
  unsigned int i;
  QPen pen;

  initPen (pen);
  p.save ();
  p.setPen (pen);
  p.setWorldMatrix (tmpMatrix, true);

  float w = outlineInfo.width == 0 ? 1.0 : outlineInfo.width;

  // highly inefficient - FIXME (Werner)
  unsigned int num = points.count ();
  for (i = 1; i < num; i++) {
    Painter::drawLine (p, points.at (i - 1)->x () + ((i==1) ? sdx : 0),
                       points.at (i - 1)->y () + ((i==1) ? sdy : 0),
                       points.at (i)->x () - ((i==num-1) ? edx : 0),
                       points.at (i)->y () - ((i==num-1) ? edy : 0));
  }

  p.restore ();
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
    // here too
    for (i = 0; i < num; i++) {
      Coord c = points.at (i)->transform (tmpMatrix);
      int x = (int) c.x ();
      int y = (int) c.y ();
      p.drawRect (x - 2, y - 2, 4, 4);
    }
  }
  p.restore ();
}

bool GPolyline::contains (const Coord& p) {
  return ((containingSegment (p.x (), p.y ())) != -1);
}

void GPolyline::setPoint (int idx, const Coord& p) {
  Coord np = p.transform (iMatrix);

  points.at (idx)->x (np.x ());
  points.at (idx)->y (np.y());

  updateRegion ();
}

void GPolyline::removePoint (int idx, bool update) {
  if (points.count () > 2) {
    points.remove (idx);
    if (update)
      updateRegion ();
  }
}

void GPolyline::insertPoint (int idx, const Coord& p, bool update) {
  addPoint (idx, p, update);
}

void GPolyline::addPoint (int idx, const Coord& p, bool update) {
  Coord np = p.transform (iMatrix);
  points.insert (idx, new Coord (np));

  if (update)
    updateRegion ();
}

void GPolyline::_addPoint (int idx, const Coord& p) {
  points.insert (idx, new Coord (p));
  updateRegion ();
}

const Coord& GPolyline::getPoint (int idx) {
  return *(points.at (idx));
}

QList<Coord>& GPolyline::getPoints () {
  return points;
}

void GPolyline::movePoint (int idx, float dx, float dy, bool /*ctrlPressed*/) {
  float x = points.at (idx)->x ();
  float y = points.at (idx)->y ();
  float ndx, ndy;

  ndx = dx * iMatrix.m11 () + dy * iMatrix.m21 ();
  ndy = dy * iMatrix.m22 () + dx * iMatrix.m12 ();

  points.at (idx)->x (x + ndx);
  points.at (idx)->y (y + ndy);

  updateRegion ();
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

GObject* GPolyline::copy () {
  return new GPolyline (*this);
}

GObject* GPolyline::clone (const QDomElement &element) {
  return new GPolyline (element);
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

  r.right (r.left () == r.right () ? r.left () + 1 : r.right ()/* + 2*/);
  r.bottom (r.top () == r.bottom () ? r.top () + 1 : r.bottom ()/* + 2*/);

  if (num < 2)
    return;

  float w = outlineInfo.width == 0 ? 1.0 : outlineInfo.width;

  if (sArrow != 0L) {
    Coord p1 = points.at (0)->transform (tmpMatrix);
    Coord p2 = points.at (1)->transform (tmpMatrix);
    sAngle = calcArrowAngle (p1, p2, 0);
    Rect sr = sArrow->boundingBox (p1, w, sAngle);
    r = r.unite (sr);

    float angle = calcArrowAngle (*(points.at (0)), *(points.at (1)), 0);
    angle = 1.0 / (RAD_FACTOR / angle);
    sdx = w * sArrow->length () * cos (angle);
    sdy = w * sArrow->length () * sin (angle);
  }
  else {
    sdx = sdy = 0;
  }
  if (eArrow != 0L) {
    Coord p1 = points.at (num - 2)->transform (tmpMatrix);
    Coord p2 = points.at (num - 1)->transform (tmpMatrix);
    eAngle = calcArrowAngle (p1, p2, 1);
    Rect er = eArrow->boundingBox (p2, w, eAngle);
    r = r.unite (er);

    float angle = calcArrowAngle (*(points.at (num - 2)),
                                  *(points.at (num - 1)), 0);
    angle = 1.0 / (RAD_FACTOR / angle);
    edx = w * eArrow->length () * cos (angle);
    edy = w * eArrow->length () * sin (angle);
  }
  else {
    edx = edy = 0;
  }

  updateBoundingBox (r);
}

void GPolyline::updateProperties (GObject::Property prop, int /*mask*/) {
    if (prop == GObject::Prop_Fill)
        return;
  if ((sArrow == 0L && outlineInfo.startArrowId > 0) ||
      (sArrow && sArrow->arrowID () != outlineInfo.startArrowId) ||
      (eArrow == 0L && outlineInfo.endArrowId > 0) ||
      (eArrow && eArrow->arrowID () != outlineInfo.endArrowId)) {
    sArrow = (outlineInfo.startArrowId > 0 ?
              Arrow::getArrow (outlineInfo.startArrowId) : 0L);
    eArrow = (outlineInfo.endArrowId > 0 ?
              Arrow::getArrow (outlineInfo.endArrowId) : 0L);
    updateRegion ();
  }
}

QDomElement GPolyline::writeToXml (QDomDocument &document) {

    QDomElement polyline=document.createElement("polyline");
    polyline.setAttribute ("arrow1", QString::number(outlineInfo.startArrowId));
    polyline.setAttribute ("arrow2", QString::number(outlineInfo.endArrowId));

    for (QListIterator<Coord> it (points); it.current (); ++it) {
        QDomElement point=document.createElement("point");
        point.setAttribute ("x", it.current ()->x ());
        point.setAttribute ("y", it.current ()->y ());
        polyline.appendChild(point);
    }
    polyline.appendChild(GObject::writeToXml(document));
    return polyline;
}

bool GPolyline::findNearestPoint (const Coord& p, float max_dist,
                                  float& dist, int& pidx, bool all) {
  float dx, dy, min_dist = max_dist + 1, d;
  pidx = -1;

  Coord np = p.transform (iMatrix);

  unsigned int i = 0;
  while (i < points.count ()) {
    dx = points.at (i)->x () - np.x ();
    dy = points.at (i)->y () - np.y ();
    d = sqrt (dx * dx + dy * dy);

    if (d < max_dist && d < min_dist) {
      dist = min_dist = d;
      pidx = i;
    }

    if (! all && i == 0) {
      // test only first and last point
      i = points.count () - 1;
    }
    else
      i++;
  }
  return pidx >= 0;
}

Rect GPolyline::calcEnvelope () {
  Rect r;
  unsigned int num = points.count ();

  if (num == 0)
    return r;

  const Coord& p = *points.at (0);

  r.left (p.x ());
  r.top (p.y ());
  r.right (p.x ());
  r.bottom (p.y ());

  for (unsigned int i = 1; i < num; i++) {
    const Coord& pi = *points.at (i);

    r.left (QMIN(pi.x (), r.left ()));
    r.top (QMIN(pi.y (), r.top ()));
    r.right (QMAX(pi.x (), r.right ()));
    r.bottom (QMAX(pi.y (), r.bottom ()));
  }
  return r;
}

void GPolyline::getPath (QValueList<Coord>& path) {
  unsigned int num = points.count ();
  for (unsigned int i = 0; i < num; i++) {
    const Coord& pi = *points.at (i);
    path.append(pi.transform (tMatrix));
  }
}

int GPolyline::containingSegment (float xpos, float ypos) {
  Coord p (xpos, ypos);
  Coord pp = p.transform (iMatrix);
  int seg = -1;

  float x1, x2, y1, y2, m, n, xp, yp;

  if (box.contains (p)) {
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
        if (abs (int (x1 - x2)) < 5) {
          if ((y1 <= pp.y () && pp.y () <= y2) ||
              (y2 <= pp.y () && pp.y () <= y1)) {
            seg = i - 1;
            break;
          }
        }
        else {
          // y = m * x + n;
          m = (y2 - y1) / (x2 - x1);
          n = y1 - m * x1;

          if (m > 1) {
            xp = ((float) pp.y () - n) / m;
            if (xp - 5 <= pp.x () && pp.x () <= xp + 5) {
              seg = i - 1;
              break;
            }
          }
          else {
            yp = m * pp.x () + n;

            if (yp - 5 <= pp.y () && pp.y () <= yp + 5) {
              seg = i - 1;
              break;
            }
          }
        }
      }
    }
  }
  return seg;
}

// Check for a valid polyline
//  (1) at least 2 points
//  (2) dimension greater or equal 1

bool GPolyline::isValid () {
  if (points.count () > 1) {
   const Coord& p0 = *points.at (0);
    for (unsigned int i = 1; i < points.count (); i++) {
      const Coord& p = *points.at (i);
      if (fabs (p.x () - p0.x ()) > 1 || fabs (p.y () - p0.y ()) > 1)
        return true;
    }
  }
  return false;
}

void GPolyline::removeAllPoints () {
  points.clear ();
}

bool GPolyline::splitAt (unsigned int idx, GObject*& obj1, GObject*& obj2) {
  bool result = false;

  if (idx > 0 && idx < points.count ()) {
    GPolyline* other1 = (GPolyline *) this->copy ();
    unsigned int i, num = points.count ();
    for (i = idx + 1; i < num; i++)
      other1->points.remove (idx + 1);
    other1->calcBoundingBox ();

    GPolyline* other2 = (GPolyline *) this->copy ();
    for (i = 0; i < idx; i++)
      other2->points.remove ((unsigned int) 0);
    other2->calcBoundingBox ();
    result = true;
    obj1 = other1;
    obj2 = other2;
  }
  return result;
}

GCurve* GPolyline::convertToCurve () const {
  GCurve* curve = new GCurve ();
  curve->setOutlineInfo (outlineInfo);
  QListIterator<Coord> it (points);
  Coord p0 = it.current ()->transform (tmpMatrix);
  ++it;
  for (; it.current (); ++it) {
    Coord p1 = it.current ()->transform (tmpMatrix);
    curve->addLineSegment (p0, p1);
    p0 = p1;
  }
  return curve;
}

#include <GPolyline.moc>
