// -*- Mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4; -*-
/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrLineObject.h"
#include "KPrUtils.h"
#include "KPrDocument.h"
#include "KPrLineObjectIface.h"
#include "KoPointArray.h"
#include <KoStyleStack.h>
#include <KoOasisContext.h>

#include <qpainter.h>
#include <qmatrix.h>
#include <qdom.h>
#include <kdebug.h>
#include <KoUnit.h>
#include <KoTextZoomHandler.h>
#include <math.h>
#include <KoDom.h>
#include <KoXmlNS.h>

using namespace std;

KPrLineObject::KPrLineObject()
    : KPrShadowObject(), KPrStartEndLine( L_NORMAL, L_NORMAL )
{
    lineType = LT_HORZ;
}

KPrLineObject::KPrLineObject( const KoPen &_pen, LineEnd _lineBegin,
                            LineEnd _lineEnd, LineType _lineType )
    : KPrShadowObject( _pen ), KPrStartEndLine( _lineBegin, _lineEnd )
{
    lineType = _lineType;
}

KPrLineObject &KPrLineObject::operator=( const KPrLineObject & )
{
    return *this;
}

DCOPObject* KPrLineObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPrLineObjectIface( this );
    return dcop;
}


void KPrLineObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const
{
    KPrShadowObject::fillStyle( styleObjectAuto, mainStyles );
    saveOasisMarkerElement( mainStyles, styleObjectAuto );
}


bool KPrLineObject::saveOasisObjectAttributes( KPOasisSaveContext &/*sc*/ ) const
{
    // nothing to do
    return true;
}

void KPrLineObject::saveOasisPosObject( KoXmlWriter &xmlWriter, int indexObj ) const
{
    xmlWriter.addAttribute( "draw:id", "object" + QString::number( indexObj ) );

    double x1 = 0.0;
    double y1 = 0.0;
    double x2 = 0.0;
    double y2 = 0.0;
    KoPoint center( ext.width() / 2, ext.height() / 2 );

    switch ( lineType )
    {
        case LT_LD_RU:
            x1 = -center.x();
            y1 = center.y();
            x2 = -x1;
            y2 = -y1;
            break;
        case LT_HORZ:
            x1 = -center.x();
            x2 = -x1;
            break;
        case LT_VERT:
            y1 = -center.y();
            y2 = -y1;
            break;
        case LT_LU_RD:
            x1 = -center.x();
            y1 = -center.y();
            x2 = -x1;
            y2 = -y1;
            break;
    }
    if ( qAbs( angle ) > 1E-6 )
    {
        double angInRad = -angle * M_PI / 180.0;
        QMatrix m( cos( angInRad ), -sin( angInRad ), sin( angInRad ), cos( angInRad ), 0, 0 );
        double transX1 = 0.0;
        double transY1 = 0.0;
        double transX2 = 0.0;
        double transY2 = 0.0;
        m.map( x1, y1, &transX1, &transY1 );
        m.map( x2, y2, &transX2, &transY2 );
        x1 = transX1;
        y1 = transY1;
        x2 = transX2;
        y2 = transY2;
    }
    
    x1 += orig.x() + center.x();
    y1 += orig.y() + center.y();
    x2 += orig.x() + center.x();
    y2 += orig.y() + center.y();

    //save all into pt
    xmlWriter.addAttributePt( "svg:x1", x1 );
    xmlWriter.addAttributePt( "svg:y1", y1 );
    xmlWriter.addAttributePt( "svg:x2", x2 );
    xmlWriter.addAttributePt( "svg:y2", y2 );
}

const char * KPrLineObject::getOasisElementName() const
{
    return "draw:line";
}

QDomDocumentFragment KPrLineObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPrShadowObject::save(doc, offset);
    if (lineType!=LT_HORZ)
        fragment.appendChild(KPrObject::createValueElement("LINETYPE", static_cast<int>(lineType), doc));
    KPrStartEndLine::save( fragment, doc );
    return fragment;
}

void KPrLineObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info)
{
    KPrShadowObject::loadOasis(element, context, info);

    double x1 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x1", QString::null ) );
    double y1 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y1", QString::null ) );
    double x2 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "x2", QString::null ) );
    double y2 = KoUnit::parseValue( element.attributeNS( KoXmlNS::svg, "y2", QString::null ) );

    kDebug()<<" KPrLineObject::loadOasis(const QDomElement &element) : x1 "<< x1 <<" y1 : "<<y1<<" x2 :"<<x2 <<" y2 "<<y2<<endl;
    double x = qMin( x1, x2 );
    double y = qMin( y1, y2 );

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

    kDebug()<<"KPrLineObject::loadOasis(const QDomElement &element) : real position x :"<<orig.x()<<" y "<<orig.y()<< " width :"<<ext.width()<<" height :"<<ext.height()<<endl;

    QString attr = (x1 <= x2) ?  "marker-start" : "marker-end";
    loadOasisMarkerElement( context, attr, lineBegin );

    attr = (x1 <= x2) ?  "marker-end" : "marker-start";
    loadOasisMarkerElement( context, attr, lineEnd );
}

double KPrLineObject::load(const QDomElement &element)
{
    double offset=KPrShadowObject::load(element);
    QDomElement e=element.namedItem("LINETYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        lineType=static_cast<LineType>(tmp);
    }
    KPrStartEndLine::load( element );
    return offset;
}

void KPrLineObject::paint( QPainter* _painter, KoTextZoomHandler*_zoomHandler,
                          int /* pageNum */, bool /*drawingShadow*/, bool drawContour )
{
    double ow = ext.width();
    double oh = ext.height();
    int _w = int( pen.pointWidth() );

    QPen pen2;
    if ( drawContour ) {
        pen2 = QPen( Qt::black, 1, Qt::DotLine );
#warning "kde4: port it"		
        //_painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        pen2 = pen.zoomedPen( _zoomHandler );
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

void KPrLineObject::flip( bool horizontal )
{
    KPrObject::flip( horizontal );
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

KoSize KPrLineObject::getRealSize() const {
    KoPoint realOrig( orig );
    KoSize size( ext );

    KoPointArray points(4);

    if ( lineType == LT_LU_RD || lineType == LT_LD_RU )
    {
        double objAngle = atan( ext.width() / ext.height() );
        double x = cos( objAngle ) * pen.pointWidth();
        double y = sin( objAngle ) * pen.pointWidth();

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
        size.setHeight( pen.pointWidth() );
    }
    else if ( angle == 0.0 && lineType == LT_VERT )
    {
        size.setWidth( pen.pointWidth() );
    }
    else
    {
        if ( lineType == LT_HORZ )
        {
            points.setPoint( 0, 0, ( ext.height() - pen.pointWidth() ) / 2.0 );
            points.setPoint( 1, 0, ( ext.height() + pen.pointWidth() ) / 2.0 );
            points.setPoint( 2, ext.width(), ( ext.height() - pen.pointWidth() ) / 2.0 );
            points.setPoint( 3, ext.width(), ( ext.height() + pen.pointWidth() ) / 2.0 );
        }
        else if ( lineType == LT_VERT )
        {
            points.setPoint( 0, ( ext.width() - pen.pointWidth() ) / 2.0, 0 );
            points.setPoint( 1, ( ext.width() + pen.pointWidth() ) / 2.0, 0 );
            points.setPoint( 2, ( ext.width() - pen.pointWidth() ) / 2.0, ext.height() );
            points.setPoint( 3, ( ext.width() + pen.pointWidth() ) / 2.0, ext.height() );
        }

        getRealSizeAndOrigFromPoints( points, angle, size, realOrig );
    }

    return size;
}

KoPoint KPrLineObject::getRealOrig() const {
    KoPoint realOrig( orig );
    KoSize size( ext );

    KoPointArray points(4);

    if ( lineType == LT_LU_RD || lineType == LT_LD_RU )
    {
        double objAngle = atan( ext.width() / ext.height() );
        double x = cos( objAngle ) * pen.pointWidth();
        double y = sin( objAngle ) * pen.pointWidth();

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
        realOrig.setY( realOrig.y() + ( ext.height() - pen.pointWidth() ) / 2.0 );
    }
    else if ( angle == 0.0 && lineType == LT_VERT )
    {
        realOrig.setX( realOrig.x() + ( ext.width() - pen.pointWidth() ) / 2.0 );
    }
    else
    {
        if ( lineType == LT_HORZ )
        {
            points.setPoint( 0, 0, ( ext.height() - pen.pointWidth() ) / 2.0 );
            points.setPoint( 1, 0, ( ext.height() + pen.pointWidth() ) / 2.0 );
            points.setPoint( 2, ext.width(), ( ext.height() - pen.pointWidth() ) / 2.0 );
            points.setPoint( 3, ext.width(), ( ext.height() + pen.pointWidth() ) / 2.0 );
        }
        else if ( lineType == LT_VERT )
        {
            points.setPoint( 0, ( ext.width() - pen.pointWidth() ) / 2.0, 0 );
            points.setPoint( 1, ( ext.width() + pen.pointWidth() ) / 2.0, 0 );
            points.setPoint( 2, ( ext.width() - pen.pointWidth() ) / 2.0, ext.height() );
            points.setPoint( 3, ( ext.width() + pen.pointWidth() ) / 2.0, ext.height() );
        }

        getRealSizeAndOrigFromPoints( points, angle, size, realOrig );
    }

    return realOrig;
}
