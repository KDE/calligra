/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KDChartPiePainter.h"
#include "KDChartParams.h"

#include <qpainter.h>
#include <qvaluestack.h>

#define DEGTORAD(d) (d)*M_PI/180

#ifdef __WINDOWS__
#include <math.h>
#else
#include <cmath>
#endif

#if defined __WINDOWS__ || defined SUN7 || ( defined HP11_aCC && defined HP1100 )
#define std
#endif


/**
   \class KDChartPiePainter KDChartPiePainter.h

   \brief A chart painter implementation that can paint pie charts.

   PENDING(kalle) Write more documentation.
*/

/**
   Constructor. Sets up internal data structures as necessary.

   \param params the KDChartParams structure that defines the chart
*/
KDChartPiePainter::KDChartPiePainter( KDChartParams* params ) :
    KDChartPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
   Destructor.
*/
KDChartPiePainter::~KDChartPiePainter()
{
    // intentionally left blank
}


/**
   Paints the actual data area.

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
   \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
   \param regions a pointer to a list of regions that will be filled
   with regions representing the data segments, if not null
*/
void KDChartPiePainter::paintData( QPainter* painter,
                                   KDChartTableData* data,
                                   bool paint2nd,
                                   KDChartDataRegionList* regions )
{
    painter->setClipRect( _dataRect );

    // find which dataset to paint
    uint dataset;
    if ( !params()->findDataset( KDChartParams::DataEntry
                                  ,
                                  dataset, dataset ) ) {
        return ; // nothing to draw
    }

    if ( dataset == KDChartParams::KDCHART_ALL_DATASETS )
        // setChartSourceMode() has not been used (or all datasets have been
        // configured to be used); use the first dataset by
        // default
        dataset = 0;


    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    if ( params()->numValues() != -1 )
        _numValues = params()->numValues();
    else
        _numValues = data->usedCols();

    _startAngles.resize( _numValues );
    _angleLens.resize( _numValues );

    // compute position
    _size = QMIN( _dataRect.width(), _dataRect.height() ); // initial size
    // if the pies explode, we need to give them additional space =>
    // make the basic size smaller
    if ( params()->explode() ) {
        double doubleSize = ( double ) _size;
        doubleSize /= ( 1.0 + params()->explodeFactor() * 2 );
        _size = ( int ) doubleSize;
    }

    int sizeFor3DEffect = 0;
    if ( !params()->threeDPies() ) {

        int x = ( _dataRect.width() == _size ) ? 0 : ( ( _dataRect.width() - _size ) / 2 );
        int y = ( _dataRect.height() == _size ) ? 0 : ( ( _dataRect.height() - _size ) / 2 );
        _position = QRect( x, y, _size, _size );
        _position.moveBy( _dataRect.left(), _dataRect.top() );
    } else {
        // threeD: width is the maximum possible width; height is 1/2 of that
        int x = ( _dataRect.width() == _size ) ? 0 : ( ( _dataRect.width() - _size ) / 2 );
        int height = _size;
        // make sure that the height plus the threeDheight is not more than the
        // available size
        if ( params()->threeDPieHeight() >= 0 ) {
            // positive pie height: absolute value
            sizeFor3DEffect = params()->threeDPieHeight();
            height = _size - sizeFor3DEffect;
        } else {
            // negative pie height: relative value
            sizeFor3DEffect = -( int ) ( ( ( double ) params()->threeDPieHeight() / 100.0 ) * ( double ) height );
            height = _size - sizeFor3DEffect;
        }
        int y = ( _dataRect.height() == height ) ? 0 : ( ( _dataRect.height() - height - sizeFor3DEffect ) / 2 );

        _position = QRect( _dataRect.left() + x, _dataRect.top() + y,
                           _size, height );
        //	_position.moveBy( _dataRect.left(), _dataRect.top() );
    }

    double sum = data->rowAbsSum( dataset );
    double sectorsPerValue = 5760.0 / sum; // 5760 == 16*360, number of sections in Qt circle

    int currentValue = params()->pieStart() * 16;
    for ( int value = 0; value < _numValues; value++ ) {
        // is there anything at all at this value
        /* see above for meaning of 16 */

        if ( data->cell( dataset, value ).isDouble() ) {
            _startAngles[ value ] = currentValue;
            double cellValue = std::fabs( data->cell( dataset, value ).doubleValue() );
            _angleLens[ value ] = ( int ) std::floor( cellValue * sectorsPerValue + 0.5 );
        } else { // mark as non-existent
            _angleLens[ value ] = 0;
            if ( value > 0 )
                _startAngles[ value ] = _startAngles[ value - 1 ];
            else
                _startAngles[ value ] = currentValue;
        }

        currentValue = _startAngles[ value ] + _angleLens[ value ];
    }

    // Find the backmost pie which is at +90° and needs to be drawn
    // first
    uint backmostpie = findPieAt( 90 * 16 );
    // Find the frontmost pie (at -90°/+270°) that should be drawn last
    uint frontmostpie = findPieAt( 270 * 16 );
    // and put the backmost pie on the TODO stack to initialize it,
    // but only if it is not the frontmostpie
    QValueStack < uint > todostack;
    if ( backmostpie != frontmostpie )
        todostack.push( backmostpie );
    else {
        // Otherwise, try to find something else
        int leftOfCurrent = findLeftPie( backmostpie );
        if ( leftOfCurrent != frontmostpie ) {
            todostack.push( leftOfCurrent );
        } else {
            int rightOfCurrent = findRightPie( backmostpie );
            if ( rightOfCurrent != frontmostpie ) {
                todostack.push( rightOfCurrent );
            }
        }
        // If we get here, there was nothing else, and we will bail
        // out of the while loop below.
    }

    // The list with pies that have already been drawn

    QValueList < uint > donelist;

    // Draw pies until the todostack is empty or only the frontmost
    // pie is there
    while ( !todostack.isEmpty() &&
            !( ( todostack.count() == 1 ) &&
               ( ( todostack.top() == frontmostpie ) ) ) ) {
        // The while loop cannot be cancelled if frontmostpie is on
        // top of the stack, but this is also backmostpie (can happen
        // when one of the pies covers more than 1/2 of the circle. In
        // this case, we need to find something else to put on the
        // stack to get things going.

        // take one pie from the stack
        uint currentpie = todostack.pop();
        // if this pie was already drawn, ignore it
        if ( donelist.find( currentpie ) != donelist.end() )
            continue;

        // If this pie is the frontmost pie, put it back, but at the
        // second position (otherwise, there would be an endless
        // loop). If this pie is the frontmost pie, there must be at
        // least one other pie, otherwise the loop would already have
        // been terminated by the loop condition.
        if ( currentpie == frontmostpie ) {
            ASSERT( !todostack.isEmpty() );
            // QValueStack::exchange() would be nice here...
            uint secondpie = todostack.pop();
            if ( currentpie == secondpie )
                // no need to have the second pie twice on the stack,
                // forget about one instance and take the third
                // instead
                if ( todostack.isEmpty() )
                    break; // done anyway
                else
                    secondpie = todostack.pop();
            todostack.push( currentpie );
            todostack.push( secondpie );
            continue;
        }

        // When we get here, we can just draw the pie and proceed.
        drawOnePie( painter, data, dataset, currentpie, sizeFor3DEffect,
                    regions );

        // Mark the pie just drawn as done.
        donelist.append( currentpie );

        // Now take the pie to the left and to the right, check
        // whether these have not been painted already, and put them
        // on the stack.
        int leftOfCurrent = findLeftPie( currentpie );
        if ( donelist.find( leftOfCurrent ) == donelist.end() )
            todostack.push( leftOfCurrent );
        int rightOfCurrent = findRightPie( currentpie );
        if ( donelist.find( rightOfCurrent ) == donelist.end() )
            todostack.push( rightOfCurrent );
    }

    // now only the frontmost pie is left to draw
    drawOnePie( painter, data, dataset, frontmostpie, sizeFor3DEffect,
                regions );
}



void KDChartPiePainter::drawOnePie( QPainter* painter,
                                    KDChartTableData* data,
                                    uint dataset, uint pie,
                                    uint threeDPieHeight,
                                    KDChartDataRegionList* regions )
{
    // Is there anything to draw at all?
    int angleLen = _angleLens[ ( int ) pie ];
    if ( angleLen ) {
        int startAngle = _startAngles[ ( int ) pie ];

        QRect drawPosition = _position;
        if ( params()->explode() ) {
            // need to compute a new position for each or some of the pie
            QValueList<int> explodeList = params()->explodeValues();
            if( explodeList.count() == 0 || // nothing on list, explode all
                explodeList.find( pie ) != explodeList.end() ) {
                double explodeAngle = ( startAngle + angleLen / 2 ) / 16;
                double explodeAngleRad = DEGTORAD( explodeAngle );
                double cosAngle = cos( explodeAngleRad );
                double sinAngle = -sin( explodeAngleRad );

                // find the explode factor for this particular pie
                double explodeFactor = 0.0;
                QMap<int,double> explodeFactors = params()->explodeFactors();
                if( !explodeFactors.contains( pie ) ) // not on factors list, use default
                    explodeFactor = params()->explodeFactor();
                else // on factors list, use segment-specific value
                    explodeFactor = explodeFactors[pie];

                double explodeX = explodeFactor * _size * cosAngle;
                double explodeY = explodeFactor * _size * sinAngle;
                drawPosition.moveBy( explodeX, explodeY );
            } else
                drawPosition = _position;
        } else
            drawPosition = _position;

        // The 3D effect needs to be drawn first because it could
        // otherwise partly hide the pie itself.
        if ( params()->threeDPies() ) {
            draw3DEffect( painter, drawPosition, dataset, pie,
                          threeDPieHeight,
                          params()->explode(), regions );
        }

        painter->setBrush( params()->dataColor( pie ) );
        if ( angleLen == 5760 ) {
            // full circle, avoid nasty line in the middle
            painter->drawEllipse( drawPosition );
            if ( regions ) {
                QPointArray hitregion;
                hitregion.makeEllipse( drawPosition.x(), drawPosition.y(),
                                       drawPosition.width(),
                                       drawPosition.height() );
                regions->append( new KDChartDataRegion( QRegion( hitregion ),
                                                        dataset,
                                                        pie ) );
            }
        } else {
            painter->drawPie( drawPosition, ( int ) startAngle, ( int ) angleLen );
            if ( regions ) {
                QPointArray hitregion;
                hitregion.makeArc( drawPosition.x(), drawPosition.y(),
                                   drawPosition.width(),
                                   drawPosition.height(),
                                   ( int ) startAngle, ( int ) angleLen );
                hitregion.resize( hitregion.size() + 1 );
                hitregion.setPoint( hitregion.size() - 1,
                                    drawPosition.center() );
                regions->append( new KDChartDataRegion( QRegion( hitregion ),
                                                        dataset, pie ) );
            }
        }
    }
}


void KDChartPiePainter::draw3DEffect( QPainter* painter,
                                      const QRect& drawPosition,
                                      uint dataset, uint pie,
                                      uint threeDHeight,
                                      bool explode,
                                      KDChartDataRegionList* regions )
{
    // NOTE: We cannot optimize away drawing some of the effects (even
    // when not exploding), because some of the pies might be left out
    // in future versions which would make some of the normally hidden
    // pies visible. Complex hidden-line algorithms would be much more
    // expensive than just drawing for nothing.

    // No need to save the brush, will be changed on return from this
    // method anyway.
    painter->setBrush( params()->dataShadow1Color( pie ) );

    int startAngle = _startAngles[ ( int ) pie ];
    int endAngle = startAngle + _angleLens[ ( int ) pie ];
    // Normalize angles
    while ( startAngle >= 5760 )
        startAngle -= 5760;
    while ( endAngle >= 5760 )
        endAngle -= 5760;
    ASSERT( startAngle >= 0 && startAngle <= 360 * 16 );
    ASSERT( endAngle >= 0 && endAngle <= 360 * 16 );

    int centerY = drawPosition.center().y();

    if ( startAngle == endAngle ||
            startAngle == endAngle - 5760 ) { // full circle
        drawArcEffectSegment( painter, drawPosition, dataset, pie,
                              threeDHeight, 2880, 5760, regions );
    } else if ( startAngle <= 90 * 16 ) {
        if ( endAngle <= 90 * 16 ) {
            if ( startAngle <= endAngle ) {
                /// starts and ends in first quadrant, less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, startAngle,
                                           regions );
            } else {
                /// starts and ends in first quadrant, more than 3/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, startAngle,
                                           regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, 2880, 5760, regions );
            }
        } else if ( endAngle <= 180 * 16 ) {
            /// starts in first quadrant, ends in second quadrant,
            /// less than 1/2
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, startAngle, regions );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, endAngle, regions );
        } else if ( endAngle <= 270 * 16 ) {
            /// starts in first quadrant, ends in third quadrant
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, startAngle,
                                       regions );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, endAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, 2880, endAngle, regions );
        } else { // 270*16 < endAngle < 360*16
            /// starts in first quadrant, ends in fourth quadrant,
            /// more than 3/4
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, startAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, 2880, endAngle, regions );
        }
    } else if ( startAngle <= 180 * 16 ) {
        if ( endAngle <= 90 * 16 ) {
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, 2880, 5760, regions );
        } else if ( endAngle <= 180 * 16 ) {
            if ( startAngle <= endAngle ) {
                /// starts in second quadrant, ends in second
                /// quadrant, less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, endAngle,
                                           regions );
            } else {
                /// starts in second quadrant, ends in second
                /// quadrant, more than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, endAngle,
                                           regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, 2880, 5760, regions );
            }
        } else if ( endAngle <= 270 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, endAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, 2880, endAngle, regions );
        } else { // 270*16 < endAngle < 360*16
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, 2880, endAngle, regions );
        }
    } else if ( startAngle <= 270 * 16 ) {
        if ( endAngle <= 90 * 16 ) {
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, startAngle, 5760, regions );
        } else if ( endAngle <= 180 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, endAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, startAngle, 5760, regions );
        } else if ( endAngle <= 270 * 16 ) {
            if ( startAngle <= endAngle ) {
                /// starts in third quadrant, ends in third quadrant,
                /// less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, endAngle,
                                           regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, startAngle, endAngle,
                                      regions );
            } else {
                /// starts in third quadrant, ends in third quadrant,
                /// more than 3/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, endAngle,
                                           regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, 2880, endAngle,
                                      regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, startAngle, 5760,
                                      regions );
            }
        } else { // 270*16 < endAngle < 360*16
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, startAngle, endAngle,
                                  regions );
        }
    } else { // 270*16 < startAngle < 360*16
        if ( endAngle <= 90 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, startAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, startAngle, 5760, regions );
        } else if ( endAngle <= 180 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, startAngle, regions );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, endAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, startAngle, 5760, regions );
        } else if ( endAngle <= 270 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, startAngle, regions );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie,
                                       threeDHeight, endAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, 2880, endAngle, regions );
            drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                  threeDHeight, startAngle, 5760, regions );
        } else { // 270*16 < endAngle < 360*16
            if ( startAngle <= endAngle ) {
                /// starts in fourth quadrant, ends in fourth
                /// quadrant, less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, startAngle,
                                           regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, startAngle, endAngle,
                                      regions );
            } else {
                /// starts in fourth quadrant, ends in fourth
                /// quadrant, more than 3/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, threeDHeight, startAngle,
                                           regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, startAngle, 5760,
                                      regions );
                drawArcEffectSegment( painter, drawPosition, dataset, pie,
                                      threeDHeight, 2880, endAngle, regions );
            }
        }
    }
}


void KDChartPiePainter::drawStraightEffectSegment( QPainter* painter,
        const QRect& rect,
        uint dataset, uint pie,
        int threeDHeight,
        int angle,
        KDChartDataRegionList* regions )
{
    QPoint center = rect.center();
    QPointArray points( 4 );
    QPoint circlePoint = pointOnCircle( rect, angle );
    points.setPoint( 0, center );
    points.setPoint( 1, circlePoint );
    points.setPoint( 2, circlePoint.x(), circlePoint.y() + threeDHeight );
    points.setPoint( 3, center.x(),
                     center.y() + threeDHeight );
    painter->drawPolygon( points );
    if ( regions )
        regions->append( new KDChartDataRegion( QRegion( points ),
                                                dataset, pie ) );
}


void KDChartPiePainter::drawArcEffectSegment( QPainter* painter,
        const QRect& rect,
        uint dataset, uint pie,
        int threeDHeight,
        int startAngle,
        int endAngle,
        KDChartDataRegionList* regions )
{
    // Start with getting the points for the inner arc.
    QPointArray collect;
    collect.makeArc( rect.x(), rect.y(),
                     rect.width(), rect.height(),
                     startAngle, endAngle - startAngle + 1 );

    // Now copy these arcs again into the same array, but in the
    // opposite direction and moved down by the 3D height.
    int arcPoints = collect.size();
    collect.resize( arcPoints * 2 );
    for ( int i = arcPoints - 1; i >= 0; i-- ) {
        QPoint pointOnFirstArc = collect.point( i );
        pointOnFirstArc.setY( pointOnFirstArc.y() + threeDHeight );
        collect.setPoint( arcPoints * 2 - i - 1, pointOnFirstArc );
    }
    painter->drawPolygon( collect );
    if ( regions )
        regions->append( new KDChartDataRegion( QRegion( collect ),
                                                dataset, pie ) );
}


uint KDChartPiePainter::findPieAt( int angle )
{
    for ( int i = 0; i < _numValues; i++ ) {
        int endseg = _startAngles[ i ] + _angleLens[ i ];
        if ( ( _startAngles[ i ] <= angle ) &&
                ( endseg >= angle ) )
            // found!
            return i;
    }

    // If we have not found it, try wrap around
    return findPieAt( angle + 5760 );
}


uint KDChartPiePainter::findLeftPie( uint pie )
{
    if ( pie == 0 )
        if ( _numValues > 1 )
            return _numValues - 1;
        else
            return 0;
    else {
        uint leftpie = pie - 1;
        if ( leftpie < 0 )
            leftpie = _numValues - 1;
        return leftpie;
    }
}


uint KDChartPiePainter::findRightPie( uint pie )
{
    uint rightpie = pie + 1;
    if ( rightpie == _numValues )
        rightpie = 0;
    return rightpie;
}


QPoint KDChartPiePainter::pointOnCircle( const QRect& rect, int angle )
{
    // There are two ways of computing this: The simple, but slow one
    // is to use QPointArray.makeArc() and take the first point. The
    // more advanced, but faster one is to do the trigonometric
    // computionations outselves. Since the comments in
    // QPointArray::makeArc() very often say that the code there is
    // "poor", we'd better do it outselves...

    double normAngle = angle / 16;
    double normAngleRad = DEGTORAD( normAngle );
    double cosAngle = cos( normAngleRad );
    double sinAngle = -sin( normAngleRad );
    double posX = std::floor( cosAngle * ( double ) rect.width() / 2.0 + 0.5 );
    double posY = std::floor( sinAngle * ( double ) rect.height() / 2.0 + 0.5 );
    return QPoint( ( int ) posX + rect.center().x(),
                   ( int ) posY + rect.center().y() );
}


/**
   This method is a specialization that returns a fallback legend text
   appropriate for pies that do not have more than one dataset

   This method is only used when automatic legends are used, because
   manual and first-column legends do not need fallback texts.

   \param uint dataset the dataset number for which to generate a
   fallback text
   \return the fallback text to use for describing the specified
   dataset in the legend
*/
QString KDChartPiePainter::fallbackLegendText( uint dataset ) const
{
    return QObject::tr( "Item " ) + QString::number( dataset + 1 );
}


/**
   This methods returns the number of elements to be shown in the
   legend in case fallback texts are used.

   This method is only used when automatic legends are used, because
   manual and first-column legends do not need fallback texts.

   \return the number of fallback texts to use
*/
uint KDChartPiePainter::numLegendFallbackTexts( KDChartTableData* data ) const
{
    return data->usedCols();
}
