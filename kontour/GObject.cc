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

#include "GObject.h"

#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>

#include "GOval.h"
#include "GRect.h"
#include "GPath.h"

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
  initTmpMatrix();
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

void GObject::drawNode(QPainter &p, int x, int y, bool active)
{
  p.save();
  QWMatrix e;
  p.scale(1.0, 1.0);
  QBrush brush(Qt::magenta);
  p.setBrush(brush);
  p.setPen(Qt::black);
  p.drawRect(x - 3, y - 3, 7, 7);
  p.fillRect(x - 2, y - 2, 5, 5, brush);
  p.restore();
}

void GObject::setZoomFactor(double f, double pf)
{

}

void GObject::style(const GStyle *s)
{
  *mStyle = *s;
}

void GObject::matrix(QWMatrix m)
{
  kdDebug(38000) << "SET MATRIX!!!" << endl;
  tMatrix = m;
  iMatrix = tMatrix.invert();
  initTmpMatrix();
  calcBoundingBox();
}

void GObject::initTmpMatrix()
{
  tmpMatrix = tMatrix;
}

void GObject::transform(const QWMatrix &m, bool update)
{
  tMatrix = tMatrix * m;
  if(update)
  {
    iMatrix = tMatrix.invert();
    initTmpMatrix();
    calcBoundingBox();
  }
}

void GObject::ttransform(const QWMatrix &m, bool update)
{
  tmpMatrix = tmpMatrix * m;
  if(update)
    calcBoundingBox();
}
  
bool GObject::contains(const KoPoint &p)
{
  kdDebug(38000) << "GObject::contains" << endl;
  return box.contains(p);
}

bool GObject::intersects(const KoRect &r)
{
  return r.intersects(box);
}

GObject *GObject::objectFactory(const QDomElement &element)
{
  if(element.tagName() == "oval")
    return new GOval(element);
  else if(element.tagName() == "rect")
    return new GRect(element);
  else if(element.tagName() == "path")
    return new GPath(element);
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

void GObject::updateBoundingBox(const KoRect &r)
{
  box = r.normalize();
}

void GObject::updateBoundingBox(const KoPoint &p1, const KoPoint &p2)
{
  KoRect r(p1, p2);
  updateBoundingBox(r);
}

void GObject::calcUntransformedBoundingBox(const KoPoint &tleft, const KoPoint &tright, const KoPoint &bright, const KoPoint &bleft)
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
  kdDebug(38000) << "Rect: L = " << r.left() << endl;
  kdDebug(38000) << "Rect: T = " << r.top() << endl;
  kdDebug(38000) << "Rect: R = " << r.right() << endl;
  kdDebug(38000) << "Rect: B = " << r.bottom() << endl;
  updateBoundingBox(r);
}

void GObject::updateRegion(bool recalcBBox)
{
  kdDebug(38000) << "GObject::updateRegion()" << endl;
/*  KoRect newbox = boundingBox();//redrawBox();
  if(recalcBBox)
  {
    calcBoundingBox();
    newbox = boundingBox().unite(newbox);//redrawBox().unite(oldbox);
  }

  layer()->page()->updateHandle();
  layer()->page()->document()->emitChanged(newbox, true);*/
}

void GObject::invalidateClipRegion()
{

}

/*---------------

void GObject::invalidateClipRegion  () {
  if (gradientFill ())
    gShape.setInvalid ();
}
---------------------*/

void GObject::setPen(QPainter *p)
{
  // TODO : set dashes, arrows, linewidth etc.
  QPen pen;
  if(mStyle->stroked())
  {
    pen.setColor(mStyle->outlineColor().color());
    pen.setWidth(mStyle->outlineWidth());
    pen.setCapStyle(mStyle->capStyle());
    pen.setJoinStyle(mStyle->joinStyle());
  }
  else
    pen.setStyle(Qt::NoPen);
  p->setPen(pen);
}

void GObject::setBrush(QPainter *p)
{
  // TODO : patterns, gradients, noFill
  QBrush brush;
  if(mStyle->filled())
  {
    brush.setColor(mStyle->fillColor().color());
    brush.setStyle(mStyle->brushStyle());
  }
  p->setBrush(brush);
}

void GObject::adjustBBox(KoPoint &tleft, KoPoint &tright, KoPoint &bright, KoPoint &bleft)
{
  tleft.setX(tleft.x() - mStyle->outlineWidth() / 2);
  tleft.setY(tleft.y() - mStyle->outlineWidth() / 2);
  tright.setX(tright.x() + mStyle->outlineWidth() / 2);
  tright.setY(tright.y() - mStyle->outlineWidth() / 2);
  bright.setX(bright.x() + mStyle->outlineWidth() / 2);
  bright.setY(bright.y() + mStyle->outlineWidth() / 2);
  bleft.setX(bleft.x() - mStyle->outlineWidth() / 2);
  bleft.setY(bleft.y() + mStyle->outlineWidth() / 2);
}

#include "GObject.moc"
