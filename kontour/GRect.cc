/* -*- C++ -*-

  $Id$
  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2001-2002 Igor Janssen (rm@linux.ru.net)

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

GRect::GRect(bool roundness):
GObject()
{
  if(roundness)
  {
    mXRoundness = 0.25;
    mYRoundness = 0.25;
  }
  else
  {
    mXRoundness = 0.0;
    mYRoundness = 0.0;
  }
}

GRect::GRect(const QDomElement &element):
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
  calcBoundingBox();
}

GRect::GRect(const GRect &obj):
GObject(obj)
{
  ePoint = obj.ePoint;
  sPoint = obj.sPoint;
  mXRoundness = obj.mXRoundness;
  mYRoundness = obj.mYRoundness;
  calcBoundingBox();
}

GObject *GRect::copy() const
{
  return new GRect(*this);
}

bool GRect::isSquare() const
{
  if(ePoint.x() - sPoint.x() == ePoint.y() - sPoint.y())
    return true;
  else
    return false;
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
  if(isSquare())
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

void GRect::draw(KoPainter *p, const QWMatrix &m, bool withBasePoints, bool outline, bool)
{
  setPen(p);
  setBrush(p);
  KoVectorPath *v = KoVectorPath::rectangle(sPoint.x(), sPoint.y(), ePoint.x() - sPoint.x(), ePoint.y() - sPoint.y(), 0.5 * mXRoundness * (ePoint.x() - sPoint.x()), 0.5 * mYRoundness * (ePoint.y() - sPoint.y()));
  v->transform(tmpMatrix * m);
  p->drawVectorPath(v);
  delete v;
}

void GRect::calcBoundingBox()
{
  KoPoint p1(sPoint.x(), sPoint.y());
  KoPoint p2(ePoint.x(), sPoint.y());
  KoPoint p3(ePoint.x(), ePoint.y());
  KoPoint p4(sPoint.x(), ePoint.y());
  box = calcUntransformedBoundingBox(p1, p2, p3, p4);
  adjustBBox(box);
}

int GRect::getNeighbourPoint(const KoPoint &p)
{
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
  KoVectorPath *vec = new KoVectorPath;
  double rx = 0.5 * mXRoundness * (ePoint.x() - sPoint.x());
  double ry = 0.5 * mYRoundness * (ePoint.y() - sPoint.y());
  if(mXRoundness != 0.0 && mYRoundness != 0.0)
  {
    double w = ePoint.x() - sPoint.x();
    double h = ePoint.y() - sPoint.y();
    double rx = 0.5 * mXRoundness * w;
    double ry = 0.5 * mYRoundness * h;
    if(rx > w / 2)
      rx = w / 2;
    if(ry > h / 2)
      ry = h / 2;
    path->beginTo(sPoint.x() + rx, sPoint.y());
    path->curveTo(sPoint.x(), sPoint.y() + ry, sPoint.x() + rx * (1 - 0.552), sPoint.y(), sPoint.x(), sPoint.y() + ry * (1 - 0.552));
    if(ry < h / 2)
      path->lineTo(sPoint.x(), sPoint.y() + h - ry);
    path->curveTo(sPoint.x() + rx, sPoint.y() + h, sPoint.x(), sPoint.y() + h - ry * (1 - 0.552), sPoint.x() + rx * (1 - 0.552), sPoint.y() + h);
    if(rx < w / 2)
      path->lineTo(sPoint.x() + w - rx, sPoint.y() + h);
    path->curveTo(sPoint.x() + w, sPoint.y() + h - ry, sPoint.x() + w - rx * (1 - 0.552), sPoint.y() + h, sPoint.x() + w, sPoint.y() + h - ry * (1 - 0.552));
    if(ry < h / 2)
      path->lineTo(sPoint.x() + w, sPoint.y() + ry);
    path->curveTo(sPoint.x() + w - rx, sPoint.y(), sPoint.x() + w, sPoint.y() + ry * (1 - 0.552), sPoint.x() + w - rx * (1 - 0.552), sPoint.y());
    if(rx < w / 2)
      path->lineTo(sPoint.x() + rx, sPoint.y());
  }
  else
  {
    path->beginTo(sPoint.x(), sPoint.y());
    path->lineTo(sPoint.x(), ePoint.y());
    path->lineTo(ePoint.x(), ePoint.y());
    path->lineTo(ePoint.x(), sPoint.y());
    path->lineTo(sPoint.x(), sPoint.y());
  }
  path->matrix(matrix());
  path->style(style());
  return path;
}

bool GRect::isConvertible() const
{
 return true;
}

#include "GRect.moc"
