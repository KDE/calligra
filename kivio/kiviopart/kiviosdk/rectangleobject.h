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
#ifndef KIVIORECTANGLEOBJECT_H
#define KIVIORECTANGLEOBJECT_H

#include <koSize.h>
#include <koPoint.h>

#include "object.h"

namespace Kivio {

/**
 * Draws a rectangle
 */
class RectangleObject : public Object
{
  public:
    RectangleObject();
    ~RectangleObject();

    /// Duplicate the object
    virtual Object* duplicate();

    /// Type of object
    virtual ShapeType type();

    /// Position of Object
    virtual KoPoint position() const;
    /// Set the position of Object to @param newPosition
    virtual void setPosition(const KoPoint& newPosition);
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
    /**
     * Resize the object in percent of the current size
     * @param percentWidth the percent the width have changed
     * @param percentHeight the percent the height have changed
     */
    virtual void resizeInPercent(double percentWidth, double percentHeight);
    /// Size of the rectangle.
    virtual KoSize size() const;
    /// Set the size of the rectangle.
    virtual void setSize(const KoSize& newSize);

    /**
     * returns the bouding box of the object
     */
    virtual KoRect boundingBox();

    /// Draws a rectangle to the canvas
    virtual void paint(QPainter& painter, KoZoomHandler* zoomHandler);

  private:
    KoPoint m_position;
    KoSize m_size;
};

}

#endif
