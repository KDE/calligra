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
#ifndef KIVIOOBJECT_H
#define KIVIOOBJECT_H

#include <qstring.h>
#include <qbrush.h>

#include "pen.h"

class QPainter;
class KoZoomHandler;
class KoRect;

namespace Kivio {

/// The different shape types
typedef enum {
  kstNone = 0,
  kstArc,
  kstPie,
  kstLineArray,
  kstPolyline,
  kstPolygon,
  kstBezier,
  kstRectangle,
  kstRoundedRectangle,
  kstEllipse,
  kstOpenPath,
  kstClosedPath,
  kstTextBox,
  kstGroup,
  kstConnector,
  kstCustomObject = 1000
} ShapeType;

/**
 * Base class for canvas objects
 */
class Object{
  public:
    Object();
    virtual ~Object();

    /// Duplicate the Object
    virtual Object* duplicate() = 0;

    /// Type of Object
    virtual ShapeType type();

    /// Name of Object
    virtual QString name() const;
    /// Set the name of Object to @param newName
    virtual void setName(const QString& newName);

    /**
     * Move the Object
     * @param xOffset number of points to move the Object horizontaly
     * @param yOffset number of points to move the Object verticaly
     */
    virtual void move(double xOffset, double yOffset) = 0;
    /**
     * Resize the Object
     * @param xOffset number of points to resize the Object horizontaly
     * @param yOffset number of points to resize the Object horizontaly
     */
    virtual void resize(double xOffset, double yOffset) = 0;
    /**
     * Resize the object in percent of the current size
     * @param percentWidth the percent the width have changed
     * @param percentHeight the percent the height have changed
     */
    virtual void resizeInPercent(double percentWidth, double percentHeight) = 0;

    /**
     * returns the bouding box of the object
     */
    virtual KoRect boundingBox() = 0;

    /// Brush used to fill Object
    virtual QBrush brush() const;
    /// Set brush used to fill Object
    virtual void setBrush(const QBrush& newBrush);
    /// Pen used to draw Object
    virtual Pen pen() const;
    /// Set pen used to draw Object
    virtual void setPen(const Pen& newPen);

    /// Reimplement this function to paint Object to the canvas
    virtual void paint(QPainter& painter, KoZoomHandler* zoomHandler) = 0;

  private:
    QString m_name;

    QBrush m_brush;
    Pen m_pen;
};

}

#endif
