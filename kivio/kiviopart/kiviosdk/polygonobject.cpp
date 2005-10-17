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
#include "polygonobject.h"

#include <qpainter.h>
#include <qpointarray.h>

#include <kozoomhandler.h>

namespace Kivio {

PolygonObject::PolygonObject()
 : PolylineObject()
{
}

PolygonObject::~PolygonObject()
{
}

Object* PolygonObject::duplicate()
{
  PolygonObject* object = new PolygonObject(*this);

  return object;
}

ShapeType PolygonObject::type()
{
  return kstPolygon;
}

void PolygonObject::paint(QPainter& painter, KoZoomHandler* zoomHandler, bool paintHandles)
{
  QValueVector<KoPoint> points = pointVector();
  QValueVector<KoPoint>::iterator it;
  QValueVector<KoPoint>::iterator itEnd = points.end();
  QPointArray pointArray(points.size());
  int i = 0;

  for(it = points.begin(); it != itEnd; ++it) {
    pointArray.setPoint(i, zoomHandler->zoomPoint((*it)));
    ++i;
  }

  painter.setPen(pen().zoomedPen(zoomHandler));
  painter.setBrush(brush());
  painter.drawPolygon(pointArray, true);
}

}
