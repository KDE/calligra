/* -*- C++ -*-

  $Id$

  This file is part of Kontour.
  Copyright (C) 1998-1999 Kai-Uwe Sattler (kus@iti.cs.uni-magdeburg.de)
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

#include "GPath.h"

#include <qdom.h>

#include <klocale.h>
#include <koVectorPath.h>
#include <koPainter.h>

#include <kdebug.h>
#include <stdlib.h> // for abs

#include "kontour_global.h"
#include "GDocument.h"

GSegment::GSegment()
{
}

GSegment::GSegment(const QDomElement &element)
{
}

GSegment::~GSegment()
{
}

const KoPoint &GSegment::point(int i) const
{
  return points[i];
}

void GSegment::point(int i, const KoPoint &c)
{
  points[i] = c;
}

void GSegment::movePoint(int idx, double dx, double dy, bool ctrlPressed)
{
  points[idx].setX(points[idx].x() + dx);
  points[idx].setY(points[idx].y() + dy);
  // TODO Ctrl Pressed
}

/*        GMove      */

GMove::GMove()
{
  points.resize(1);
}

GMove::GMove(const QDomElement &element)
{
  points.resize(1);
  points[0].setX(element.attribute("x").toDouble());
  points[0].setY(element.attribute("y").toDouble());
}

const char GMove::type() const
{
  return 'm';
}

QDomElement GMove::writeToXml(QDomDocument &document)
{
  QDomElement move = document.createElement("m");
  move.setAttribute("x", points[0].x());
  move.setAttribute("y", points[0].y());
  return move;
}

double GMove::length() const
{
  return 0.0;
}

/*        GLine      */

GLine::GLine()
{
  points.resize(1);
}

GLine::GLine(const QDomElement &element)
{
  points.resize(1);
  points[0].setX(element.attribute("x").toDouble());
  points[0].setY(element.attribute("y").toDouble());
  kdDebug() << "LINETO x=" << points[0].x() << " y=" << points[0].y() << endl;
}

const char GLine::type() const
{
  return 'l';
}

QDomElement GLine::writeToXml(QDomDocument &document)
{
  QDomElement line = document.createElement("l");
  line.setAttribute("x", points[0].x());
  line.setAttribute("y", points[0].y());
  return line;
}

double GLine::length() const
{
  return Kontour::segLength(points[0], points[1]);
}

/*      GCubicBezier      */

GCubicBezier::GCubicBezier()
{
  points.resize(3);
}

GCubicBezier::GCubicBezier(const QDomElement &element)
{
  points.resize(3);
  points[0].setX(element.attribute("x").toDouble());
  points[0].setY(element.attribute("y").toDouble());
  points[1].setX(element.attribute("x1").toDouble());
  points[1].setY(element.attribute("y1").toDouble());
  points[2].setX(element.attribute("x2").toDouble());
  points[2].setY(element.attribute("y2").toDouble());
}

const char GCubicBezier::type() const
{
  return 'c';
}

QDomElement GCubicBezier::writeToXml(QDomDocument &document)
{
  QDomElement arc = document.createElement("c");
  arc.setAttribute("x", points[0].x());
  arc.setAttribute("y", points[0].y());
  arc.setAttribute("x1", points[1].x());
  arc.setAttribute("y1", points[1].y());
  arc.setAttribute("x2", points[2].x());
  arc.setAttribute("y2", points[2].y());
  return arc;
}

double GCubicBezier::length() const
{
  return Kontour::segLength(points[0], points[1]);
}

/*******************[GPath]*********************/

GPath::GPath(bool aClosed):
GObject()
{
  mClosed = aClosed;
  segments.setAutoDelete(true);
}

GPath::GPath(const QDomElement &element):
GObject(element.namedItem("go").toElement())
{
  segments.setAutoDelete(true);
  GSegment *seg;
  QDomNode n = element.firstChild();
  while(!n.isNull())
  {
    QDomElement child = n.toElement();
    if(child.tagName() == "m")
      seg = new GMove(child);
    else if(child.tagName() == "l")
      seg = new GLine(child);
    else if(child.tagName() == "c")
      seg = new GCubicBezier(child);
    else
      seg = 0L;
    if(seg)
    segments.append(seg);
    n = n.nextSibling();
  }
  calcBoundingBox();
}

GPath::GPath(const GPath &obj):
GObject(obj)
{
  segments.setAutoDelete(true);
  segments = obj.segments;
  calcBoundingBox();
}

GObject *GPath::copy() const
{
  return new GPath(this);
}

void GPath::closed(bool aClosed)
{
  mClosed = aClosed;
}

void GPath::beginTo(const double x, const double y)
{
  segments.clear();
  GMove *seg = new GMove;
  seg->point(0, KoPoint(x, y));
  segments.append(seg);
  calcBoundingBox();
}

void GPath::moveTo(const double x, const double y)
{
  GMove *seg = new GMove;
  seg->point(0, KoPoint(x, y));
  segments.append(seg);
  calcBoundingBox();
}

void GPath::lineTo(const double x, const double y)
{
  GLine *seg = new GLine;
  seg->point(0, KoPoint(x, y));
  segments.append(seg);
  calcBoundingBox();
}

void GPath::curveTo(const double x, const double y, const double x1, const double y1, const double x2, const double y2)
{
  GCubicBezier *seg = new GCubicBezier;
  seg->point(0, KoPoint(x, y));
  seg->point(1, KoPoint(x1, y1));
  seg->point(2, KoPoint(x2, y2));
  segments.append(seg);
  calcBoundingBox();
}

void GPath::arcTo(const double x1, const double y1, const double x2, const double y2, const double r)
{
  //<karbon_code>
  // we need to calculate the tangent points. therefore calculate tangents
  // D10=P1P0 and D12=P1P2 first:
  double dx10 = segments.getLast()->point(0).x() - x1;
  double dy10 = segments.getLast()->point(0).y() - y1;
  double dx12 = x2 - x1;
  double dy12 = y2 - y1;

  // calculate distance squares:
  double dsq10 = dx10 * dx10 + dy10 * dy10;
  double dsq12 = dx12 * dx12 + dy12 * dy12;

  // we now calculate tan(a/2) where a is the angular between D10 and D12.
  // we take advantage of D10*D12=d10*d12*cos(a), |D10xD12|=d10*d12*sin(a)
  // (cross product) and tan(a/2)=sin(a)/[1-cos(a)].
  double num   = dx10 * dy12 - dy10 * dx12;
  double denom = sqrt(dsq10 * dsq12) - dx10 * dx12 + dy10 * dy12;

  if(1.0 + denom == 1.0)	// points are co-linear
    lineTo(x1, y1);	        // just add a line to first point
  else
  {
  // calculate distances from P1 to tangent points:
    double dist = fabs(r * num / denom);
    double d1t0 = dist / sqrt(dsq10);
    double d1t1 = dist / sqrt(dsq12);

  // TODO: check for r<0

    double bx0 = x1 + dx10 * d1t0;
    double by0 = y1 + dy10 * d1t0;

  // if(bx0,by0) deviates from current point, add a line to it:
  // TODO: decide via radius<XXX or sthg?
    if(bx0 != segments.getLast()->point(0).x() || by0 != segments.getLast()->point(0).y())
    {
      lineTo(bx0, by0);
    }
    double bx3 = x1 + dx12 * d1t1;
    double by3 = y1 + dy12 * d1t1;

    // the two bezier-control points are located on the tangents at a fraction
    // of the distance [tangent points<->tangent intersection].
    double distsq = (x1 - bx0) * (x1 - bx0) + (y1 - by0) * (y1 - by0);
    double rsq = r * r;
    double fract;

  // TODO: make this nicer?

    if(distsq >= rsq * 1.0e8) // r is very small
      fract = 0.0; // dist==r==0
    else
      fract = ( 4.0 / 3.0 ) / ( 1.0 + sqrt( 1.0 + distsq / rsq ));

    double bx1 = bx0 + (x1 - bx0) * fract;
    double by1 = by0 + (y1 - by0) * fract;
    double bx2 = bx3 + (x1 - bx3) * fract;
    double by2 = by3 + (y1 - by3) * fract;

    // finally add the bezier-segment:
    curveTo(bx3, by3, bx1, by1, bx2, by2);
  }
  //</karbon_code>
}

QString GPath::typeName() const
{
  if(mClosed)
    return i18n("Closed Path");
  else
    return i18n("Path");
}

QDomElement GPath::writeToXml(QDomDocument &document)
{
  QDomElement path = document.createElement("path");
  path.setAttribute("closed", mClosed);
  path.appendChild(GObject::writeToXml(document));
  for(QPtrListIterator<GSegment> seg(segments); seg.current(); ++seg)
    path.appendChild((*seg)->writeToXml(document));
  return path;
}

void GPath::draw(KoPainter *p, int aXOffset, int aYOffset, bool withBasePoints, bool outline, bool withEditMarks)
{
  setPen(p);
  setBrush(p);
  KoVectorPath *v = new KoVectorPath;

  for(QPtrListIterator<GSegment> seg(segments); seg.current(); ++seg)
  {
    GSegment *s = *seg;
    if(s->type() == 'm')
    {
      v->moveTo(s->point(0).x(), s->point(0).y());
    }
    else if(s->type() == 'l')
    {
      v->lineTo(s->point(0).x(), s->point(0).y());
    }
    else if(s->type() == 'c')
    {
      v->lineTo(s->point(0).x(), s->point(0).y());
//      v->bezierTo(s->point(0).x(), s->point(0).y(), s->point(1).x(), s->point(1).y(), s->point(2).x(), s->point(2).y());
    }
  }
  v->end();
  QWMatrix m;
  m = m.translate(aXOffset, aYOffset);
  v->transform(m * tmpMatrix);
  p->drawVectorPath(v);
  delete v;
  if(withBasePoints)
  {
    int x;
    int y;
    KoPoint c;
    for(QPtrListIterator<GSegment> seg(segments); seg.current(); ++seg)
    {
      c = (*seg)->point(0).transform(tmpMatrix);
      x = static_cast<int>(c.x());
      y = static_cast<int>(c.y());
      drawNode(p, x + aXOffset, y + aYOffset, false);
    }
  }
}

int GPath::getNeighbourPoint(const KoPoint &point)
{
  return -1;
}

void GPath::movePoint(int idx, double dx, double dy, bool ctrlPressed)
{
}

void GPath::removePoint(int idx, bool update)
{
}

bool GPath::contains(const KoPoint &p)
{
  double x;
  double y;
  double xc;
  double yc;
  for(QPtrListIterator<GSegment> seg(segments); seg.current(); ++seg)
  {
    GSegment *s = *seg;
    if(s->type() == 'm')
    {
      x = s->point(0).x();
      y = s->point(0).y();
    }
    else if(s->type() == 'l')
    {
      x = s->point(0).x();
      y = s->point(0).y();
    }
    else if(s->type() == 'c')
    {
      x = s->point(0).x();
      y = s->point(0).y();
    }
  }
  return false;
}

bool GPath::findNearestPoint(const KoPoint &p, double max_dist, double &dist, int &pidx, bool all)
{
  return false;
}

void GPath::calcBoundingBox()
{
  QPtrListIterator<GSegment> seg(segments);
  if(!seg.current())
  {
    box = KoRect();
    return;
  }
  double xmin = (*seg)->point(0).x();
  double xmax = (*seg)->point(0).x();
  double ymin = (*seg)->point(0).y();
  double ymax = (*seg)->point(0).y();
  double x;
  double y;
  ++seg;
  for(; seg.current(); ++seg)
  {
    x = (*seg)->point(0).x();
    y = (*seg)->point(0).y();
    if(x < xmin)
      xmin = x;
    if(x > xmax)
      xmax = x;
    if(y < ymin)
      ymin = y;
    if(y > ymax)
      ymax = y;
  }
  box = KoRect(xmin, ymin, xmax - xmin, ymax - ymin).transform(tmpMatrix);
}

GPath *GPath::convertToPath() const
{
  return 0L;
}

bool GPath::isConvertible() const
{
  return false;
}

#include "GPath.moc"
