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
#include "arcobject.h"

#include <qpainter.h>
#include <qrect.h>

#include <kozoomhandler.h>
#include <koRect.h>

namespace Kivio {

ArcObject::ArcObject()
 : RectangleObject()
{
}

ArcObject::~ArcObject()
{
}

Object* ArcObject::duplicate()
{
  ArcObject* object = new ArcObject(*this);

  return object;
}

ShapeType ArcObject::type()
{
  return kstArc;
}

int ArcObject::startAngle() const
{
  return m_startAngle;
}

void ArcObject::setStartAngle(int newAngle)
{
  m_startAngle = newAngle;
}

int ArcObject::lengthAngle() const
{
  return m_lengthAngle;
}

void ArcObject::setLengthAngle(int newAngle)
{
  m_lengthAngle = newAngle;
}

void ArcObject::paint(QPainter& painter, KoZoomHandler* zoomHandler, bool paintHandles)
{
  QRect rect = zoomHandler->zoomRect(KoRect(position(), size()));
  painter.setBrush(brush());
  painter.setPen(pen().zoomedPen(zoomHandler));
  painter.drawArc(rect, startAngle(), lengthAngle());
}

}
