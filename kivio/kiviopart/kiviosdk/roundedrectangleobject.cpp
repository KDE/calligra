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
#include "roundedrectangleobject.h"

#include <qpainter.h>
#include <qrect.h>

#include <koRect.h>
#include <kozoomhandler.h>

namespace Kivio {

RoundedRectangleObject::RoundedRectangleObject()
 : RectangleObject()
{
}

RoundedRectangleObject::~RoundedRectangleObject()
{
}

Object* RoundedRectangleObject::duplicate()
{
  RoundedRectangleObject* object = new RoundedRectangleObject(*this);

  return object;
}

ShapeType RoundedRectangleObject::type()
{
  return kstRoundedRectangle;
}

int RoundedRectangleObject::xRoundness() const
{
  return m_xRoundness;
}

void RoundedRectangleObject::setXRoundness(int newRoundness)
{
  m_xRoundness = newRoundness;
}

int RoundedRectangleObject::yRoundness() const
{
  return m_yRoundness;
}

void RoundedRectangleObject::setYRoundness(int newRoundness)
{
  m_yRoundness = newRoundness;
}

void RoundedRectangleObject::paint(QPainter& painter, KoZoomHandler* zoomHandler)
{
  QRect rect = zoomHandler->zoomRect(KoRect(position(), size()));
  painter.setPen(pen().zoomedPen(zoomHandler));
  painter.setBrush(brush());
  painter.drawRoundRect(rect, m_xRoundness, m_yRoundness);
}

}
