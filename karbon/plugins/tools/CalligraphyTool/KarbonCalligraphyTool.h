/* This file is part of the KDE project
 * Copyright (C) 2008 Fela Winkelmolen <fela.kde@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _KARBONCALLIGRAPHYTOOL_H_
#define _KARBONCALLIGRAPHYTOOL_H_

#include <KoTool.h>
#include <KoPathShape.h>

// the indexes of the path will be similar to:
//        7--6--5--4   <- pointCount() / 2
// start  |        |   end    ==> (direction of the stroke)
//        0--1--2--3
class KarbonCalligraphicPath : public KoPathShape
{
public:
    void insertPoints( const QPointF &p1, const QPointF &p2 );

    KoPathShape *simplified( float error );

    // returns the bounding rect of whan needs to be repainted
    // after new points are added
    const QRectF lastPieceBoundingRect();
    
private:
    // function that actually insererts the points
    // without doing any additional check
    // the points should be given in canvas coordinates
    void insertPointsAux( const QPointF &p1, const QPointF &p2 );
   
    // function to detect a flip, given the points being inserted
    // it returns 0 if there is no flip
    // +1 if the flip is in the direction of the higher indexes
    // -1 if the flip is in the direction of the lower indexes
    // TODO: values not used, maybe just return a boolean
    int flipDetected( const QPointF &p1, const QPointF &p2 );

    // determine whether the points given are in counterclockwise order or not
    // returns +1 if they are, -1 if they are given in clockwise order
    // and 0 if they form a degenerate triangle
    static int ccw( const QPointF &p1, const QPointF &p2, const QPointF &p3 );

    // offset of the points when mapped against the canvas
    QPointF m_offset;
    // when true p1 is connected to the previous
    bool m_flipped;
};

class KoPathShape;
class KoShapeGroup;
class KoPathPoint;

class KarbonCalligraphyTool : public KoTool
{
    Q_OBJECT
public:
    explicit KarbonCalligraphyTool(KoCanvasBase *canvas);
    ~KarbonCalligraphyTool();

    void paint( QPainter &painter, const KoViewConverter &converter );
    
    void mousePressEvent( KoPointerEvent *event ) ;
    void mouseMoveEvent( KoPointerEvent *event );
    void mouseReleaseEvent( KoPointerEvent *event );
    
    
    void activate ( bool temporary=false );
    void deactivate();

private:
    void addPoint( const QPointF &point );

    //QPointF m_point0;
    //QPointF m_point1;
    QPointF m_lastPoint;
    KarbonCalligraphicPath *m_path;
    //QList<KoPathShape *> m_pieces;
    
    double m_strokeWidth;
    double m_angle;
    double m_mass;
    
    bool m_isDrawing;
    
    
    // dinamic parameters
    QPointF m_speed;
};

#endif // _KARBONCALLIGRAPHYTOOL_H_
