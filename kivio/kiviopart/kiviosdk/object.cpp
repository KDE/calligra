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
#include "object.h"

namespace Kivio {

Object::Object()
{
}

Object::~Object()
{
}

ShapeType Object::type()
{
  return kstNone;
}

QString Object::name() const
{
  return m_name;
}

void Object::setName(const QString& newName)
{
  m_name= newName;
}

QBrush Object::brush() const
{
  return m_brush;
}

void Object::setBrush(const QBrush& newBrush)
{
  m_brush = newBrush;
}

Pen Object::pen() const
{
  return m_pen;
}

void Object::setPen(const Pen& newPen)
{
  m_pen = newPen;
}

}
