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
#ifndef KIVIO_LINE_STYLE_H
#define KIVIO_LINE_STYLE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <qcolor.h>
#include <qdom.h>
#include <qpen.h>


class KivioLineStyle
{
protected:
    QColor m_color;
    float  m_width;
    int    m_capStyle;
    int    m_joinStyle;
    int    m_style;

public:
    KivioLineStyle();
    KivioLineStyle( const KivioLineStyle & );

    virtual ~KivioLineStyle();

    void copyInto( KivioLineStyle * );

    QDomElement saveXML( QDomDocument & );
    bool loadXML( const QDomElement & );

    QPen pen( float ) const;

    QColor color() const { return m_color; }
    void setColor( QColor c ){ m_color=c; }

    float width() const { return m_width; }
    void setWidth( float f ){ m_width=f; }

    int capStyle() const { return m_capStyle; }
    void setCapStyle( int p ){ m_capStyle=p; }

    int joinStyle() const { return m_joinStyle; }
    void setJoinStyle( int j ){ m_joinStyle=j; }

    int style() const { return m_style; }
    void setStyle( int p ) { m_style=p; }
};

#endif

