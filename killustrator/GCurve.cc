/* -*- C++ -*-

  $Id$

  This file is part of KIllustrator.
  Copyright (C) 1998-99 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)

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

#include <qdom.h>
#include <qpainter.h>

#include <klocale.h>

#include <GCurve.h>
#include <GBezier.h>
#include <Painter.h>
#include "GDocument.h"

static Coord computePoint (int idx, const GSegment& s1, const GSegment& s2) {
  // s1 == Line, s2 == Bezier
  float xp, yp;

  //float llen = s1.length ();
  float blen = s2.length ();
  float slen = 0;
  for (int i = 0; i < idx; i++) {
     slen += seg_length (s2.pointAt (i), s2.pointAt (i + 1));
  }
  float dx = s1.pointAt (1).x () - s1.pointAt (0).x ();
  float dy = s1.pointAt (1).y () - s1.pointAt (0).y ();
  xp = slen / blen * dx + s1.pointAt (0).x ();
  yp = slen / blen * dy + s1.pointAt (0).y ();

  return Coord (xp, yp);
}

static Coord blendPoints (const Coord& p1, const Coord& p2,
                          int step, int num) {
  float dx = ((p2.x () - p1.x ()) / (num + 1.0)) * (step + 1.0);
  float dy = ((p2.y () - p1.y ()) / (num + 1.0)) * (step + 1.0);
  return Coord (p1.x () + dx, p1.y () + dy);
}

static GSegment blendSegments (const GSegment& s1, const GSegment& s2,
                               int step, int num) {
  GSegment::Kind kind = GSegment::sk_Bezier;
  if (s1.kind () == GSegment::sk_Line && s2.kind () == GSegment::sk_Line)
    kind = GSegment::sk_Line;
  GSegment seg (kind);
  if (kind == GSegment::sk_Line) {
    seg.setPoint (0, blendPoints (s1.pointAt (0), s2.pointAt (0), step, num));
    seg.setPoint (1, blendPoints (s1.pointAt (1), s2.pointAt (1), step, num));
  }
  else {
    if (s1.kind () == GSegment::sk_Line) {
      GSegment snew (GSegment::sk_Bezier);
      snew.setPoint (0, s1.pointAt (0));
      snew.setPoint (1, computePoint (1, s1, s2));
      snew.setPoint (2, computePoint (2, s1, s2));
      snew.setPoint (3, s1.pointAt (1));
      for (int i = 0; i < 4; i++)
        seg.setPoint (i, blendPoints (snew.pointAt (i), s2.pointAt (i),
                                      step, num));
    }
    else if (s2.kind () == GSegment::sk_Line) {
      GSegment snew (GSegment::sk_Bezier);
      snew.setPoint (0, s2.pointAt (0));
      snew.setPoint (1, computePoint (1, s2, s1));
      snew.setPoint (2, computePoint (2, s2, s1));
      snew.setPoint (3, s2.pointAt (1));
      for (int i = 0; i < 4; i++)
        seg.setPoint (i, blendPoints (s1.pointAt (i), snew.pointAt (i),
                                      step, num));
    }
    else {
      for (int i = 0; i < 4; i++)
        seg.setPoint (i, blendPoints (s1.pointAt (i), s2.pointAt (i),
                                      step, num));
    }
  }
  return seg;
}

GSegment::GSegment (Kind sk)
:skind (sk)
,bpoints (4)
{
}

GSegment::GSegment(const QDomElement &element)
{

    skind = (GSegment::Kind)element.attribute("kind").toInt();
    int i=0;
    Coord p;
    QDomElement point = element.firstChild().toElement();
    for( ; !point.isNull() && i<(skind==0 ? 2 : 4); point = point.nextSibling().toElement(), ++i ) {
        p.x(point.attribute("x").toFloat());
        p.y(point.attribute("y").toFloat());
        setPoint(i, p);
    }
}

const Coord& GSegment::pointAt (int i) const {
  assert (i >= 0 && ((skind == sk_Bezier && i < 4) ||
                     (skind == sk_Line && i < 2)));
  return points[i];
}

void GSegment::setPoint (int i, const Coord& c) {
  assert (i >= 0 && ((skind == sk_Bezier && i < 4) ||
                     (skind == sk_Line && i < 2)));
  points[i] = c;
  if (skind == sk_Bezier) {
    bpoints.setPoint (i, qRound (c.x ()), qRound (c.y ()));
  }
}

QDomElement GSegment::writeToXml (QDomDocument &document) {

    QDomElement element=document.createElement("seg");
    element.setAttribute ("kind", (skind == sk_Line ? 0 : 1));

    int num = (skind == sk_Line ? 2 : 4);
    for (int i = 0; i < num; i++) {
        QDomElement point=document.createElement("point");
        point.setAttribute ("x", points[i].x ());
        point.setAttribute ("y", points[i].y ());
        element.appendChild(point);
    }
    return element;
}

bool GSegment::contains (const Coord& p) {
  if (skind == sk_Line) {
    float x1, x2, y1, y2;

    x1 = points[0].x ();
    x2 = points[1].x ();
    if (x2 <= x1) {
      // swap the points
      x2 = x1;
      x1 = points[1].x ();
      y2 = points[0].y ();
      y1 = points[1].y ();
    }
    else {
      y1 = points[0].y ();
      y2 = points[1].y ();
    }
    if (x1 - 3 <= p.x () && p.x () <= x2 + 3) {
      if (abs (int (x1 - x2)) < 5) {
        if ((y1 <= p.y () && p.y () <= y2) ||
            (y2 <= p.y () && p.y () <= y1))
            return true;
      }
      else {
        float xp, yp, m, n;

        // y = m * x + n;
        m = (y2 - y1) / (x2 - x1);
        n = y1 - m * x1;

        if (m > 1) {
          xp = ((float) p.y () - n) / m;
          if (xp - 5 <= p.x () && p.x () <= xp + 5)
            return true;
        }
        else {
          yp = m * p.x () + n;

          if (yp - 5 <= p.y () && p.y () <= yp + 5)
            return true;
        }
      }
    }
  }
  else {
    return GBezier::bezier_segment_contains (points[0], points[1],
                                             points[2], points[3], p);
  }
  return false;
}

void GSegment::draw (QPainter& p, bool withBasePoints, bool /*outline*/,
                     bool drawFirst) {
  if (skind == sk_Line)
    Painter::drawLine (p, points[0].x (), points[0].y (),
                points[1].x (), points[1].y ());
  else
    p.drawQuadBezier (bpoints);
  if (withBasePoints) {
    p.save ();
    p.setPen (Qt::black);
    if (drawFirst)
      Painter::drawRect (p, points[0].x () - 2, points[0].y () - 2, 4, 4);
    if (skind == sk_Line)
      Painter::drawRect (p, points[1].x () - 2, points[1].y () - 2, 4, 4);
    else
      Painter::drawRect (p, points[3].x () - 2, points[3].y () - 2, 4, 4);
    p.restore ();
  }
}

void GSegment::movePoint (int idx, float dx, float dy, bool /*ctrlPressed*/) {
  assert (idx >= 0 && ((skind == sk_Bezier && idx < 4) ||
                       (skind == sk_Line && idx < 2)));
  points[idx].x (points[idx].x () + dx);
  points[idx].y (points[idx].y () + dy);
  if (skind == sk_Bezier) {
    bpoints.setPoint (idx, qRound (points[idx].x ()), qRound (points[idx].y ()));
  }
}

Rect GSegment::boundingBox () {
  Rect r;
  if (skind == sk_Line)
    r = Rect (points[0], points[1]);
  else {
    r = Rect (points[0], points[0]);
    for (unsigned int i = 1; i < 4; i++) {
      const Coord& p = points[i];
      r.left (QMIN(p.x (), r.left ()));
      r.top (QMIN(p.y (), r.top ()));
      r.right (QMAX(p.x (), r.right ()));
      r.bottom (QMAX(p.y (), r.bottom ()));
    }
  }
  return r.normalize ();
}

QPointArray GSegment::getPoints () const {

  if (skind == sk_Line) {
    QPointArray result (2);
    result.setPoint (0, qRound (points[0].x ()), qRound (points[0].y ()));
    result.setPoint (1, qRound (points[1].x ()), qRound (points[1].y ()));
    return result;
  }
  else
    return bpoints.quadBezier ();
}

float GSegment::length () const {
  float len = 0.0;

  if (skind == sk_Line) {
    len = seg_length (points[0], points[1]);
  }
  else
   for (int i = 0; i < 3; i++)
     len += seg_length (points[i], points[i + 1]);
  return len;
}

GCurve::GCurve (GDocument* doc)
: GObject (doc)
{
  closed = false;
}

GCurve::GCurve (GDocument* doc, const QDomElement &element)
:GObject (doc, element.namedItem("gobject").toElement())
{

    closed=(element.attribute("closed").toInt()==1);
    QDomElement segment = element.firstChild().toElement();
    for( ; !segment.isNull(); segment = segment.nextSibling().toElement() ) {
        if(segment.tagName()=="seg")
            addSegment(GSegment(segment));
    }
    if (closed)
        updatePath();
}

GCurve::GCurve (const GCurve& obj)
:GObject(obj)
{
  segments = obj.segments;
}

void GCurve::draw (QPainter& p, bool withBasePoints, bool outline, bool) {
  QPen pen;
  QBrush brush;
  initPen (pen);
  p.save ();
  p.setPen (pen);
  p.setWorldMatrix (tmpMatrix, true);
  bool drawFirst = true;

  if (closed) {
    if (! workInProgress () && !outline) {
      initBrush (brush);
      p.setBrush (brush);

      if (gradientFill ()) {
        //if (! gShape.valid ())
          updateGradientShape (p);
        gShape.draw (p);
      }
    }
    p.drawPolygon (points);
  if (withBasePoints) {
    p.setPen (Qt::black);
    p.setBrush (Qt::white);
    for (unsigned int i = 0; i < points.size (); i++) {
      QPoint pnt = points.point (i);
      Painter::drawRect (p, pnt.x () - 2, pnt.y () - 2, 4, 4);
    }
  }

  }
  else {
    QValueList<GSegment>::Iterator i;
    for (i = segments.begin (); i != segments.end (); ++i) {
      (*i).draw (p, withBasePoints, outline, drawFirst);
      drawFirst = false;
    }
  }
  p.restore ();
}

bool GCurve::contains (const Coord& p) {
  Coord pp = p.transform (iMatrix);
  if (box.contains (pp)) {
    return (containingSegment (pp) != segments.end ());
  }
  else
    return false;
}

void GCurve::movePoint (int idx, float dx, float dy, bool /*ctrlPressed*/) {
  int pidx = 0;
  QValueList<GSegment>::Iterator i;
  float ndx = dx * iMatrix.m11 () + dy * iMatrix.m21 ();
  float ndy = dy * iMatrix.m22 () + dx * iMatrix.m12 ();

  for (i = segments.begin (); i != segments.end (); ++i) {
    int num = ((*i).kind () == GSegment::sk_Line ? 2 : 4);
    pidx += num;
    if (pidx > idx) {
      // move point of segment[i]
      int sidx = idx - (pidx - num);
      (*i).movePoint (sidx, ndx, ndy);
      if (sidx == num - 1) {
        // it's a endpoint, so move the first point of segment[i+1]
        ++i;
        if (i != segments.end ()) {
          (*i).movePoint (0, ndx, ndy);
        }
      }
      else if (sidx == 0) {
        // it's a startpoint, so move the first point of segment[i-1]
        if (i == segments.begin () && closed) {
            GSegment& seg = segments.last();
            seg.movePoint (seg.kind () == GSegment::sk_Line ? 1 : 3, ndx, ndy);
        }
      }
      updatePath ();
      updateRegion (true);
      return;
    }
  }
}

int GCurve::getNeighbourPoint (const Coord& p) {
  Coord c;
  int idx = 0;

  QValueList<GSegment>::Iterator i;
  for (i = segments.begin (); i != segments.end (); ++i) {
    int num  = ((*i).kind () == GSegment::sk_Line ? 2 : 4);
    for (int n = 0; n < num; n++) {
      c = (*i).pointAt (n).transform (tMatrix);
      if (c.isNear (p, NEAR_DISTANCE)) {
        return idx;
      }
      idx++;
    }
  }
  return -1;
}

QString GCurve::typeName () const {
  if (closed)
    return i18n("Closed Curve");
  else
    return i18n("Curve");
}

GObject* GCurve::copy () {
  return new GCurve (*this);
}

/*GObject* GCurve::create (GDocument *doc, const QDomElement &element)
{
  return new GCurve(doc, element);
}*/

QDomElement GCurve::writeToXml (QDomDocument &document) {

    QDomElement element=document.createElement("curve");
    element.setAttribute ("closed", (int) closed);

    QValueList<GSegment>::Iterator i;
    for (i = segments.begin (); i != segments.end (); ++i)
        element.appendChild((*i).writeToXml (document));
    element.appendChild(GObject::writeToXml(document));
    return element;
}

void GCurve::getPath (QValueList<Coord>& ) {
}

void GCurve::calcBoundingBox () {
  QValueList<GSegment>::Iterator i = segments.begin ();
  if (i == segments.end ()) {
    box = Rect ();
    return;
  }

  Rect r = (*i).boundingBox ();
  ++i;
  while (i != segments.end ()) {
    Rect r2 = (*i).boundingBox ();
    r = r.unite (r2);
    ++i;
  }
  box = r.transform (tmpMatrix);
}

void GCurve::updateGradientShape (QPainter& ) {
}

void GCurve::setClosed (bool flag) {
  if (! closed) {
    closed = flag;
    updatePath ();
    updateRegion ();
  }
  else
    closed = flag;
}

void GCurve::addLineSegment (const Coord& p1, const Coord& p2) {
  GSegment seg (GSegment::sk_Line);
  seg.setPoint (0, p1);
  seg.setPoint (1, p2);
  segments.append(seg);
  updatePath ();
  updateRegion (true);
}

void GCurve::addBezierSegment (const Coord& p1, const Coord& p2,
                               const Coord& p3, const Coord& p4) {
  GSegment seg (GSegment::sk_Bezier);
  seg.setPoint (0, p1);
  seg.setPoint (1, p2);
  seg.setPoint (2, p3);
  seg.setPoint (3, p4);
  segments.append(seg);
  updatePath ();
  updateRegion (true);
}

void GCurve::addSegment (const GSegment& s) {
  segments.append(s);
  updatePath ();
  updateRegion (true);
}

const GSegment& GCurve::getSegment (int idx) {
    return (*segments.at(idx));
}

void GCurve::removePoint (int /*idx*/, bool update) {
    //int pidx = 0;
    //QValueList<GSegment>::Iterator i;

  if (segments.count() > 1) {
#if 0
    if (idx == 0) {
      // remove the first segment
      segments.erase (segments.begin ());
    }
    else {
      for (i = segments.begin (); i != segments.end (); i++) {
        int num = ((*i).kind () == GSegment::sk_Line ? 2 : 4);
        pidx += num;
        if (pidx > idx) {
          // remove this segment and set the first point
          // of the following
        }
      }
    }
#endif
  }

  if (update)
    updateRegion ();
}

GCurve* GCurve::blendCurves (GCurve *start, GCurve *end, int step, int num) {
  GCurve *res = new GCurve ();
  // TODO: Attribute setzen
  res->outlineInfo = start->outlineInfo;
  res->fillInfo = start->fillInfo;

  QValueList<GSegment>::Iterator si, ei;
  si = start->segments.begin ();
  ei = end->segments.begin ();

  // (1) both curves with the same number of segments
  if (start->numOfSegments () <= end->numOfSegments ()) {
    while (si != start->segments.end ()) {
      GSegment seg = blendSegments (*si, *ei, step, num);
      res->segments.append(seg);
      ++si; ++ei;
    }
    if (start->numOfSegments () < end->numOfSegments ()) {
      // create a pseudo segment with the last point
      GSegment lseg (GSegment::sk_Line);
      const GSegment& last = start->segments.last();
      Coord p = (last.kind () == GSegment::sk_Line ? last.pointAt (1) :
                 last.pointAt (3));
      lseg.setPoint (0, p);
      lseg.setPoint (1, p);

      while (ei != end->segments.end ()) {
        GSegment seg = blendSegments (lseg, *ei, step, num);
        res->segments.append(seg);
        ++ei;
      }
    }
  }
  else {
    while (ei != end->segments.end ()) {
      GSegment seg = blendSegments (*si, *ei, step, num);
      res->segments.append(seg);
      ++si; ++ei;
    }
    GSegment lseg (GSegment::sk_Line);
    const GSegment& last = end->segments.last();
    Coord p = (last.kind () == GSegment::sk_Line ? last.pointAt (1) :
               last.pointAt (3));
    lseg.setPoint (0, p);
    lseg.setPoint (1, p);

    while (si != start->segments.end ()) {
      GSegment seg = blendSegments (*si, lseg, step, num);
      res->segments.append(seg);
      ++si;
    }
  }
  res->setOutlineColor (blendColors (start->getOutlineColor (),
                                     end->getOutlineColor (), step, num));
  if (start->isClosed () && end->isClosed ()) {
    // blend fill properties
    if (start->getFillStyle () == end->getFillStyle () &&
        start->getFillStyle () == GObject::FillInfo::SolidFill) {
      res->setFillStyle (GObject::FillInfo::SolidFill);
      res->setFillColor (blendColors (start->getFillColor (),
                                      end->getFillColor (), step, num));
    }
    res->setClosed (true);
  }
  res->calcBoundingBox ();
  return res;
}

QColor GCurve::blendColors (const QColor& c1, const QColor& c2, int step,
                            int num) {
  if (c1 == c2)
    return c1;
  else {
    int r, g, b;
    r = (c2.red () - c1.red ()) / (num + 1) * (step + 1) + c1.red ();
    g = (c2.green () - c1.green ()) / (num + 1) * (step + 1) + c1.green ();
    b = (c2.blue () - c1.blue ()) / (num + 1) * (step + 1) + c1.blue ();
    return QColor (r, g, b);
  }
}

QValueList<GSegment>::Iterator GCurve::containingSegment (const Coord& p) {
  QValueList<GSegment>::Iterator i;
  for (i = segments.begin (); i != segments.end (); ++i)
    if ((*i).contains (p))
      return i;
  return segments.end ();
}

void GCurve::updatePath () {
  if (! closed)
    return;

  points.resize (0);
  unsigned int last = 0;
  QValueList<GSegment>::Iterator i;
  for (i = segments.begin (); i != segments.end (); ++i) {
    QPointArray parray = (*i).getPoints ();
    points.resize (last + parray.size ());
    for (unsigned int i = 0; i < parray.size (); i++)
      points.setPoint (last + i, parray.point (i));
    last += parray.size ();
  }
}

#include <GCurve.moc>
