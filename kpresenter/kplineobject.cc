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
#include "KPLineObjectIface.h"

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <kdebug.h>
#include <kozoomhandler.h>
#include <math.h>
using namespace std;

/******************************************************************/
/* Class: KPLineObject                                             */
/******************************************************************/

/*================ default constructor ===========================*/
KPLineObject::KPLineObject()
    : KPShadowObject()
{
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
    lineType = LT_HORZ;
}

/*================== overloaded constructor ======================*/
KPLineObject::KPLineObject( const QPen &_pen, LineEnd _lineBegin, LineEnd _lineEnd, LineType _lineType )
    : KPShadowObject( _pen )
{
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
    lineType = _lineType;
}

KPLineObject &KPLineObject::operator=( const KPLineObject & )
{
    return *this;
}

DCOPObject* KPLineObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPLineObjectIface( this );
    return dcop;
}


/*========================= save =================================*/
QDomDocumentFragment KPLineObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPShadowObject::save(doc, offset);
    if (lineType!=LT_HORZ)
        fragment.appendChild(KPObject::createValueElement("LINETYPE", static_cast<int>(lineType), doc));
    if (lineBegin!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    if (lineEnd!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
    return fragment;
}

/*========================== load ================================*/
double KPLineObject::load(const QDomElement &element)
{
    double offset=KPShadowObject::load(element);
    QDomElement e=element.namedItem("LINETYPE").toElement();
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
    return offset;
}

/*======================== paint =================================*/
void KPLineObject::paint( QPainter* _painter, KoZoomHandler*_zoomHandler,
                          bool /*drawingShadow*/, bool drawContour )
{
    double ow = ext.width();
    double oh = ext.height();
    int _w = pen.width();

    QPen pen2;
    if ( drawContour ) {
        pen2 = QPen( Qt::black, 1, Qt::DotLine );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        pen2 = pen;
        pen2.setWidth( _zoomHandler->zoomItX( pen.width() ) );
    }
    _painter->setPen( pen2 );


    KoSize diff1( 0, 0 ), diff2( 0, 0 );

    if ( lineBegin != L_NORMAL )
        diff1 = getBoundingSize( lineBegin, _w, _zoomHandler );

    if ( lineEnd != L_NORMAL )
        diff2 = getBoundingSize( lineEnd, _w, _zoomHandler );

    double unzoom_diff1_width = _zoomHandler->unzoomItX( (int)diff1.width() );
    double unzoom_diff1_height = _zoomHandler->unzoomItX( (int)diff1.height() );
    double unzoom_diff2_width = _zoomHandler->unzoomItX( (int)diff2.width() );
    double unzoom_diff2_height = _zoomHandler->unzoomItX( (int)diff2.height() );

    switch ( lineType )
    {
    case LT_HORZ: {
        if ( lineBegin != L_NORMAL && !drawContour )
            drawFigure( lineBegin, _painter,
                        KoPoint( unzoom_diff1_width / 2.0, oh / 2.0 ),
                        pen2.color(), _w, 180.0, _zoomHandler );

        if ( lineEnd != L_NORMAL && !drawContour )
            drawFigure( lineEnd, _painter,
                        KoPoint( ow - unzoom_diff2_width / 2.0, oh / 2.0 ),
                        pen2.color(), _w, 0.0, _zoomHandler );

        _painter->drawLine( (int)diff1.width() / 2,
                            _zoomHandler->zoomItY( oh / 2 ),
                            _zoomHandler->zoomItX( ow - unzoom_diff2_width / 2 ),
                            _zoomHandler->zoomItY( oh / 2) );
    } break;
    case LT_VERT: {
        if ( lineBegin != L_NORMAL && !drawContour )
            drawFigure( lineBegin, _painter,
                        KoPoint( ow / 2.0, unzoom_diff1_width / 2.0 ),
                        pen2.color(), _w, 270.0, _zoomHandler );

        if ( lineEnd != L_NORMAL && !drawContour )
            drawFigure( lineEnd, _painter,
                        KoPoint( ow / 2.0, oh - unzoom_diff2_width / 2.0 ),
                        pen2.color(), _w, 90.0, _zoomHandler );

        _painter->drawLine( _zoomHandler->zoomItX( ow / 2 ),
                            (int)diff1.width() / 2,
                            _zoomHandler->zoomItX( ow / 2 ),
                            _zoomHandler->zoomItY( oh - unzoom_diff2_width / 2 ) );
    } break;
    case LT_LU_RD: {
        KoRect _rect = KoRect( orig, ext );
        KoPoint pnt1 = _rect.topLeft();
        KoPoint pnt2 = _rect.bottomRight();

        float _angle = KoPoint::getAngle( pnt1, pnt2 );

        if ( lineBegin != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( unzoom_diff1_height / 2 ),
                                 _zoomHandler->zoomItY( unzoom_diff1_width / 2 ) );
            drawFigure( lineBegin, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle, _zoomHandler );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( ow - unzoom_diff2_height / 2 ),
                                 _zoomHandler->zoomItY( oh - unzoom_diff2_width / 2 ) );
            drawFigure( lineEnd, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle - 180, _zoomHandler );
            _painter->restore();

        }
        _painter->drawLine( _zoomHandler->zoomItX( unzoom_diff1_height / 2 + _w / 2 ),
                            _zoomHandler->zoomItY( unzoom_diff1_width / 2 + _w / 2 ),
                            _zoomHandler->zoomItX( ow - unzoom_diff2_height / 2 - _w / 2 ),
                            _zoomHandler->zoomItY( oh - unzoom_diff2_width / 2 - _w / 2 ) );
    } break;
    case LT_LD_RU: {
        KoRect _rect = KoRect( orig, ext );
        KoPoint pnt1 = _rect.topRight();
        KoPoint pnt2 = _rect.bottomLeft();

        float _angle = KoPoint::getAngle( pnt1, pnt2 );

        if ( lineBegin != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( unzoom_diff1_height / 2),
                                 _zoomHandler->zoomItY( oh - unzoom_diff1_width / 2 ) );
            drawFigure( lineBegin, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle - 180,_zoomHandler );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( ow - unzoom_diff2_height / 2 ),
                                 _zoomHandler->zoomItY( unzoom_diff2_width / 2) );
            drawFigure( lineEnd, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle,_zoomHandler );
            _painter->restore();
        }
        _painter->drawLine( _zoomHandler->zoomItX( unzoom_diff1_height / 2 + _w / 2 ),
                            _zoomHandler->zoomItY( oh - unzoom_diff1_width / 2 - _w / 2 ),
                            _zoomHandler->zoomItX( ow - unzoom_diff2_height / 2 - _w / 2 ),
                            _zoomHandler->zoomItY( unzoom_diff2_width / 2 + _w / 2 ) );
    } break;
    }
}

void KPLineObject::flip(bool horizontal )
{
    if ( horizontal )
    {
        if ( lineType == LT_LU_RD )
            lineType = LT_LD_RU;
        else if ( lineType == LT_LD_RU )
            lineType = LT_LU_RD;
    }
    else
    {
        if ( lineType == LT_LU_RD )
            lineType = LT_LD_RU;
        else if ( lineType == LT_LD_RU )
            lineType = LT_LU_RD;

        LineEnd tmp = lineBegin;
        lineBegin = lineEnd;
        lineEnd = tmp;
    }
}
