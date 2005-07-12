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
#ifndef KIVIOSMLOBJECTLOADER_H
#define KIVIOSMLOBJECTLOADER_H

#include <qmap.h>

#include "object.h"

class QDomElement;
class QBrush;

namespace Kivio {

class Pen;

/**
 * Loads objects in Kivio's sml format
 */
class SmlObjectLoader{
  public:
    SmlObjectLoader();
    ~SmlObjectLoader();

    Object* loadShape(const QDomElement& shapeElement);
    Object* loadObject(const QDomElement& shapeElement);

  protected:
    Object* loadRectangleObject(const QDomElement& shapeElement);
    Object* loadRoundedRectangleObject(const QDomElement& shapeElement);
    Object* loadBezierObject(const QDomElement& shapeElement);
    Object* loadEllipseObject(const QDomElement& shapeElement);
    Object* loadPolygonObject(const QDomElement& shapeElement);
    Object* loadPolylineObject(const QDomElement& shapeElement);
    Object* loadLineArrayObject(const QDomElement& shapeElement);

    Pen loadPen(const QDomElement& element);
    QBrush loadBrush(const QDomElement& element);

  private:
    QMap<QString, ShapeType> m_shapeTypeMap;
};

}

#endif
