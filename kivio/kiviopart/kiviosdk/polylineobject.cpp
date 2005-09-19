/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2005 Peter Simonsson
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#include "polylineobject.h"

#include <qpainter.h>
#include <qpointarray.h>

#include <kozoomhandler.h>
#include <koRect.h>

#include "tkmath.h"

namespace Kivio {

PolylineObject::PolylineObject()
 : Object()
{
}

PolylineObject::~PolylineObject()
{
}

KoPoint PolylineObject::position() const
{
  if(m_pointVector.count()) {
    return m_pointVector[0];
  }

  return KoPoint();
}

void PolylineObject::setPosition(const KoPoint& newPosition)
{
  if(m_pointVector.count()) {
    KoPoint offset = newPosition - m_pointVector[0];
    move(offset.x(), offset.y());
  } else {
    addPoint(newPosition);
  }
}

Object* PolylineObject::duplicate()
{
  PolylineObject* object = new PolylineObject(*this);

  return object;
}

ShapeType PolylineObject::type()
{
  return kstPolyline;
}

KoRect PolylineObject::boundingBox()
{
  KoPoint topLeft = m_pointVector[0], bottomRight, current;
  QValueVector<KoPoint>::iterator it;
  QValueVector<KoPoint>::iterator itEnd = m_pointVector.end();

  for(it = m_pointVector.begin(); it != itEnd; ++it) {
    current = *it;
    topLeft.setX(QMIN(current.x(), topLeft.x()));
    topLeft.setY(QMIN(current.y(), topLeft.y()));
    bottomRight.setX(QMAX(current.x(), bottomRight.x()));
    bottomRight.setY(QMAX(current.y(), bottomRight.y()));
  }

  KoRect rect;
  rect.moveTopLeft(topLeft);
  rect.setWidth(bottomRight.x() - topLeft.x());
  rect.setHeight(bottomRight.y() - topLeft.y());

  return rect;
}

QValueVector<KoPoint> PolylineObject::pointVector() const
{
  return m_pointVector;
}

void PolylineObject::setPointVector(const QValueVector<KoPoint>& newVector)
{
  m_pointVector = newVector;
}

void PolylineObject::addPoint(const KoPoint& point)
{
  m_pointVector.append(point);
}

void PolylineObject::changePoint(unsigned int index, const KoPoint& point)
{
  m_pointVector[index] = point;
}

void PolylineObject::paint(QPainter& painter, KoZoomHandler* zoomHandler)
{
  QValueVector<KoPoint>::iterator it;
  QValueVector<KoPoint>::iterator itEnd = m_pointVector.end();
  QPointArray pointArray(m_pointVector.size());
  int i = 0;

  for(it = m_pointVector.begin(); it != itEnd; ++it) {
    pointArray.setPoint(i, zoomHandler->zoomPoint((*it)));
    ++i;
  }

  painter.setPen(pen().zoomedPen(zoomHandler));
  painter.setBrush(brush());
  painter.drawPolyline(pointArray);
}

void PolylineObject::move(double xOffset, double yOffset)
{
  KoPoint offset(xOffset, yOffset);
  QValueVector<KoPoint>::iterator it;
  QValueVector<KoPoint>::iterator itEnd = m_pointVector.end();

  for(it = m_pointVector.begin(); it != itEnd; ++it) {
    (*it) += offset;
  }
}

void PolylineObject::resize(double /*xOffset*/, double /*yOffset*/)
{
  // FIXME implement resizing
}

void PolylineObject::resizeInPercent(double percentWidth, double percentHeight)
{
  QValueVector<KoPoint>::iterator it;
  QValueVector<KoPoint>::iterator itEnd = m_pointVector.end();
  KoPoint newPoint;

  for(it = m_pointVector.begin(); it != itEnd; ++it) {
    newPoint = *it;
    newPoint.setX(newPoint.x() * percentWidth);
    newPoint.setY(newPoint.y() * percentHeight);
    (*it) = newPoint;
  }
}

int PolylineObject::contains(const KoPoint& point)
{
  unsigned int i = 0;
  KoPoint point1, point2;
  uint count = m_pointVector.count();

  count -= 1; // As we need current + 1;

  while(i < count) {
    point1 = m_pointVector[i];
    point2 = m_pointVector[i + 1];

    if(collisionLine(point1.x(), point1.y(),
       point2.x(), point2.y(), point.x(), point.y(), 1))
    {
      return CTBody;
    }

    i++;
  }

  return CTNone;
}

}
