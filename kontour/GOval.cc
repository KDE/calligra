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

#include "GOval.h"

#include <math.h>

#include <qdom.h>
#include <qpainter.h>

#include <klocale.h>

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

void GOval::type(Type t)
{
  mType = t;
}

void GOval::startPoint(const KoPoint &p)
{
  sPoint = p;
  calcBoundingBox();
}

void GOval::endPoint(const KoPoint &p)
{
  ePoint = p;
  calcBoundingBox();
}

void GOval::setAngles(double sa, double ea)
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
  r = r.normalize();

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

void GOval::draw(QPainter &p, bool withBasePoints, bool outline, bool)
{
  p.save();
  p.setWorldMatrix(tmpMatrix, true);
  setPen(&p);
  setBrush(&p);

  p.drawEllipse((int)sPoint.x(), (int)sPoint.y(), (int)(ePoint.x() - sPoint.x()), (int)(ePoint.y() - sPoint.y()));

  p.restore();

/*  double alen = 0;
  QPen pen;
  QBrush brush;

  initPen (pen);


  

  if (! workInProgress () && ! outline) {
    initBrush (brush);
    p.setBrush (brush);
    if (gradientFill () &&
        outlineInfo.shape != GObject::OutlineInfo::ArcShape) {
      //if (! gShape.valid ())
        updateGradientShape (p);
      gShape.draw (p);
    }
  }

  switch (outlineInfo.shape) {
  case GObject::OutlineInfo::DefaultShape:
    Painter::drawEllipse (p, sPoint.x (), sPoint.y (),
                          ePoint.x () - sPoint.x (),
                          ePoint.y () - sPoint.y ());
    break;
  case GObject::OutlineInfo::PieShape:
    alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    Painter::drawPie (p, sPoint.x (), sPoint.y (),
                      ePoint.x () - sPoint.x (),
                      ePoint.y () - sPoint.y (),
                      -eAngle * 16, -alen * 16);
    break;
  case GObject::OutlineInfo::ArcShape:
    alen = (eAngle > sAngle ? 360 - eAngle + sAngle : sAngle - eAngle);
    Painter::drawArc (p, sPoint.x (), sPoint.y (),
                      ePoint.x () - sPoint.x (),
                      ePoint.y () - sPoint.y (),
                      -eAngle * 16, -alen * 16);
    break;
  }
  p.restore ();
  p.save ();
  if (withBasePoints) {
    p.setPen (black);
    p.setBrush (white);
    for (int i = 0; i < 2; i++) {
      Coord c = segPoint[i].transform (tmpMatrix);
      Painter::drawRect (p, c.x () - 2.0, c.y () - 2.0, 4, 4);
    }
  }
 */
}

void GOval::calcBoundingBox()
{
  KoPoint p1(sPoint.x(), sPoint.y());
  KoPoint p2(ePoint.x(), sPoint.y());
  KoPoint p3(ePoint.x(), ePoint.y());
  KoPoint p4(sPoint.x(), ePoint.y());
  adjustBBox(p1, p2, p3, p4);
  calcUntransformedBoundingBox(p1, p2, p3, p4);
//  calcUntransformedBoundingBox(sPoint, KoPoint(ePoint.x(), sPoint.y()), ePoint, KoPoint(sPoint.x(), ePoint.y()));
  double x, y;

  KoRect r(sPoint, ePoint);
  r.normalize ();
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
  double xx = (sPoint.x() + ePoint.x()) / 2.0;
  double yy = (sPoint.y() + ePoint.y()) / 2.0;
  GPath *path = new GPath(true);
  path->moveTo(sPoint.x(), yy);
  path->lineTo(xx, ePoint.y());
  path->lineTo(ePoint.x(), yy);
  path->lineTo(xx, sPoint.y());
  path->lineTo(sPoint.x(), yy);
  path->matrix(matrix());
//  path->style(style());  //why???
  return path;
}

/*bool GOval::isValid()
{
  return (fabs(sPoint.x() - ePoint.x()) > 1 || fabs(sPoint.y() - ePoint.y()) > 1);
}*/


/*void GOval::updateGradientShape(QPainter &p)
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

#include "GOval.moc"
