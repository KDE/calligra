/* This file is part of the KDE project
   Copyright (C) 2008 Fela Winkelmolen <fela.kde@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#ifndef KARBONCALLIGRAPHICSHAPE_H
#define KARBONCALLIGRAPHICSHAPE_H

#include <KoParameterShape.h>

#define KarbonCalligraphicShapeId "KarbonCalligraphicShape"

class KarbonCalligraphicPoint
{
public:
    KarbonCalligraphicPoint(const QPointF &point, double angle, double width)
        : m_point(point), m_angle(angle), m_width(width) {}

    QPointF point() const {return m_point;}
    double angle() const {return m_angle;}
    double width() const {return m_width;}

    void setPoint(const QPointF &point) {m_point = point;}

private:
    QPointF m_point; // in shape coordinates
    double m_angle;
    double m_width;
};

/*class KarbonCalligraphicShape::Point
{
public:
    KoPainterPath(KoPathPoint *point) : m_prev(point), m_next(0) {}

    // calculates the effective point
    QPointF point() {
        if (m_next = 0)
            return m_prev.point();

        // m_next != 0
        kDebug() << "not implemented yet!!!!";
        return QPointF();
    }

private:
    KoPainterPath m_prev;
    KoPainterPath m_next;
    double m_percentage;
};*/

// the indexes of the path will be similar to:
//        7--6--5--4   <- pointCount() / 2
// start  |        |   end    ==> (direction of the stroke)
//        0--1--2--3
class KarbonCalligraphicShape : public KoParameterShape
{
public:
    KarbonCalligraphicShape();
    ~KarbonCalligraphicShape();

    void appendPoint( const QPointF &p1, double angle, double width );
    void appendPointToPath(const KarbonCalligraphicPoint &p);

    //KoPathShape *simplified( float error );

    // returns the bounding rect of whan needs to be repainted
    // after new points are added
    const QRectF lastPieceBoundingRect();

    void setSize( const QSizeF &newSize );
    //virtual QPointF normalize();

    QPointF normalize();

    void simplifyPath();
    
    // reimplemented
    virtual QString pathShapeId() const;
    
protected:
    // reimplemented
    void moveHandleAction( int handleId,
                           const QPointF & point,
                           Qt::KeyboardModifiers modifiers = Qt::NoModifier );

    // reimplemented
    void updatePath( const QSizeF &size );

private:
    // auxiliary function that actually insererts the points
    // without doing any additional checks
    // the points should be given in canvas coordinates
    void appendPointsToPathAux( const QPointF &p1, const QPointF &p2 );
   
    // function to detect a flip, given the points being inserted
    // it returns 0 if there is no flip
    // +1 if the flip is in the direction of the higher indexes
    // -1 if the flip is in the direction of the lower indexes
    // TODO: values not used, maybe just return a boolean
    int flipDetected( const QPointF &p1, const QPointF &p2 );

    void smoothLastPoints();
    void smoothPoint( const int index );

    // determine whether the points given are in counterclockwise order or not
    // returns +1 if they are, -1 if they are given in clockwise order
    // and 0 if they form a degenerate triangle
    static int ccw( const QPointF &p1, const QPointF &p2, const QPointF &p3 );

    // when true p1 is connected to the previous
    bool m_flipped;

    // the actual data then determines it's shape
    QList<KarbonCalligraphicPoint *> m_points;
};

#endif // KARBONCALLIGRAPHICSHAPE_H

