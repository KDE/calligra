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
#include "shapecollection.h"

#include "object.h"

namespace Kivio {

ShapeCollection::ShapeCollection()
{
}

ShapeCollection::~ShapeCollection()
{
  QValueList<Object*>::iterator it = m_shapeList.begin();
  QValueList<Object*>::iterator itEnd = m_shapeList.end();
  Object* object;

  while(it != itEnd) {
    object = *it;
    it = m_shapeList.remove(it);
    delete object;
  }
}

void ShapeCollection::addShape(Object* shape)
{
  if(!shape) {
    return;
  }

  m_shapeList.append(shape);
}

}
