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
#ifndef KIVIO_SHAPE_H
#define KIVIO_SHAPE_H

#include "kivio_shape_data.h"
#include <qdom.h>


class KivioShape
{
protected:
    KivioShapeData m_shapeData; // The data inside this shape

public:
    KivioShape();
    KivioShape( const KivioShape & );

    virtual ~KivioShape();

    void copyInto( KivioShape * ) const;

    bool loadXML( const QDomElement & );
    QDomElement saveXML( QDomDocument & );

    KivioShapeData::KivioShapeType shapeType() { return m_shapeData.shapeType(); }

    KivioShapeData *shapeData() { return &m_shapeData; }

    static KivioShape *loadShapeArc( const QDomElement & );
    static KivioShape *loadShapeClosedPath( const QDomElement & );
    static KivioShape *loadShapeBezier( const QDomElement & );
    static KivioShape *loadShapeEllipse( const QDomElement & );
    static KivioShape *loadShapeLineArray( const QDomElement & );
    static KivioShape *loadShapeOpenPath( const QDomElement & );
    static KivioShape *loadShapePie( const QDomElement & );
    static KivioShape *loadShapePolygon( const QDomElement & );
    static KivioShape *loadShapePolyline( const QDomElement & );
    static KivioShape *loadShapeRectangle( const QDomElement & );
    static KivioShape *loadShapeRoundRectangle( const QDomElement & );
    static KivioShape *loadShapeTextBox( const QDomElement & );
};

#endif


