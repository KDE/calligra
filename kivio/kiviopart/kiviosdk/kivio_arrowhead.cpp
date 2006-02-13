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
#include "kivio_arrowhead.h"
#include "kivio_common.h"
#include "kivio_painter.h"
#include "kivio_point.h"
#include "kivio_fill_style.h"

#include <math.h>
#include <kdebug.h>
#include <KoZoomHandler.h>

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
        case kahtForwardSlash:
        case kahtBackSlash:
        case kahtPipe:
        case kahtCrowFoot:
        case kahtFork:
        case kahtCrowFootPipe:
        case kahtMidForwardSlash:
        case kahtMidBackSlash:
        case kahtMidPipe:
            m_cut = 0.0f;
            break;

        case kahtArrowTriangleSolid:
        case kahtArrowTriangleHollow:
        case kahtDoubleTriangleSolid:
        case kahtDoubleTriangleHollow:
        case kahtDiamondSolid:
        case kahtDiamondHollow:
//        case kahtCircleSolid:
//        case kahtCircleHollow:
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
 * @param zoomHandler The zoom handler to use to scale
 *
 * The (x,y) should be the vertex
 * representing the endpoint of the line.  The <vecX, vecY> is a vector
 * pointing to (x,y) (in the direction of the arrow head).
 */
void KivioArrowHead::paint( KivioPainter *painter, float x, float y, float vecX, float vecY, KoZoomHandler* zoomHandler )
{
    KivioArrowHeadData d;

    d.painter = painter;
    d.x = x;
    d.y = y;
    d.vecX = vecX;
    d.vecY = vecY;
    d.zoomHandler = zoomHandler;

    // One big-ass switch statement
    switch( m_type )
    {
        case kahtNone:
            return;

        case kahtArrowLine:
            paintArrowLine( &d );
            break;

        case kahtArrowTriangleSolid:
            paintArrowTriangle( &d, true );
            break;

        case kahtArrowTriangleHollow:
            paintArrowTriangle( &d, false );
            break;

        case kahtDoubleTriangleSolid:
            paintDoubleTriangle( &d, true );
            break;

        case kahtDoubleTriangleHollow:
            paintDoubleTriangle( &d, false );
            break;

        case kahtForwardSlash:
            paintForwardSlash( &d );
            break;

        case kahtBackSlash:
            paintBackSlash( &d );
            break;

        case kahtPipe:
            paintPipe( &d );
            break;

        case kahtMidForwardSlash:
            paintMidForwardSlash( &d );
            break;

        case kahtMidBackSlash:
            paintMidBackSlash( &d );
            break;

        case kahtMidPipe:
            paintMidPipe( &d );
            break;

        case kahtDiamondSolid:
            paintDiamond( &d, true );
            break;

        case kahtDiamondHollow:
            paintDiamond( &d, false );
            break;

        case kahtCrowFoot:
            paintCrowFoot( &d );
            break;

        case kahtFork:
            paintFork( &d );
            break;

        case kahtCrowFootPipe:
            paintCrowFootPipe( &d );
            break;
/*
        case kahtCircleSolid:
            paintCircle( &d, true );
            break;

        case kahtCircleHollow:
            paintCircle( &d, false );
            break;
*/
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

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QPtrList<KivioPoint>l;
    l.setAutoDelete(true);

    float _x;
    float _y;

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX(d->x);
    _y = zoomHandler->zoomItY(d->y);

    l.append( new KivioPoint( _x, _y ) );       // point


    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );


    painter->drawPolyline( &l );
}

void KivioArrowHead::paintArrowTriangle( KivioArrowHeadData *d, bool solid )
{
    KivioPainter *painter = d->painter;
    QColor cbg = painter->bgColor();

    if(solid) {
      painter->setBGColor(painter->fgColor());
    }

    float vecX = d->vecX;
    float vecY = d->vecY;

    float length;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QPtrList<KivioPoint>l;
    l.setAutoDelete(true);

    float _x = zoomHandler->zoomItX(d->x);
    float _y = zoomHandler->zoomItY(d->y);

    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    l.append( new KivioPoint( zoomHandler->zoomItX(d->x), zoomHandler->zoomItY(d->y) ) );

    painter->drawPolygon( &l );
    painter->setBGColor(cbg);
}

void KivioArrowHead::paintDoubleTriangle( KivioArrowHeadData *d, bool solid )
{
    KivioPainter *painter = d->painter;
    QColor cbg = painter->bgColor();

    if(solid) {
      painter->setBGColor(painter->fgColor());
    }

    float vecX = d->vecX;
    float vecY = d->vecY;

    float length;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QPtrList<KivioPoint> l1;
    l1.setAutoDelete(true);
    QPtrList<KivioPoint> l2;
    l2.setAutoDelete(true);

    float _x = zoomHandler->zoomItX(d->x);
    float _y = zoomHandler->zoomItY(d->y);
    l1.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX(d->x + nvecX * (m_l / 2.0f));
    _y = zoomHandler->zoomItY(d->y + nvecY * (m_l / 2.0f));
    l2.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * (m_l / 2.0f)) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * (m_l / 2.0f)) + pvecY*(m_w/2.0f));
    l1.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));
    l2.append( new KivioPoint( _x, _y ) );

    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = zoomHandler->zoomItX((d->x + nvecX * (m_l / 2.0f)) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * (m_l / 2.0f)) + pvecY*(m_w/2.0f));
    l1.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));
    l2.append( new KivioPoint( _x, _y ) );

    l1.append( new KivioPoint( zoomHandler->zoomItX(d->x), zoomHandler->zoomItY(d->y) ) );

    _x = zoomHandler->zoomItX(d->x + nvecX * (m_l / 2.0f));
    _y = zoomHandler->zoomItY(d->y + nvecY * (m_l / 2.0f));
    l2.append( new KivioPoint( _x, _y ) );

    painter->drawPolygon( &l1 );
    painter->drawPolygon( &l2 );
    painter->setBGColor(cbg);
}

void KivioArrowHead::paintForwardSlash( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX((d->x + nvecX * (m_l/2.0f)) + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY((d->y + nvecY * (m_l/2.0f)) + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX((d->x - nvecX * (m_l/2.0f)) - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY((d->y - nvecY * (m_l/2.0f)) - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
}

void KivioArrowHead::paintBackSlash( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX((d->x - nvecX * (m_l/2.0f)) + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY((d->y - nvecY * (m_l/2.0f)) + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX((d->x + nvecX * (m_l/2.0f)) - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY((d->y + nvecY * (m_l/2.0f)) - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
}

void KivioArrowHead::paintPipe( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX(d->x + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY(d->y + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX(d->x - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY(d->y - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
}

void KivioArrowHead::paintDiamond( KivioArrowHeadData *d, bool solid )
{
    KivioPainter *painter = d->painter;
    QColor cbg = painter->bgColor();

    if(solid) {
      painter->setBGColor(painter->fgColor());
    }

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QPtrList<KivioPoint> l;
    l.setAutoDelete(true);

    float _x = zoomHandler->zoomItX(d->x);
    float _y = zoomHandler->zoomItY(d->y);
    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * (m_l/2.0f)) - pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * (m_l/2.0f)) - pvecY*(m_w/2.0f));
    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l));
    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * (m_l/2.0f)) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * (m_l/2.0f)) + pvecY*(m_w/2.0f));
    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX(d->x);
    _y = zoomHandler->zoomItY(d->y);
    l.append( new KivioPoint( _x, _y ) );

    painter->drawPolygon( &l );
    painter->setBGColor(cbg);
}

void KivioArrowHead::paintCircle( KivioArrowHeadData *d, bool solid )
{
    KivioPainter *painter = d->painter;
    QColor cbg = painter->bgColor();

    if(solid) {
      painter->setBGColor(painter->fgColor());
    }

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x = zoomHandler->zoomItX(d->x + nvecX * (m_l / 2.0f));
    int y = zoomHandler->zoomItY(d->y + nvecY * (m_l / 2.0f));
    int w = zoomHandler->zoomItY(m_w);
    int h = zoomHandler->zoomItX(m_l);

    painter->drawEllipse(x, y, w, h);
    painter->setBGColor(cbg);
}

void KivioArrowHead::paintCrowFoot( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    float length;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QPtrList<KivioPoint>l;
    l.setAutoDelete(true);

    float _x;
    float _y;

    _x = zoomHandler->zoomItX(d->x + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY(d->y + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX(d->x + nvecX * m_l);
    _y = zoomHandler->zoomItY(d->y + nvecY * m_l);

    l.append( new KivioPoint( _x, _y ) );       // point


    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = zoomHandler->zoomItX(d->x + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY(d->y + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );


    painter->drawPolyline( &l );
}

void KivioArrowHead::paintFork( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    float length;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    QPtrList<KivioPoint>l;
    l.setAutoDelete(true);

    float _x;
    float _y;

    _x = zoomHandler->zoomItX(d->x + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY(d->y + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    pvecX *= -1.0f;
    pvecY *= -1.0f;

    _x = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );

    _x = zoomHandler->zoomItX(d->x + pvecX*(m_w/2.0f));
    _y = zoomHandler->zoomItY(d->y + pvecY*(m_w/2.0f));

    l.append( new KivioPoint( _x, _y ) );


    painter->drawPolyline( &l );
}

void KivioArrowHead::paintCrowFootPipe( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX((d->x + nvecX * m_l) - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY((d->y + nvecY * m_l) - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
    paintCrowFoot( d );
}

void KivioArrowHead::paintMidForwardSlash( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX(d->x + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY(d->y + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX((d->x + nvecX * m_l) - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY((d->y + nvecY * m_l) - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
}

void KivioArrowHead::paintMidBackSlash( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX((d->x + nvecX * m_l) + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY((d->y + nvecY * m_l) + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX(d->x - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY(d->y - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
}

void KivioArrowHead::paintMidPipe( KivioArrowHeadData *d )
{
    KivioPainter *painter = d->painter;

    float vecX = d->vecX;
    float vecY = d->vecY;

    KoZoomHandler* zoomHandler = d->zoomHandler;

    float nvecX, nvecY; // normalized vectors
    float pvecX, pvecY; // normal perpendicular vector

    float length = sqrt( vecX*vecX + vecY*vecY );

    nvecX = - vecX / length;
    nvecY = - vecY / length;

    pvecX = nvecY;
    pvecY = -nvecX;

    int x1 = zoomHandler->zoomItX((d->x + nvecX * (m_l/2.0f)) + pvecX*(m_w/2.0f));
    int y1 = zoomHandler->zoomItY((d->y + nvecY * (m_l/2.0f)) + pvecY*(m_w/2.0f));
    int x2 = zoomHandler->zoomItX((d->x + nvecX * (m_l/2.0f)) - pvecX*(m_w/2.0f));
    int y2 = zoomHandler->zoomItY((d->y + nvecY * (m_l/2.0f)) - pvecY*(m_w/2.0f));

    painter->drawLine(x1, y1, x2, y2);
}
