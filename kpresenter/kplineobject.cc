/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kplineobject.h>
#include <kpresenter_utils.h>

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <kdebug.h>

#include <math.h>
using namespace std;

/******************************************************************/
/* Class: KPLineObject                                             */
/******************************************************************/

/*================ default constructor ===========================*/
KPLineObject::KPLineObject()
    : KPObject(), pen()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    lineType = LT_HORZ;
}

/*================== overloaded constructor ======================*/
KPLineObject::KPLineObject( QPen _pen, LineEnd _lineBegin, LineEnd _lineEnd, LineType _lineType )
    : KPObject(), pen( _pen )
{
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
    lineType = _lineType;
}

KPLineObject &KPLineObject::operator=( const KPLineObject & )
{
    return *this;
}

/*========================= save =================================*/
QDomDocumentFragment KPLineObject::save( QDomDocument& doc )
{
    QDomDocumentFragment fragment=KPObject::save(doc);
    fragment.appendChild(KPObject::createPenElement("PEN", pen, doc));
    if (lineType!=LT_HORZ)
        fragment.appendChild(KPObject::createValueElement("LINETYPE", static_cast<int>(lineType), doc));
    if (lineBegin!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    if (lineEnd!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
    return fragment;
}

/*========================== load ================================*/
void KPLineObject::load(const QDomElement &element)
{
    KPObject::load(element);
    QDomElement e=element.namedItem("PEN").toElement();
    if(!e.isNull())
        setPen(KPObject::toPen(e));
    e=element.namedItem("LINETYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineType=static_cast<LineType>(tmp);
    }
    e=element.namedItem("LINEBEGIN").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineBegin=static_cast<LineEnd>(tmp);
    }
    e=element.namedItem("LINEEND").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineEnd=static_cast<LineEnd>(tmp);
    }
}

/*========================= draw =================================*/
void KPLineObject::draw( QPainter *_painter, int _diffx, int _diffy )
{
    if ( move )
    {
        KPObject::draw( _painter, _diffx, _diffy );
        return;
    }

    int ox = orig.x() - _diffx;
    int oy = orig.y() - _diffy;
    int ow = ext.width();
    int oh = ext.height();

    _painter->save();

    if ( shadowDistance > 0 )
    {
        QPen tmpPen( pen );
        pen.setColor( shadowColor );

        if ( angle == 0 )
        {
            int sx = ox;
            int sy = oy;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            _painter->translate( sx, sy );
            paint( _painter );
        }
        else
        {
            _painter->translate( ox, oy );

            QRect br = QRect( 0, 0, ow, oh );
            int pw = br.width();
            int ph = br.height();
            QRect rr = br;
            int yPos = -rr.y();
            int xPos = -rr.x();
            rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

            int sx = 0;
            int sy = 0;
            getShadowCoords( sx, sy, shadowDirection, shadowDistance );

            QWMatrix m, mtx, m2;
            mtx.rotate( angle );
            m.translate( pw / 2, ph / 2 );
            m2.translate( rr.left() + xPos + sx, rr.top() + yPos + sy );
            m = m2 * mtx * m;

            _painter->setWorldMatrix( m, true );
            paint( _painter );
        }

        pen = tmpPen;
    }

    _painter->restore();

    _painter->save();
    _painter->translate( ox, oy );

    if ( angle == 0 )
        paint( _painter );
    else
    {
        QRect br = QRect( 0, 0, ow, oh );
        int pw = br.width();
        int ph = br.height();
        QRect rr = br;
        int yPos = -rr.y();
        int xPos = -rr.x();
        rr.moveTopLeft( QPoint( -rr.width() / 2, -rr.height() / 2 ) );

        QWMatrix m, mtx, m2;
        mtx.rotate( angle );
        m.translate( pw / 2, ph / 2 );
        m2.translate( rr.left() + xPos, rr.top() + yPos );
        m = m2 * mtx * m;

        _painter->setWorldMatrix( m, true );
        paint( _painter );
    }

    _painter->restore();

    KPObject::draw( _painter, _diffx, _diffy );
}

/*===================== get angle ================================*/
float KPLineObject::getAngle( QPoint p1, QPoint p2 )
{
    float _angle = 0.0;

    if ( p1.x() == p2.x() )
    {
        if ( p1.y() < p2.y() )
            _angle = 270.0;
        else
            _angle = 90.0;
    }
    else
    {
        float x1, x2, y1, y2;

        if ( p1.x() <= p2.x() )
        {
            x1 = p1.x(); y1 = p1.y();
            x2 = p2.x(); y2 = p2.y();
        }
        else
        {
            x2 = p1.x(); y2 = p1.y();
            x1 = p2.x(); y1 = p2.y();
        }

        float m = -( y2 - y1 ) / ( x2 - x1 );
        _angle = atan( m ) * RAD_FACTOR;

        if ( p1.x() < p2.x() )
            _angle = 180.0 - _angle;
        else
            _angle = -_angle;
    }

    return _angle;
}

/*======================== paint =================================*/
void KPLineObject::paint( QPainter* _painter )
{
    int ow = ext.width();
    int oh = ext.height();

    switch ( lineType )
    {
    case LT_HORZ:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        if ( lineBegin != L_NORMAL )
            drawFigure( lineBegin, _painter, QPoint( diff1.width() / 2, oh / 2 ), pen.color(), _w, 180.0 );

        if ( lineEnd != L_NORMAL )
            drawFigure( lineEnd, _painter, QPoint( ow - diff2.width() / 2, oh / 2 ), pen.color(), _w, 0.0 );

        _painter->setPen( pen );
        _painter->drawLine( diff1.width() / 2, oh / 2, ow - diff2.width() / 2, oh / 2 );
    } break;
    case LT_VERT:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        if ( lineBegin != L_NORMAL )
            drawFigure( lineBegin, _painter, QPoint( ow / 2, diff1.width() / 2 ), pen.color(), _w, 270.0 );

        if ( lineEnd != L_NORMAL )
            drawFigure( lineEnd, _painter, QPoint( ow / 2, oh - diff2.width() / 2 ), pen.color(), _w, 90.0 );

        _painter->setPen( pen );
        _painter->drawLine( ow / 2, diff1.width() / 2, ow / 2, oh - diff2.width() / 2 );
    } break;
    case LT_LU_RD:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        QPoint pnt1( diff1.height() / 2 + _w / 2, diff1.width() / 2 + _w / 2 );
        QPoint pnt2( ow - diff2.height() / 2 - _w / 2, oh - diff2.width() / 2 - _w / 2 );
        float _angle;

        _angle = getAngle( pnt1, pnt2 );

        if ( lineBegin != L_NORMAL )
        {
            _painter->save();
            _painter->translate( diff1.height() / 2, diff1.width() / 2 );
            drawFigure( lineBegin, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL )
        {
            _painter->save();
            _painter->translate( ow - diff2.height() / 2, oh - diff2.width() / 2 );
            drawFigure( lineEnd, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle - 180 );
            _painter->restore();
        }

        _painter->setPen( pen );
        _painter->drawLine( diff1.height() / 2 + _w / 2, diff1.width() / 2 + _w / 2,
                            ow - diff2.height() / 2 - _w / 2, oh - diff2.width() / 2 - _w / 2 );
    } break;
    case LT_LD_RU:
    {
        QSize diff1( 0, 0 ), diff2( 0, 0 );
        int _w = pen.width();

        if ( lineBegin != L_NORMAL )
            diff1 = getBoundingSize( lineBegin, _w );

        if ( lineEnd != L_NORMAL )
            diff2 = getBoundingSize( lineEnd, _w );

        QPoint pnt1( diff1.height() / 2 + _w / 2, oh - diff1.width() / 2 - _w / 2 );
        QPoint pnt2( ow - diff2.height() / 2 - _w / 2, diff2.width() / 2 + _w / 2 );
        float _angle;

        _angle = getAngle( pnt1, pnt2 );

        if ( lineBegin != L_NORMAL )
        {
            _painter->save();
            _painter->translate( diff1.height() / 2, oh - diff1.width() / 2 );
            drawFigure( lineBegin, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL )
        {
            _painter->save();
            _painter->translate( ow - diff2.height() / 2, diff2.width() / 2 );
            drawFigure( lineEnd, _painter, QPoint( 0, 0 ), pen.color(), _w, _angle - 180 );
            _painter->restore();
        }

        _painter->setPen( pen );
        _painter->drawLine( diff1.height() / 2 + _w / 2, oh - diff1.width() / 2 - _w / 2,
                            ow - diff2.height() / 2 - _w / 2, diff2.width() / 2 + _w / 2 );
    } break;
    }
}




