/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include "KDChartCartesianGrid.h"
#include "KDChartPaintContext.h"

#include <QPainter>

#include <KDABLibFakes>


static qreal _trunc( qreal v )
{
    return (( v > 0.0 ) ? floor( v ) : ceil(  v ));
}


using namespace KDChart;


void CartesianGrid::drawGrid( PaintContext* context )
{
    //qDebug() << "KDChart::CartesianGrid::drawGrid( PaintContext* context ) called";

    CartesianCoordinatePlane* plane = dynamic_cast<CartesianCoordinatePlane*>(context->coordinatePlane());
    Q_ASSERT_X ( plane, "CartesianGrid::drawGrid",
                 "Bad function call: PaintContext::coodinatePlane() NOT a cartesian plane." );

    const GridAttributes gridAttrsX( plane->gridAttributes( Qt::Horizontal ) );
    const GridAttributes gridAttrsY( plane->gridAttributes( Qt::Vertical ) );

    //qDebug() << "OK:";
    if ( !gridAttrsX.isGridVisible() && !gridAttrsY.isGridVisible() ) return;
    //qDebug() << "A";

    // important: Need to update the calculated mData,
    //            before we may use it!
    updateData( plane );

    // test for programming errors: critical
    Q_ASSERT_X ( mData.count() == 2, "CartesianGrid::drawGrid",
                 "Error: updateData did not return exactly two dimensions." );

    // test for invalid boundaries: non-critical
    if( !isBoundariesValid( mData ) ) return;
    //qDebug() << "B";

    DataDimension& dimX = mData.first();
    const DataDimension& dimY = mData.last();
    // test for other programming errors: critical
    Q_ASSERT_X ( dimX.stepWidth, "CartesianGrid::drawGrid",
                 "Error: updateData returned a Zero step width for the X grid." );
    Q_ASSERT_X ( dimY.stepWidth, "CartesianGrid::drawGrid",
                 "Error: updateData returned a Zero step width for the Y grid." );


    qreal numberOfUnitLinesX =
        qAbs( dimX.distance() / dimX.stepWidth )
        + (dimX.isCalculated ? 1.0 : 0.0);
    qreal numberOfUnitLinesY =
        qAbs( dimY.distance() / dimY.stepWidth )
        + (dimY.isCalculated ? 1.0 : 0.0);
    //qDebug("numberOfUnitLinesX: %f    numberOfUnitLinesY: %f",numberOfUnitLinesX,numberOfUnitLinesY);

    // do not draw a Zero size grid (and do not divide by Zero)
    if( numberOfUnitLinesX <= 0.0 || numberOfUnitLinesY <= 0.0 ) return;
    //qDebug() << "C";

    const QPointF p1 = plane->translate( QPointF(dimX.start, dimY.start) );
    const QPointF p2 = plane->translate( QPointF(dimX.end, dimY.end) );
//qDebug() << "dimX.isCalculated:" << dimX.isCalculated << "dimY.isCalculated:" << dimY.isCalculated;
//qDebug() << "dimX.start: " << dimX.start << "dimX.end: " << dimX.end;
//qDebug() << "dimY.start: " << dimY.start << "dimY.end: " << dimY.end;
//qDebug() << "p1:" << p1 << "  p2:" << p2;

    const qreal screenRangeX = qAbs ( p1.x() - p2.x() );
    const qreal screenRangeY = qAbs ( p1.y() - p2.y() );

    const qreal MinimumPixelsBetweenLines = 10.0;

//    qreal unitFactorX = 1.0;
//    qreal unitFactorY = 1.0;

    //FIXME(khz): Remove this code, and do the calculation in the grid calc function
    if( ! dimX.isCalculated ){
        while( screenRangeX / numberOfUnitLinesX <= MinimumPixelsBetweenLines ){
            dimX.stepWidth *= 10.0;
            dimX.subStepWidth *= 10.0;
            //qDebug() << "adjusting dimX.stepWidth to" << dimX.stepWidth;
            numberOfUnitLinesX = qAbs( dimX.distance() / dimX.stepWidth );
        }
    }

    const bool drawUnitLinesX = (screenRangeX / numberOfUnitLinesX > MinimumPixelsBetweenLines);
    const bool isLogarithmicX = dimX.isCalculated && (dimX.calcMode == AbstractCoordinatePlane::Logarithmic );
    const bool drawUnitLinesY = (screenRangeY / numberOfUnitLinesY > MinimumPixelsBetweenLines);
    const bool isLogarithmicY = (dimY.calcMode == AbstractCoordinatePlane::Logarithmic );
/*
    while ( !drawUnitLinesX ) {
        unitFactorX *= 10.0;
        drawUnitLinesX = screenRangeX / (numberOfUnitLinesX / unitFactorX) > MinimumPixelsBetweenLines;
    }
    while ( !drawUnitLinesY ) {
        unitFactorY *= 10.0;
        drawUnitLinesY = screenRangeY / (numberOfUnitLinesY / unitFactorY) > MinimumPixelsBetweenLines;
    }
*/

    const bool drawSubGridLinesX = isLogarithmicX ||
        ((dimX.subStepWidth != 0.0) &&
        (screenRangeX / (numberOfUnitLinesX / dimX.stepWidth * dimX.subStepWidth) > MinimumPixelsBetweenLines) &&
        gridAttrsX.isSubGridVisible());
    const bool drawSubGridLinesY = isLogarithmicY ||
        ((dimY.subStepWidth != 0.0) &&
        (screenRangeY / (numberOfUnitLinesY / dimY.stepWidth * dimY.subStepWidth) > MinimumPixelsBetweenLines) &&
        gridAttrsY.isSubGridVisible());

    const qreal minValueX = qMin( dimX.start, dimX.end );
    const qreal maxValueX = qMax( dimX.start, dimX.end );
    const qreal minValueY = qMin( dimY.start, dimY.end );
    const qreal maxValueY = qMax( dimY.start, dimY.end );

    if ( drawSubGridLinesX ) {
        context->painter()->setPen( gridAttrsX.subGridPen() );
        qreal f = minValueX;
        qreal fLogSubstep = minValueX;
        int logSubstep = 0;
        while ( f <= maxValueX ) {
            //qDebug() << "sub grid line X at" << f;
            QPointF topPoint( f, maxValueY );
            QPointF bottomPoint( f, minValueY );
            topPoint = plane->translate( topPoint );
            bottomPoint = plane->translate( bottomPoint );
            context->painter()->drawLine( topPoint, bottomPoint );
            if ( isLogarithmicX ){
                if( logSubstep == 9 ){
                    fLogSubstep *= 10.0;
                    logSubstep = 0;
                }
                f += fLogSubstep;
                ++logSubstep;
            }else{
                f += dimX.subStepWidth;
            }
        }
    }

    if ( drawSubGridLinesY ) {
        context->painter()->setPen( gridAttrsY.subGridPen() );
        qreal f = minValueY;
        qreal fLogSubstep = minValueY;
        int logSubstep = 0;
        while ( f <= maxValueY ) {
            //qDebug() << "sub grid line Y at" << f;
            QPointF leftPoint( minValueX, f );
            QPointF rightPoint( maxValueX, f );
            leftPoint = plane->translate( leftPoint );
            rightPoint = plane->translate( rightPoint );
            context->painter()->drawLine( leftPoint, rightPoint );
            if ( isLogarithmicY ){
                if( logSubstep == 9 ){
                    fLogSubstep *= 10.0;
                    logSubstep = 0;
                }
                f += fLogSubstep;
                ++logSubstep;
            }else{
                f += dimY.subStepWidth;
            }
        }
    }

    const bool drawXZeroLineX
        = dimX.isCalculated &&
        gridAttrsX.zeroLinePen().style() != Qt::NoPen;

    const bool drawZeroLineY
        = gridAttrsY.zeroLinePen().style() != Qt::NoPen;

    if ( drawUnitLinesX || drawXZeroLineX ) {
        //qDebug() << "E";
        if ( drawUnitLinesX )
            context->painter()->setPen( gridAttrsX.gridPen() );
//        const qreal minX = dimX.start;

        qreal f = minValueX;
        while ( f <= maxValueX ) {
            // PENDING(khz) FIXME: make draving/not drawing of Zero line more sophisticated?:
            const bool zeroLineHere = drawXZeroLineX && (f == 0.0);
            if ( drawUnitLinesX || zeroLineHere ){
                //qDebug("main grid line X at: %f",f);
                QPointF topPoint( f, maxValueY );
                QPointF bottomPoint( f, minValueY );
                topPoint = plane->translate( topPoint );
                bottomPoint = plane->translate( bottomPoint );
                if ( zeroLineHere )
                    context->painter()->setPen( gridAttrsX.zeroLinePen() );
                context->painter()->drawLine( topPoint, bottomPoint );
                if ( zeroLineHere )
                    context->painter()->setPen( gridAttrsX.gridPen() );
            }
            if ( isLogarithmicX )
                f *= 10.0;
            else
                f += dimX.stepWidth;
        }
    }
    if ( drawUnitLinesY || drawZeroLineY ) {
        //qDebug() << "F";
        if ( drawUnitLinesY )
            context->painter()->setPen( gridAttrsY.gridPen() );
        //const qreal minY = dimY.start;

        //qDebug("minY: %f   maxValueY: %f   dimY.stepWidth: %f",minY,maxValueY,dimY.stepWidth);
        qreal f = minValueY;
        while ( f <= maxValueY ) {
            // PENDING(khz) FIXME: make draving/not drawing of Zero line more sophisticated?:
            //qDebug("main grid line Y at: %f",f);
            const bool zeroLineHere = (f == 0.0);
            if ( drawUnitLinesY || zeroLineHere ){
                QPointF leftPoint(  minValueX, f );
                QPointF rightPoint( maxValueX, f );
                leftPoint  = plane->translate( leftPoint );
                rightPoint = plane->translate( rightPoint );
                if ( zeroLineHere )
                    context->painter()->setPen( gridAttrsY.zeroLinePen() );
                context->painter()->drawLine( leftPoint, rightPoint );
                if ( zeroLineHere )
                    context->painter()->setPen( gridAttrsY.gridPen() );
            }
            if ( isLogarithmicY )
                f *= 10.0;
            else
                f += dimY.stepWidth;
        }
    }
    //qDebug() << "Z";
}


DataDimensionsList CartesianGrid::calculateGrid(
    const DataDimensionsList& rawDataDimensions ) const
{
    Q_ASSERT_X ( rawDataDimensions.count() == 2, "CartesianGrid::calculateGrid",
                 "Error: calculateGrid() expects a list with exactly two entries." );
    DataDimensionsList l( rawDataDimensions );
    // rule:  Returned list is either empty, or it is providing two
    //        valid dimensions, complete with two non-Zero step widths.
    if( isBoundariesValid( l ) ) {
        //qDebug("CartesianGrid::calculateGrid()   l.first().start: %f   l.first().end: %f", l.first().start, l.first().end);
        const DataDimension dimX
            = calculateGridXY( l.first(), Qt::Horizontal );
        if( dimX.stepWidth ){
            //qDebug("CartesianGrid::calculateGrid()   l.last().start: %f   l.last().end: %f", l.last().start, l.last().end);
            const DataDimension dimY
                = calculateGridXY( l.last(), Qt::Vertical );
            if( dimY.stepWidth ){
                l.first().start        = dimX.start;
                l.first().end          = dimX.end;
                l.first().stepWidth    = dimX.stepWidth;
                l.first().subStepWidth = dimX.subStepWidth;
                l.last().start        = dimY.start;
                l.last().end          = dimY.end;
                l.last().stepWidth    = dimY.stepWidth;
                // calculate some reasonable subSteps if the
                // user did not set the sub grid but did set
                // the stepWidth.
                if ( dimY.subStepWidth == 0 )
                    l.last().subStepWidth = dimY.stepWidth/2;
                else
                    l.last().subStepWidth = dimY.subStepWidth;
            }
        }
    }
    return l;
}


void adjustUpperLowerRange( qreal& start, qreal& end, qreal stepWidth )
{
    const qreal startAdjust = ( start >= 0.0 ) ? 0.0 : -1.0;
    const qreal endAdjust   = ( end   >= 0.0 ) ? 1.0 :  0.0;
    if ( fmod( start, stepWidth ) != 0.0 )
        start = stepWidth * (_trunc( start / stepWidth ) + startAdjust);
    if ( fmod( end, stepWidth ) != 0.0 )
        end = stepWidth * (_trunc( end / stepWidth ) + endAdjust);
}
qreal fastPow10( int x )
{
    qreal res = 1.0;
    if( 0 <= x ){
        for( int i = 1; i <= x; ++i )
            res *= 10.0;
    }else{
        for( int i = -1; i >= x; --i )
            res /= 10.0;
    }
    return res;
}

#if defined ( Q_WS_WIN)
#define trunc(x) ((int)(x))
#endif

DataDimension CartesianGrid::calculateGridXY(
    const DataDimension& rawDataDimension,
    Qt::Orientation orientation ) const
{
    DataDimension dim( rawDataDimension );
    if( dim.isCalculated && dim.start != dim.end ){
        if( dim.calcMode == AbstractCoordinatePlane::Linear ){
            // linear ( == not-logarithmic) calculation
            if( dim.stepWidth == 0.0 ){
                QList<qreal> granularities;
                switch( dim.sequence ){
                    case KDChartEnums::GranularitySequence_10_20:
                        granularities << 1.0 << 2.0;
                        break;
                    case KDChartEnums::GranularitySequence_10_50:
                        granularities << 1.0 << 5.0;
                        break;
                    case KDChartEnums::GranularitySequence_25_50:
                        granularities << 2.5 << 5.0;
                        break;
                    case KDChartEnums::GranularitySequenceIrregular:
                        granularities << 1.0 << 2.0 << 2.5 << 5.0;
                        break;
                    default:
                        break;
                }
                //qDebug("CartesianGrid::calculateGridXY()   dim.start: %f   dim.end: %f", dim.start, dim.end);
                calculateStepWidth(
                    dim.start, dim.end, granularities, orientation,
                    dim.stepWidth, dim.subStepWidth );
            }
            // if needed, adjust start/end to match the step width:
            adjustUpperLowerRange( dim.start, dim.end, dim.stepWidth );
            //qDebug() << "CartesianGrid::calculateGridXY() returns linear range: min " << dim.start << " and max" << dim.end;
        }else{
            // logarithmic calculation (ignoring all negative values)
            qreal min;
            const qreal minRaw = qMax( qMin( dim.start, dim.end ), 0.0 );
            const int minLog = static_cast<int>(trunc( log10( minRaw ) ) );
            if( minLog <= 0 )
                min = 1;
            else
                min = fastPow10( minLog-1 );

            qreal max;
            const qreal maxRaw = qMax( qMax( dim.start, dim.end ), 0.0 );
            const int maxLog = static_cast<int>(trunc( log10( maxRaw ) ) );
            if( maxLog <= 0 )
                max = 1;
            else if( fastPow10( maxLog ) < maxRaw )
                max = fastPow10( maxLog+1 );
            else
                max = fastPow10( maxLog );
            dim.start = min;
            dim.end   = max;
            dim.stepWidth = qAbs(max - min) / 10.0;
            //qDebug() << "CartesianGrid::calculateGridXY() returns logarithmic:  min " << min << " and max" << max;
        }
    }else{
        dim.stepWidth = 1.0;
    }
    return dim;
}


void calculateSteps(
    qreal start_, qreal end_, const QList<qreal>& list,
    int minSteps, int maxSteps,
    int power,
    qreal& steps, qreal& stepWidth )
{
    //qDebug("-----------------------------------\nstart: %f   end: %f   power-of-ten: %i", start_, end_, power);

    qreal distance;
    steps = 0.0;

    const int lastIdx = list.count()-1;
    for( int i = 0;  i <= lastIdx;  ++i ){
        const qreal testStepWidth = list.at(lastIdx - i) * fastPow10( power );
        //qDebug( "testing step width: %f", testStepWidth);
        qreal start = qMin( start_, end_ );
        qreal end   = qMax( start_, end_ );
        //qDebug("pre adjusting    start: %f   end: %f", start, end);
        adjustUpperLowerRange( start, end, testStepWidth );
        //qDebug("post adjusting   start: %f   end: %f", start, end);

        const qreal testDistance = qAbs(end - start);
        const qreal testSteps    = testDistance / testStepWidth;

        if( (minSteps <= testSteps) && (testSteps <= maxSteps)
              && ( (steps == 0.0) || (testDistance < distance) ) ){
            steps     = testSteps;
            stepWidth = testStepWidth;
            distance  = testDistance;
            //qDebug( "start: %f   end: %f   step width: %f   steps: %f   distance: %f",
            //        start, end, stepWidth, steps, distance);
        }
    }
}


void CartesianGrid::calculateStepWidth(
    qreal start_, qreal end_,
    const QList<qreal>& granularities,
    Qt::Orientation orientation,
    qreal& stepWidth, qreal& subStepWidth ) const
{

    Q_ASSERT_X ( granularities.count(), "CartesianGrid::calculateStepWidth",
                 "Error: The list of GranularitySequence values is empty." );
    QList<qreal> list( granularities );
    qSort( list );

    const qreal start = qMin( start_, end_);
    const qreal end   = qMax( start_, end_);
    const qreal distance = end - start;
    //qDebug( "raw data start: %f   end: %f", start, end);

    //FIXME(khz): make minSteps and maxSteps configurable by the user.
    const int minSteps = 2;
    const int maxSteps = 12;

    qreal steps;
    int power = 0;
    while( list.last() * fastPow10( power ) < distance ){
        ++power;
    };
    // We have the sequence *two* times in the calculation test list,
    // so we will be sure to find the best match:
    const int count = list.count();
    QList<qreal> testList;
    for( int i = 0;  i < count;  ++i )
        testList << list.at(i) * 0.1;
    testList << list;
    do{
        //qDebug() << "list:" << testList;
        //qDebug( "calculating steps: power: %i", power);
        calculateSteps( start, end, testList, minSteps, maxSteps, power, steps, stepWidth );
        --power;
    }while( steps == 0.0 );
    ++power;
    //qDebug( "steps calculated:  stepWidth: %f   steps: %f", stepWidth, steps);

    // find the matching sub-grid line width in case it is
    // not set by the user

    if (  subStepWidth == 0.0 ) {
        if( stepWidth == list.first() * fastPow10( power ) ){
            subStepWidth = list.last() * fastPow10( power-1 );
            //qDebug("A");
        }else if( stepWidth == list.first() * fastPow10( power-1 ) ){
            subStepWidth = list.last() * fastPow10( power-2 );
            //qDebug("B");
        }else{
            qreal smallerStepWidth = list.first();
            for( int i = 1;  i < list.count();  ++i ){
                if( stepWidth == list.at( i ) * fastPow10( power ) ){
                    subStepWidth = smallerStepWidth * fastPow10( power );
                    break;
                }
                if( stepWidth == list.at( i ) * fastPow10( power-1 ) ){
                    subStepWidth = smallerStepWidth * fastPow10( power-1 );
                    break;
                }
                smallerStepWidth = list.at( i );
            }

            //qDebug("C");
        }
    }
    //qDebug("CartesianGrid::calculateStepWidth() found stepWidth %f (%f steps) and sub-stepWidth %f",
    //      stepWidth, steps, subStepWidth);
}
