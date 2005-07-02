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
#ifndef KIVIOELLIPSEOBJECT_H
#define KIVIOELLIPSEOBJECT_H

#include <koSize.h>

#include "rectangleobject.h"

namespace Kivio {

/**
 * Draws an ellipse
 */
class EllipseObject : public RectangleObject
{
  public:
    EllipseObject();
    ~EllipseObject();

    /// Duplicate the object
    virtual Object* duplicate();

    /// Type of object
    virtual ShapeType type();

    /// Draws a ellipse to the canvas
    virtual void paint(QPainter& painter, KoZoomHandler* zoomHandler);
};

}

#endif
