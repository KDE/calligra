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
#include "rectangleobject.h"

#include <qpainter.h>
#include <qrect.h>

#include <koRect.h>
#include <kozoomhandler.h>

namespace Kivio {

RectangleObject::RectangleObject()
 : Object()
{
}

RectangleObject::~RectangleObject()
{
}

Object* RectangleObject::duplicate()
{
  RectangleObject* object = new RectangleObject(*this);

  return object;
}

ShapeType RectangleObject::type()
{
  return kstRectangle;
}

KoPoint RectangleObject::position() const
{
  return m_position;
}

void RectangleObject::setPosition(const KoPoint& newPosition)
{
  m_position = newPosition;
}

void RectangleObject::move(double xOffset, double yOffset)
{
  KoPoint offset(xOffset, yOffset);
  setPosition(position() + offset);
}

KoSize RectangleObject::size() const
{
  return m_size;
}

void RectangleObject::setSize(const KoSize& newSize)
{
  m_size = newSize;
}

void RectangleObject::resize(double xOffset, double yOffset)
{
  KoSize offset(xOffset, yOffset);

  m_size += offset;
}

void RectangleObject::resizeInPercent(double percentWidth, double percentHeight)
{
  KoPoint newPosition;
  newPosition.setX(m_position.x() * percentWidth);
  newPosition.setY(m_position.y() * percentHeight);
  setPosition(newPosition);

  KoSize newSize;
  newSize.setWidth(m_size.width() * percentWidth);
  newSize.setHeight(m_size.height() * percentHeight);
  setSize(newSize);
}

void RectangleObject::paint(QPainter& painter, KoZoomHandler* zoomHandler)
{
  QRect rect = zoomHandler->zoomRect(KoRect(position(), size()));
  painter.setPen(pen().zoomedPen(zoomHandler));
  painter.setBrush(brush());
  painter.drawRect(rect);
}

}
