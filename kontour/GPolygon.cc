/* -*- C++ -*-

  $Id$
  This file is part of Kontour.
  Copyright (C) 1998 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
  Copyright (C) 2002 Igor Janssen (rm@kde.org)

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

#include "GPolygon.h"

#include <cmath>

#include <qdom.h>

#include <klocale.h>
#include <koVectorPath.h>
#include <koPainter.h>

#include "kontour_global.h"
#include "GPath.h"

GPolygon::GPolygon():
GObject()
{
}

GPolygon::GPolygon(const QDomElement &element):
GObject(element.namedItem("go").toElement())
{
  double cx = element.attribute("cx").toDouble();
  double cy = element.attribute("cy").toDouble();
  mCenter.setX(cx);
  mCenter.setY(cy);
  mVertex = element.attribute("n").toInt();
  mAAngle = element.attribute("aa").toDouble();
  mBAngle = element.attribute("ba").toDouble();
  mARadius = element.attribute("ra").toDouble();
  mBRadius = element.attribute("rb").toDouble();
  calcBoundingBox();
}

GPolygon::GPolygon(const GPolygon &obj):
GObject(obj)
{
  mVertex = obj.mVertex;
  mCenter = obj.mCenter;
  mAAngle = obj.mAAngle;
  mBAngle = obj.mBAngle;
  mARadius = obj.mARadius;
  mBRadius = obj.mBRadius;
  calcBoundingBox();
}

GObject *GPolygon::copy() const
{
  return new GPolygon(*this);
}

void GPolygon::createPolygon(const KoPoint &p, int n, double r)
{

}

QString GPolygon::typeName() const
{
  return i18n("Polygon");
}

QDomElement GPolygon::writeToXml(QDomDocument &document)
{
  QDomElement polygon = document.createElement("polygon");
  polygon.setAttribute("cx", mCenter.x());
  polygon.setAttribute("cy", mCenter.y());
  polygon.setAttribute("n", mVertex);
  polygon.setAttribute("aa", mAAngle);
  polygon.setAttribute("ab", mBAngle);
  polygon.setAttribute("ra", mARadius);
  polygon.setAttribute("rb", mBRadius);
  polygon.appendChild(GObject::writeToXml(document));
  return polygon;
}

void GPolygon::draw(KoPainter *p, const QWMatrix &m, bool withBasePoints, bool outline, bool)
{
  setPen(p);
  setBrush(p);
  KoVectorPath *v = new KoVectorPath;
  double a = 2.0 * Kontour::pi / static_cast<double>(mVertex);
  double caa = mAAngle;
  double cab = mBAngle;
  double x = mCenter.x() + mARadius * cos(caa);
  double y = mCenter.y() + mARadius * sin(caa);
  v->moveTo(x, y);
  for(int i = 1; i <= mVertex; i++)
  {
    x = mCenter.x() + mBRadius * cos(cab);
    y = mCenter.y() + mBRadius * sin(cab);
    v->lineTo(x, y);
    cab += a;
    caa += a;
    x = mCenter.x() + mARadius * cos(caa);
    y = mCenter.y() + mARadius * sin(caa);
    v->lineTo(x, y);
  }
  v->end();
  v->transform(tmpMatrix * m);
  p->drawVectorPath(v);
  delete v;
  if(withBasePoints)
  {
    KoPoint c;
    caa = mAAngle;
    cab = mBAngle;
    for(int i = 0; i < mVertex; i++)
    {
      c.setX(mCenter.x() + mARadius * cos(caa));
      c.setY(mCenter.y() + mARadius * sin(caa));
      c = c.transform(tmpMatrix * m);
      drawNode(p, static_cast<int>(c.x()), static_cast<int>(c.y()), false);
      c.setX(mCenter.x() + mBRadius * cos(cab));
      c.setY(mCenter.y() + mBRadius * sin(cab));
      c = c.transform(tmpMatrix * m);
      drawNode(p, static_cast<int>(c.x()), static_cast<int>(c.y()), false);
      caa += a;
      cab += a;
    }
  }
}

void GPolygon::calcBoundingBox()
{
  double xmax, xmin;
  double ymax, ymin;
  double a = 2.0 * Kontour::pi / static_cast<double>(mVertex);
  double caa = mAAngle;
  double cab = mBAngle;
  KoPoint c;
  c.setX(mCenter.x() + mARadius * cos(caa));
  c.setY(mCenter.y() + mARadius * sin(caa));
  c = c.transform(tmpMatrix);
  xmax = c.x();
  xmin = c.x();
  ymax = c.y();
  ymin = c.y();
  c.setX(mCenter.x() + mBRadius * cos(cab));
  c.setY(mCenter.y() + mBRadius * sin(cab));
  c = c.transform(tmpMatrix);
  if(c.x() < xmin)
    xmin = c.x();
  if(c.x() > xmax)
    xmax = c.x();
  if(c.y() < ymin)
    ymin = c.y();
  if(c.y() > ymax)
    ymax = c.y();
  for(int i = 1; i < mVertex; i++)
  {
    c.setX(mCenter.x() + mARadius * cos(caa));
    c.setY(mCenter.y() + mARadius * sin(caa));
    c = c.transform(tmpMatrix);
    if(c.x() < xmin)
      xmin = c.x();
    if(c.x() > xmax)
      xmax = c.x();
    if(c.y() < ymin)
      ymin = c.y();
    if(c.y() > ymax)
      ymax = c.y();
    c.setX(mCenter.x() + mBRadius * cos(cab));
    c.setY(mCenter.y() + mBRadius * sin(cab));
    c = c.transform(tmpMatrix);
    if(c.x() < xmin)
      xmin = c.x();
    if(c.x() > xmax)
      xmax = c.x();
    if(c.y() < ymin)
      ymin = c.y();
    if(c.y() > ymax)
      ymax = c.y();
    caa += a;
    cab += a;
  }
  box.setLeft(xmin);
  box.setRight(xmax);
  box.setTop(ymin);
  box.setBottom(ymax);
  adjustBBox(box);
}

int GPolygon::getNeighbourPoint(const KoPoint &p)
{
  return -1;
}

void GPolygon::movePoint(int idx, double dx, double dy, bool /*ctrlPressed*/)
{
}

void GPolygon::removePoint(int idx, bool update)
{
}

bool GPolygon::contains(const KoPoint &p)
{
/*  double x1, y1, x2, y2;

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
  }*/
  return false;
}

bool GPolygon::findNearestPoint(const KoPoint &p, double max_dist, double &dist, int &pidx, bool all)
{
  return true;
}

GPath *GPolygon::convertToPath() const
{
  GPath *path = new GPath(true);
  double a = 2.0 * Kontour::pi / static_cast<double>(mVertex);
  double caa = mAAngle;
  double cab = mBAngle;
  double x = mCenter.x() + mARadius * cos(caa);
  double y = mCenter.y() + mARadius * sin(caa);
  path->beginTo(x, y);
  for(int i = 1; i <= mVertex; i++)
  {
    x = mCenter.x() + mBRadius * cos(cab);
    y = mCenter.y() + mBRadius * sin(cab);
    path->lineTo(x, y);
    cab += a;
    caa += a;
    x = mCenter.x() + mARadius * cos(caa);
    y = mCenter.y() + mARadius * sin(caa);
    path->lineTo(x, y);
  }
  path->matrix(matrix());
  path->style(style());
  return path;
}

bool GPolygon::isConvertible() const
{
  return true;
}

#include "GPolygon.moc"
