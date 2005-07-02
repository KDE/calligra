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
#ifndef KIVIOPOLYLINEOBJECT_H
#define KIVIOPOLYLINEOBJECT_H

#include <qvaluevector.h>

#include "object.h"

namespace Kivio {

/**
 * Draws a polyline
 */
class PolylineObject : public Object
{
  public:
    PolylineObject();
    ~PolylineObject();

    /// Duplicate the object
    virtual Object* duplicate();

    /// Type of object
    virtual ShapeType type();

    /**
     * Move the Object
     * @param xOffset number of points to move the Object horizontaly
     * @param yOffset number of points to move the Object verticaly
     */
    virtual void move(double xOffset, double yOffset);

    /**
     * Resize the Object
     * @param xOffset number of points to resize the Object horizontaly
     * @param yOffset number of points to resize the Object horizontaly
     */
    virtual void resize(double xOffset, double yOffset);

    /// Returns a vector containing the 4 control points of the bezier curve
    QValueVector<KoPoint> pointVector() const;
    /// Set a vector containing the control points of the polyline
    void setPointVector(const QValueVector<KoPoint>& newVector);

    /// Draws a polyline to the canvas
    virtual void paint(QPainter& painter, KoZoomHandler* zoomHandler);

  private:
    QValueVector<KoPoint> m_pointVector;
};

}

#endif
