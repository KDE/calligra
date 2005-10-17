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
#ifndef KIVIOGROUPOBJECT_H
#define KIVIOGROUPOBJECT_H

#include <qvaluelist.h>

#include "rectangleobject.h"

namespace Kivio {

/**
 * Object that can contain several other objects
 */
class GroupObject : public RectangleObject
{
  public:
    GroupObject();
    ~GroupObject();

    /// Duplicate the Object
    virtual Object* duplicate();

    /// Type of Object
    virtual ShapeType type();

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

    /**
     * returns the bouding box of the object
     */
    virtual KoRect boundingBox();

    /// Brush used to fill Object
    virtual QBrush brush() const;
    /// Set brush used to fill Object
    virtual void setBrush(const QBrush& newBrush);
    /// Pen used to draw Object
    virtual Pen pen() const;
    /// Set pen used to draw Object
    virtual void setPen(const Pen& newPen);

    /// Reimplement this function to paint Object to the canvas
    virtual void paint(QPainter& painter, KoZoomHandler* zoomHandler, bool paintHandles = true);

    virtual void addObject(Object* newObject);
    virtual QValueList<Object*> objectList() const;
  private:
    QValueList<Object*> m_objectList;
};

}

#endif
