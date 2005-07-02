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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#include "polylineobject.h"

#include <qpainter.h>
#include <qpointarray.h>

#include <kozoomhandler.h>

namespace Kivio {

PolylineObject::PolylineObject()
 : Object()
{
}

PolylineObject::~PolylineObject()
{
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

QValueVector<KoPoint> PolylineObject::pointVector() const
{
  return m_pointVector;
}

void PolylineObject::setPointVector(const QValueVector<KoPoint>& newVector)
{
  m_pointVector = newVector;
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

}
