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
#include "kivio_arrowhead.h"
#include "kivio_common.h"
#include "kivio_painter.h"
#include "kivio_point.h"

#include <math.h>
#include <kdebug.h>

KivioArrowHead::KivioArrowHead()
{
    // Both of these values are in pixels
    m_cut = 0.0f;
    m_w = m_l = 10.0f;

    m_type = kahtNone;
}

KivioArrowHead::~KivioArrowHead()
{
}

QDomElement KivioArrowHead::saveXML( QDomDocument &doc )
{
    QDomElement e = doc.createElement("KivioArrowHead");

    XmlWriteFloat( e, "cut", m_cut );
    XmlWriteFloat( e, "w", m_w );
    XmlWriteFloat( e, "l", m_l );
    XmlWriteInt( e, "type", m_type );

    return e;
}

bool KivioArrowHead::loadXML( const QDomElement &e )
{
    m_cut   = XmlReadFloat( e, "cut", 0.0f );
    m_w     = XmlReadFloat( e, "w", 10.0f );
    m_l     = XmlReadFloat( e, "l", 10.0f );
    m_type  = XmlReadInt( e, "type", m_type );

    return true;
}

void KivioArrowHead::setType(int t)
{
    m_type = t;

    switch( t )
    {
        case kahtNone:
            m_cut = 0.0f;
            break;

        case kahtArrowLine:
            m_cut = 0.0f;
            break;

        case kahtArrowTriangleSolid:
            m_cut = KIVIO_CUT_LENGTH;
            break;

        default:
            m_type = kahtNone;
            m_cut = 0.0f;
            break;
    }
}

float KivioArrowHead::cut()
{
    if( m_cut == KIVIO_CUT_LENGTH )
        return m_l;

    else if( m_cut == KIVIO_CUT_HALF_LENGTH )
        return m_l / 2.0f;

    return m_cut;
}

/**
 * Paints the arrow head at a given point, direction, and scale.
 *
 * @param painter The painter object to draw with
 * @param x The x position of the arrow head (the point)
 * @param y The y position of the arrow head (the point)
 * @param vecX The X component of the negative direction vector
 * @param vecY The Y component of the negative direction vector
 * @param scale The scale we are drawing at
 *
 * The (x,y) should be the vertex
 * representing the endpoint of the line.  The <vecX, vecY> is a vector
 * pointing to (x,y) (in the direction of the arrow head).
 */
void KivioArrowHead::paint( KivioPainter *painter, float x, float y, float vecX, float vecY, float scale )
{
    KivioArrowHeadData d;

    d.painter = painter;
    d.x = x;
    d.y = y;
    d.vecX = vecX;
    d.vecY = vecY;
    d.scale = scale;

    // One big-ass switch statement
    switch( m_type )
    {
        case kahtNone:
            return;

        case kahtArrowLine:
            paintArrowLine( &d );
            break;

        case kahtArrowTriangleSolid:
            paintArrowTriangleSolid( &d );
            break;

        default:
            break;
    }
}

void KivioArrowHead::paintArrowLine( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    float length;

    float scale = d->scale;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QList<KivioPoint>l;
    l.setAutoDelete(true);

    float _x;
    float _y;

    _x = (d->x + nvecX * m_l) + pvecX*(m_w/2.0f);
    _y = (d->y + nvecY * m_l) + pvecY*(m_w/2.0f);
    _x *= scale;
    _y *= scale;

    l.append( new KivioPoint( _x, _y ) );

    _x = d->x * d->scale;
    _y = d->y * d->scale;

    l.append( new KivioPoint( _x, _y ) );       // point


    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = (d->x + nvecX * m_l) + pvecX*(m_w/2.0f);
    _y = (d->y + nvecY * m_l) + pvecY*(m_w/2.0f);
    _x *= scale;
    _y *= scale;

    l.append( new KivioPoint( _x, _y ) );


    painter->drawPolyline( &l );
}

void KivioArrowHead::paintArrowTriangleSolid( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    float length;

    float scale = d->scale;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QList<KivioPoint>l;
    l.setAutoDelete(true);

    float _x = d->x * d->scale;
    float _y = d->y * d->scale;

    l.append( new KivioPoint( _x, _y ) );

    _x = (d->x + nvecX * m_l) + pvecX*(m_w/2.0f);
    _y = (d->y + nvecY * m_l) + pvecY*(m_w/2.0f);
    _x *= scale;
    _y *= scale;

    l.append( new KivioPoint( _x, _y ) );

    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = (d->x + nvecX * m_l) + pvecX*(m_w/2.0f);
    _y = (d->y + nvecY * m_l) + pvecY*(m_w/2.0f);
    _x *= scale;
    _y *= scale;

    l.append( new KivioPoint( _x, _y ) );

    l.append( new KivioPoint( d->x * scale, d->y * scale ) );

    painter->drawPolygon( &l );
}
