/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KDChart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KDChart licenses may use this file in
 ** accordance with the KDChart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.klaralvdalens-datakonsult.se/?page=products for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@klaralvdalens-datakonsult.se if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/
#include "KDChartEnums.h"
#include "KDChartPiePainter.h"
#include "KDChartParams.h"

#include <qpainter.h>
#include <qvaluestack.h>
#include <qmessagebox.h>

#define DEGTORAD(d) (d)*M_PI/180

#include <math.h>

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

//static bool bHelp=true;

/**
  Paints the actual data area.

  \param painter the QPainter onto which the chart should be painted
  \param data the data that will be displayed as a chart
  \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartPiePainter::paintData( QPainter* painter,
        KDChartTableDataBase* data,
        bool paint2nd,
        KDChartDataRegionList* regions )
{
//bHelp=true;
    uint chart = paint2nd ? 1 : 0;

    QRect ourClipRect( _dataRect );
    ourClipRect.addCoords( -1,-1,1,1 );

    const QWMatrix & world = painter->worldMatrix();
    ourClipRect =
#if COMPAT_QT_VERSION >= 0x030000
        world.mapRect( ourClipRect );
#else
    world.map( ourClipRect );
#endif

    painter->setClipRect( ourClipRect );

    // find which dataset to paint
    uint dataset;
    if ( !params()->findDataset( KDChartParams::DataEntry
                ,
                dataset, dataset ) ) {
        return ; // nothing to draw
    }

    if ( dataset == KDCHART_ALL_DATASETS )
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
        //  _position.moveBy( _dataRect.left(), _dataRect.top() );
    }

    double sum = data->rowAbsSum( dataset );
    if( sum==0 ) //nothing to draw
        return;
    double sectorsPerValue = 5760.0 / sum; // 5760 == 16*360, number of sections in Qt circle

    int currentValue = params()->pieStart() * 16;
    bool atLeastOneValue = false; // guard against completely empty tables
    QVariant vValY;
    for ( int value = 0; value < _numValues; value++ ) {
        // is there anything at all at this value
        /* see above for meaning of 16 */

        if( data->cellCoord( dataset, value, vValY, 1 ) &&
            QVariant::Double == vValY.type() ){
            _startAngles[ value ] = currentValue;
            const double cellValue = fabs( vValY.toDouble() );
            _angleLens[ value ] = ( int ) floor( cellValue * sectorsPerValue + 0.5 );
            atLeastOneValue = true;
        } else { // mark as non-existent
            _angleLens[ value ] = 0;
            if ( value > 0 )
                _startAngles[ value ] = _startAngles[ value - 1 ];
            else
                _startAngles[ value ] = currentValue;
        }

        currentValue = _startAngles[ value ] + _angleLens[ value ];
    }

    // If there was no value at all, bail out, to avoid endless loops
    // later on (e.g. in findPieAt()).
    if( !atLeastOneValue )
        return;


    // Find the backmost pie which is at +90° and needs to be drawn
    // first
    int backmostpie = findPieAt( 90 * 16 );
    // Find the frontmost pie (at -90°/+270°) that should be drawn last
    int frontmostpie = findPieAt( 270 * 16 );
    // and put the backmost pie on the TODO stack to initialize it,
    // but only if it is not the frontmostpie
    QValueStack < int > todostack;
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

    QValueList < int > donelist;

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
        int currentpie = todostack.pop();
        // if this pie was already drawn, ignore it
        if ( donelist.find( currentpie ) != donelist.end() )
            continue;

        // If this pie is the frontmost pie, put it back, but at the
        // second position (otherwise, there would be an endless
        // loop). If this pie is the frontmost pie, there must be at
        // least one other pie, otherwise the loop would already have
        // been terminated by the loop condition.
        if ( currentpie == frontmostpie ) {
            Q_ASSERT( !todostack.isEmpty() );
            // QValueStack::exchange() would be nice here...
            int secondpie = todostack.pop();
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
        drawOnePie( painter, data, dataset, currentpie, chart,
                sizeFor3DEffect,
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
    drawOnePie( painter, data, dataset, frontmostpie, chart,
            sizeFor3DEffect,
            regions );
}


/**
  Internal method that draws one of the pies in a pie chart.

  \param painter the QPainter to draw in
  \param dataset the dataset to draw the pie for
  \param pie the pie to draw
  \param the chart to draw the pie in
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartPiePainter::drawOnePie( QPainter* painter,
        KDChartTableDataBase* /*data*/,
        uint dataset, uint pie, uint chart,
        uint threeDPieHeight,
        KDChartDataRegionList* regions )
{
    // Is there anything to draw at all?
    int angleLen = _angleLens[ ( int ) pie ];
    if ( angleLen ) {
        int startAngle = _startAngles[ ( int ) pie ];

        KDChartDataRegion* datReg = 0;
        QRegion* region = 0;
        bool mustDeleteRegion = false;
        if ( regions ){
            region = new QRegion();
            mustDeleteRegion = true;
        }

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
                drawPosition.moveBy( static_cast<int>( explodeX ), static_cast<int>( explodeY ) );
            } else
                drawPosition = _position;
        } else
            drawPosition = _position;

        // The 3D effect needs to be drawn first because it could
        // otherwise partly hide the pie itself.
        if ( params()->threeDPies() ) {
            draw3DEffect( painter, drawPosition, dataset, pie, chart,
                          threeDPieHeight,
                          params()->explode(), region );
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
                datReg = new KDChartDataRegion( region->unite( QRegion( hitregion ) ),
                                                dataset,
                                                pie,
                                                chart );
                datReg->points[ KDChartEnums::PosCenter ]
                    = drawPosition.center();
                datReg->points[ KDChartEnums::PosCenterRight ]
                    = pointOnCircle( drawPosition,    0 );
                datReg->points[ KDChartEnums::PosTopRight ]
                    = pointOnCircle( drawPosition,  720 );
                datReg->points[ KDChartEnums::PosTopCenter ]
                    = pointOnCircle( drawPosition, 1440 );
                datReg->points[ KDChartEnums::PosTopLeft ]
                    = pointOnCircle( drawPosition, 2160 );
                datReg->points[ KDChartEnums::PosCenterLeft ]
                    = pointOnCircle( drawPosition, 2880 );
                datReg->points[ KDChartEnums::PosBottomLeft ]
                    = pointOnCircle( drawPosition, 3600 );
                datReg->points[ KDChartEnums::PosBottomCenter ]
                    = pointOnCircle( drawPosition, 4320 );
                datReg->points[ KDChartEnums::PosBottomRight ]
                    = pointOnCircle( drawPosition, 5040 );
                datReg->startAngle = 2880;
                datReg->angleLen   = 5760;
                regions->append( datReg );
            }
        } else {
            // draw the top of this piece
            // Start with getting the points for the arc.
            const int arcPoints = angleLen;
            QPointArray collect(arcPoints+2);
            int i=0;
            for ( ; i<=angleLen; ++i){
                collect.setPoint(i, pointOnCircle( drawPosition, startAngle+i ));
            }
            // Adding the center point of the piece.
            collect.setPoint(i, drawPosition.center() );



            painter->drawPolygon( collect );

//if( bHelp ){
//              painter->drawPolyline( collect );
//bHelp=false;
//}



            if ( regions ) {
                QPointArray hitregion;
                hitregion.makeArc( drawPosition.x(), drawPosition.y(),
                        drawPosition.width(),
                        drawPosition.height(),
                        ( int ) startAngle, ( int ) angleLen );
                hitregion.resize( hitregion.size() + 1 );
                hitregion.setPoint( hitregion.size() - 1,
                        drawPosition.center() );
                datReg = new KDChartDataRegion( region->unite( QRegion( hitregion ) ),
                                                dataset,
                                                pie,
                                                chart );

                datReg->points[ KDChartEnums::PosTopLeft ]
                    = pointOnCircle( drawPosition, startAngle + angleLen );
                datReg->points[ KDChartEnums::PosTopCenter ]
                    = pointOnCircle( drawPosition, startAngle + angleLen / 2 );
                datReg->points[ KDChartEnums::PosTopRight ]
                    = pointOnCircle( drawPosition, startAngle );

                datReg->points[   KDChartEnums::PosBottomLeft   ] = drawPosition.center();
                datReg->points[   KDChartEnums::PosBottomCenter ]
                    = datReg->points[ KDChartEnums::PosBottomLeft   ];
                datReg->points[   KDChartEnums::PosBottomRight  ]
                    = datReg->points[ KDChartEnums::PosBottomLeft   ];

                datReg->points[ KDChartEnums::PosCenterLeft ]
                    = QPoint( (   datReg->points[ KDChartEnums::PosTopLeft      ].x()
                                + datReg->points[ KDChartEnums::PosBottomLeft   ].x() ) / 2,
                            (   datReg->points[ KDChartEnums::PosTopLeft      ].y()
                                + datReg->points[ KDChartEnums::PosBottomLeft   ].y() ) / 2 );
                datReg->points[ KDChartEnums::PosCenter ]
                    = QPoint( (   datReg->points[ KDChartEnums::PosTopCenter    ].x()
                                + datReg->points[ KDChartEnums::PosBottomCenter ].x() ) / 2,
                            (   datReg->points[ KDChartEnums::PosTopCenter    ].y()
                                + datReg->points[ KDChartEnums::PosBottomCenter ].y() ) / 2 );
                datReg->points[ KDChartEnums::PosCenterRight ]
                    = QPoint( (   datReg->points[ KDChartEnums::PosTopRight     ].x()
                                + datReg->points[ KDChartEnums::PosBottomRight  ].x() ) / 2,
                            (   datReg->points[ KDChartEnums::PosTopRight     ].y()
                                + datReg->points[ KDChartEnums::PosBottomRight  ].y() ) / 2 );

                datReg->startAngle = startAngle;
                datReg->angleLen   = angleLen;
                regions->append( datReg );
            }
        }
        if( mustDeleteRegion )
            delete region;
    }
}


/**
  Internal method that draws the shadow creating the 3D effect of a pie

  \param painter the QPainter to draw in
  \param rect the position to draw at
  \param dataset the dataset to draw the pie for
  \param pie the pie to draw the shadow for
  \param the chart to draw the pie in
  \param threeDHeight the height of the shadow
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartPiePainter::draw3DEffect( QPainter* painter,
        const QRect& drawPosition,
        uint dataset, uint pie, uint chart,
        uint threeDHeight,
        bool /*explode*/,
        QRegion* region )
{
    // NOTE: We cannot optimize away drawing some of the effects (even
    // when not exploding), because some of the pies might be left out
    // in future versions which would make some of the normally hidden
    // pies visible. Complex hidden-line algorithms would be much more
    // expensive than just drawing for nothing.

    // No need to save the brush, will be changed on return from this
    // method anyway.
    painter->setBrush( QBrush( params()->dataShadow1Color( pie ),
                params()->shadowPattern() ) );

    int startAngle = _startAngles[ ( int ) pie ];
    int endAngle = startAngle + _angleLens[ ( int ) pie ];
    // Normalize angles
    while ( startAngle >= 5760 )
        startAngle -= 5760;
    while ( endAngle >= 5760 )
        endAngle -= 5760;
    Q_ASSERT( startAngle >= 0 && startAngle <= 360 * 16 );
    Q_ASSERT( endAngle >= 0 && endAngle <= 360 * 16 );

    //int centerY = drawPosition.center().y();

    if ( startAngle == endAngle ||
            startAngle == endAngle - 5760 ) { // full circle
        drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                threeDHeight, 2880, 5760, region );
    } else if ( startAngle <= 90 * 16 ) {
        if ( endAngle <= 90 * 16 ) {
            if ( startAngle <= endAngle ) {
                /// starts and ends in first quadrant, less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, chart, threeDHeight, startAngle,
                                           region );
            } else {
                /// starts and ends in first quadrant, more than 3/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                                           pie, chart, threeDHeight, startAngle,
                                           region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                                      threeDHeight, 2880, 5760, region );
            }
        } else if ( endAngle <= 180 * 16 ) {
            /// starts in first quadrant, ends in second quadrant,
            /// less than 1/2
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                                       threeDHeight, startAngle, region );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                                       threeDHeight, endAngle, region );
        } else if ( endAngle <= 270 * 16 ) {
            /// starts in first quadrant, ends in third quadrant
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle,
                    region );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, endAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, 2880, endAngle, region );
        } else { // 270*16 < endAngle < 360*16
            /// starts in first quadrant, ends in fourth quadrant,
            /// more than 3/4
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, 2880, endAngle, region );
        }
    } else if ( startAngle <= 180 * 16 ) {
        if ( endAngle <= 90 * 16 ) {
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, 2880, 5760, region );
        } else if ( endAngle <= 180 * 16 ) {
            if ( startAngle <= endAngle ) {
                /// starts in second quadrant, ends in second
                /// quadrant, less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                        pie, chart, threeDHeight, endAngle,
                        region );
            } else {
                /// starts in second quadrant, ends in second
                /// quadrant, more than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                        pie, chart, threeDHeight, endAngle,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, 2880, 5760, region );
            }
        } else if ( endAngle <= 270 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, endAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, 2880, endAngle, region );
        } else { // 270*16 < endAngle < 360*16
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, 2880, endAngle, region );
        }
    } else if ( startAngle <= 270 * 16 ) {
        if ( endAngle <= 90 * 16 ) {
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, 5760, region );
        } else if ( endAngle <= 180 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, endAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, 5760, region );
        } else if ( endAngle <= 270 * 16 ) {
            if ( startAngle <= endAngle ) {
                /// starts in third quadrant, ends in third quadrant,
                /// less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                        pie, chart, threeDHeight, endAngle,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, startAngle, endAngle,
                        region );
            } else {
                /// starts in third quadrant, ends in third quadrant,
                /// more than 3/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                        pie, chart, threeDHeight, endAngle,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, 2880, endAngle,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, startAngle, 5760,
                        region );
            }
        } else { // 270*16 < endAngle < 360*16
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, endAngle,
                    region );
        }
    } else { // 270*16 < startAngle < 360*16
        if ( endAngle <= 90 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, 5760, region );
        } else if ( endAngle <= 180 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, region );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, endAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, 5760, region );
        } else if ( endAngle <= 270 * 16 ) {
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, region );
            drawStraightEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, endAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, 2880, endAngle, region );
            drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                    threeDHeight, startAngle, 5760, region );
        } else { // 270*16 < endAngle < 360*16
            if ( startAngle <= endAngle ) {
                /// starts in fourth quadrant, ends in fourth
                /// quadrant, less than 1/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                        pie, chart, threeDHeight, startAngle,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, startAngle, endAngle,
                        region );
            } else {
                /// starts in fourth quadrant, ends in fourth
                /// quadrant, more than 3/4
                drawStraightEffectSegment( painter, drawPosition, dataset,
                        pie, chart, threeDHeight, startAngle,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, startAngle, 5760,
                        region );
                drawArcEffectSegment( painter, drawPosition, dataset, pie, chart,
                        threeDHeight, 2880, endAngle, region );
            }
        }
    }
}


/**
  Internal method that draws a segment with a straight 3D effect

  \param painter the QPainter to draw in
  \param rect the position to draw at
  \param threeDHeight the height of the shadow
  \param angle the angle of the segment
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartPiePainter::drawStraightEffectSegment( QPainter* painter,
        const QRect& rect,
        uint /*dataset*/, uint /*pie*/, uint /*chart*/,
        int threeDHeight,
        int angle,
        QRegion* region )
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
    if ( region )
        *region += QRegion( points );
}


/**
  Internal method that draws a segment with an arc 3D effect

  \param painter the QPainter to draw in
  \param rect the position to draw at
  \param threeDHeight the height of the shadow
  \param startAngle the starting angle of the segment
  \param endAngle the ending angle of the segment
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartPiePainter::drawArcEffectSegment( QPainter* painter,
        const QRect& rect,
        uint /*dataset*/, uint /*pie*/, uint /*chart*/,
        int threeDHeight,
        int startAngle,
        int endAngle,
        QRegion* region )
{
    // Start with getting the points for the inner arc.
    const int startA = QMIN(startAngle, endAngle);
    const int endA   = QMAX(startAngle, endAngle);
    const int arcPoints = endA-startA+1;
    QPointArray collect(arcPoints * 2);
    for ( int angle=endA; angle>=startA; --angle){
        collect.setPoint(endA-angle, pointOnCircle( rect, angle ));
    }

    // Now copy these arcs again into the same array, but in the
    // opposite direction and moved down by the 3D height.
    for ( int i = arcPoints - 1; i >= 0; --i ) {
        QPoint pointOnFirstArc = collect.point( i );
        pointOnFirstArc.setY( pointOnFirstArc.y() + threeDHeight );
        collect.setPoint( arcPoints * 2 - i - 1, pointOnFirstArc );
    }
    painter->drawPolygon( collect );
    if ( region )
        *region += QRegion( collect );
}


/**
  Internal method that finds the pie that is located at the position
  specified by \c angle.

  \param angle the angle at which to search for a pie
  \return the number of the pie found
  */
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


/**
  Internal method that finds the pie that is located to the left of
  the pie specified by \c pie.

  \param pie the pie to start the search from
  \return the number of the pie to the left of \c pie
  */
uint KDChartPiePainter::findLeftPie( uint pie )
{
    if ( pie == 0 )
        if ( _numValues > 1 )
            return _numValues - 1;
        else
            return 0;
    else {
        return pie - 1;
    }
}


/**
  Internal method that finds the pie that is located to the right of
  the pie specified by \c pie.

  \param pie the pie to start the search from
  \return the number of the pie to the right of \c pie
  */
uint KDChartPiePainter::findRightPie( uint pie )
{
    int rightpie = pie + 1;
    if ( rightpie == _numValues )
        rightpie = 0;
    return rightpie;
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
uint KDChartPiePainter::numLegendFallbackTexts( KDChartTableDataBase* data ) const
{
    return data->usedCols();
}
