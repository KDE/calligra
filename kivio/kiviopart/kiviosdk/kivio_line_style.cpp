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
#include "kivio_common.h"
#include "kivio_line_style.h"

KivioLineStyle::KivioLineStyle()
{
    m_color = QColor(0,0,0);
    m_width = 1.0f;
    m_capStyle = Qt::FlatCap;
    m_joinStyle = Qt::RoundJoin;
    m_style = Qt::SolidLine;
}

KivioLineStyle::KivioLineStyle( const KivioLineStyle &ls )
{
    m_color     = ls.color();
    m_width     = ls.width();
    m_capStyle  = ls.capStyle();
    m_joinStyle = ls.joinStyle();
    m_style = ls.style();
}

KivioLineStyle::~KivioLineStyle()
{
}

void KivioLineStyle::copyInto( KivioLineStyle *pTarget )
{
    pTarget->m_color     = m_color;
    pTarget->m_width     = m_width;
    pTarget->m_capStyle  = m_capStyle;
    pTarget->m_joinStyle = m_joinStyle;
    pTarget->m_style     = m_style;
}

QDomElement KivioLineStyle::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioLineStyle");

    XmlWriteColor( e, "color",     m_color );
    XmlWriteFloat( e, "width",     m_width );
    XmlWriteInt(   e, "capStyle",  m_capStyle );
    XmlWriteInt(   e, "joinStyle", m_joinStyle );
    XmlWriteInt(   e, "pattern",   m_style );
    return e;
}

bool KivioLineStyle::loadXML( const QDomElement &e )
{
    m_color     = XmlReadColor( e, "color",     QColor(0,0,0) );
    m_width     = XmlReadFloat( e, "width",     1.0f );
    m_capStyle  = XmlReadInt(   e, "capStyle",  Qt::RoundCap );
    m_joinStyle = XmlReadInt(   e, "joinStyle", Qt::RoundJoin );
    m_style     = XmlReadInt(   e, "pattern",   Qt::SolidLine );
    return true;
}

QPen KivioLineStyle::pen( float scale ) const
{
    QPen p;
    float s = m_width * scale;

    p.setColor( m_color );
    p.setWidth( (int)s );
    p.setJoinStyle( (Qt::PenJoinStyle)m_joinStyle );
    p.setCapStyle( (Qt::PenCapStyle)m_capStyle );
    p.setStyle( (Qt::PenStyle)m_style );

    return p;
}
