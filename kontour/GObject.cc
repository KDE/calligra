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

#include "GObject.h"

#include <qdom.h>

GObject::GObject()
{ 
  mLayer = 0L;
  sflag = false;
  inWork = false;
}

GObject::GObject(const QDomElement &element)
{
  mLayer = 0L;
  sflag = false;
  inWork = false;
  //transform(toMatrix(element.namedItem("matrix").toElement()), false);
}

GObject::GObject(const GObject& obj)
{
  mLayer = obj.mLayer;
  sflag = false;
  inWork = false;
  st = obj.st;
  tMatrix = obj.tMatrix;
  tmpMatrix = tMatrix;
  iMatrix = obj.iMatrix;
}

GObject::~GObject()
{
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
  QDomElement obj = document.createElement("obj");
 // if(hasId())
//    element.setAttribute ("id", id);
    // This is strange, because it's done by the child class itself... I'll clean up later (Werner)
//  if(hasRefId())
//    obj.setAttribute("ref", getRefId());
	
  obj.appendChild(createMatrixElement(tMatrix, document));
  return obj;
}

void GObject::setZoomFactor(double f, double pf)
{

}
  
void GObject::initTmpMatrix()
{
  tmpMatrix = tMatrix;
}

void GObject::transform(const QWMatrix &m, bool update = false)
{
  tMatrix = tMatrix * m;
  iMatrix = tMatrix.invert();
  initTmpMatrix ();
//  gShape.setInvalid();
  if(update)
    updateRegion();
}

void GObject::ttransform(const QWMatrix &m, bool update = false)
{
  tmpMatrix = tmpMatrix * m;
  if(update)
    updateRegion();
}
  
bool GObject::contains(const KoPoint &p)
{
  return box.contains(p);
}

bool GObject::intersects(const KoRect &r)
{
  return r.intersects(box);
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

/*  p[0] = tleft.transform(tmpMatrix);
  p[1] = tright.transform(tmpMatrix);
  p[2] = bleft.transform(tmpMatrix);
  p[3] = bright.transform(tmpMatrix);

  r.left(p[0].x());
  r.top(p[0].y());
  r.right(p[0].x());
  r.bottom(p[0].y());

  for(unsigned int i = 1; i < 4; i++)
  {
    r.left(QMIN(p[i].x(), r.left()));
    r.top(QMIN(p[i].y(), r.top()));
    r.right(QMAX(p[i].x(), r.right()));
    r.bottom(QMAX(p[i].y(), r.bottom()));
  }*/
  updateBoundingBox(r);
}

void GObject::updateRegion(bool recalcBBox)
{
  KoRect newbox = boundingBox();//redrawBox();
  if(recalcBBox)
  {
    KoRect oldbox = newbox;
    calcBoundingBox();
    newbox = boundingBox().unite(oldbox);//redrawBox().unite(oldbox);
  }

//  if(isSelected())
    // the object is selected, so enlarge the update region in order
    // to redraw the handle
//    newbox.enlarge(8);
//  else
    // a workaround for some problems
//    newbox.enlarge(2);

  emit changed(newbox);
}

void GObject::invalidateClipRegion()
{

}

/*---------------
GObject *GObject::objectFactory(const QDomElement &element, KIllustratorDocument *doc)
{
  if(element.tagName() == "polyline")
    return new GPolyline(doc->gdoc(), element);
  else if(element.tagName() == "ellipse")
    return new GOval(doc->gdoc(), element);
  else if(element.tagName() == "bezier")
    return new GBezier(doc->gdoc(), element);
  else if(element.tagName() == "rectangle")
    return new GPolygon(doc->gdoc(), element, GPolygon::PK_Rectangle);
  else if(element.tagName() == "polygon")
    return new GPolygon(doc->gdoc(), element);
  else if(element.tagName() == "clipart")
    return new GClipart(doc->gdoc(), element);
  else if(element.tagName() == "pixmap")
    return new GPixmap(doc->gdoc(), element);
  else if(element.tagName() == "path")
    return new GCurve(doc->gdoc(), element);
  else if(element.tagName() == "text")
    return new GText(doc->gdoc(), element);
  else if(element.tagName() == "group")
    return new GGroup(doc->gdoc(), element);
  else if(element.tagName() == "object")
    return new GPart(doc->gdoc(), doc, element);
  return 0L;
}

void GObject::invalidateClipRegion  () {
  if (gradientFill ())
    gShape.setInvalid ();
}
---------------------*/


#include "GObject.moc"
