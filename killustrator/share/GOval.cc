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

#include <math.h>
#include <GOval.h>
#include <GPolyline.h>
#include <GCurve.h>
#include <Painter.h>

#include <qdom.h>
#include <qstring.h>
#include <kapp.h>
#include <qpainter.h>
#include <klocale.h>

GOval::GOval (bool cFlag) : circleFlag (cFlag) {
  sAngle = eAngle = 270;
}

GOval::GOval (const QDomElement &element, bool cFlag)
  : GObject (element.namedItem("gobject").toElement()) {

      float x = 0, y = 0, rx = 0, ry = 0;
      sAngle = eAngle = 270;

      x=element.attribute("x").toFloat();
      y=element.attribute("y").toFloat();
      rx=element.attribute("rx").toFloat();
      ry=element.attribute("ry").toFloat();
      sAngle=element.attribute("angle1").toFloat();
      eAngle=element.attribute("angle2").toFloat();
      QString v=element.attribute("kind");
      if (v == "arc")
          outlineInfo.shape = GObject::OutlineInfo::ArcShape;
      else if (v == "pie")
          outlineInfo.shape = GObject::OutlineInfo::PieShape;
      else
          outlineInfo.shape = GObject::OutlineInfo::DefaultShape;

      sPoint.x (x - rx); sPoint.y (y - ry);
      ePoint.x (x + rx); ePoint.y (y + ry);
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

QString GOval::typeName () const {
  if (circleFlag)
    return i18n("Circle");
  else
    return i18n("Ellipse");
}

void GOval::draw (QPainter& p, bool withBasePoints, bool outline) {
  float alen = 0;
  QPen pen;
  QBrush brush;

  initPen (pen);
  p.save ();
  p.setPen (pen);
  p.setWorldMatrix (tmpMatrix, true);

  if (! workInProgress () && ! outline) {
    initBrush (brush);
    p.setBrush (brush);
    if (gradientFill () &&
        outlineInfo.shape != GObject::OutlineInfo::ArcShape) {
      if (! gShape.valid ())
        updateGradientShape (p);
      gShape.draw (p);
    }
  }

  switch (outlineInfo.shape) {
  case GObject::OutlineInfo::DefaultShape:
    Painter::drawEllipse (p, sPoint.x (), sPoint.y (),
                          ePoint.x () - sPoint.x (),
                          ePoint.y () - sPoint.y ());
    break;
  case GObject::OutlineInfo::PieShape:
    alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    Painter::drawPie (p, sPoint.x (), sPoint.y (),
                      ePoint.x () - sPoint.x (),
                      ePoint.y () - sPoint.y (),
                      -eAngle * 16, -alen * 16);
    break;
  case GObject::OutlineInfo::ArcShape:
    alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    Painter::drawArc (p, sPoint.x (), sPoint.y (),
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
      Painter::drawRect (p, c.x () - 2.0, c.y () - 2.0, 4, 4);
    }
  }
  p.restore ();
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

GObject* GOval::clone (const QDomElement &element) {
  return new GOval (element);
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
  float adx = std::fabs (dx);
  float ady = std::fabs (dy);
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
    angle = std::acos (x / a) * RAD_FACTOR;
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
    angle = std::asin (y / b) * RAD_FACTOR;
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

  gShape.setInvalid ();

  updateRegion ();
}

void GOval::update_segments () {
  float x, y;

  Rect r (sPoint, ePoint);
  r.normalize ();
  float a = r.width () / 2.0;
  float b = r.height () / 2.0;

  float angle = sAngle * M_PI / 180.0;
  x = a * std::cos (angle) + r.left () + a;
  y = b * std::sin (angle) + r.top () + b;

  segPoint[0].x (x);
  segPoint[0].y (y);

  angle = eAngle * M_PI / 180.0;
  x = a * std::cos (angle) + r.left () + a;
  y = b * std::sin (angle) + r.top () + b;

  segPoint[1].x (x);
  segPoint[1].y (y);

}

QDomElement GOval::writeToXml (QDomDocument &document) {
  static const char* kind[] = { "full", "arc", "pie" };

  Rect r (sPoint, ePoint);
  Rect nr = r.normalize ();
  float w2 = nr.width () / 2.0, h2 = nr.height () / 2.0;

  QDomElement element=document.createElement("ellipse");
  element.setAttribute ("x", nr.left () + w2);
  element.setAttribute ("y", nr.top () + h2);
  element.setAttribute ("rx", w2);
  element.setAttribute ("ry", h2);
  element.setAttribute ("angle1", sAngle);
  element.setAttribute ("angle2", eAngle);
  element.setAttribute ("kind", kind[outlineInfo.shape]);
  element.appendChild(GObject::writeToXml(document));
  return element;
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
      {
          QPointArray pnts;
          pnts.makeEllipse (rect.x (), rect.y (),
                            rect.width (), rect.height ());
          gShape.setRegion (QRegion (matrix.map (pnts)));
          break;
      }
  case GObject::OutlineInfo::PieShape:
    {
        QPointArray epnts;
        epnts.makeEllipse (rect.x (), rect.y (),
                           rect.width (), rect.height ());
        QRegion region (matrix.map (epnts));

      float a = fabs (sAngle - eAngle);

      // polygon for clipping
      QPointArray pnts (5);
      float x, y;

      // center
      Rect r (sPoint, ePoint);
      r = r.normalize ();
      Coord m = r.center ();

      // point #0: center
      pnts.setPoint (0, QPoint (qRound (m.x ()), qRound (m.y ())));
      // point #1: segPoint[0]
      pnts.setPoint (1, QPoint (qRound (segPoint[0].x ()),
                                qRound (segPoint[0].y ())));
      // point #4: segPoint[1]
      pnts.setPoint (4, QPoint (qRound (segPoint[1].x ()),
                                qRound (segPoint[1].y ())));

      if ((sAngle >= eAngle && a >= 180) || (sAngle < eAngle && a <= 180)) {
#define WINKEL 90.0
        // point #2
        float x1 = m.x () - segPoint[0].x ();
        float y1 = m.y () - segPoint[0].y ();

        // x = x1 * cos (-WINKEL) - y1 * sin (-WINKEL);
        // y = x1 * sin (-WINKEL) + y1 * cos (-WINKEL);
        x = y1; y = -x1;

        x *= 1.5; y *= 1.5;

        x += segPoint[0].x ();
        y += segPoint[0].y ();

        pnts.setPoint (2, QPoint (qRound (x), qRound (y)));

        // point #3
        x1 = m.x () - segPoint[1].x ();
        y1 = m.y () - segPoint[1].y ();

        x = x1 * std::cos (WINKEL) - y1 * std::sin (WINKEL);
        y = x1 * std::sin (WINKEL) + y1 * std::cos (WINKEL);

        x *= 1.5; y *= 1.5;

        x += segPoint[1].x ();
        y += segPoint[1].y ();

        pnts.setPoint (3, QPoint (qRound (x), qRound (y)));

        region = region.subtract (QRegion (matrix.map (pnts)));
      }
      else {
        // point #2
        float x1 = m.x () - segPoint[0].x ();
        float y1 = m.y () - segPoint[0].y ();

        // x = x1 * cos (WINKEL) - y1 * sin (WINKEL);
        // y = x1 * sin (WINKEL) + y1 * cos (WINKEL);
        x = -y1;
        y = x1;

        x *= 1.5; y *= 1.5;

        x += segPoint[0].x ();
        y += segPoint[0].y ();

        pnts.setPoint (2, QPoint (qRound (x), qRound (y)));

        // point #3
        x1 = m.x () - segPoint[1].x ();
        y1 = m.y () - segPoint[1].y ();

        x = x1 * std::cos (-WINKEL) - y1 * std::sin (-WINKEL);
        y = x1 * std::sin (-WINKEL) + y1 * std::cos (-WINKEL);

        x *= 1.5; y *= 1.5;

        x += segPoint[1].x ();
        y += segPoint[1].y ();

        pnts.setPoint (3, QPoint (qRound (x), qRound (y)));
        region = region.intersect (QRegion (matrix.map (pnts)));
      }
      gShape.setRegion (region);
      break;
    }
  default:
    return;
    break;
  }

  // update the gradient information
  gShape.setGradient (fillInfo.gradient);

  // and create a new gradient pixmap
  gShape.updatePixmap ();
}

void GOval::getPath (QValueList<Coord>& path) {
  // this is not the right way.
  // we should reimplement this !!
  QPointArray parray;
  if (outlineInfo.shape == GObject::OutlineInfo::DefaultShape)
    parray.makeArc (sPoint.x (), sPoint.y (),
                    ePoint.x () - sPoint.x (),
                    ePoint.y () - sPoint.y (),
                    -180 * 16, -360 * 16);
  else {
    float alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    parray.makeArc (sPoint.x (), sPoint.y (),
                    ePoint.x () - sPoint.x (),
                    ePoint.y () - sPoint.y (), -eAngle * 16,
                    -alen * 16);
  }
  unsigned int num = parray.size ();
  for (unsigned int i = 0; i < num; i++) {
    const QPoint& p = parray[i];
    Coord pi (p.x (), p.y ());
    path.append(pi.transform (tMatrix));
  }
}

GCurve* GOval::convertToCurve () const {
  // this is not the right way.
  // we should reimplement this !!
  QPointArray parray;
  if (outlineInfo.shape == GObject::OutlineInfo::DefaultShape)
    parray.makeArc (sPoint.x (), sPoint.y (),
                    ePoint.x () - sPoint.x (),
                    ePoint.y () - sPoint.y (),
                    -180 * 16, -360 * 16);
  else {
    float alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    parray.makeArc (sPoint.x (), sPoint.y (),
                    ePoint.x () - sPoint.x (),
                    ePoint.y () - sPoint.y (), -eAngle * 16,
                    -alen * 16);
  }
  unsigned int num = parray.size ();
  GCurve* curve = new GCurve ();

  QApplication::setOverrideCursor(Qt::waitCursor);
  Coord p0 (parray[0].x (), parray[0].y ());
  p0 = p0.transform (tmpMatrix);
  Coord p = p0;
  for (unsigned int i = 1; i < num; i++) {
    Coord p1 (parray[i].x (), parray[i].y ());
    p1 = p1.transform (tmpMatrix);
    curve->addLineSegment (p0, p1);
    p0 = p1;
  }
  curve->addLineSegment (p0, p);
  curve->setClosed (true);
  curve->setOutlineInfo (outlineInfo);
  curve->setFillInfo (fillInfo);
  QApplication::restoreOverrideCursor();
  kapp->processEvents();
  return curve;
}

bool GOval::isValid () {
  return (fabs (sPoint.x () - ePoint.x ()) > 1 ||
          fabs (sPoint.y () - ePoint.y ()) > 1);
}

#include <GOval.moc>
