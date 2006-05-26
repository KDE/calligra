/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Reginald Stadlbauer <reggie@kde.org>
   Copyright (C) 2005-2006 Thorsten Zachmann <zachmann@kde.org>

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

#include "KPrPieObject.h"
#include "KPrGradient.h"
#include "KPrUtils.h"
#include "KPrPieObjectIface.h"

#include <KoTextZoomHandler.h>
#include <KoOasisContext.h>
#include <KoStyleStack.h>
#include <KoXmlNS.h>

#include <kdebug.h>

#include <qregion.h>
#include <q3picture.h>
#include <qdom.h>
#include <qpainter.h>
#include <qbitmap.h>
using namespace std;

KPrPieObject::KPrPieObject()
: KPr2DObject()
, KPrStartEndLine( L_NORMAL, L_NORMAL )
{
    pieType = PT_PIE;
    p_angle = 45 * 16;
    p_len = 270 * 16;
}

KPrPieObject::KPrPieObject( const KoPen &_pen, const QBrush &_brush, FillType _fillType,
                          const QColor &_gColor1, const QColor &_gColor2, BCType _gType,
                          PieType _pieType, int _p_angle, int _p_len,
                          LineEnd _lineBegin, LineEnd _lineEnd,
                          bool _unbalanced, int _xfactor, int _yfactor )
: KPr2DObject( _pen, _brush, _fillType, _gColor1, _gColor2, _gType, _unbalanced, _xfactor, _yfactor )
, KPrStartEndLine( _lineBegin, _lineEnd )
{
    pieType = _pieType;
    p_angle = _p_angle;
    p_len = _p_len;
}

DCOPObject* KPrPieObject::dcopObject()
{
    if ( !dcop )
        dcop = new KPrPieObjectIface( this );
    return dcop;
}

KPrPieObject &KPrPieObject::operator=( const KPrPieObject & )
{
    return *this;
}

QDomDocumentFragment KPrPieObject::save( QDomDocument& doc, double offset )
{
    QDomDocumentFragment fragment=KPr2DObject::save(doc, offset);
    KPrStartEndLine::save( fragment, doc );
    if (p_angle!=720)
        fragment.appendChild(KPrObject::createValueElement("PIEANGLE", p_angle, doc));
    if (p_len!=1440)
        fragment.appendChild(KPrObject::createValueElement("PIELENGTH", p_len, doc));
    if (pieType!=PT_PIE)
        fragment.appendChild(KPrObject::createValueElement("PIETYPE", static_cast<int>(pieType), doc));
    return fragment;
}

bool KPrPieObject::saveOasisObjectAttributes( KPOasisSaveContext &sc ) const
{
    switch( pieType )
    {
        case PT_PIE:
            sc.xmlWriter.addAttribute( "draw:kind", "section" );
            break;
        case PT_CHORD:
            sc.xmlWriter.addAttribute( "draw:kind", "cut" );
            break;
        case PT_ARC:
            sc.xmlWriter.addAttribute( "draw:kind", "arc" );
            break;
        default:
            kDebug() << " type of pie not supported" << endl;
    }

    int startangle = ( (int)p_angle / 16 );
    sc.xmlWriter.addAttribute( "draw:start-angle", startangle );

    int endangle = ( (int) p_len / 16 ) + startangle;
    sc.xmlWriter.addAttribute( "draw:end-angle", endangle );

    return true;
}

void KPrPieObject::fillStyle( KoGenStyle& styleObjectAuto, KoGenStyles& mainStyles ) const
{
    KPrShadowObject::fillStyle( styleObjectAuto, mainStyles );
    if ( pieType == PT_ARC )
    {
        saveOasisMarkerElement( mainStyles, styleObjectAuto );
    }
    else
    {
        m_brush.saveOasisFillStyle( styleObjectAuto, mainStyles );
    }
}

const char * KPrPieObject::getOasisElementName() const
{
    return ext.width() == ext.height() ? "draw:circle" : "draw:ellipse";
}


void KPrPieObject::loadOasis(const QDomElement &element, KoOasisContext & context, KPrLoadingInfo *info)
{
    kDebug()<<"void KPrPieObject::loadOasis(const QDomElement &element) ***************\n";
    KPr2DObject::loadOasis(element, context, info);
    QString kind = element.attributeNS( KoXmlNS::draw, "kind", QString::null );
    if ( kind == "section" )
        pieType = PT_PIE;
    else if ( kind == "cut" )
        pieType = PT_CHORD;
    else if ( kind == "arc" )
        pieType =PT_ARC;
    else
    {
        kDebug()<<" KPrPieObject::loadOasis(const QDomElement &element) type indefined :"<<kind<<endl;
        pieType = PT_PIE;
    }
    kDebug()<<" type of pie object :"<<( ( pieType == PT_PIE ) ? "pie" : ( pieType == PT_CHORD )?"cut" : "arc" )<<endl;

    int start = (int) ( element.attributeNS( KoXmlNS::draw, "start-angle", QString::null ).toDouble() );
    p_angle=start*16;

    int end = (int) ( element.attributeNS( KoXmlNS::draw, "end-angle", QString::null ).toDouble() );
    if ( end < start )
        p_len = ( ( 360 - start + end ) * 16 );
    else
        p_len = (  ( end - start ) * 16 );

    kDebug()<<"KPrPieObject::loadOasis(const QDomElement &element) : p_angle :"<<p_angle<<" p_len :"<<p_len<<endl;
    if ( pieType == PT_ARC )
    {
        loadOasisMarkerElement( context, "marker-start", lineBegin );
        loadOasisMarkerElement( context, "marker-end", lineEnd );
    }
}

double KPrPieObject::load(const QDomElement &element)
{
    double offset=KPr2DObject::load(element);
    KPrStartEndLine::load( element );
    QDomElement e=element.namedItem("PIEANGLE").toElement();
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
    else
        p_len=1440; //necessary to reinitialise p_len
    //I don't know who change default value
    e=element.namedItem("PIETYPE").toElement();
    if(!e.isNull()) {
        int tmp=0;
        if(e.hasAttribute("value"))
            tmp=e.attribute("value").toInt();
        pieType=static_cast<PieType>(tmp);
    }
    return offset;
}

void KPrPieObject::paint( QPainter* _painter, KoTextZoomHandler*_zoomHandler,
                         int /* pageNum */, bool drawingShadow, bool drawContour )
{
    double ow = ext.width();
    double oh = ext.height();
    double pw = ( ( pen.style() == Qt::NoPen ) ? 1 : pen.pointWidth() ) / 2.0;

    if ( drawContour ) {
        QPen pen3( Qt::black, 1, Qt::DotLine );
        _painter->setPen( pen3 );
        _painter->setRasterOp( Qt::NotXorROP );
    }
    else {
        QPen pen2 = pen.zoomedPen( _zoomHandler );
        _painter->setPen( pen2 );
        if ( drawingShadow || getFillType() == FT_BRUSH || !gradient )
        {
            _painter->setBrush( getBrush() );
        }
        else
        {
            if ( pieType != PT_ARC )
            {
                QSize size( _zoomHandler->zoomSize( ext ) );

                if ( m_redrawGradientPix || gradient->size() != size )
                {
                    m_redrawGradientPix = false;
                    gradient->setSize( size );

                    m_gradientPix.resize ( size );
                    m_gradientPix.fill( Qt::white );
                    QPainter p;
                    p.begin( &m_gradientPix );
                    p.drawPixmap( 0, 0, gradient->pixmap() );
                    p.end();

                    QBitmap mask( size, true );
                    p.begin( &mask );
                    p.setPen( QPen( Qt::color1 ) );
                    p.setBrush( QBrush( Qt::color1 ) );
                    if ( pieType == PT_CHORD )
                    {
                        p.drawChord( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                                     _zoomHandler->zoomItX(ow - 2 * pw),
                                     _zoomHandler->zoomItY(oh - 2 * pw), p_angle, p_len );
                    }
                    else
                    {
                        p.drawPie( _zoomHandler->zoomItX(pw), _zoomHandler->zoomItY(pw),
                                   _zoomHandler->zoomItX( ow - 2 * pw),
                                   _zoomHandler->zoomItY( oh - 2 * pw), p_angle, p_len );
                    }
                    p.end();
                    m_gradientPix.setMask( mask );
                }
                _painter->drawPixmap( 0, 0, m_gradientPix, 0, 0, size.width(), size.height() );
                _painter->setBrush( Qt::NoBrush );
            }
        }
        if ( pieType == PT_ARC )
        {
            KoPointArray points( 2 );
            setEndPoints( points );
            KoPoint start( points.point( 0 ) );
            KoPoint end( points.point( 1 ) );

            double ys = ( ( 1 - start.x() / ( ext.width() * ext.width() / 4 ) ) * ext.height() * ext.height() / 4 ) / start.y();
            double s_angle = 90 + ( atan( ( start.x() - 1 ) / ( start.y() - ys ) ) * 180 / M_PI );
            if ( p_angle / 16 >= 90 && p_angle / 16 <= 270 )
            {
                s_angle += 180.0;
            }
            double ye = ( ( 1 - end.x() / ( ext.width() * ext.width() / 4 ) ) * ext.height() * ext.height() / 4 ) / end.y();
            double e_angle = 270 + ( atan( ( end.x() - 1 ) / ( end.y() - ye ) ) * 180 / M_PI );
            if ( ( ( p_angle + p_len ) / 16 ) % 360 >= 90 && ( ( p_angle + p_len ) / 16 ) % 360 <= 270 )
            {
                e_angle -= 180.0;
            }

            start = KoPoint( ext.width() / 2.0 + start.x(), ext.height() / 2.0 - start.y() );
            end = KoPoint( ext.width() / 2.0 + end.x(), ext.height() / 2.0 - end.y() );



            if ( lineBegin != L_NORMAL )
                drawFigureWithOffset( lineBegin, _painter, start,
                            pen2.color(), int( pen.pointWidth() ), s_angle, _zoomHandler, true );

            if ( lineEnd != L_NORMAL )
                drawFigureWithOffset( lineEnd, _painter, end,
                            pen2.color(), int( pen.pointWidth() ), e_angle, _zoomHandler, false );
        }
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

void KPrPieObject::flip( bool horizontal )
{
    KPr2DObject::flip( horizontal );
    if ( ! horizontal )
    {
        p_angle = 360*16 - p_angle -p_len;
    }
    else
    {
        p_angle = 180*16 - p_angle - p_len;
    }
    // angle smaller 0
    while ( p_angle < 0 ) {
        p_angle += 360*16;
    }

}


void KPrPieObject::setMinMax( double &min_x, double &min_y,
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
void KPrPieObject::getRealSizeAndOrig( KoSize &size, KoPoint &realOrig ) const {
    double radius1 = size.width() / 2.0;
    double radius2 = size.height() / 2.0;

    // the rotation angle
    double angInRad = angle * M_PI / 180;

    // 1. calulate position of end points
    KoPointArray points(2);
    setEndPoints( points );

    // rotate point
    for ( int i = 0; i < 2; i++ ) {
        if ( angle != 0 ) {
            double sinus = sin( angInRad );
            double cosinus = cos( angInRad );

            double tmp_x = points.point( i ).x();
            double tmp_y = points.point( i ).y();

            double x = tmp_x * cosinus + tmp_y * sinus;
            double y = - tmp_x * sinus + tmp_y * cosinus;
            points.setPoint( i, x, y );
        }
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

void KPrPieObject::setEndPoints( KoPointArray &points ) const
{
    int angles[] = { p_angle, ( p_angle + p_len ) % ( 16 * 360 ) };
    double anglesInRad[] = { p_angle / 16.0 * M_PI / 180, ( angles[1] ) / 16.0 * M_PI / 180 };

    double radius1 = ext.width() / 2.0;
    double radius2 = ext.height() / 2.0;

    double prop = radius2 / radius1;

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
        points.setPoint( i, x, y );
    }
}

KoSize KPrPieObject::getRealSize() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    getRealSizeAndOrig( size, realOrig );
    return size;
}


KoPoint KPrPieObject::getRealOrig() const {
    KoSize size( ext );
    KoPoint realOrig( orig );
    getRealSizeAndOrig( size, realOrig );
    return realOrig;
}
