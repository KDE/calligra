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

GPolygon::GPolygon(int n, double r, double a):
GObject()
{
  double k = Kontour::pi / static_cast<double>(n);
  mVertex = n;
  mAAngle = a;
  mBAngle = a + k;
  mARadius = r;
  mBRadius = r * cos(k);
  calcBoundingBox();
}

GPolygon::GPolygon(int n, double r1, double r2, double a):
GObject()
{
  double k = Kontour::pi / static_cast<double>(n);
  mVertex = n;
  mAAngle = a;
  mBAngle = a + k;
  mARadius = r1;
  mBRadius = r2;
  calcBoundingBox();
}

GPolygon::GPolygon(const QDomElement &element):
GObject(element.namedItem("go").toElement())
{
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

QString GPolygon::typeName() const
{
  return i18n("Polygon");
}

QDomElement GPolygon::writeToXml(QDomDocument &document)
{
  QDomElement polygon = document.createElement("polygon");
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
  double x = mARadius * cos(caa);
  double y = mARadius * sin(caa);
  v->moveTo(x, y);
  for(int i = 1; i <= mVertex; i++)
  {
    x = mBRadius * cos(cab);
    y = mBRadius * sin(cab);
    v->lineTo(x, y);
    cab += a;
    caa += a;
    x = mARadius * cos(caa);
    y = mARadius * sin(caa);
    v->lineTo(x, y);
  }
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
      c.setX(mARadius * cos(caa));
      c.setY(mARadius * sin(caa));
      c = c.transform(tmpMatrix * m);
      drawNode(p, static_cast<int>(c.x()), static_cast<int>(c.y()), false);
      c.setX(mBRadius * cos(cab));
      c.setY(mBRadius * sin(cab));
      c = c.transform(tmpMatrix * m);
      drawNode(p, static_cast<int>(c.x()), static_cast<int>(c.y()), false);
      caa += a;
      cab += a;
    }
  }
}

int GPolygon::getNeighbourPoint(const KoPoint &p, const double distance)
{
  return -1;
}

void GPolygon::movePoint(int idx, double dx, double dy, bool /*ctrlPressed*/)
{
//  if(idx < mVertex)  
}

void GPolygon::removePoint(int idx)
{
}

bool GPolygon::contains(const KoPoint &p)
{
  // TODO Implement...
  return false;
}

void GPolygon::calcBoundingBox()
{
  double xmax, xmin;
  double ymax, ymin;
  double a = 2.0 * Kontour::pi / static_cast<double>(mVertex);
  double caa = mAAngle;
  double cab = mBAngle;
  KoPoint c;
  c.setX(mARadius * cos(caa));
  c.setY(mARadius * sin(caa));
  c = c.transform(tmpMatrix);
  xmax = c.x();
  xmin = c.x();
  ymax = c.y();
  ymin = c.y();
  c.setX(mBRadius * cos(cab));
  c.setY(mBRadius * sin(cab));
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
    caa += a;
    cab += a;
    c.setX(mARadius * cos(caa));
    c.setY(mARadius * sin(caa));
    c = c.transform(tmpMatrix);
    if(c.x() < xmin)
      xmin = c.x();
    if(c.x() > xmax)
      xmax = c.x();
    if(c.y() < ymin)
      ymin = c.y();
    if(c.y() > ymax)
      ymax = c.y();
    c.setX(mBRadius * cos(cab));
    c.setY(mBRadius * sin(cab));
    c = c.transform(tmpMatrix);
    if(c.x() < xmin)
      xmin = c.x();
    if(c.x() > xmax)
      xmax = c.x();
    if(c.y() < ymin)
      ymin = c.y();
    if(c.y() > ymax)
      ymax = c.y();
  }
  mSBox.setLeft(xmin);
  mSBox.setRight(xmax);
  mSBox.setTop(ymin);
  mSBox.setBottom(ymax);
  mBBox = mSBox;
  adjustBBox(mBBox);
}

GPath *GPolygon::convertToPath() const
{
  GPath *path = new GPath();
  double a = 2.0 * Kontour::pi / static_cast<double>(mVertex);
  double caa = mAAngle;
  double cab = mBAngle;
  double x = mARadius * cos(caa);
  double y = mARadius * sin(caa);
  path->beginTo(x, y);
  for(int i = 1; i <= mVertex; i++)
  {
    x = mBRadius * cos(cab);
    y = mBRadius * sin(cab);
    path->lineTo(x, y);
    cab += a;
    caa += a;
    x = mARadius * cos(caa);
    y = mARadius * sin(caa);
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
