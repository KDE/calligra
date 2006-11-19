/* This file is part of the KDE project
   Copyright (C) 2006 Thorsten Zachmann <zachmann@kde.org>

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

#ifndef KOELLIPSESHAPE_H
#define KOELLIPSESHAPE_H

#include "KoParameterShape.h"

#define KoEllipseShapeId "KoEllipseShape"

/**
 * This class adds support for arc, pie, chord, circle and ellipse 
 * shapes.
 */
class KoEllipseShape : public KoParameterShape
{
public:    
    KoEllipseShape();
    ~KoEllipseShape();

    void resize( const QSizeF &newSize );
    virtual QPointF normalize();

protected:    
    void moveHandleAction( int handleId, const QPointF & point, Qt::KeyboardModifiers modifiers = Qt::NoModifier );
    void updatePath( const QSizeF &size );
    void createPath( const QSizeF &size );

private:    
    enum KoEllipseType
    {
        Arc = 0,
        Pie = 1,
        Chord = 2
    };

    void updateKindHandle();

    // start angle in degree
    double m_startAngle;
    // end angle in degree
    double m_endAngle;
    // angle for modifying the kind in radiant
    double m_kindAngle;
    // the center of the ellipse
    QPointF m_center;
    // the radii of the ellips
    QPointF m_radii;
    KoEllipseType m_type;

    KoSubpath m_points;
};

#endif /* KOELLIPSESHAPE_H */

