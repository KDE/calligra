/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000-2001 theKompany.com & Dave Marotti
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#ifndef KIVIO_POINT_H
#define KIVIO_POINT_H

#include <qdom.h>

class KivioPoint
{
public:
    // KivioPoint type enumerations
    typedef enum {
        kptNone=0,      // Bounds check
        kptNormal,      // Normal point (for polygons, etc)
        kptBezier,      // Bezier point (these come in groups of 4)
        kptArc,         // Arc point (I think these come in groups of 3... unimplemented)
        kptLast         // Bounds check
    } KivioPointType;
    
protected:
    double m_x, m_y;                 // Coordinates of the point
    KivioPointType m_pointType;     // The point type

public:
    KivioPoint();
    KivioPoint( const KivioPoint & );
    KivioPoint( double, double, KivioPointType pt=kptNormal );
    virtual ~KivioPoint();
    
    void copyInto( KivioPoint * ) const;
    
    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );

    static KivioPoint::KivioPointType pointTypeFromString( const QString & );

    inline double x() const { return m_x; }
    inline double y() const { return m_y; }
    inline KivioPointType pointType() const { return m_pointType; }

    inline void setX( double newX ) { m_x=newX; }
    inline void setY( double newY ) { m_y=newY; }
    inline void setPointType( KivioPointType pt ) { m_pointType=pt; }

    inline void set( double newX, double newY, KivioPointType pt=kptNormal ) { m_x=newX; m_y=newY; m_pointType=pt; }
    inline void set( const KivioPoint &p, KivioPointType pt=kptNormal ) { m_x=p.x(); m_y=p.y(); m_pointType=pt; }

    inline void moveBy( double dx, double dy ) { m_x += dx; m_y += dy; }
    inline void moveBy( const KivioPoint &p ) { m_x += p.x(); m_y += p.y(); }
};

#endif


