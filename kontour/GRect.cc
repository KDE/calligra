/* -*- C++ -*-

  $Id$
  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001 Igor Janssen (rm@linux.ru.net)

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

#include "GRect.h"

#include <math.h>

#include <qdom.h>

#include <klocale.h>
#include <koVectorPath.h>
#include <koPainter.h>
#include <kdebug.h>

#include "kontour_global.h"
#include "GPath.h"

GRect::GRect(bool sFlag):
GObject()
{
  squareFlag = sFlag;
  mType = Rectangle;
}

GRect::GRect(const QDomElement &element, bool sFlag):
GObject(element.namedItem("go").toElement())
{
  double x = 0;
  double y = 0;
  double sx = 0;
  double sy = 0;

  x = element.attribute("x").toDouble();
  y = element.attribute("y").toDouble();
  sx = element.attribute("sx").toDouble();
  sy = element.attribute("sy").toDouble();

  sPoint.setX(x);
  sPoint.setY(y);
  ePoint.setX(x + sx);
  ePoint.setY(y + sy);
  squareFlag = sFlag;
  calcBoundingBox();
}

GRect::GRect(const GRect &obj):
GObject(obj)
{
  squareFlag = obj.squareFlag;
  ePoint = obj.ePoint;
  sPoint = obj.sPoint;
  calcBoundingBox();
}

GObject *GRect::copy() const
{
  return new GRect(*this);
}

void GRect::type(Type t)
{
  mType = t;
}

void GRect::startPoint(const KoPoint &p)
{
  sPoint = p;
  calcBoundingBox();
}

void GRect::endPoint(const KoPoint &p)
{
  ePoint = p;
  calcBoundingBox();
}

QString GRect::typeName() const
{
  if(squareFlag)
    return i18n("Square");
  else
    return i18n("Rectangle");
}

QDomElement GRect::writeToXml(QDomDocument &document)
{
  KoRect r(sPoint, ePoint);
  r = r.normalize();

  QDomElement rect = document.createElement("rect");
  rect.setAttribute("x", r.left());
  rect.setAttribute("y", r.top());
  rect.setAttribute("sx", r.width());
  rect.setAttribute("sy", r.height());
  rect.appendChild(GObject::writeToXml(document));
  return rect;
}

void GRect::draw(KoPainter *p, int aXOffset, int aYOffset, bool withBasePoints, bool outline, bool)
{
  setPen(p);
  setBrush(p);
  KoVectorPath *v = KoVectorPath::rectangle(sPoint.x(), sPoint.y(), ePoint.x() - sPoint.x(), ePoint.y() - sPoint.y(), 0, 0);
  QWMatrix m;
  m = m.translate(aXOffset, aYOffset);
  v->transform(m * tmpMatrix);
  p->drawVectorPath(v);
}

void GRect::calcBoundingBox()
{
  KoPoint p1(sPoint.x(), sPoint.y());
  KoPoint p2(ePoint.x(), sPoint.y());
  KoPoint p3(ePoint.x(), ePoint.y());
  KoPoint p4(sPoint.x(), ePoint.y());
  adjustBBox(p1, p2, p3, p4);
  calcUntransformedBoundingBox(p1, p2, p3, p4);
}

int GRect::getNeighbourPoint(const KoPoint &p)
{
// TODO implement isNear() and transform()
/*  for(int i = 1; i >= 0; i--)
  {
    KoPoint c = segPoint[i].transform(tMatrix);
    if(c.isNear(p, Kontour::nearDistance))
      return i;
  }*/
  return -1;
}

void GRect::movePoint(int idx, double dx, double dy, bool /*ctrlPressed*/)
{
/*  double adx = fabs (dx);
  double ady = fabs (dy);
  double angle = 0;

  if (idx == 0 && segPoint[0] == segPoint[1])
    idx = 1;

  Rect r (sPoint, ePoint);
  r.normalize ();

  double a = r.width () / 2.0;
  double b = r.height () / 2.0;

  if (adx > ady) {
    double x = segPoint[idx].x () + dx;
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
    double y = segPoint[idx].y () + dy;
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
  double a1 = qRound (sAngle < 0 ? sAngle + 360 : sAngle);
  double a2 = qRound (eAngle < 0 ? eAngle + 360 : eAngle);
  if (a1 >= a2 - 1 && a1 <= a2 + 1) {
    eAngle = sAngle;
    outlineInfo.shape = GObject::OutlineInfo::DefaultShape;
  }
  else if (outlineInfo.shape == GObject::OutlineInfo::DefaultShape)
    outlineInfo.shape = GObject::OutlineInfo::ArcShape;

  gShape.setInvalid ();

  updateRegion ();*/
}

void GRect::removePoint(int idx, bool update)
{
}

bool GRect::contains(const KoPoint &p)
{
  double x1, y1, x2, y2;

  if(box.contains(p))
  {
    QPoint pp = iMatrix.map(QPoint(static_cast<int>(p.x()), static_cast<int>(p.y())));
    if(sPoint.x() >= ePoint.x())
    {
      x1 = ePoint.x();
      x2 = sPoint.x();
    }
    else
    {
      x1 = sPoint.x();
      x2 = ePoint.x();
    }
    if(sPoint.y() >= ePoint.y())
    {
      y1 = ePoint.y();
      y2 = sPoint.y();
    }
    else
    {
      y1 = sPoint.y();
      y2 = ePoint.y();
    }

    if(pp.x() <= x2 && pp.x() >= x1 && pp.y() <= y2 && pp.y() >= y1)
      return true;
  }
  return false;
}

bool GRect::findNearestPoint(const KoPoint &p, double max_dist, double &dist, int &pidx, bool all)
{
  return true;
}

GPath *GRect::convertToPath() const
{
  GPath *path = new GPath(true);
  path->beginTo(sPoint.x(), sPoint.y());
  path->lineTo(sPoint.x(), ePoint.y());
  path->lineTo(ePoint.x(), ePoint.y());
  path->lineTo(ePoint.x(), sPoint.y());
  path->lineTo(sPoint.x(), sPoint.y());
  path->matrix(matrix());
  path->style(style());
  return path;
}

bool GRect::isConvertible() const
{
 return true;
}

/*bool GRect::isValid()
{
  return (fabs(sPoint.x() - ePoint.x()) > 1 || fabs(sPoint.y() - ePoint.y()) > 1);
}*/


/*void GRect::updateGradientShape(QPainter &p)
{
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

      double a = fabs (sAngle - eAngle);

      // polygon for clipping
      QPointArray pnts (5);
      double x, y;

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
        double x1 = m.x () - segPoint[0].x ();
        double y1 = m.y () - segPoint[0].y ();

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

        x = x1 * cos (WINKEL) - y1 * sin (WINKEL);
        y = x1 * sin (WINKEL) + y1 * cos (WINKEL);

        x *= 1.5; y *= 1.5;

        x += segPoint[1].x ();
        y += segPoint[1].y ();

        pnts.setPoint (3, QPoint (qRound (x), qRound (y)));

        region = region.subtract (QRegion (matrix.map (pnts)));
      }
      else {
        // point #2
        double x1 = m.x () - segPoint[0].x ();
        double y1 = m.y () - segPoint[0].y ();

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

        x = x1 * cos (-WINKEL) - y1 * sin (-WINKEL);
        y = x1 * sin (-WINKEL) + y1 * cos (-WINKEL);

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
}*/

#include "GRect.moc"
