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

#include "GObject.h"

#include <qdom.h>

#include <koPainter.h>
#include <kdebug.h>

#include "GOval.h"
#include "GRect.h"
#include "GPolygon.h"
#include "GPath.h"
#include "GImage.h"
#include "GGroup.h"

unsigned int GObject::mCurId = 0;

GObject::GObject()
{ 
  mId = mCurId;
  mCurId++;
  rcount = 0;
  mLayer = 0L;
  sflag = false;
  inWork = false;
  mStyle = new GStyle;
}

GObject::GObject(const QDomElement &element)
{
  rcount = 0;
  mLayer = 0L;
  sflag = false;
  inWork = false;
  mId = element.attribute("id").toUInt();
  mStyle = new GStyle(element.namedItem("style").toElement());
  tMatrix = toMatrix(element.namedItem("matrix").toElement());
  iMatrix = tMatrix.invert();
  tmpMatrix = tMatrix;
}

GObject::GObject(const GObject &obj)
{
  mId = mCurId;
  mCurId++;
  rcount = 0;
  mLayer = obj.mLayer;
  sflag = false;
  inWork = false;
  mStyle = new GStyle;
  *mStyle = *obj.mStyle;
  tMatrix = obj.tMatrix;
  tmpMatrix = tMatrix;
  iMatrix = obj.iMatrix;
}

GObject::~GObject()
{
  kdDebug(38000) << "GObject::~GObject()" << endl;
}

void GObject::ref()
{
  rcount++;
  kdDebug(38000) << "REF: " << rcount << endl;
}

void GObject::unref()
{
  if(--rcount == 0)
    delete this;
  kdDebug(38000) << "id = " << mId << " UNREF: " << rcount << endl;
}

void GObject::layer(GLayer *l)
{
  mLayer = l;
  if(l == 0L)
    emit deleted();
}

void GObject::select(bool flag)
{
  sflag = flag;
}

void GObject::setWorkInProgress(bool flag)
{
  inWork = flag;
}
  
QDomElement GObject::writeToXml(QDomDocument &document)
{
  QDomElement go = document.createElement("go");
  go.setAttribute("id", mId);
  go.appendChild(createMatrixElement(tMatrix, document));
  go.appendChild(mStyle->writeToXml(document));
  return go;
}

void GObject::drawNode(KoPainter *p, int x, int y, bool active)
{
// TODO Fix!
  p->fillAreaRGB(QRect(x - 2, y - 3, 5, 7), KoColor::black());
  p->fillAreaRGB(QRect(x - 3, y - 2, 7, 5), KoColor::black());
  p->fillAreaRGB(QRect(x - 2, y - 2, 5, 5), KoColor::magenta());
}

void GObject::setZoomFactor(double f, double pf)
{

}

void GObject::style(const GStyle *s)
{
  *mStyle = *s;
  calcBoundingBox();
}

void GObject::matrix(QWMatrix m)
{
  tMatrix = m;
  iMatrix = tMatrix.invert();
  initTmpMatrix();
  calcBoundingBox();
}

void GObject::initTmpMatrix()
{
  tmpMatrix = tMatrix;
  calcBoundingBox();
}

void GObject::transform(const QWMatrix &m)
{
  tMatrix = tMatrix * m;
  iMatrix = tMatrix.invert();
  initTmpMatrix();
}

void GObject::ttransform(const QWMatrix &m)
{
  tmpMatrix = tmpMatrix * m;
  calcBoundingBox();
}

GObject *GObject::objectFactory(const QDomElement &element)
{
  if(element.tagName() == "rect")
    return new GRect(element);
  else if(element.tagName() == "oval")
    return new GOval(element);
  else if(element.tagName() == "polygon")
    return new GPolygon(element);
  else if(element.tagName() == "path")
    return new GPath(element);
  else if(element.tagName() == "image")
    return new GImage(element);
  else if(element.tagName() == "group")
    return new GGroup(element);
  else
    return 0L;
}

QDomElement GObject::createMatrixElement(const QWMatrix &matrix, QDomDocument &document)
{
  QDomElement m = document.createElement("matrix");
  m.setAttribute("m11", matrix.m11());
  m.setAttribute("m12", matrix.m12());
  m.setAttribute("m21", matrix.m21());
  m.setAttribute("m22", matrix.m22());
  m.setAttribute("dx", matrix.dx());
  m.setAttribute("dy", matrix.dy());
  return m;
}

QWMatrix GObject::toMatrix(const QDomElement &matrix)
{
  if(matrix.isNull())
    return QWMatrix();
  double m11 = matrix.attribute("m11").toDouble();
  double m12 = matrix.attribute("m12").toDouble();
  double m21 = matrix.attribute("m21").toDouble();
  double m22 = matrix.attribute("m22").toDouble();
  double dx = matrix.attribute("dx").toDouble();
  double dy = matrix.attribute("dy").toDouble();
  return QWMatrix(m11, m12, m21, m22, dx, dy);
}

KoRect GObject::calcUntransformedBoundingBox(const KoPoint &tleft, const KoPoint &tright, const KoPoint &bright, const KoPoint &bleft)
{
  KoPoint p[4];
  KoRect r;

  p[0] = tleft.transform(tmpMatrix);
  p[1] = tright.transform(tmpMatrix);
  p[2] = bleft.transform(tmpMatrix);
  p[3] = bright.transform(tmpMatrix);

  r.setLeft(p[0].x());
  r.setTop(p[0].y());
  r.setRight(p[0].x());
  r.setBottom(p[0].y());

  for(unsigned int i = 1; i < 4; i++)
  {
    r.setLeft(QMIN(p[i].x(), r.left()));
    r.setTop(QMIN(p[i].y(), r.top()));
    r.setRight(QMAX(p[i].x(), r.right()));
    r.setBottom(QMAX(p[i].y(), r.bottom()));
  }
  return r;
}

void GObject::setPen(KoPainter *p)
{
  p->outline(mStyle->outline());
}

void GObject::setBrush(KoPainter *p)
{
  p->fill(mStyle->fill());
}

void GObject::adjustBBox(KoRect &rect)
{
  double t = mStyle->outlineWidth() * 0.5;
  rect.setLeft(rect.left() - t);
  rect.setRight(rect.right() + t);
  rect.setTop(rect.top() - t);
  rect.setBottom(rect.bottom() + t);
}

#include "GObject.moc"
