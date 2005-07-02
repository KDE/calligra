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
#ifndef KIVIOOBJECT_H
#define KIVIOOBJECT_H

#include <qstring.h>
#include <qbrush.h>

#include <koPoint.h>

#include "pen.h"

class QPainter;
class KoZoomHandler;

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
  kstTextBox
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

    /// Id of Object
    virtual QString id() const;
    /// Set the id of Object to @param newId
    virtual void setId(const QString& newId);

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
    virtual void resize(double xOffset, double yOffset) = 0;

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
    QString m_id;
    KoPoint m_position;

    QBrush m_brush;
    Pen m_pen;
};

}

#endif
