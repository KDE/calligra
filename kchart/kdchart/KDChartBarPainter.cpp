/* -*- Mode: C++ -*-
   $Id$
   KDChart - a multi-platform charting engine
*/

/****************************************************************************
** Copyright (C) 2001-2002 Klarälvdalens Datakonsult AB.  All rights reserved.
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
** See http://www.klaralvdalens-datakonsult.se/Public/products/ for
**   information about KDChart Commercial License Agreements.
**
** Contact info@klaralvdalens-datakonsult.se if any conditions of this
** licensing are not clear to you.
**
**********************************************************************/
#include "KDChartBarPainter.h"
#include <KDChartParams.h>

#include <qpainter.h>

#include <stdlib.h>

/**
   \class KDChartBarPainter KDChartBarPainter.h

   \brief A chart painter implementation that can paint bar charts.
*/

/**
   Constructor. Sets up internal data structures as necessary.

   \param params the KDChartParams structure that defines the chart
*/
KDChartBarPainter::KDChartBarPainter( KDChartParams* params ) :
    KDChartAxesPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
   Destructor.
*/
KDChartBarPainter::~KDChartBarPainter()
{
    // intentionally left blank
}


/**
   Paints the actual data area and registers the region for the data
   points if \a regions is not 0.

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
   \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
   \param regions a pointer to a list of regions that will be filled
   with regions representing the data segments, if not null
*/
void KDChartBarPainter::paintData( QPainter* painter,
                                   KDChartTableDataBase* data,
                                   bool paint2nd,
                                   KDChartDataRegionList* regions )
{
    uint chart = paint2nd ? 1 : 0;

    // find out the ordinate axis (or axes, resp.) belonging to this chart
    // (up to 4 ordinates might be in use: 2 left ones and 2 right ones)
    uint axesCount;
    KDChartParams::Ordinates axes;
    if( !params()->chartAxes( chart, axesCount, axes ) ) {
        // no axis - no fun!
        return;
        // We cannot draw data without an axis having calculated high/low
        // values and position of the zero line before.

        // PENDING(khz) Allow drawing without having a visible axis!
    }

    bool barNormal = KDChartParams::BarNormal == params()->barChartSubType();

    //const KDChartParams::ChartType params_chartType
    //  = paint2nd ? params()->additionalChartType() : params()->chartType();

    double logHeight = _dataRect.height();
    double logWidth = _dataRect.width();
    double areaWidthP1000 = logWidth / 1000.0;
    int clipShiftUp = ( barNormal && !params()->threeDBars() )
                      ? static_cast < int > ( areaWidthP1000 * 16.0 )
                      : 0;
    QRect ourClipRect( _dataRect );
    if ( 0 < ourClipRect.top() ) {
        ourClipRect.setTop( ourClipRect.top() - clipShiftUp );
        ourClipRect.setHeight( ourClipRect.height() + clipShiftUp - 1 );
    } else
        ourClipRect.setHeight( ourClipRect.height() + clipShiftUp / 2 - 1 );

    const QWMatrix & world = painter->worldMatrix();
    ourClipRect.moveBy( static_cast < int > ( world.dx() ),
                        static_cast < int > ( world.dy() ) );

    painter->setClipRect( ourClipRect );
    painter->translate( _dataRect.x(), _dataRect.y() );

    painter->setPen( params()->outlineDataColor() );

    // find out which datasets are to be represented by this chart
    uint chartDatasetStart, chartDatasetEnd;
    if ( params()->neverUsedSetChartSourceMode()
         || !params()->findDataset( KDChartParams::DataEntry,
                                    chartDatasetStart,
                                    chartDatasetEnd,
                                    chart ) ) {
        uint maxRow, maxRowMinus1;
        switch ( data->usedRows() ) {
        case 0:
            return ;
        case 1:
            maxRow = 0;
            maxRowMinus1 = 0;
            break;
        default:
            maxRow = data->usedRows() - 1;
            maxRowMinus1 = maxRow - 1;
        }
        chartDatasetStart = paint2nd ? maxRow : 0;
        chartDatasetEnd = paint2nd
                          ? maxRow
                          : (   ( KDChartParams::NoType
                                  == params()->additionalChartType() )
                                ? maxRow
                                : maxRowMinus1 );

    }




    /* old code: supporting only one axis per chart

    const KDChartAxisParams& axisPara
    = params()->axisParams( paint2nd
                             ? KDChartAxisParams::AxisPosRight
                             : KDChartAxisParams::AxisPosLeft );
*/


    for( uint aI = 0; aI < axesCount; ++aI ) {
        uint axis = axes[ aI ];
        const KDChartAxisParams* axisPara = &params()->axisParams( axis );

        uint datasetStart, datasetEnd;
        uint axisDatasetStart, axisDatasetEnd;
        uint dummy;
        if(    params()->axisDatasets( axis,
                                       axisDatasetStart,
                                       axisDatasetEnd, dummy )
               && ( KDChartParams::KDCHART_ALL_DATASETS != axisDatasetStart ) ) {
            /*
            qDebug("axis   %u   axisDatasetStart %u   chartDatasetStart %u   axisDatasetEnd %u   chartDatasetEnd %u",
            axis, axisDatasetStart, chartDatasetStart, axisDatasetEnd, chartDatasetEnd );
            if( KDChartParams::KDCHART_ALL_DATASETS == axisDatasetStart )
                qDebug("  all");
            if( KDChartParams::KDCHART_NO_DATASET == axisDatasetStart )
                qDebug("  no");
*/
            if(    axisDatasetStart >= chartDatasetStart
                   && axisDatasetStart <= chartDatasetEnd )
                datasetStart = QMAX( axisDatasetStart, chartDatasetStart );
            else if(    axisDatasetStart <= chartDatasetStart
                        && axisDatasetEnd   >= chartDatasetStart )
                datasetStart = chartDatasetStart;
            else
                datasetStart = 20;
            if(    axisDatasetEnd >= chartDatasetStart
                   && axisDatasetEnd <= chartDatasetEnd )
                datasetEnd = QMIN( axisDatasetEnd, chartDatasetEnd );
            else if(    axisDatasetEnd   >= chartDatasetEnd
                        && axisDatasetStart <= chartDatasetEnd )
                datasetEnd = chartDatasetEnd;
            else
                datasetEnd = 0;
        } else {
            datasetStart = chartDatasetStart;
            datasetEnd   = chartDatasetEnd;
        }


        uint datasetNum = ( chartDatasetEnd - chartDatasetStart ) + 1;

        // Number of values: If -1, use all values, otherwise use the
        // specified number of values.
        int numValues = 0;
        if ( params()->numValues() != -1 )
            numValues = params()->numValues();
        else
            numValues = data->usedCols();

        double datasetGap = params()->datasetGap()
                            * ( params()->datasetGapIsRelative()
                                ? areaWidthP1000
                                : 1.0 );
        double valueBlockGap = params()->valueBlockGap()
                               * ( params()->valueBlockGapIsRelative()
                                   ? areaWidthP1000
                                   : 1.0 );

        // This is the same for all three bar types.
        double spaceBetweenValueBlocks = valueBlockGap
                                         * numValues;

        // Set some geometry values that apply to bar charts only
        double totalNumberOfBars = 0.0;
        double spaceBetweenDatasets = 0.0;
        double maxColumnValue = 0.0, minColumnValue = 0.0;
        switch ( params()->barChartSubType() ) {
        case KDChartParams::BarNormal: {
            totalNumberOfBars = datasetNum * numValues;
            spaceBetweenDatasets = datasetGap
                                   * ( totalNumberOfBars
                                       - numValues );
            //maxColumnValue = QMAX( data->maxValue(), 0.0 );
            //minColumnValue = QMIN( data->minValue(), 0.0  );
            maxColumnValue = axisPara->trueAxisHigh();
            minColumnValue = axisPara->trueAxisLow();
            break;
        }
        case KDChartParams::BarStacked:
        case KDChartParams::BarPercent:
            totalNumberOfBars = numValues;
            spaceBetweenDatasets = 0; // always 0 when stacked/percent
            maxColumnValue = QMAX( data->maxColSum(), 0.0 );
            minColumnValue = QMIN( data->minColSum(), 0.0 );
            break;
        default:
            qFatal( "Unsupported bar type" );
        };
        double columnValueDistance = maxColumnValue - minColumnValue;

        // These are the same for all three bar types.
        double barWidth = ( logWidth
                            - spaceBetweenValueBlocks
                            - spaceBetweenDatasets ) / totalNumberOfBars;
        double frontBarWidth = 0.0;
        if ( params()->threeDBars() )
            frontBarWidth = barWidth / ( 1.0 + params()->cosThreeDBarAngle() );
        else
            frontBarWidth = barWidth;
        double sideBarWidth = barWidth - frontBarWidth;
        double sideBarHeight = sideBarWidth;
        double pixelsPerUnit = 0.0;
        if ( params()->barChartSubType() != KDChartParams::BarPercent )  // not used for percent
            pixelsPerUnit =   ( logHeight - sideBarHeight )
                              / ( columnValueDistance ? columnValueDistance : 10 );
        /*
        qDebug("axisPara->trueAxisLow() = %f",axisPara->trueAxisLow());
        qDebug("axisPara->trueAxisHigh() = %f",axisPara->trueAxisHigh());
*/
        double zeroXAxisI;
        double shiftUpperBars = 0.0;
        if ( params()->barChartSubType() == KDChartParams::BarPercent ) {
            if ( minColumnValue == 0.0 )
                zeroXAxisI = 0.0;
            else if ( maxColumnValue == 0.0 )
                zeroXAxisI = logHeight - sideBarHeight;
            else
                zeroXAxisI = ( logHeight - sideBarHeight ) / 2.0;
        } else {
            // zeroXAxisI = - minColumnValue * pixelsPerUnit;
            zeroXAxisI =   logHeight
                           - axisPara->axisZeroLineStartY()
                           + _dataRect.y();
            if ( axisPara->axisTrueLineWidth() % 2 )
                shiftUpperBars = 1.0;
        }

        // Initializing drawing positions
        double xpos = 0.0 + valueBlockGap / 2.0,
      yposPositives = logHeight,
      yposNegatives = logHeight;

        // Now actually drawing the bars from left to right
        double valueTotal = 0.0; // Will only be used for percent bars
        for ( int value = 0; value < numValues; value++ ) {

            if ( params()->barChartSubType() == KDChartParams::BarPercent )
                valueTotal = data->colAbsSum( value );
            double lastPositive = 0.0;
            double maxValueInThisColumn = 0.0, minValueInThisColumn = 0.0;
            if ( params()->barChartSubType() == KDChartParams::BarStacked ||
                 params()->barChartSubType() == KDChartParams::BarPercent ) {
                lastPositive = data->lastPositiveCellInColumn( value );
                maxValueInThisColumn = data->maxInColumn( value );
                minValueInThisColumn = data->minInColumn( value );
            }
            for ( uint dataset  = chartDatasetStart;
                  dataset <= chartDatasetEnd;
                  ++dataset ) {
                if ( data->cell( dataset, value ).isDouble() ) {

                    // there is a numeric value
                    double cellValue = data->cell( dataset, value ).doubleValue();
                    double barHeight;
                    if ( params()->barChartSubType() == KDChartParams::BarPercent )
                        //     barHeight = ( cellValue / valueTotal ) * logHeight;
                        barHeight =   ( cellValue / valueTotal )
                                    * fabs( zeroXAxisI - logHeight + sideBarHeight );

                    else
                        barHeight = pixelsPerUnit * cellValue;


                    // draw only the bars belonging to the axis
                    // which we are processing currently
                    if( dataset >= datasetStart && dataset <= datasetEnd ) {
                        // Configure colors
                        if ( params()->dataColor( dataset ).isValid() )
                            painter->setBrush( params()->dataColor( dataset ) );
                        else
                            painter->setBrush( NoBrush );

                        // Prepare region for detection of mouse clicks
                        // and for finding anchor positions of data value texts
                        QRegion* region = 0;
                        if ( regions )
                            region = new QRegion();

                        // Start drawing
                        if ( barHeight < 0 ) {
                            //
                            //  Negative values:
                            //
                            double yZero = yposNegatives - zeroXAxisI;
                            bool tooLow = (barHeight - yZero + logHeight) < - 1.01;
                            if ( tooLow && barNormal ) {
                                double height  = -1.0 * ( yZero - logHeight );
                                double delta   = 0.0125 * logHeight;
                                double height1 = height - 3.0 * delta;
                                int yArrowGap = static_cast < int > ( 2.5 * delta );
                                int x4 = static_cast < int > ( xpos + frontBarWidth );
                                int xm = static_cast < int > ( xpos + frontBarWidth / 2.0 );
                                QRect rect( ourClipRect );
                                rect.setHeight( static_cast<int>( rect.height() + 3.0 * delta ) );
                                painter->setClipRect( rect );
                                QPointArray points( 5 );
                                points.setPoint( 0, static_cast<int>( xpos ), static_cast<int>( yZero ) );
                                points.setPoint( 1, x4, static_cast<int>( yZero ) );
                                points.setPoint( 2, x4, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                points.setPoint( 3, xm, static_cast<int>( yZero + height1 ) );
                                points.setPoint( 4, static_cast<int>( xpos ), static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                painter->drawPolygon( points );
                                // Don't use points for drawing after this!
                                if ( region ) {
                                    points.translate( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( points );
                                }

                                QPointArray points2( 6 );
                                points2.setPoint( 0, static_cast<int>( xpos ), static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                points2.setPoint( 1, xm, static_cast<int>( yZero + height1 ) );
                                points2.setPoint( 2, x4, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                points2.setPoint( 3, x4, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                points2.setPoint( 4, xm, static_cast<int>( yZero + height1 - 0.75 * delta ) );
                                points2.setPoint( 5, static_cast<int>( xpos ), static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                points2.translate( 0, yArrowGap );
                                painter->drawPolygon( points2 );
                                if ( region ) {
                                    QPointArray points2cpy( points2 );
                                    points2cpy.translate( _dataRect.x(),
                                                          _dataRect.y() );
                                    *region += QRegion( points2cpy );
                                }
                                points2.translate( 0, yArrowGap );
                                painter->drawPolygon( points2 );
                                // Don't use points2 for drawing after this!
                                if ( region ) {
                                    points2.translate( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( points2 );
                                }

                                painter->setClipRect( ourClipRect );
                            } else {
                                //
                                // old code (sometimes not touching the grid):
                                // QRect rec( xpos, yZero, frontBarWidth, -barHeight );
                                // painter->drawRect( rec );
                                //
                                //bool fromBottom = barNormal && !params()->threeDBars();
                                double pt1X = xpos;
                                double pt1Y =   yZero
                                                - barHeight;
                                QPoint pt1( static_cast < int > ( pt1X ),
                                            static_cast < int > ( pt1Y ) );
                                QPoint pt2( static_cast < int >
                                            ( pt1X + frontBarWidth ),
                                            static_cast < int > ( yZero ) );
                                if( pt2.y() < pt1Y ) {
                                    pt1.setY( pt2.y() );
                                    pt2.setY( static_cast<int>( pt1Y ) );
                                }
                                if( pt2.x() < pt1X ) {
                                    pt1.setX( pt2.x() );
                                    pt2.setX( static_cast<int>( pt1X ) );
                                }
                                QSize siz( pt2.x() - pt1.x(),
                                           pt2.y() - pt1.y() );
                                QRect rect( pt1, siz );
                                painter->drawRect( rect );

                                // Don't use rect for drawing after this!
                                if ( region ) {
                                    rect.moveBy( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( rect );
                                }
                            }
                        } else {
                            //
                            //  Positive values:
                            //
                            double yZero = yposPositives - zeroXAxisI;
                            bool tooHigh = (barHeight - yZero) > 1.01;
                            if ( tooHigh && barNormal ) {
                                double height  = -1.0 * static_cast < int > ( yZero );
                                double delta   = -0.0125 * logHeight;
                                double height1 = height + -3.0 * delta;
                                int yArrowGap = static_cast < int > ( 2.5 * delta );
                                int x4 = static_cast < int > ( xpos + frontBarWidth );
                                int xm = static_cast < int > ( xpos + frontBarWidth / 2.0 );
                                QRect rect( ourClipRect );
                                rect.setTop( static_cast<int>( rect.top() + 3 * delta ) );
                                rect.setHeight( static_cast<int>( rect.height() - 3 * delta ) );
                                painter->setClipRect( rect );
                                QPointArray points( 5 );
                                points.setPoint( 0, static_cast<int>( xpos ), static_cast<int>( yZero ) );
                                points.setPoint( 1, x4, static_cast<int>( yZero ) );
                                points.setPoint( 2, x4, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                points.setPoint( 3, xm, static_cast<int>( yZero + height1 ) );
                                points.setPoint( 4, static_cast<int>( xpos ), static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                painter->drawPolygon( points );

                                // Don't use points for drawing after this!
                                if ( region ) {
                                    points.translate( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( points );
                                }

                                QPointArray points2( 6 );
                                points2.setPoint( 0, static_cast<int>( xpos ), static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                points2.setPoint( 1, xm, static_cast<int>( yZero + height1 ) );
                                points2.setPoint( 2, x4, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                points2.setPoint( 3, x4, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                points2.setPoint( 4, xm, static_cast<int>( yZero + height1 - 0.75 * delta ) );
                                points2.setPoint( 5, static_cast<int>( xpos ), static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                points2.translate( 0, yArrowGap );
                                painter->drawPolygon( points2 );
                                if ( region ) {
                                    QPointArray points2cpy( points2 );
                                    points2cpy.translate( _dataRect.x(),
                                                          _dataRect.y() );
                                    *region += QRegion( points2cpy );
                                }
                                points2.translate( 0, yArrowGap );
                                painter->drawPolygon( points2 );

                                // Don't use points2 for drawing after this!
                                if ( region ) {
                                    points2.translate( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( points2 );
                                }

                                painter->setClipRect( ourClipRect );
                            } else {
                                //bool fromBottom = barNormal && !params()->threeDBars();
                                double y0 = yposPositives - zeroXAxisI;
                                double pt1X = xpos;
                                double pt1Y =   y0
                                                - barHeight;
                                QPoint pt1( static_cast < int > ( pt1X ),
                                            static_cast < int > ( pt1Y ) );
                                QPoint pt2( static_cast < int >
                                            ( pt1X + frontBarWidth ),
                                            static_cast < int >
                                            ( y0 + shiftUpperBars ) );
                                if( pt2.y() < pt1Y ) {
                                    pt1.setY( pt2.y() );
                                    pt2.setY( static_cast<int>( pt1Y ) );
                                }
                                if( pt2.x() < pt1X ) {
                                    pt1.setX( pt2.x() );
                                    pt2.setX( static_cast<int>( pt1X ) );
                                }
                                QSize siz( pt2.x() - pt1.x(),
                                           pt2.y() - pt1.y() );
                                QRect rect( pt1, siz );
                                painter->drawRect( rect );

                                // Don't use rect for drawing after this
                                if ( region ) {
                                    rect.moveBy( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( rect );
                                }
                            }
                        }

                        if ( params()->threeDBars() ) {
                            // drawing the right side
                            QPointArray points( 4 );
                            if ( barHeight < 0 ) {
                                points.setPoint( 0, static_cast<int>( xpos + frontBarWidth ),
                                                 static_cast<int>( yposNegatives - zeroXAxisI ) );
                                points.setPoint( 1, static_cast<int>( xpos + frontBarWidth ),
                                                 static_cast<int>( yposNegatives - zeroXAxisI - barHeight ) );
                                points.setPoint( 2, static_cast<int>( xpos + barWidth ),
                                                 static_cast<int>( yposNegatives - zeroXAxisI - barHeight - sideBarHeight ) );
                                points.setPoint( 3, static_cast<int>( xpos + barWidth ),
                                                 static_cast<int>( yposNegatives - zeroXAxisI - sideBarHeight ) );
                            } else {
                                points.setPoint( 0, static_cast<int>( xpos + frontBarWidth ),
                                                 static_cast<int>( yposPositives - zeroXAxisI ) );
                                points.setPoint( 1, static_cast<int>( xpos + frontBarWidth ),
                                                 static_cast<int>( yposPositives - zeroXAxisI - barHeight ) );
                                points.setPoint( 2, static_cast<int>( xpos + barWidth ),
                                                 static_cast<int>( yposPositives - zeroXAxisI - barHeight - sideBarHeight ) );
                                points.setPoint( 3, static_cast<int>( xpos + barWidth ),
                                                 static_cast<int>( yposPositives - zeroXAxisI - sideBarHeight ) );
                            }

                            if ( params()->dataColor( dataset ).isValid() )
                                painter->setBrush( QBrush( params()->dataShadow2Color( dataset ), params()->shadowPattern() ) );
                            else
                                painter->setBrush( NoBrush );
                            painter->drawPolygon( points );
                            if ( region ) {
                                QPointArray pointscpy( points );
                                pointscpy.translate( _dataRect.x(), _dataRect.y() );
                                *region += QRegion( pointscpy );
                            }

                            // drawing the top, but only for the topmost piece for stacked and percent
                            if ( barNormal ||
                                 // For stacked and percent bars, there are three ways to determine
                                 // the top:
                                 // 1. all values are negative: the top is the one in the first dataset
                                 ( maxValueInThisColumn <= 0.0 && dataset == 0 ) ||
                                 // 2. all values are positive: the top is the one in the last dataset
                                 ( minValueInThisColumn >= 0.0 && dataset == datasetEnd ) ||
                                 // 3. some values are positive, some negative:
                                 // the top is the one in the last positive
                                 // dataset value
                                 ( dataset == lastPositive ) ) {
                                if ( barHeight < 0 ) {
                                    points.setPoint( 0, static_cast<int>( xpos ), static_cast<int>( yposNegatives - zeroXAxisI ) );
                                    points.setPoint( 1, static_cast<int>( xpos + sideBarWidth ),
                                                     static_cast<int>( yposNegatives - zeroXAxisI - sideBarHeight ) );
                                    points.setPoint( 2, static_cast<int>( xpos + barWidth ),
                                                     static_cast<int>( yposNegatives - zeroXAxisI - sideBarHeight ) );
                                    points.setPoint( 3, static_cast<int>( xpos + frontBarWidth ),
                                                     static_cast<int>( yposNegatives - zeroXAxisI ) );
                                } else {
                                    points.setPoint( 0, static_cast<int>( xpos ),
                                                     static_cast<int>( yposPositives - zeroXAxisI - barHeight ) );
                                    points.setPoint( 1, static_cast<int>( xpos + sideBarWidth ),
                                                     static_cast<int>( yposPositives - zeroXAxisI - barHeight - sideBarHeight ) );
                                    points.setPoint( 2, static_cast<int>( xpos + barWidth ),
                                                     static_cast<int>( yposPositives - zeroXAxisI - barHeight - sideBarHeight ) );
                                    points.setPoint( 3, static_cast<int>( xpos + frontBarWidth ),
                                                     static_cast<int>( yposPositives - zeroXAxisI - barHeight ) );
                                }
                                if ( barHeight < 0 )
                                    painter->setBrush( black );
                                else
                                    painter->setBrush( QBrush( params()->dataShadow1Color( dataset ), params()->shadowPattern() ) );
                                if ( !params()->dataColor( dataset ).isValid() )
                                    painter->setBrush( NoBrush ); // override prev. setting
                                painter->drawPolygon( points );
                                // Don't use points for drawing after this!
                                if ( region ) {
                                    points.translate( _dataRect.x(), _dataRect.y() );
                                    *region += QRegion( points );
                                }
                            }
                        }//if ( params()->threeDBars() )

                        if ( regions && region ) {
                            regions->append( new KDChartDataRegion( *region, dataset, value, chart ) );
                            delete region;
                        }
                    }// if( dataset >= datasetStart && dataset <= datasetEnd )


                    // Vertical advancement in stacked or percent only if there was a value
                    if ( params()->barChartSubType() == KDChartParams::BarStacked ||
                         params()->barChartSubType() == KDChartParams::BarPercent )
                        if ( barHeight < 0.0 )
                            yposNegatives -= barHeight;
                        else
                            yposPositives -= barHeight;

                } // no else, we simply do not paint if there is nothing to paint

                // Advance to next value; only for normal bars
                if ( barNormal ) {
                    xpos += barWidth;
                    // skip gap between datasets, unless last dataset
                    if ( dataset < datasetNum - 1 )
                        xpos += datasetGap;
                }
            }

            // Advancement between value blocks
            if ( barNormal )
                // skip gap between value blocks, don't worry about last one here
                xpos += valueBlockGap;
            else {
                // start at bottom with next value group
                yposPositives = logHeight;
                yposNegatives = logHeight;
                // skip gap between value blocks
                xpos += valueBlockGap + barWidth;
            }
        }
    }
    painter->translate( - _dataRect.x(), - _dataRect.y() );
}
