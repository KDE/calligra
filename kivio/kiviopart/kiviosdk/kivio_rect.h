/*
 * Kivio - Visual Modelling and Flowcharting
 * Copyright (C) 2000 theKompany.com
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
#ifndef KIVIO_RECT_H
#define KIVIO_RECT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "kivio_point.h"

class KivioRect
{
protected:
    KivioPoint m_pos, m_dim;

public:
    KivioRect() { m_pos.set(0.0f, 0.0f); m_dim.set(-1.0f, -1.0f); }
    KivioRect( const KivioPoint &newPos, const KivioPoint &newDim );
    KivioRect( float x, float y, float w, float h);

    void setRect( float x, float y, float w, float h );
    void setRect( const KivioPoint &, const KivioPoint & );

    void setCoords( float x1, float y1, float x2, float y2 );
    void setCoords( const KivioPoint &, const KivioPoint & );

    float x() const { return m_pos.x(); }
    float y() const { return m_pos.y(); }
    float w() const { return m_dim.x(); }
    float h() const { return m_dim.y(); }

    void setX( float f ) { m_pos.setX(f); }
    void setY( float f ) { m_pos.setY(f); }
    void setW( float f ) { m_dim.setX(f); }
    void setH( float f ) { m_dim.setY(f); }

    float left()   const { return m_pos.x(); }
    float right()  const { return m_pos.x()+m_dim.x(); }
    float top()    const { return m_pos.y(); }
    float bottom() const { return m_pos.y()+m_dim.y(); }

    KivioPoint size() const { return m_dim; }
    KivioPoint pos() const { return m_pos; }
    KivioPoint center() const;

    void setSize( const KivioPoint & );
    void setSize( float, float );
    void setPos( const KivioPoint & );
    void setPos( float, float );

    void moveBy( float, float );
    void moveBy( const KivioPoint & );

    bool contains( const KivioPoint &, bool proper=false ) const;
    bool contains( float, float, bool proper=false ) const;
    bool contains( const KivioRect &, bool proper=false ) const;

    KivioRect unite( const KivioRect & ) const;

    bool isValid() const;
    bool isNull() const;
};

#endif

