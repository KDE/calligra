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

#include <stdlib.h>
#include <iostream.h>
#include <math.h>
#include <assert.h>
#include <values.h>
#include "GBezier.h"
#include "GBezier.moc"

#include <qpntarry.h>
#include <klocale.h>
#include <kapp.h>

static bool bezier_segment_contains (const Coord& p0, const Coord& p1, 
				     const Coord& p2, const Coord& p3, 
				     const Coord& c) {
  return true;
}

GBezier::GBezier () : GPolyline () {
  wSegment = 0;
}
  
GBezier::GBezier (const list<XmlAttribute>& attribs) : GPolyline (attribs) {
  wSegment = -1;
}

GBezier::GBezier (const GBezier& obj) : GPolyline (obj) {
  wSegment = -1;
}
  
void GBezier::setPoint (int idx, const Coord& p) {
  points.at (idx)->x (p.x ());
  points.at (idx)->y (p.y());
  if (! isEndPoint (idx))
    updateBasePoint (cPoint (idx));
  calcBoundingBox ();
  emit changed ();
}

void GBezier::movePoint (int idx, float dx, float dy) {
  float x = points.at (idx)->x ();
  float y = points.at (idx)->y ();
  float ndx, ndy;

  QWMatrix mi = tMatrix.invert ();
  ndx = dx * mi.m11 () + dy * mi.m21 ();
  ndy = dy * mi.m22 () + dx * mi.m12 ();

  points.at (idx)->x (x + ndx);
  points.at (idx)->y (y + ndy);
  if (isEndPoint (idx)) {
    points.at (idx - 1)->x (points.at (idx - 1)->x () + ndx);
    points.at (idx - 1)->y (points.at (idx - 1)->y () + ndy);
    points.at (idx + 1)->x (points.at (idx + 1)->x () + ndx);
    points.at (idx + 1)->y (points.at (idx + 1)->y () + ndy);
  }
  else {
    updateBasePoint (cPoint (idx));
  }
  calcBoundingBox ();
  emit changed ();
}

const char* GBezier::typeName () {
  return i18n ("Bezier curve");
}

void GBezier::draw (Painter& p, bool withBasePoints) {
  QPen pen (outlineInfo.color, (uint) outlineInfo.width, 
            outlineInfo.style);
  p.save ();

  p.setPen (pen);

  p.setWorldMatrix (tmpMatrix, true);
  unsigned int i, num = points.count ();
  for (i = 1; i + 3 < num; i += 3) {
    if (points.at (i + 1)->x () == MAXFLOAT ||
	points.at (i + 2)->x () == MAXFLOAT) {
      p.drawLine (points.at (i)->x (), points.at (i)->y (),
		  points.at (i + 3)->x (), points.at (i + 3)->y ());
    }
    else {
      p.drawBezier (points,i);
    }
  }
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

void GBezier::drawHelpLines (Painter& p) {
  unsigned int i, num = points.count ();

  p.save ();
  for (i = 0; i < num; i++) {
    Coord c = points.at (i)->transform (tmpMatrix);
    int x = (int) c.x ();
    int y = (int) c.y ();
    if (isEndPoint (i))
      p.setPen (black);
    else
      p.setPen (blue);
    p.drawRect (x - 2, y - 2, 4, 4);
  }

  QPen pen (blue, 1, DotLine);
  p.setPen (pen);
  for (i = 0; i + 2 < num; i += 3) {
    if (points.at (i)->x () == MAXFLOAT ||
	points.at (i + 2)->x () == MAXFLOAT)
      continue;
    
    Coord c1 = points.at (i)->transform (tmpMatrix);
    Coord c2 = points.at (i + 2)->transform (tmpMatrix);
    p.drawLine (c1.x (), c1.y (), c2.x (), c2.y ());
  }
  p.restore ();
}

void GBezier::drawHelpLinesForWorkingSegment (Painter& p) {
  p.save ();

  QPen pen1 (blue, 1, DotLine);
  QPen pen2 (blue);

  for (int i = wSegment * 3; i <= (wSegment + 1) * 3; i += 3) {
    if (i + 2 >= (int) points.count () || 
	points.at (i)->x () == MAXFLOAT ||
	points.at (i + 2)->x () == MAXFLOAT) {
      return;
    }

    p.setPen (pen1);
    Coord c1 = points.at (i)->transform (tmpMatrix);
    Coord c2 = points.at (i + 2)->transform (tmpMatrix);
    p.drawLine (c1.x (), c1.y (), c2.x (), c2.y ());
    p.setPen (pen2);
    p.drawRect (c1.x () - 2, c1.y () - 2, 4, 4);
    p.drawRect (c2.x () - 2, c2.y () - 2, 4, 4);
  }
  p.restore ();
}

bool GBezier::contains (const Coord& p) {
  if (box.contains (p)) {
    QWMatrix mi = tMatrix.invert ();
    Coord pc = p.transform (mi);

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
	/*
       	if (bezier_segment_contains (*(points.at (i)), *(points.at (i + 1)),
				     *(points.at (i + 2)), 
				     *(points.at (i + 3)), pc))
				     */
	return true;
      }
    }
  }
  return false;
}

GObject* GBezier::copy () {
  return new GBezier (*this);
}

void GBezier::initBasePoint (int idx) {
  Coord epoint = *(points.at (idx + 1));
  float dx = epoint.x ();
  float dy = epoint.y ();
  points.at (idx)->x (2 * dx - points.at (idx + 2)->x ());
  points.at (idx)->y (2 * dy - points.at (idx + 2)->y ());
  emit changed ();
}

void GBezier::updateBasePoint (int idx) {
  int eidx = (idx == 0 ? 1 : (cPoint (idx) < idx ? idx - 1 : idx + 1));
  if (idx < 0 || eidx < 0)
    return;

  Coord epoint = *(points.at (eidx));
  if (points.at (cPoint (idx))->x () == MAXFLOAT)
    return;

  float dx = epoint.x ();
  float dy = epoint.y ();
  points.at (idx)->x (2 * dx - points.at (cPoint (idx))->x ());
  points.at (idx)->y (2 * dy - points.at (cPoint (idx))->y ());
  emit changed ();
}

void GBezier::setWorkingSegment (int seg) {
  wSegment = seg;
}

void GBezier::calcBoundingBox () {
  // don't include first and last base point in bounding box computation
  Rect r;
  unsigned int num = points.count ();
  if (num > 1) {
    Coord p = points.at (1)->transform (tmpMatrix);

    r.left (p.x ());
    r.top (p.y ());
    r.right (p.x ());
    r.bottom (p.y ());
  }
  for (unsigned int i = 2; i < num - 1; i++) {
    Coord p = points.at (i)->transform (tmpMatrix);

    r.left (QMIN(p.x (), r.left ()));
    r.top (QMIN(p.y (), r.top ()));
    r.right (QMAX(p.x (), r.right ()));
    r.bottom (QMAX(p.y (), r.bottom ()));
  }

  if (sArrow != 0L) {
    Coord p1 = points.at (0)->transform (tmpMatrix);
    Coord p2 = points.at (2)->transform (tmpMatrix);
    sAngle = calcArrowAngle (p1, p2, 0);
  }
  if (eArrow != 0L) {
    Coord p1 = points.at (num - 3)->transform (tmpMatrix);
    Coord p2 = points.at (num - 1)->transform (tmpMatrix);
    eAngle = calcArrowAngle (p1, p2, 1);
  }

  updateBoundingBox (r);
}

int GBezier::cPoint (int idx) { 
  if (idx > 1)
    return idx + (isEndPoint (idx - 1) ? -2 : 2); 
  else
    return idx + (isEndPoint (idx + 1) ? 2 : -2); 
}

void GBezier::writeToXml (XmlWriter& xml) {
  xml.startTag ("bezier", false);
  writePropertiesToXml (xml);
  xml.addAttribute ("arrow1", outlineInfo.custom.arrow.startId);
  xml.addAttribute ("arrow2", outlineInfo.custom.arrow.endId);
  xml.closeTag (false);

  for (QListIterator<Coord> it (points); it.current (); ++it) {
    xml.startTag ("point", false);
    xml.addAttribute ("x", it.current ()->x ());
    xml.addAttribute ("y", it.current ()->y ());
    xml.closeTag (true);
  }
  xml.endTag ();
}
