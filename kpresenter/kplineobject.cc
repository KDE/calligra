// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
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

#include "kplineobject.h"
#include "kpresenter_utils.h"
#include "kpresenter_doc.h"
#include "KPLineObjectIface.h"
#include "koPointArray.h"
#include <koStyleStack.h>
#include <kooasiscontext.h>

#include <qpainter.h>
#include <qwmatrix.h>
#include <qdom.h>
#include <kdebug.h>
#include <koUnit.h>
#include <kozoomhandler.h>
#include <math.h>
#include <kodom.h>
#include <koxmlns.h>

using namespace std;

KPLineObject::KPLineObject()
    : KPShadowObject(), KPStartEndLine( L_NORMAL, L_NORMAL )
{
    lineType = LT_HORZ;
}

KPLineObject::KPLineObject( const QPen &_pen, LineEnd _lineBegin,
                            LineEnd _lineEnd, LineType _lineType )
    : KPShadowObject( _pen ), KPStartEndLine( _lineBegin, _lineEnd )
{
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


QString KPLineObject::saveOasisStrokeElement( KoGenStyles& mainStyles ) const
{
    KoGenStyle styleobjectauto( KPresenterDoc::STYLE_GRAPHICAUTO, "graphic" );
    saveOasisMarkerElement( mainStyles, styleobjectauto );
    KPShadowObject::saveOasisStrokeElement( mainStyles, styleobjectauto );
    saveOasisShadowElement( styleobjectauto );
    saveOasisObjectProtectStyle( styleobjectauto );
    return mainStyles.lookup( styleobjectauto, "gr" );
}

bool KPLineObject::saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj )  const
{
    xmlWriter.startElement( "draw:line" );
    xmlWriter.addAttribute( "draw:style-name", saveOasisStrokeElement( context.mainStyles() ) );

    float x1 = orig.x();
    float y1 = orig.y();
    float x2 = x1 + ext.width();
    float y2 = y1;

    switch( lineType )
    {
    case LT_LD_RU:
        y1 += ext.height();
        break;
    case LT_HORZ:
        y1 += ext.height() / 2.0;
        y2 = y1;
        break;
    case LT_VERT:
        x1 += ext.width() / 2.0;
        x2 = x1;
        // no break
    case LT_LU_RD:
        y2 += ext.height();
        break;
    }

    xmlWriter.addAttributePt( "svg:y1", y1 );
    xmlWriter.addAttributePt( "svg:y2", y2 );
    xmlWriter.addAttributePt( "svg:x1", x1 );
    xmlWriter.addAttributePt( "svg:x2", x2 );

    if( !objectName.isEmpty())
        xmlWriter.addAttribute( "draw:name", objectName );
    xmlWriter.endElement();
    return true;
}


QDomDocumentFragment KPLineObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPShadowObject::save(doc, offset);
    if (lineType!=LT_HORZ)
        fragment.appendChild(KPObject::createValueElement("LINETYPE", static_cast<int>(lineType), doc));
    KPStartEndLine::save( fragment, doc );
    return fragment;
}

void KPLineObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    KPShadowObject::loadOasis(element, context, info);

    double x1 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x1", QString::null ) );
    double y1 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y1", QString::null ) );
    double x2 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x2", QString::null ) );
    double y2 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y2", QString::null ) );

    kdDebug()<<" KPLineObject::loadOasis(const QDomElement &element) : x1 "<< x1 <<" y1 : "<<y1<<" x2 :"<<x2 <<" y2 "<<y2<<endl;
    double x = QMIN( x1, x2 );
    double y = QMIN( y1, y2 );

    orig.setX( x );
    orig.setY( y );

    ext.setWidth( fabs( x1 - x2 ) );
    ext.setHeight( fabs( y1 - y2 ) );

    if ( y1 == y2 )
    {
        lineType=LT_HORZ;
        //define default height
        ext.setHeight( 10 );
        orig.setY( y - 5.0 );
    }
    else if ( x1 == x2 )
    {
        lineType=LT_VERT;
        //define default width
        ext.setWidth( 10 );
        orig.setX( x - 5.0 );
    }
    else if ( ( x1 < x2 && y1 < y2 ) || ( x1 > x2 && y1 > y2 ) )
         lineType=LT_LU_RD;
    else
        lineType=LT_LD_RU;

    kdDebug()<<"KPLineObject::loadOasis(const QDomElement &element) : real position x :"<<orig.x()<<" y "<<orig.y()<< " width :"<<ext.width()<<" height :"<<ext.height()<<endl;

    QString attr = (x1 < x2) ?  "marker-start" : "marker-end";
    loadOasisMarkerElement( context, attr, lineBegin );

    attr = (x1 < x2) ?  "marker-end" : "marker-start";
    loadOasisMarkerElement( context, attr, lineEnd );
}

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
    KPStartEndLine::load( element );
    return offset;
}

void KPLineObject::paint( QPainter* _painter, KoZoomHandler*_zoomHandler,
                          int /* pageNum */, bool /*drawingShadow*/, bool drawContour )
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

    float _angle = 0;
    switch ( lineType )
    {
    case LT_HORZ: {
        _angle = 0;
    } break;
    case LT_VERT: {
        _angle = 90 ;
    } break;
    case LT_LU_RD: {
        KoRect _rect = KoRect( orig, ext );
        KoPoint pnt1 = _rect.topLeft();
        KoPoint pnt2 = _rect.bottomRight();

        _angle = KoPoint::getAngle( pnt1, pnt2 ) - 180.0;
    } break;
    case LT_LD_RU: {
        KoRect _rect = KoRect( orig, ext );
        KoPoint pnt1 = _rect.bottomLeft();
        KoPoint pnt2 = _rect.topRight();

        _angle = KoPoint::getAngle( pnt1, pnt2 ) - 180.0;
    } break;
    }

    if ( lineBegin != L_NORMAL )
        diff1 = getOffset( lineBegin, _w, _angle + 180.0);

    if ( lineEnd != L_NORMAL )
        diff2 = getOffset( lineEnd, _w, _angle );

    switch ( lineType )
    {
    case LT_HORZ: {
        if ( lineBegin != L_NORMAL && !drawContour )
            drawFigure( lineBegin, _painter,
                        KoPoint( 0, oh / 2.0 ),
                        pen2.color(), _w, 180.0, _zoomHandler );

        if ( lineEnd != L_NORMAL && !drawContour )
            drawFigure( lineEnd, _painter,
                        KoPoint( ow , oh / 2.0),
                        pen2.color(), _w, 0.0, _zoomHandler );

        _painter->drawLine( _zoomHandler->zoomItX( - diff1.width() ),
                            _zoomHandler->zoomItY( oh / 2 ),
                            _zoomHandler->zoomItX( ow - diff2.width() ),
                            _zoomHandler->zoomItY( oh / 2 ) );
    } break;
    case LT_VERT: {
        if ( lineBegin != L_NORMAL && !drawContour )
            drawFigure( lineBegin, _painter,
                        KoPoint( ow / 2.0, 0 ),
                        pen2.color(), _w, 270.0, _zoomHandler );

        if ( lineEnd != L_NORMAL && !drawContour )
            drawFigure( lineEnd, _painter,
                        KoPoint( ow / 2.0, oh ),
                        pen2.color(), _w, 90.0, _zoomHandler );

        _painter->drawLine( _zoomHandler->zoomItX( ow / 2 ),
                            _zoomHandler->zoomItX( - diff1.height() ),
                            _zoomHandler->zoomItX( ow / 2 ),
                            _zoomHandler->zoomItY( oh - diff2.height() ) );
    } break;
    case LT_LU_RD: {
        if ( lineBegin != L_NORMAL && !drawContour ) {
            _painter->save();
            drawFigure( lineBegin, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle + 180, _zoomHandler );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( ow ),
                                 _zoomHandler->zoomItY( oh ) );
            drawFigure( lineEnd, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle, _zoomHandler );
            _painter->restore();

        }
        _painter->drawLine( _zoomHandler->zoomItX( - diff1.width() ),
                            _zoomHandler->zoomItY( - diff1.height() ),
                            _zoomHandler->zoomItX( ow - diff2.width() ),
                            _zoomHandler->zoomItY( oh - diff2.height() ) );
    } break;
    case LT_LD_RU: {
        if ( lineBegin != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( 0 ),
                                 _zoomHandler->zoomItY( oh ) );
            drawFigure( lineBegin, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle + 180,_zoomHandler );
            _painter->restore();
        }
        if ( lineEnd != L_NORMAL && !drawContour ) {
            _painter->save();
            _painter->translate( _zoomHandler->zoomItX( ow ),
                                 _zoomHandler->zoomItY( 0 ) );
            drawFigure( lineEnd, _painter,
                        KoPoint( 0, 0 ), pen2.color(),
                        _w, _angle,_zoomHandler );
            _painter->restore();
        }
        _painter->drawLine( _zoomHandler->zoomItX( - diff1.width() ),
                            _zoomHandler->zoomItY( oh - diff1.height() ),
                            _zoomHandler->zoomItX( ow - diff2.width() ),
                            _zoomHandler->zoomItY( - diff2.height() ));
    } break;
    }
}

void KPLineObject::flip( bool horizontal )
{
    KPObject::flip( horizontal );
    if ( ! horizontal )
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

KoSize KPLineObject::getRealSize() const {
    KoPoint realOrig( orig );
    KoSize size( ext );

    KoPointArray points(4);

    if ( lineType == LT_LU_RD || lineType == LT_LD_RU )
    {
        double objAngle = atan( ext.width() / ext.height() );
        double x = cos( objAngle ) * pen.width();
        double y = sin( objAngle ) * pen.width();

        if ( lineType == LT_LU_RD )
        {
            points.setPoint( 0, x, 0 );
            points.setPoint( 1, 0, y );
            points.setPoint( 2, ext.width() + x, ext.height() );
            points.setPoint( 3, ext.width(), ext.height() + y );
        }
        else
        {
            points.setPoint( 0, 0, ext.height() );
            points.setPoint( 1, x, ext.height() + y );
            points.setPoint( 2, ext.width(), 0 );
            points.setPoint( 3, ext.width() + x, y );
        }
        realOrig.setX( realOrig.x() - x / 2.0 );
        realOrig.setY( realOrig.y() - y / 2.0 );
        size.setWidth( size.width() + x );
        size.setHeight( size.height() + y );
    }

    if ( angle == 0.0 && lineType == LT_HORZ )
    {
        size.setHeight( pen.width() );
    }
    else if ( angle == 0.0 && lineType == LT_VERT )
    {
        size.setWidth( pen.width() );
    }
    else
    {
        if ( lineType == LT_HORZ )
        {
            points.setPoint( 0, 0, ( ext.height() - pen.width() ) / 2.0 );
            points.setPoint( 1, 0, ( ext.height() + pen.width() ) / 2.0 );
            points.setPoint( 2, ext.width(), ( ext.height() - pen.width() ) / 2.0 );
            points.setPoint( 3, ext.width(), ( ext.height() + pen.width() ) / 2.0 );
        }
        else if ( lineType == LT_VERT )
        {
            points.setPoint( 0, ( ext.width() - pen.width() ) / 2.0, 0 );
            points.setPoint( 1, ( ext.width() + pen.width() ) / 2.0, 0 );
            points.setPoint( 2, ( ext.width() - pen.width() ) / 2.0, ext.height() );
            points.setPoint( 3, ( ext.width() + pen.width() ) / 2.0, ext.height() );
        }

        getRealSizeAndOrigFromPoints( points, angle, size, realOrig );
    }

    return size;
}

KoPoint KPLineObject::getRealOrig() const {
    KoPoint realOrig( orig );
    KoSize size( ext );

    KoPointArray points(4);

    if ( lineType == LT_LU_RD || lineType == LT_LD_RU )
    {
        double objAngle = atan( ext.width() / ext.height() );
        double x = cos( objAngle ) * pen.width();
        double y = sin( objAngle ) * pen.width();

        if ( lineType == LT_LU_RD )
        {
            points.setPoint( 0, x, 0 );
            points.setPoint( 1, 0, y );
            points.setPoint( 2, ext.width() + x, ext.height() );
            points.setPoint( 3, ext.width(), ext.height() + y );
        }
        else
        {
            points.setPoint( 0, 0, ext.height() );
            points.setPoint( 1, x, ext.height() + y );
            points.setPoint( 2, ext.width(), 0 );
            points.setPoint( 3, ext.width() + x, y );
        }
        realOrig.setX( realOrig.x() - x / 2.0 );
        realOrig.setY( realOrig.y() - y / 2.0 );
        size.setWidth( size.width() + x );
        size.setHeight( size.height() + y );
    }

    if ( angle == 0.0 && lineType == LT_HORZ )
    {
        realOrig.setY( realOrig.y() + ( ext.height() - pen.width() ) / 2.0 );
    }
    else if ( angle == 0.0 && lineType == LT_VERT )
    {
        realOrig.setX( realOrig.x() + ( ext.width() - pen.width() ) / 2.0 );
    }
    else
    {
        if ( lineType == LT_HORZ )
        {
            points.setPoint( 0, 0, ( ext.height() - pen.width() ) / 2.0 );
            points.setPoint( 1, 0, ( ext.height() + pen.width() ) / 2.0 );
            points.setPoint( 2, ext.width(), ( ext.height() - pen.width() ) / 2.0 );
            points.setPoint( 3, ext.width(), ( ext.height() + pen.width() ) / 2.0 );
        }
        else if ( lineType == LT_VERT )
        {
            points.setPoint( 0, ( ext.width() - pen.width() ) / 2.0, 0 );
            points.setPoint( 1, ( ext.width() + pen.width() ) / 2.0, 0 );
            points.setPoint( 2, ( ext.width() - pen.width() ) / 2.0, ext.height() );
            points.setPoint( 3, ( ext.width() + pen.width() ) / 2.0, ext.height() );
        }

        getRealSizeAndOrigFromPoints( points, angle, size, realOrig );
    }

    return realOrig;
}
