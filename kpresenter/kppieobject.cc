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

#include "kppieobject.h"
#include "kpgradient.h"
#include "KPPieObjectIface.h"

#include <qregion.h>
#include <qpicture.h>
#include <qdom.h>
#include <qpainter.h>
#include <kdebug.h>
#include <kozoomhandler.h>
#include <kooasiscontext.h>
#include <koStyleStack.h>
using namespace std;

KPPieObject::KPPieObject()
    : KP2DObject()
{
    pieType = PT_PIE;
    p_angle = 720; //45 * 16
    p_len = 1440; //90 * 16
    lineBegin = L_NORMAL;
    lineEnd = L_NORMAL;
}

KPPieObject::KPPieObject( const QPen &_pen, const QBrush &_brush, FillType _fillType,
                          const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                          PieType _pieType, int _p_angle, int _p_len,
                          LineEnd _lineBegin, LineEnd _lineEnd,
                          bool _unbalanced, int _xfactor, int _yfactor )
    : KP2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
{
    pieType = _pieType;
    p_angle = _p_angle;
    p_len = _p_len;
    lineBegin = _lineBegin;
    lineEnd = _lineEnd;
}

DCOPObject* KPPieObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPPieObjectIface( this );
    return dcop;
}

KPPieObject &KPPieObject::operator=( const KPPieObject & )
{
    return *this;
}

QDomDocumentFragment KPPieObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KP2DObject::save(doc, offset);
    if (lineBegin!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEBEGIN", static_cast<int>(lineBegin), doc));
    if (lineEnd!=L_NORMAL)
        fragment.appendChild(KPObject::createValueElement("LINEEND", static_cast<int>(lineEnd), doc));
    if (p_angle!=720)
        fragment.appendChild(KPObject::createValueElement("PIEANGLE", p_angle, doc));
    if (p_len!=1440)
        fragment.appendChild(KPObject::createValueElement("PIELENGTH", p_len, doc));
    if (pieType!=PT_PIE)
        fragment.appendChild(KPObject::createValueElement("PIETYPE", static_cast<int>(pieType), doc));
    return fragment;
}

bool KPPieObject::saveOasis( KoXmlWriter &xmlWriter, KoSavingContext& context, int indexObj ) const
{
    xmlWriter.startElement( ( ext.width() == ext.height() ) ? "draw:circle" : "draw:ellipse" );
    xmlWriter.addAttribute( "draw:style-name", KP2DObject::saveOasisBackgroundStyle( xmlWriter, context.mainStyles(),indexObj ) );

    if( !objectName.isEmpty())
        xmlWriter.addAttribute( "draw:name", objectName );
    switch( pieType )
    {
    case PT_PIE:
        xmlWriter.addAttribute( "draw:kind", "section" );
        break;
    case PT_CHORD:
        xmlWriter.addAttribute( "draw:kind", "cut" );
        break;
    case PT_ARC:
        xmlWriter.addAttribute( "draw:kind", "arc" );
        break;
    default:
        kdDebug()<<" type of pie not supported\n";
    }
    int startangle = 45;
    if ( p_angle != 0.0 )
        startangle = ( ( int )p_angle )/16;
    xmlWriter.addAttribute( "draw:start-angle", startangle );
    int endangle = endangle = ( ( int ) p_len/16 )+startangle;
    xmlWriter.addAttribute( "draw:end-angle", endangle );

    //we don't have a simple object
    xmlWriter.endElement();
    return true;
}


void KPPieObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPRLoadingInfo *info)
{
    kdDebug()<<"void KPPieObject::loadOasis(const QDomElement &element) ***************\n";
    KP2DObject::loadOasis(element, context, info);
    QString kind = element.attribute( "draw:kind" );
    if ( kind == "section" )
        pieType = PT_PIE;
    else if ( kind == "cut" )
        pieType = PT_CHORD;
    else if ( kind == "arc" )
        pieType =PT_ARC;
    else
    {
        kdDebug()<<" KPPieObject::loadOasis(const QDomElement &element) type indefined :"<<kind<<endl;
        pieType = PT_PIE;
    }
    kdDebug()<<" type of pie object :"<<( ( pieType == PT_PIE ) ? "pie" : ( pieType == PT_CHORD )?"cut" : "arc" )<<endl;

    int start = (int) ( element.attribute( "draw:start-angle" ).toDouble() );
    p_angle=start*16;

    int end = (int) ( element.attribute( "draw:end-angle" ).toDouble() );
    if ( end < start )
        p_len = ( ( 360 - start + end ) * 16 );
    else
        p_len = (  ( end - start ) * 16 );

    kdDebug()<<"KPPieObject::loadOasis(const QDomElement &element) : p_angle :"<<p_angle<<" p_len :"<<p_len<<endl;
    KoStyleStack & styleStack = context.styleStack();
    styleStack.setTypeProperties( "" );

    //fixme !!!!!!
    //we use value use into oo and not style (see kplineobject)
    if ( styleStack.hasAttribute( "draw:marker-start" ) )
    {
        QString type = styleStack.attribute( "draw:marker-start" );
        kdDebug()<<"type arrow start :"<<type<<endl;
        if ( type == "Arrow" || type == "Small Arrow" || type == "Rounded short Arrow" ||
             type == "Symmetric Arrow" || type == "Rounded large Arrow" || type == "Arrow concave" )
            lineBegin =  L_ARROW;
        else if ( type == "Square" )
            lineBegin =  L_SQUARE;
        else if ( type == "Circle" || type == "Square 45" )
            lineBegin = L_CIRCLE;
        else if ( type == "Line Arrow" )
            lineBegin = L_LINE_ARROW;
        else if ( type == "Dimension Lines" )
            lineBegin = L_DIMENSION_LINE;
        else if ( type == "Double Arrow" )
            lineBegin = L_DOUBLE_LINE_ARROW;
    }
    if ( styleStack.hasAttribute( "draw:marker-end" ) )
    {
        QString type = styleStack.attribute( "draw:marker-end" );
        kdDebug()<<"type arrow end :"<<type<<endl;
        if ( type == "Arrow" || type == "Small Arrow" || type == "Rounded short Arrow" ||
             type == "Symmetric Arrow" || type == "Rounded large Arrow" || type == "Arrow concave" )
            lineEnd =  L_ARROW;
        else if ( type == "Square" )
            lineEnd =  L_SQUARE;
        else if ( type == "Circle" || type == "Square 45" )
            lineEnd = L_CIRCLE;
        else if ( type == "Line Arrow" )
            lineEnd = L_LINE_ARROW;
        else if ( type == "Dimension Lines" )
            lineEnd = L_DIMENSION_LINE;
        else if ( type == "Double Arrow" )
            lineEnd = L_DOUBLE_LINE_ARROW;
    }
}

double KPPieObject::load(const QDomElement &element)
{
    double offset=KP2DObject::load(element);
    QDomElement e=element.namedItem("LINEBEGIN").toElement();
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
    e=element.namedItem("PIEANGLE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        p_angle=tmp;
    }
    e=element.namedItem("PIELENGTH").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        p_len=tmp;
    }
    e=element.namedItem("PIETYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        pieType=static_cast<PieType>(tmp);
    }
    return offset;
}

void KPPieObject::paint( QPainter* _painter, KoZoomHandler*_zoomHandler,
                         bool /*drawingShadow*/, bool drawContour )
{
    double ow = ext.width();
    double oh = ext.height();
    double pw = ( ( pen.style() == Qt::NoPen ) ? 1 : pen.width() ) / 2.0;

    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        QPen pen2( pen );
        pen2.setWidth(_zoomHandler->zoomItX( pen2.width()));
        _painter->setPen( pen2 );
        _painter->setBrush( brush );
    }
    switch ( pieType )
    {
    case PT_PIE:
        _painter->drawPie( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY( pw),
                           _zoomHandler->zoomItX( ow - 2 * pw),
                           _zoomHandler->zoomItY( oh - 2 * pw), p_angle, p_len );
        break;
    case PT_ARC:
        _painter->drawArc( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                           _zoomHandler->zoomItX(ow - 2 * pw),
                           _zoomHandler->zoomItY(oh - 2 * pw), p_angle, p_len );
        break;
    case PT_CHORD:
        _painter->drawChord( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                             _zoomHandler->zoomItX(ow - 2 * pw),
                             _zoomHandler->zoomItY(oh - 2 * pw), p_angle, p_len );
        break;
    default: break;
    }
}

void KPPieObject::flip( bool horizontal )
{
    KP2DObject::flip( horizontal );
    if ( horizontal )
    {
        p_angle = 360*16 - p_angle -p_len;
    }
    else
    {
        p_angle = 180*16 - p_angle - p_len;
    }
    // angle smaller 0°
    while ( p_angle < 0 ) {
        p_angle += 360*16;
    }

}


void KPPieObject::setMinMax( double &min_x, double &min_y,
                             double &max_x, double &max_y, KoPoint point ) const
{
    double tmp_x = point.x();
    double tmp_y = point.y();

    if ( tmp_x < min_x ) {
        min_x = tmp_x;
    }
    else if ( tmp_x > max_x ) {
        max_x = tmp_x;
    }

    if ( tmp_y < min_y ) {
        min_y = tmp_y;
    }
    else if ( tmp_y > max_y ) {
        max_y = tmp_y;
    }

    kdDebug(33001) << "setMinMax min(x,y) max(x,y) min(" << min_x << "," << min_y << ") max(" << max_x << "," << max_y << ")" << endl;
}


/*
 * The calculation of the real size and origin for a pie object is a little more
 * complicated. It took me quite a whlie to get it right.
 * Here is how it works:
 * 1. calculate the position of the end points
 * 2. calculate the 4 maximal points, the points with max x or y position, of the
 *    hole ellipse
 * 3. find minimal and maximal points
 * 4. check if the maximal points lie on the arc
 *
 */
void KPPieObject::getRealSizeAndOrig( KoSize &size, KoPoint &realOrig ) const {
    // the angles of the object
    int angles[] = { p_angle, ( p_angle + p_len ) % ( 16 * 360 ) };
    double anglesInRad[] = { p_angle / 16.0 * M_PI / 180, ( angles[1] ) / 16.0 * M_PI / 180 };

    double radius1 = size.width() / 2.0;
    double radius2 = size.height() / 2.0;

    double prop = radius2 / radius1;

    // the rotation angle
    double angInRad = angle * M_PI / 180;

    // 1. calulate position of end points
    KoPointArray points(2);
    for ( int i = 0; i < 2; i++ ) {
        double x = 0;
        double y = 0;

        // be carefull
        if ( angles[i] == 90 * 16 ) {
            y = radius2;
        }
        else if ( angles[i] == 270 * 16 ) {
            y = -radius2;
        }
        else {
            // The real angle is not what was given. It is only ok if radius1 == radius2,
            // otherwise it is arctan ( radius2 / radius1 tan ( angle ) )
            double tanalpha = tan( anglesInRad[i] ) * prop;
            x = sqrt( 1 / ( pow ( 1 / radius1, 2 ) + pow( tanalpha / radius2, 2 ) ) );
            if ( angles[i] > 90 * 16 && angles[i] < 270 * 16 )
              x = -x;
            y = tanalpha * x;
        }

        // rotate point
        if ( angle != 0 ) {
            double sinus = sin( angInRad );
            double cosinus = cos( angInRad );

            double tmp_x = x;
            double tmp_y = y;

            x = tmp_x * cosinus + tmp_y * sinus;
            y = - tmp_x * sinus + tmp_y * cosinus;
        }
        points.setPoint( i, x, y );
    }

    KoPoint firstPoint( points.point(0) );
    KoPoint secondPoint( points.point(1) );

    // 2. calulate maximal points
    KoPointArray maxPoints(4);
    if ( angle == 0 ) {
        maxPoints.setPoint( 0, 0, radius2 );
        maxPoints.setPoint( 1, radius1, 0 );
        maxPoints.setPoint( 2, 0, -radius2 );
        maxPoints.setPoint( 3, -radius1, 0 );
    }
    else {
        double sinus = sin( angInRad );
        double cosinus = cos( angInRad );

        double x = sqrt( pow( radius1 * cosinus , 2 ) + pow(radius2 * sinus, 2));
        double y = ( pow( radius2, 2 ) - pow( radius1, 2) ) * sinus * cosinus / x;
        maxPoints.setPoint( 0, x, y );
        maxPoints.setPoint( 1, -x, -y );

        y = sqrt( pow( radius1 * sinus , 2 ) + pow(radius2 * cosinus, 2));
        x = ( pow( radius1, 2 ) - pow( radius2, 2) ) * sinus * cosinus / y;
        maxPoints.setPoint( 2, x, y);
        maxPoints.setPoint( 3, -x, -y );
    }

    // 3. find minimal and maximal points
    double min_x = firstPoint.x();
    double min_y = firstPoint.y();
    double max_x = firstPoint.x();
    double max_y = firstPoint.y();

    if ( pieType == PT_PIE ) {
        KoPoint zero(0,0);
        setMinMax( min_x, min_y, max_x, max_y, zero );
    }
    setMinMax( min_x, min_y, max_x, max_y, secondPoint );

    /* 4. check if maximal points lie on the arc.
     * There are three posibilities how many sections have to
     * been checked.
     * 1. the arc is only once on one side of the x axis
     * 2. the arc is on both sides of the x axis
     * 3. the arc is twice on one one side of the x axis
     *
     * 1)                 2)              3)
     *      y                  y               y
     *    ex|xx              xx|xs           s |
     *      |  x            x  |            x  |  e
     *      |   s          x   |           x   |   x
     *  ----+----  x       ----+----  x    ----+----  x
     *      |              x   |           x   |   x
     *      |               x  |            x  |  x
     *      |                e |             xx|xx
     *
     */
    if ( firstPoint.y() >= 0 ) {
        if ( secondPoint.y() >= 0 ) {
            if ( firstPoint.x() > secondPoint.x() || p_len == 0 ) {
                // 1 section
                // f.x() <= x <= s.x() && y >= 0
                KoPointArray::ConstIterator it( maxPoints.begin() );
                for ( ; it != maxPoints.end(); ++it ) {
                    if ( (*it).y() >= 0 &&
                         (*it).x() <= firstPoint.x() && (*it).x() >= secondPoint.x() )
                    {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
            }
            else {
                // 3 sections
                // x <= f.x() && y >= 0
                // y < 0
                // x >= s.x() && y >= 0
                KoPointArray::ConstIterator it( maxPoints.begin() );
                for ( ; it != maxPoints.end(); ++it ) {
                    if ( (*it).y() >= 0 ) {
                        if ( (*it).x() <= firstPoint.x() || (*it).x() >= secondPoint.x() ) {
                            setMinMax( min_x, min_y, max_x, max_y, *it );
                        }
                    }
                    else {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
            }
        }
        else {
            // 2 sections
            // x <= f.x() && y >= 0
            // x <= s.x() && y < 0
            KoPointArray::ConstIterator it( maxPoints.begin() );
            for ( ; it != maxPoints.end(); ++it ) {
                if ( (*it).y() >= 0 ) {
                    if ( (*it).x() <= firstPoint.x() ) {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
                else {
                    if ( (*it).x() <= secondPoint.x() ) {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
            }
        }
    }
    else {
        if ( secondPoint.y() >= 0 ) {
            // 2 sections
            // x >= f.x() && y < 0
            // x >= s.x() && y >= 0
            KoPointArray::ConstIterator it( maxPoints.begin() );
            for ( ; it != maxPoints.end(); ++it ) {
                if ( (*it).y() < 0 ) {
                    if ( (*it).x() >= firstPoint.x() ) {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
                else {
                    if ( (*it).x() >= secondPoint.x() ) {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
            }
        }
        else {
            if ( firstPoint.x() < secondPoint.x() || p_len == 0 ) {
                // 1 section
                // f.x() <= x <= s.x() && y < 0
                KoPointArray::ConstIterator it( maxPoints.begin() );
                for ( ; it != maxPoints.end(); ++it ) {
                    if ( (*it).y() < 0 &&
                         (*it).x() >= firstPoint.x() && (*it).x() <= secondPoint.x() )
                    {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
            }
            else {
                // 3 sections
                // x >= f.x() && y < 0
                // y >= 0
                // x <= s.x() && y < 0
                KoPointArray::ConstIterator it( maxPoints.begin() );
                for ( ; it != maxPoints.end(); ++it ) {
                    if ( (*it).y() < 0 ) {
                        if ( (*it).x() >= firstPoint.x() || (*it).x() <= secondPoint.x() ) {
                            setMinMax( min_x, min_y, max_x, max_y, *it );
                        }
                    }
                    else {
                        setMinMax( min_x, min_y, max_x, max_y, *it );
                    }
                }
            }
        }
    }

    double mid_x = size.width() / 2;
    double mid_y = size.height() / 2;

    size.setWidth( max_x - min_x );
    size.setHeight( max_y - min_y );

    realOrig.setX( realOrig.x() + mid_x + min_x );
    realOrig.setY( realOrig.y() + mid_y - max_y );
}

KoSize KPPieObject::getRealSize() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    getRealSizeAndOrig( size, realOrig );
    return size;
}


KoPoint KPPieObject::getRealOrig() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    getRealSizeAndOrig( size, realOrig );
    return realOrig;
}
