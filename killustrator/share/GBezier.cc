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

#include <float.h>
#include <values.h>
#include <GBezier.h>
#include <GCurve.h>
#include <Arrow.h>
#include <Painter.h>
#include <stdlib.h>

#include <qdom.h>
#include <qpointarray.h>
#include <qpainter.h>
#include <klocale.h>
#include <kdebug.h>
#include "GDocument.h"
#define DELTA 0.05

static bool bezier_segment_part_contains (int x0, int y0, int x1,int y1,
                                          const Coord& pp) {
  float  m, n, yp, xp;
  int xh, yh;

  if(pp.x () <= QMAX(x0,x1) + 5 && pp.x() >= QMIN(x0,x1) - 5 &&
     pp.y () <= QMAX(y0,y1) + 5 && pp.y() >= QMIN(y0,y1) - 5) {

    if (x1 <= x0) {
      // swap the points
      xh = x1;
      yh = y1;
      x1 = x0;
      y1 = y0;
      x0 = xh;
      y0 = yh;
    }

    if (x0 - 5 <= pp.x () && pp.x () <= x1 + 5) {
      if (abs (int (x0 - x1)) < 5) {
        if ((y0 <= pp.y () && pp.y () <= y1) ||
            (y1 <= pp.y () && pp.y () <= y0)) {
          return true;
        }
      }
      else {
        // y = m * x + n;
        m = (float)(y1 - y0) / (float)(x1 - x0);
        n = (float)y0 - m * (float)x0;

        if (m > 1) {
          xp = ((float) pp.y () - n) / m;
          if (xp - 5 <= pp.x () && pp.x () <= xp + 5) {
            return true;

          }
        }
        else {
          yp = m * (float) pp.x () + n;
          if (yp - 5 <= pp.y () && pp.y () <= yp + 5) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

bool GBezier::bezier_segment_contains (const Coord& p0, const Coord& p1,
                                       const Coord& p2, const Coord& p3,
                                       const Coord& c)
{
  /** every bezier_segment contains 1/DELTA lines, we have to compute the
      lines
      to check later in bezier_segment_part_contains, if the Coord c on a
      line
  **/

  float th,th2,th3;
  float t,t2,t3;
  int x0,y0,x1,y1;

  x1 = (int)p0.x();
  y1 = (int)p0.y();
  for(t = 0; t < 1.01; t += DELTA) {
    x0 = x1;
    y0 = y1;
    th = 1 - t;
    t2= t*t;
    t3 = t2*t;
    th2 = th*th;
    th3 = th2*th;
    x1 = (int) (th3*  p0.x() +
                3*t*th2*p1.x() +
                3*t2*th*p2.x() +
                t3*p3.x());
    y1 = (int) (th3* p0.y() +
                3*t*th2*p1.y() +
                3*t2*th*p2.y() +
                t3*p3.y());
    if (bezier_segment_part_contains(x0, y0, x1, y1, c)) return true;
  }

  return false;
}

GBezier::GBezier (GDocument *doc )
: GPolyline (doc)
{
  wSegment = 0; closed = false;
}

GBezier::GBezier (GDocument *doc, const QDomElement &element)
:GPolyline (doc, element.namedItem("polyline").toElement())
{

    wSegment = -1;
    closed=(element.attribute("closed").toInt()==1);
    calcBoundingBox();
    computePPoints();
}

GBezier::GBezier (const GBezier& obj) : GPolyline (obj) {
  wSegment = -1;
  closed = obj.closed;
  ppoints = obj.ppoints;
  calcBoundingBox();
}

void GBezier::setPoint (int idx, const Coord& p) {
  Coord np = p.transform (iMatrix);

  points.at (idx)->x (np.x ());
  points.at (idx)->y (np.y());
  if (! isEndPoint (idx))
    updateBasePoint (cPoint (idx));
  else {
    computePPoints ();
    updateRegion ();
  }
}

void GBezier::movePoint (int idx, float dx, float dy, bool ctrlPressed) {
  float x = points.at (idx)->x ();
  float y = points.at (idx)->y ();
  float ndx, ndy;
  int cidx = -1; // >= 0, if the curve is closed and we move
                 // the first or last point

  ndx = dx * iMatrix.m11 () + dy * iMatrix.m21 ();
  ndy = dy * iMatrix.m22 () + dx * iMatrix.m12 ();

  points.at (idx)->x (x + ndx);
  points.at (idx)->y (y + ndy);
  if (closed) {
    int n = points.count ();
    if (idx < 3) {
      cidx = n - (3 - idx);
      points.at (cidx)->x (x + ndx);
      points.at (cidx)->y (y + ndy);
    }
    else if (idx + 3 >= (int) points.count ()) {
      cidx = 3 - (n - idx);
      points.at (cidx)->x (x + ndx);
      points.at (cidx)->y (y + ndy);
    }
  }
  if (isEndPoint (idx) && !ctrlPressed) {
      points.at (idx - 1)->x (points.at (idx - 1)->x () + ndx);
      points.at (idx - 1)->y (points.at (idx - 1)->y () + ndy);
      points.at (idx + 1)->x (points.at (idx + 1)->x () + ndx);
      points.at (idx + 1)->y (points.at (idx + 1)->y () + ndy);
      if (cidx >= 0) {
          points.at (cidx - 1)->x (points.at (cidx - 1)->x () + ndx);
          points.at (cidx - 1)->y (points.at (cidx - 1)->y () + ndy);
          points.at (cidx + 1)->x (points.at (cidx + 1)->x () + ndx);
          points.at (cidx + 1)->y (points.at (cidx + 1)->y () + ndy);
      }
  }
  else if(!ctrlPressed) {
    updateBasePoint (cPoint (idx));
    if (cidx >=0)
      updateBasePoint (cPoint (cidx));
  }
  computePPoints ();
  updateRegion ();
}

QString GBezier::typeName () const
{
  return i18n("Bezier curve");
}

void GBezier::draw (QPainter& p, bool withBasePoints, bool outline) {
  QPen pen;
  QBrush brush;
  int sdx = 0;
  int sdy = 0;
  int edx = 0;
  int edy = 0;

  initPen (pen);
  p.save ();
  p.setPen (pen);
  p.setWorldMatrix (tmpMatrix, true);
  unsigned num = points.count ();

  float w = outlineInfo.width == 0 ? 1.0 : outlineInfo.width;

  // check for arrows
  // (xAngle/RAD_FACTOR) doesnt work!?
  if (sArrow != 0L) {
    sdx = qRound (w*sArrow->length () * cos (1/(RAD_FACTOR/(sAngle))));
    sdy = qRound (w*sArrow->length () * sin (1/(RAD_FACTOR/(sAngle))));
  }

  if (eArrow != 0L) {
    edx = qRound (w*eArrow->length () * cos (1/(RAD_FACTOR/(eAngle))));
    edy = qRound (w*eArrow->length () * sin (1/(RAD_FACTOR/(eAngle))));
  }

#if 0
  for (unsigned int i = 1; i + 3 < num; i += 3) {
    if (points.at (i + 1)->x () == FLT_MAX ||
        points.at (i + 2)->x () == FLT_MAX) {
      p.drawLine (points.at (i)->x (), points.at (i)->y (),
                  points.at (i + 3)->x (), points.at (i + 3)->y ());
    }
    else {
      p.drawBezier (points,i);
    }
  }
#else
  if (num > 3) {
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
      p.drawPolygon (ppoints);
    }
    else {
      //      p.drawPolyline (ppoints);
      for (unsigned int i = 1; i + 3 < num; i += 3) {
        if (points.at (i + 1)->x () == FLT_MAX ||
                  points.at (i + 2)->x () == FLT_MAX) {
                p.drawLine (points.at (i)->x () + ((i==1) ? sdx : 0),
                            points.at (i)->y () + ((i==1) ? sdy : 0),
                            points.at (i + 3)->x () + ((i==num-2) ? edx : 0),
                            points.at (i + 3)->y () + ((i==num-2) ? edy : 0));
              }
              else {
                QPointArray bpoints (4);
          bpoints.setPoint (0, qRound (points.at (i)->x () + ((i==1) ? sdx : 0)),
                                           qRound (points.at (i)->y () + ((i==1) ? sdy : 0)));
          bpoints.setPoint (1, qRound (points.at (i + 1)->x ()),
                                           qRound (points.at (i + 1)->y ()));
          bpoints.setPoint (2, qRound (points.at (i + 2)->x ()),
                                           qRound (points.at (i + 2)->y ()));
          bpoints.setPoint (3, qRound (points.at (i + 3)->x () + ((i==num-5) ? edx : 0)),
                                           qRound (points.at (i + 3)->y () + ((i==num-5) ? edy : 0)));
                p.drawQuadBezier (bpoints);
              }
      }
    }
  }
#endif
  p.setClipping (false);
  p.restore ();
  if (sArrow != 0L) {
    Coord pp = points.at (1)->transform (tmpMatrix);
    sArrow->draw (p, pp, outlineInfo.color,
                  outlineInfo.width, sAngle);
  }
  if (eArrow != 0L) {
    Coord pp = points.at (num - 2)->transform (tmpMatrix);
    eArrow->draw (p, pp, outlineInfo.color,
                  outlineInfo.width, eAngle);
  }
  if (withBasePoints)
    drawHelpLines (p);
  if (wSegment != -1)
    drawHelpLinesForWorkingSegment (p);
}

void GBezier::drawHelpLines (QPainter& p) {
  unsigned int i, num = points.count ();

  p.save ();
  for (i = 0; i < num; i++) {
    Coord c = points.at (i)->transform (tmpMatrix);
    if (isEndPoint (i))
      p.setPen (black);
    else
      p.setPen (blue);
    Painter::drawRect (p, c.x () - 2.0, c.y () - 2.0, 4, 4);
  }

  QPen pen (blue, 1, DotLine);
  p.setPen (pen);
  for (i = 0; i + 2 < num; i += 3) {
    if (points.at (i)->x () == FLT_MAX ||
        points.at (i + 2)->x () == FLT_MAX)
      continue;

    Coord c = points.at (i + 1)->transform (tmpMatrix);
    Coord c1 = points.at (i)->transform (tmpMatrix);
    Coord c2 = points.at (i + 2)->transform (tmpMatrix);
    Painter::drawLine (p, c1.x (), c1.y (), c.x (), c.y ());
    Painter::drawLine (p, c.x (), c.y (), c2.x (), c2.y ());
  }
  p.restore ();
}

void GBezier::drawHelpLinesForWorkingSegment (QPainter& p) {
  p.save ();

  QPen pen1 (blue, 1, DotLine);
  QPen pen2 (blue);

  for (int i = wSegment * 3; i <= (wSegment + 1) * 3; i += 3) {
    if (i + 2 >= (int) points.count () ||
        points.at (i)->x () == FLT_MAX ||
        points.at (i + 2)->x () == FLT_MAX) {
      return;
    }

    p.setPen (pen1);
    Coord c = points.at (i + 1)->transform (tmpMatrix);
    Coord c1 = points.at (i)->transform (tmpMatrix);
    Coord c2 = points.at (i + 2)->transform (tmpMatrix);
    Painter::drawLine (p, c1.x (), c1.y (), c.x (), c.y ());
    Painter::drawLine (p, c.x (), c.y (), c2.x (), c2.y ());
    p.setPen (pen2);
    Painter::drawRect (p, c1.x () - 2, c1.y () - 2, 4, 4);
    Painter::drawRect (p, c2.x () - 2, c2.y () - 2, 4, 4);
  }
  p.restore ();
}

bool GBezier::contains (const Coord& p) {
  if (rbox.contains (p)) {
    Coord pc = p.transform (iMatrix);

    for (unsigned int i = 1; i + 3 < points.count (); i += 3) {
      // detect the containing curve segment
      Rect r;
      Coord p = *(points.at (i));
      r.left (p.x ());
      r.top (p.y ());
      r.right (p.x ());
      r.bottom (p.y ());

      for (unsigned int j = i + 1; j < i + 4; j++) {
        Coord pn = *(points.at (j));
        r.left (QMIN(pn.x (), r.left ()));
        r.top (QMIN(pn.y (), r.top ()));
        r.right (QMAX(pn.x (), r.right ()));
        r.bottom (QMAX(pn.y (), r.bottom ()));
      }
      if (r.contains (pc)) {
        if (bezier_segment_contains (*(points.at (i)), *(points.at (i + 1)),
                                     *(points.at (i + 2)),
                                     *(points.at (i + 3)), pc))
        return true;
      }
    }
  }
  return false;
}

GObject* GBezier::copy () {
  return new GBezier (*this);
}

/*GObject* GBezier::create (GDocument *doc, const QDomElement &element)
{
  return new GBezier (doc, element);
}*/

void GBezier::initBasePoint (int idx) {
  Coord epoint = *(points.at (idx + 1));
  float dx = epoint.x ();
  float dy = epoint.y ();
  points.at (idx)->x (2 * dx - points.at (idx + 2)->x ());
  points.at (idx)->y (2 * dy - points.at (idx + 2)->y ());
  updateRegion (false);
}

void GBezier::updateBasePoint (int idx) {
  int eidx = (idx == 0 ? 1 : (cPoint (idx) < idx ? idx - 1 : idx + 1));
  if (idx < 0 || eidx < 0)
    return;

  Coord epoint = *(points.at (eidx));
  if (points.at (cPoint (idx))->x () == FLT_MAX)
    return;

  float dx = epoint.x ();
  float dy = epoint.y ();
  points.at (idx)->x (2 * dx - points.at (cPoint (idx))->x ());
  points.at (idx)->y (2 * dy - points.at (cPoint (idx))->y ());
  computePPoints ();
  updateRegion ();
}

void GBezier::setWorkingSegment (int seg) {
  wSegment = seg;
  updateRegion (false);
}

void GBezier::calcBoundingBox () {
  Rect r;
  unsigned int num = points.count ();
  Coord p = points.at (0)->transform (tmpMatrix);

  computePPoints ();

  r.left (p.x ());
  r.top (p.y ());
  r.right (p.x ());
  r.bottom (p.y ());
  for (unsigned int i = 1; i < num; i++) {
    Coord p = points.at (i)->transform (tmpMatrix);

    if (p.x () != FLT_MAX && p.y () != FLT_MAX) {
      r.left (QMIN(p.x (), r.left ()));
      r.top (QMIN(p.y (), r.top ()));
      r.right (QMAX(p.x (), r.right ()));
      r.bottom (QMAX(p.y (), r.bottom ()));
    }
  }

  float w = outlineInfo.width == 0 ? 1.0 : outlineInfo.width;

  if (sArrow != 0L && num > 2) {
    Coord p1 = points.at (0)->transform (tmpMatrix);
    Coord p2 = points.at (2)->transform (tmpMatrix);
    sAngle = calcArrowAngle (p1, p2, 0);
    Rect sr = sArrow->boundingBox (p1, w, sAngle);
    r = r.unite (sr);
  }
  if (eArrow != 0L && num >= 3) {
    Coord p1 = points.at (num - 3)->transform (tmpMatrix);
    Coord p2 = points.at (num - 1)->transform (tmpMatrix);
    eAngle = calcArrowAngle (p1, p2, 1);
    Rect er = eArrow->boundingBox (p2, w, eAngle);
    r = r.unite (er);
  }

  r.enlarge (2); // for the help lines
  //  updateBoundingBox (r);
  rbox = r.normalize ();
  QRect pbox = ppoints.boundingRect ();
  box = Rect (pbox.x (), pbox.y (), pbox.width (), pbox.height ());
  box = box.transform (tmpMatrix);
}

void GBezier::removePoint (int idx, bool update) {
  if (points.count () > 6) {
    points.remove (idx - 1);
    points.remove (idx - 1);
    points.remove (idx - 1);
    if (update)
      updateRegion ();
  }
}

void GBezier::insertPoint (int idx, const Coord& p, bool update) {
  Coord p0 (p.x () - 20, p.y () - 20);
  Coord p1 (p.x () + 20, p.y () + 20);
  addPoint (idx, p0, false);
  addPoint (idx + 1, p, false);
  addPoint (idx + 2, p1, update);
}

int GBezier::containingSegment (float xpos, float ypos) {
  Coord p (xpos, ypos);
  Coord pc = p.transform (iMatrix);
  int seg = 0;

  for (unsigned int i = 1; i + 3 < points.count (); i += 3) {
    // detect the containing curve segment
    Rect r;
    Coord p = *(points.at (i));
    r.left (p.x ());
    r.top (p.y ());
    r.right (p.x ());
    r.bottom (p.y ());

    for (unsigned int j = i + 1; j < i + 4; j++) {
      Coord pn = *(points.at (j));
      r.left (QMIN(pn.x (), r.left ()));
      r.top (QMIN(pn.y (), r.top ()));
      r.right (QMAX(pn.x (), r.right ()));
      r.bottom (QMAX(pn.y (), r.bottom ()));
    }
    if (r.contains (pc)) {
      if (bezier_segment_contains (*(points.at (i)), *(points.at (i + 1)),
                                   *(points.at (i + 2)),
                                   *(points.at (i + 3)), pc)) {
        return seg;
      }
    }
    seg++;
  }
  return -1;
}

int GBezier::cPoint (int idx) {
  if (idx > 1)
    return idx + (isEndPoint (idx - 1) ? -2 : 2);
  else
    return idx + (isEndPoint (idx + 1) ? 2 : -2);
}

QDomElement GBezier::writeToXml (QDomDocument &document) {

    QDomElement bezier=document.createElement("bezier");
    bezier.setAttribute ("closed", (int) closed);
    bezier.appendChild(GPolyline::writeToXml(document));
    return bezier;
}

bool GBezier::findNearestPoint (const Coord& p, float max_dist,
                                float& dist, int& pidx, bool all) {
  float dx, dy, min_dist = max_dist + 1, d;
  pidx = -1;

  Coord np = p.transform (iMatrix);

  unsigned int i = 1;
  while (i <= points.count () - 2) {
    dx = points.at (i)->x () - np.x ();
    dy = points.at (i)->y () - np.y ();
    d = sqrt (dx * dx + dy * dy);

    if (d < max_dist && d < min_dist) {
      dist = min_dist = d;
      pidx = i;
    }

    if (! all && i == 1) {
      // test only first and last point
      i = points.count () - 2;
    }
    else
      i += 3;
  }
  return pidx >= 0;
}

void GBezier::computePPoints () {
  unsigned int i, num = points.count ();
  unsigned int idx = 0;

  ppoints.resize (num);
  for (i = 1; i + 3 < num; i += 3) {
    if (points.at (i + 1)->x () == FLT_MAX ||
        points.at (i + 2)->x () == FLT_MAX) {
      if (ppoints.size () < idx + 2)
        ppoints.resize (ppoints.size () + 2);
      ppoints.setPoint (idx++, points.at (i)->x (), points.at (i)->y ());
      ppoints.setPoint (idx++, points.at (i+3)->x (), points.at (i+3)->y ());
    }
    else
      idx = createPolyline (i, idx);
  }
  ppoints.resize (idx);
}

void GBezier::setClosed (bool flag) {
  if (flag && points.count () < 6)
    return;

  closed = flag;
  if (closed) {
    // Point #n-2 := Point #0
    // Point #n-1 := Point #1
    // Point #n := Point #2
    unsigned int n = points.count () - 1;
    *points.at (n) = *points.at (2);
    *points.at (n-1) = *points.at (1);
    *points.at (n-2) = *points.at (0);
  }
  computePPoints ();
}

int GBezier::createPolyline (int index, int pidx) {
  double th, th2, th3;
  double t, t2, t3;
  int x4, y4;

  double x0 = points.at (index)->x();
  double y0 = points.at (index)->y();
  double x1 = points.at (index+1)->x();
  double y1 = points.at (index+1)->y();
  double x2 = points.at (index+2)->x();
  double y2 = points.at (index+2)->y();
  double x3 = points.at (index+3)->x();
  double y3 = points.at (index+3)->y();

  if (ppoints.size () - pidx < ((points.count ())/ DELTA / 3))
    ppoints.resize ((points.count ()) /DELTA / 3 + pidx);

  for (t = 0; t < 1.01; t += DELTA) {
    t2 = t * t;
    t3 = t2 * t;
    th = 1 - t;
    th2 = th * th;
    th3 = th2 * th;
    x4 = (int) (th3 * x0 + 3. * t * th2 * x1 + 3. *t2 *th *x2 + t3 * x3);
    y4 = (int) (th3 * y0 + 3. * t * th2 * y1 + 3. * t2 * th * y2 + t3 * y3);
    ppoints.setPoint (pidx, x4, y4);
    pidx++;
  }
  ppoints.resize (pidx);
  return pidx;
}

void GBezier::updateGradientShape (QPainter& p) {
  // define the rectangular box for the gradient pixmap
  // (in object coordinate system)
  gShape.setBox (calcEnvelope ());

  // define the clipping region
  QWMatrix matrix = p.worldMatrix ();
  gShape.setRegion (QRegion (matrix.map (ppoints)));

  // update the gradient information
  gShape.setGradient (fillInfo.gradient);

  // and create a new gradient pixmap
  gShape.updatePixmap ();
}

void GBezier::getPath (QValueList<Coord>& path) {
  unsigned int num = ppoints.size ();
  for (unsigned int i = 0; i < num; i++) {
    const QPoint& p = ppoints.point (i);
    Coord pi (p.x (), p.y ());
    path.append(pi.transform (tMatrix));
  }
}

bool GBezier::intersects (const Rect& r) {
  return r.intersects (rbox);
  //  return GObject::intersects (r);
}

bool GBezier::splitAt (unsigned int idx, GObject*& obj1, GObject*& obj2) {
  bool result = false;

  if (isEndPoint (idx)) {
    if (closed) {
      GBezier* other = new GBezier (*this);
      other->closed = false;
      other->removeAllPoints ();
      unsigned int i, num = points.count ();
      for (i = idx - 1; i < num; i++)
        other->points.append (new Coord (*points.at (i)));
      for (i = 0; i <= idx + 1; i++)
        other->points.append (new Coord (*points.at (i)));
      other->calcBoundingBox ();
      result = true;
      obj1 = other;
      obj2 = 0L;
    }
    else if (idx > 1 && idx < points.count () - 1) {
      GBezier* other1 = (GBezier *) this->copy ();
      unsigned int i, num = points.count ();
      for (i = idx + 2; i < num; i++)
        other1->points.remove (idx + 2);
      other1->calcBoundingBox ();

      GBezier* other2 = (GBezier *) this->copy ();
      for (i = 0; i < idx - 1; i++)
        other2->points.remove ((unsigned int) 0);
      other2->calcBoundingBox ();
      result = true;
      obj1 = other1;
      obj2 = other2;
    }
  }
  return result;
}

GCurve* GBezier::convertToCurve () const {
  unsigned int nsegs = (points.count () - 3) / 3;
  GCurve* curve = new GCurve (m_gdoc);
  curve->setOutlineInfo (outlineInfo);
  QListIterator<Coord> it (points);
  ++it;
  Coord p1 = it.current ()->transform (tmpMatrix); ++it;
  for (unsigned int i = 0; i < nsegs; i++) {
    Coord p2 = it.current ()->transform (tmpMatrix); ++it;
    Coord p3 = it.current ()->transform (tmpMatrix); ++it;
    Coord p4 = it.current ()->transform (tmpMatrix); ++it;
    curve->addBezierSegment (p1, p2, p3, p4);
    p1 = p4;
  }

  curve->setClosed (closed);
  return curve;
}

#include <GBezier.moc>
