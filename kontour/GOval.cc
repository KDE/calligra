/* -*- C++ -*-

  $Id$
  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@kde.org)

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

#include "GOval.h"

#include <math.h>

#include <qdom.h>

#include <klocale.h>
#include <koVectorPath.h>
#include <koPainter.h>

#include "kontour_global.h"
#include "GPath.h"

GOval::GOval(bool cFlag):
GObject()
{
  circleFlag = cFlag;
  mType = Ellipse;
  sAngle = 270.0;
  eAngle = 270.0;
}

GOval::GOval(const QDomElement &element, bool cFlag):
GObject(element.namedItem("go").toElement())
{
  double x = 0;
  double y = 0;
  double rx = 0;
  double ry = 0;
  sAngle = eAngle = 270;

  x = element.attribute("x").toDouble();
  y = element.attribute("y").toDouble();
  rx = element.attribute("rx").toDouble();
  ry = element.attribute("ry").toDouble();
  sAngle = element.attribute("sa").toDouble();
  eAngle = element.attribute("ea").toDouble();

  sPoint.setX(x - rx);
  sPoint.setY(y - ry);
  ePoint.setX(x + rx);
  ePoint.setY(y + ry);
  circleFlag = cFlag;
  calcBoundingBox();
}

GOval::GOval(const GOval &obj):
GObject(obj)
{
  circleFlag = obj.circleFlag;
  ePoint = obj.ePoint;
  sPoint = obj.sPoint;
  sAngle = obj.sAngle;
  eAngle = obj.eAngle;
  calcBoundingBox();
}

GObject *GOval::copy() const
{
  return new GOval(*this);
}

void GOval::type(Type t)
{
  mType = t;
}

void GOval::setPoints(const KoPoint &p1, const KoPoint &p2)
{
  double xmin = p1.x();
  double xmax = p2.x();
  double ymin = p1.y();
  double ymax = p2.y();
  if(xmin > xmax)
  {
    double s = xmax;
    xmax = xmin;
    xmin = s;
  }
  if(ymin > ymax)
  {
    double s = ymax;
    ymax = ymin;
    ymin = s;
  }
  sPoint.setX(xmin);
  sPoint.setY(ymin);
  ePoint.setX(xmax);
  ePoint.setY(ymax);
  calcBoundingBox();
}

void GOval::setAngles(const double sa, const double ea)
{
  sAngle = sa;
  eAngle = ea;
}

QString GOval::typeName() const
{
  if(circleFlag)
    return i18n("Circle");
  else
    return i18n("Ellipse");
}

QDomElement GOval::writeToXml(QDomDocument &document)
{
  KoRect r(sPoint, ePoint);
  QDomElement oval = document.createElement("oval");
  oval.setAttribute("x", r.left() + r.width() / 2.0);
  oval.setAttribute("y", r.top() + r.height() / 2.0);
  oval.setAttribute("rx", r.width() / 2.0);
  oval.setAttribute("ry", r.height() / 2.0);
  oval.setAttribute("sa", sAngle);
  oval.setAttribute("ea", eAngle);
  oval.appendChild(GObject::writeToXml(document));
  return oval;
}

void GOval::draw(KoPainter *p, const QWMatrix &m, bool withBasePoints, bool outline, bool)
{
  setPen(p);
  setBrush(p);
  double rx = (ePoint.x() - sPoint.x()) * 0.5;
  double ry = (ePoint.y() - sPoint.y()) * 0.5;
  double cx = (ePoint.x() + sPoint.x()) * 0.5;
  double cy = (ePoint.y() + sPoint.y()) * 0.5;
  KoVectorPath *v = KoVectorPath::ellipse(cx, cy, rx, ry);
  v->transform(tmpMatrix * m);
  p->drawVectorPath(v);
  delete v;
}

void GOval::calcBoundingBox()
{
  KoPoint p1(sPoint.x(), sPoint.y());
  KoPoint p2(ePoint.x(), sPoint.y());
  KoPoint p3(ePoint.x(), ePoint.y());
  KoPoint p4(sPoint.x(), ePoint.y());
  box = calcUntransformedBoundingBox(p1, p2, p3, p4);
  adjustBBox(box);

  double x, y;

  KoRect r(sPoint, ePoint);
  double a = r.width() / 2.0;
  double b = r.height() / 2.0;

  double angle = sAngle * M_PI / 180.0;
  x = a * cos(angle) + r.left() + a;
  y = b * sin(angle) + r.top() + b;

  segPoint[0].setX(x);
  segPoint[0].setY(y);

  angle = eAngle * M_PI / 180.0;
  x = a * cos(angle) + r.left () + a;
  y = b * sin(angle) + r.top () + b;

  segPoint[1].setX(x);
  segPoint[1].setY(y);
}

int GOval::getNeighbourPoint(const KoPoint &p)
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

void GOval::movePoint (int idx, double dx, double dy, bool /*ctrlPressed*/)
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

void GOval::removePoint(int idx, bool update)
{
}

bool GOval::contains(const KoPoint &p)
{
  double x1, y1, x2, y2;

  if(box.contains(p))
  {
    QPoint pp = iMatrix.map(QPoint(static_cast<int>(p.x()), static_cast<int>(p.y())));
    x1 = sPoint.x();
    x2 = ePoint.x();
    if(x1 >= x2)
    {
      x1 = x2;
      x2 = sPoint.x();
      y1 = ePoint.y();
      y2 = sPoint.y();
    }
    else
    {
      y1 = sPoint.y();
      y2 = ePoint.y();
    }

    double x, a, b, sqr;
    double mx, my;

    mx = (x1 + x2) / 2;
    my = (y1 + y2) / 2;
    a = (x2 - x1) / 2;
    b = (y2 - y1) / 2;
    x = pp.x();
    if(x1 <= x && x <= x2)
    {
      sqr = sqrt((1 - ((x - mx) * (x - mx)) / (a * a)) * (b * b));
      if(my - sqr <= pp.y() && pp.y() <= my + sqr)
        return true;
    }
  }
  return false;
}

bool GOval::findNearestPoint(const KoPoint &p, double max_dist, double &dist, int &pidx, bool all)
{
  return true;
}

GPath *GOval::convertToPath() const
{
  double w = ePoint.x() - sPoint.x();
  double h = ePoint.y() - sPoint.y();
  double cx = (ePoint.x() + sPoint.x()) / 2.0;
  double cy = (ePoint.y() + sPoint.y()) / 2.0;
  GPath *path = new GPath(true);
  path->beginTo(-0.5, 0.0);
  path->arcTo(-0.5, 0.5, 0.0, 0.5, 0.5);
  path->arcTo(0.5, 0.5, 0.5, 0.0, 0.5);
  path->arcTo(0.5, -0.5, 0.0, -0.5, 0.5);
  path->arcTo(-0.5, -0.5, -0.5, 0.0, 0.5);
  QWMatrix m;
  m.translate(cx, cy);
  m.scale(w, h);
  path->matrix(m * matrix());
  path->style(style());
  return path;
}

bool GOval::isConvertible() const
{
  return true;
}

#include "GOval.moc"
