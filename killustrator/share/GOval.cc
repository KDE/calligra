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
#include "GOval.h"
#include "GOval.moc"
#include "GPolyline.h"

#include <klocale.h>
#include <kapp.h>

#ifndef M_PI // not ANSI C++, so it maybe...
#define M_PI            3.14159265358979323846  /* pi */
#endif

#define RAD_FACTOR 180.0 / M_PI

GOval::GOval (bool cFlag) : circleFlag (cFlag) {
  sAngle = eAngle = 270;
}

GOval::GOval (const list<XmlAttribute>& attribs, bool cFlag) 
  : GObject (attribs) {
  list<XmlAttribute>::const_iterator first = attribs.begin ();
  float x = 0, y = 0, rx = 0, ry = 0;

  sAngle = eAngle = 270;

  while (first != attribs.end ()) {
    const string& attr = (*first).name ();
    if (attr == "x")
      x = (*first).floatValue ();
    else if (attr == "y")
      y = (*first).floatValue ();
    else if (attr == "rx")
      rx = (*first).floatValue ();
    else if (attr == "ry")
      ry = (*first).floatValue ();
    else if (attr == "angle1")
      sAngle = (*first).floatValue ();
    else if (attr == "angle2")
      eAngle = (*first).floatValue ();
    else if (attr == "kind") {
      const string& v = (*first).stringValue ();
      if (v == "arc") 
	outlineInfo.shape = GObject::OutlineInfo::ArcShape;
      else if (v == "pie") 
	outlineInfo.shape = GObject::OutlineInfo::PieShape;
      else
	outlineInfo.shape = GObject::OutlineInfo::DefaultShape;
    }
    first++;
  }
  sPoint.x (x - rx); sPoint.y (y - ry);
  ePoint.x (x + rx - 1); ePoint.y (y + ry - 1);
  circleFlag = cFlag;
  calcBoundingBox ();
}

GOval::GOval (const GOval& obj) : GObject (obj) {
  circleFlag = obj.circleFlag;
  ePoint = obj.ePoint;
  sPoint = obj.sPoint;
  sAngle = obj.sAngle;
  eAngle = obj.eAngle;
  calcBoundingBox ();
}

const char* GOval::typeName () {
  if (circleFlag)
    return i18n ("Circle");
  else
    return i18n ("Ellipse");
}

void GOval::draw (Painter& p, bool withBasePoints) {
  float alen = 0;
  QPen pen;
  QBrush brush;

  initPen (pen);
  p.save ();
  p.setPen (pen);
  p.setWorldMatrix (tmpMatrix, true);

  if (! workInProgress ()) {
    initBrush (brush);
    p.setBrush (brush);
    if (gradientFill ()) {
      if (! gShape.valid ())
	updateGradientShape (p);
      gShape.draw (p);
    }
  }

  switch (outlineInfo.shape) {
  case GObject::OutlineInfo::DefaultShape:
    p.drawEllipse (sPoint.x (), sPoint.y (),
		   ePoint.x () - sPoint.x (), 
		   ePoint.y () - sPoint.y ());
    break;
  case GObject::OutlineInfo::PieShape:
    alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    p.drawPie (sPoint.x (), sPoint.y (),
	       ePoint.x () - sPoint.x (), 
	       ePoint.y () - sPoint.y (),
	       -eAngle * 16, -alen * 16);
    break;
  case GObject::OutlineInfo::ArcShape:
    alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    p.drawArc (sPoint.x (), sPoint.y (),
	       ePoint.x () - sPoint.x (), 
	       ePoint.y () - sPoint.y (),
	       -eAngle * 16, -alen * 16);
    break;
  }
  p.restore ();
  p.save ();
  if (withBasePoints) {
    p.setPen (black);
    p.setBrush (white);
    for (int i = 0; i < 2; i++) {
      Coord c = segPoint[i].transform (tmpMatrix);
      int x = (int) c.x ();
      int y = (int) c.y ();
      p.drawRect (x - 2, y - 2, 4, 4);
    }
  }
  p.restore ();
}

void GOval::writeToPS (ostream& os) {
  GObject::writeToPS (os);
  if (sPoint.x () < ePoint.x ())
    os << sPoint.x () << ' ' << sPoint.y () << ' '
       << ePoint.x () << ' ' << ePoint.y () << ' ';
  else
    os << ePoint.x () << ' ' << ePoint.y () << ' '
       << sPoint.x () << ' ' << sPoint.y () << ' ';
  switch (outlineInfo.shape) {
  case GObject::OutlineInfo::PieShape:
    os << sAngle << ' ' << eAngle
       << (fillInfo.fstyle == GObject::FillInfo::NoFill ? " false" : " true")
       << " DrawPie\n";
    break;
  case GObject::OutlineInfo::ArcShape:
    os << sAngle << ' ' << eAngle << " DrawArc\n";
    break;
  case GObject::OutlineInfo::DefaultShape:
    os << (fillInfo.fstyle == GObject::FillInfo::NoFill ? " false" : " true")
       << " DrawEllipse\n";
    break;
  }
}

bool GOval::contains (const Coord& p) {
  float x1, y1, x2, y2;

  if (box.contains (p)) {
    QPoint pp = iMatrix.map (QPoint ((int) p.x (), (int) p.y ()));

    x1 = sPoint.x (); x2 = ePoint.x ();
    if (x1 >= x2) {
      x1 = x2; x2 = sPoint.x ();
      y1 = ePoint.y (); y2 = sPoint.y ();
    }
    else {
      y1 = sPoint.y (); y2 = ePoint.y ();
    }

    float x, a, b, sqr;
    float mx, my;

    mx = (x1 + x2) / 2;
    my = (y1 + y2) / 2;
    a = (x2 - x1) / 2;
    b = (y2 - y1) / 2;
    x = pp.x ();
    if (x1 <= x && x <= x2) {
      sqr = sqrt ((1 - ((x-mx)*(x-mx)) / (a*a)) * (b*b));
      if (my - sqr <= pp.y () && pp.y () <= my + sqr)
	return true;
    }
  }
  return false;
}

void GOval::setAngles (float a1, float a2) {
  sAngle = a1;
  eAngle = a2;
  emit changed ();
}

void GOval::setStartPoint (const Coord& p) {
  sPoint = p;
  updateRegion ();
}

void GOval::setEndPoint (const Coord& p) {
  if (circleFlag && ePoint.x () != 0 && ePoint.y () != 0) {
    float dx = (float) fabs (p.x () - sPoint.x ());
    float dy = (float) fabs (p.y () - sPoint.y ());
    float xoff = p.x () - sPoint.x ();
    float yoff = p.y () - sPoint.y ();
    if (dx > dy) {
      ePoint.x (p.x ());
      ePoint.y (sPoint.y () + xoff);
    }
    else {
      ePoint.x (sPoint.x () + yoff);
      ePoint.y (p.y ());
    }
  }
  else
    ePoint = p;
  updateRegion ();
}

void GOval::calcBoundingBox () {
  calcUntransformedBoundingBox (sPoint, Coord (ePoint.x (), sPoint.y ()),
				ePoint,
				Coord (sPoint.x (), ePoint.y ()));
  update_segments ();
}

GObject* GOval::copy () {
  return new GOval (*this);
}

int GOval::getNeighbourPoint (const Coord& p) {
  for (int i = 1; i >= 0; i--) {
    Coord c = segPoint[i].transform (tMatrix);
    if (c.isNear (p, NEAR_DISTANCE))
      return i;
  }
  return -1;
}

void GOval::movePoint (int idx, float dx, float dy) {
  float adx = fabs (dx);
  float ady = fabs (dy);
  float angle = 0;

  if (idx == 0 && segPoint[0] == segPoint[1])
    idx = 1;

  Rect r (sPoint, ePoint);
  r.normalize ();

  float a = r.width () / 2.0;
  float b = r.height () / 2.0;

  if (adx > ady) {
    float x = segPoint[idx].x () + dx;
    if (x < r.left ())
      x = r.left ();
    else if (x > r.right ())
      x = r.right ();

    x -= (r.left () + a);
    angle = acos (x / a) * RAD_FACTOR;
    if (segPoint[idx].y () < r.center ().y ())
      angle = 360 - angle;
  }
  else {
    float y = segPoint[idx].y () + dy;
    if (y < r.top ())
      y = r.top ();
    else if (y > r.bottom ())
      y = r.bottom ();

    y -= (r.top () + b);
    angle = asin (y / b) * RAD_FACTOR;
    if (segPoint[idx].y () < r.center ().y ()) {
      if (segPoint[idx].x () > r.center ().x ())
	angle += 360;
      else
	angle = 180 - angle;;
    }
    else if (segPoint[idx].x () < r.center ().x ())
      angle = 180 - angle;
  }
  if (idx == 0)
    sAngle = angle;
  else
    eAngle = angle;

  // test for equality
  float a1 = qRound (sAngle < 0 ? sAngle + 360 : sAngle);
  float a2 = qRound (eAngle < 0 ? eAngle + 360 : eAngle);
  if (a1 >= a2 - 1 && a1 <= a2 + 1) {
    eAngle = sAngle;
    outlineInfo.shape = GObject::OutlineInfo::DefaultShape;
  }
  else if (outlineInfo.shape == GObject::OutlineInfo::DefaultShape)
    outlineInfo.shape = GObject::OutlineInfo::ArcShape;

  updateRegion ();
}

void GOval::update_segments () {
  float x, y;

  Rect r (sPoint, ePoint);
  r.normalize ();
  float a = r.width () / 2.0;
  float b = r.height () / 2.0;

  float angle = sAngle * M_PI / 180.0;
  x = a * cos (angle) + r.left () + a;
  y = b * sin (angle) + r.top () + b;

  segPoint[0].x (x);
  segPoint[0].y (y);

  angle = eAngle * M_PI / 180.0;
  x = a * cos (angle) + r.left () + a;
  y = b * sin (angle) + r.top () + b;

  segPoint[1].x (x);
  segPoint[1].y (y);

}

void GOval::writeToXml (XmlWriter& xml) {
  static const char* kind[] = { "full", "arc", "pie" };

  Rect r (sPoint, ePoint);
  Rect nr = r.normalize ();
  float w2 = nr.width () / 2.0, h2 = nr.height () / 2.0;

  xml.startTag ("ellipse", false);
  writePropertiesToXml (xml);
  xml.addAttribute ("x", nr.left () + w2);
  xml.addAttribute ("y", nr.top () + h2);
  xml.addAttribute ("rx", w2);
  xml.addAttribute ("ry", h2);
  xml.addAttribute ("angle1", sAngle);
  xml.addAttribute ("angle2", eAngle);
  xml.addAttribute ("kind", kind[outlineInfo.shape]);
  xml.closeTag (true);
}

void GOval::updateGradientShape (QPainter& p) {
  // define the rectangular box for the gradient pixmap 
  // (in object coordinate system)
  gShape.setBox (Rect (sPoint, ePoint));

  // define the clipping region
  QWMatrix matrix = p.worldMatrix ();
  QRect rect (qRound (sPoint.x ()), qRound (sPoint.y ()),
	      qRound (ePoint.x () - sPoint.x ()),
	      qRound (ePoint.y () - sPoint.y ()));
  switch (outlineInfo.shape) {
  case GObject::OutlineInfo::DefaultShape:
    gShape.setRegion (QRegion (matrix.map (rect), QRegion::Ellipse));
    break;
  default:
    break;
  }

  // update the gradient information
  gShape.setGradient (fillInfo.gradient);

  // and create a new gradient pixmap
  gShape.updatePixmap ();
}
