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
#ifndef KIVIOARCOBJECT_H
#define KIVIOARCOBJECT_H

#include <koSize.h>

#include "rectangleobject.h"

class QPainter;
class KoZoomHandler;

namespace Kivio {

/**
 * Object that draws an arc to the canvas
 */
class ArcObject : public RectangleObject
{
  public:
    ArcObject();
    ~ArcObject();

    /// Duplicate the object
    virtual Object* duplicate();

    /// Type of object
    virtual ShapeType type();

    /// Returns the angle at which the arc starts. The angle is in 1/16 of a degree (16 * 360 == full circle).
    int startAngle() const;
    /// Set the angle at which the arc begins. The angle is in 1/16 of a degree (16 * 360 == full circle).
    void setStartAngle(int newAngle);
    /// Returns the angle at which defines the arc's length. The angle is in 1/16 of a degree (16 * 360 == full circle).
    int lengthAngle() const;
    /// Set the angle which defines the arc's length. The angle is in 1/16 of a degree (16 * 360 == full circle).
    void setLengthAngle(int newAngle);

    /// Draw an arc to the canvas
    virtual void paint(QPainter& painter, KoZoomHandler* zoomHandler);

  private:
    int m_startAngle;
    int m_lengthAngle;
};

}

#endif
