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

#include <cmath>

#include <qdom.h>

#include <klocale.h>
#include <koVectorPath.h>
#include <koPainter.h>
#include <kdebug.h>

#include "kontour_global.h"
#include "GPath.h"

GRect::GRect(double sx, double sy, bool roundness):
GObject()
{
  mSX = sx;
  mSY = sy;
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
  mSX = element.attribute("sx").toDouble();
  mSY = element.attribute("sy").toDouble();
  calcBoundingBox();
}

GRect::GRect(const GRect &obj):
GObject(obj)
{
  mSX = obj.mSX;
  mSY = obj.mSY;
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
  if(mSX == mSY)
    return true;
  else
    return false;
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
  QDomElement rect = document.createElement("rect");
  rect.setAttribute("sx", mSX);
  rect.setAttribute("sy", mSY);
  rect.appendChild(GObject::writeToXml(document));
  return rect;
}

void GRect::draw(KoPainter *p, const QWMatrix &m, bool withBasePoints, bool outline, bool)
{
  setPen(p);
  setBrush(p);
  KoVectorPath *v = KoVectorPath::rectangle(0.0, 0.0, mSX, mSY, 0.5 * mXRoundness * mSX, 0.5 * mYRoundness * mSY);
  v->transform(tmpMatrix * m);
  p->drawVectorPath(v);
  delete v;
}

void GRect::calcBoundingBox()
{
  KoPoint p1(0.0, 0.0);
  KoPoint p2(0.0, mSY);
  KoPoint p3(mSX, mSY);
  KoPoint p4(mSX, 0.0);
  mSBox = calcUntransformedBoundingBox(p1, p2, p3, p4);
  mBBox = mSBox;
  adjustBBox(mBBox);
}

int GRect::getNeighbourPoint(const KoPoint &p, const double distance)
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

void GRect::removePoint(int idx)
{
}

bool GRect::contains(const KoPoint &p)
{
  QPoint pp = iMatrix.map(QPoint(static_cast<int>(p.x()), static_cast<int>(p.y())));
  if(pp.x() <= mSX && pp.x() >= 0.0 && pp.y() <= mSY && pp.y() >= 0.0)
    return true;
  else
    return false;
}

GPath *GRect::convertToPath() const
{
  GPath *path = new GPath();
  double rx = 0.5 * mXRoundness * mSX;
  double ry = 0.5 * mYRoundness * mSY;
  if(mXRoundness != 0.0 && mYRoundness != 0.0)
  {
    if(rx > mSX / 2.0)
      rx = mSX / 2.0;
    if(ry > mSY / 2.0)
      ry = mSY / 2.0;
    path->beginTo(rx, 0.0);
    path->curveTo(0.0, ry, rx * 0.448, 0.0, 0.0, ry * 0.448);
    if(ry < mSY / 2.0)
      path->lineTo(0.0, mSY - ry);
    path->curveTo(rx, mSY, 0.0, mSY - ry * 0.448, rx * 0.448, mSY);
    if(rx < mSX / 2.0)
      path->lineTo(mSX - rx, mSY);
    path->curveTo(mSX, mSY - ry, mSX - rx * 0.448, mSY, mSX, mSY - ry * 0.448);
    if(ry < mSY / 2.0)
      path->lineTo(mSX, ry);
    path->curveTo(mSX - rx, 0.0, mSX, ry * 0.448, mSX - rx * 0.448, 0.0);
    if(rx < mSX / 2.0)
      path->lineTo(rx, 0.0);
  }
  else
  {
    path->beginTo(0.0, 0.0);
    path->lineTo(mSX, 0.0);
    path->lineTo(mSX, mSY);
    path->lineTo(0.0, mSY);
    path->lineTo(0.0, 0.0);
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
