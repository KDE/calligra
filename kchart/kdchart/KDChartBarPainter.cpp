/* -*- Mode: C++ -*-
   $Id$
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

bool KDChartBarPainter::isNormalMode() const
{
    return KDChartParams::BarNormal == params()->barChartSubType();
}

int KDChartBarPainter::clipShiftUp( bool normalMode, double areaWidthP1000 ) const
{
    return   ( normalMode && !_bThreeDBars )
        ? static_cast < int > ( areaWidthP1000 * 16.0 )
        : 0;
}


void KDChartBarPainter::initMyPainter( QPainter* painter )
{
    _myPainter = painter;
    _painterDX = 0.0;
    _painterDY = 0.0;
}

void KDChartBarPainter::shiftMyPainter( double dx, double dy )
{
    if( dx != 0.0 || dy != 0.0 ){
        _myPainter->translate(dx, dy);
        _painterDX += dx;
        _painterDY += dy;
    }
}

void KDChartBarPainter::shiftMyPainterBack()
{
    if( _painterDX != 0.0 || _painterDY != 0.0 ){
        _myPainter->translate(-_painterDX, -_painterDY);
        _painterDX = 0.0;
        _painterDY = 0.0;
    }
}


void KDChartBarPainter::specificPaintData( QPainter* painter,
                                           const QRect& ourClipRect,
                                           KDChartTableDataBase* data,
                                           KDChartDataRegionList* regions,
                                           const KDChartAxisParams* ordinatePara,
                                           bool bNormalMode,
                                           uint chart,
                                           double logWidth,
                                           double areaWidthP1000,
                                           double logHeight,
                                           double axisYOffset,
                                           double minColumnValue,
                                           double maxColumnValue,
                                           double columnValueDistance,
                                           uint chartDatasetStart,
                                           uint chartDatasetEnd,
                                           uint datasetStart,
                                           uint datasetEnd )
{
    if( !data ) return;

    const QPen defaultOutlinePen( params()->outlineDataColor(),
                                  params()->outlineDataLineWidth(),
                                  params()->outlineDataLineStyle() );

    abscissaInfos ai;
    ai.bCenterThePoints = true;
    calculateAbscissaInfos( *params(), *data,
                            datasetStart, datasetEnd,
                            logWidth, _dataRect,
                            ai );

    const QRect devRect( painter->window() );

    initMyPainter( painter );

    const bool ordinateIsLogarithmic
        = KDChartAxisParams::AxisCalcLogarithmic == ordinatePara->axisCalcMode();

    const bool bMultiRows = KDChartParams::BarMultiRows == params()->barChartSubType();
    _bThreeDBars = params()->threeDBars() || bMultiRows;
    int numChartDataEntryDatasets = 0;
    uint myLastDataEntryDataset = 0;
    for( uint iD = chartDatasetStart; iD <= chartDatasetEnd; ++iD ){
        if( params()->chartSourceMode( iD ) == KDChartParams::DataEntry ){
            ++numChartDataEntryDatasets;
            myLastDataEntryDataset = iD;
        }
    }

    const bool bHadClipping = painter->hasClipping();
    if( bMultiRows )
        painter->setClipping( false );

    // Number of columns in one dataset: If -1, use all values,
    // otherwise use the specified number of values.
    int numValues = 0;
    if ( params()->numValues() != -1 )
        numValues = params()->numValues();
    else
        numValues = data->usedCols();

    double datasetGap = bMultiRows
                        ? 0.0
                        : params()->datasetGap()
                        * ( params()->datasetGapIsRelative()
                            ? areaWidthP1000
                            : 1.0 );
    double valueBlockGap = bMultiRows
                           ? 0.0
                           : params()->valueBlockGap()
                           * ( params()->valueBlockGapIsRelative()
                               ? areaWidthP1000
                               : 1.0 );

    // This is the same for all three bar types except for multi-bar Surface charts.
    double spaceBetweenValueBlocks = bMultiRows
                                     ? 0.0
                                     : valueBlockGap * numValues;

    // Set some geometry values that apply to bar charts only
    double totalNumberOfBars = 0.0;
    double spaceBetweenDatasets = 0.0;
    switch ( params()->barChartSubType() ) {
    case KDChartParams::BarNormal: {
        totalNumberOfBars = numChartDataEntryDatasets * numValues;
        spaceBetweenDatasets = datasetGap
                               * ( totalNumberOfBars - numValues );
        break;
    }
    case KDChartParams::BarStacked:
    case KDChartParams::BarPercent:
    case KDChartParams::BarMultiRows:
        totalNumberOfBars = numValues;
        spaceBetweenDatasets = 0; // always 0 when stacked/percent/multi-rows
        break;
    default:
        qFatal( "Unsupported bar chart type" );
    };

    const double barWidth =
        (   logWidth
            - spaceBetweenValueBlocks
            - spaceBetweenDatasets ) / totalNumberOfBars;
    const double sideBarWidth = _bThreeDBars
                                ? ( barWidth - barWidth / (1.0 + params()->cosThreeDBarAngle()) ) *
                                params()->threeDBarDepth()
                                : 0.0;
    const double frontBarWidth = _bThreeDBars && !bMultiRows
                                 ? barWidth - sideBarWidth
                                 : barWidth;
    const double sideBarHeight = sideBarWidth;

    double pixelsPerUnit = 0.0;
    if ( params()->barChartSubType() != KDChartParams::BarPercent )  // not used for percent
        pixelsPerUnit = logHeight / ( columnValueDistance ? columnValueDistance : 10 );

    //qDebug("\nordinatePara->trueAxisLow()  = %f", ordinatePara->trueAxisLow());
    //qDebug(  "ordinatePara->trueAxisHigh() = %f", ordinatePara->trueAxisHigh());
    //qDebug(  "sideBarHeight            = %f", sideBarHeight);
    //qDebug(  "pixelsPerUnit            = %f", pixelsPerUnit);

    double zeroXAxisI;
    if ( params()->barChartSubType() == KDChartParams::BarPercent ) {
        if ( minColumnValue == 0.0 )
            zeroXAxisI = 0.0;
        else if ( maxColumnValue == 0.0 )
            zeroXAxisI = logHeight - sideBarHeight;
        else
            zeroXAxisI = ( logHeight - sideBarHeight ) / 2.0;
    } else {
        zeroXAxisI =   logHeight
                       - ordinatePara->axisZeroLineStartY()
                       + _dataRect.y();
    }

    double shiftUpperBars =    (params()->barChartSubType() != KDChartParams::BarPercent)
                               && (ordinatePara->axisTrueLineWidth() % 2)
                               ? 1.0
                               : 0.0;

    // Initializing drawing positions
    double yposPositivesStart = logHeight;
    double yposNegativesStart = logHeight;
    if( params()->barChartSubType() == KDChartParams::BarPercent ){
        yposPositivesStart += axisYOffset;
        yposNegativesStart += axisYOffset;
    }

    for ( int iPaintExtraLinesOrTheData = 0;
          iPaintExtraLinesOrTheData < 3;
          ++iPaintExtraLinesOrTheData )
    {
        const bool bDrawExtraLines        = (1 != iPaintExtraLinesOrTheData);
        const bool bDrawExtraLinesInFront = (2 == iPaintExtraLinesOrTheData);

        double xpos = 0.0 + valueBlockGap / 2.0;

        double yposPositives = yposPositivesStart;
        double yposNegatives = yposNegativesStart;

        double totalThreeDBarWidth = bMultiRows
                                     ? barWidth + sideBarWidth
                                     : barWidth;

        double nShiftX = bMultiRows
                         ? sideBarWidth
                         : 0.0;
        double nShiftY = bMultiRows
                         ? sideBarHeight
                         : 0.0;

        double valueTotal = 0.0; // valueTotal is used for percent bars only

        // iterate over all columns: item1, item2, item3 ...
        for ( int value = 0; value < numValues; ++value ) {

            double lastPositive = 0.0;
            double maxValueInThisColumn = 0.0, minValueInThisColumn = 0.0;
            if ( params()->barChartSubType() == KDChartParams::BarStacked ||
                 params()->barChartSubType() == KDChartParams::BarPercent ) {
                valueTotal = 0.0;
                // iterate over datasets of this axis only:
                for ( uint dataset  = datasetStart;
                      dataset <= datasetEnd;
                      ++dataset ) {

                    const KDChartData& cell = data->cell( dataset, value );

                    if( params()->chartSourceMode( dataset ) == KDChartParams::DataEntry
                        && cell.isDouble() ){

                        double cellValue;
                        if( ordinateIsLogarithmic )
                            cellValue = log10(cell.doubleValue());
                        else
                            cellValue = cell.doubleValue();
                        //qDebug("value   %u    dataset   %u   logHeight %f", value,dataset,logHeight);
                        if( 0.0 <= cellValue )
                            lastPositive = cellValue;
                        maxValueInThisColumn = QMAX( maxValueInThisColumn, cellValue );
                        minValueInThisColumn = QMIN( minValueInThisColumn, cellValue );
                        if( params()->barChartSubType() == KDChartParams::BarPercent )
                            valueTotal += cellValue;
                        //qDebug("a");
                    }
                }
            }

            //qDebug("shiftMyPainter( (numChartDataEntryDatasets-1)*nShiftX, (numChartDataEntryDatasets-1)*-nShiftY );");
            shiftMyPainter( (numChartDataEntryDatasets-1)*nShiftX, (numChartDataEntryDatasets-1)*-nShiftY );

            // iterate over all datasets of this chart:
            // (but draw only the bars of this axis)
            for ( uint dataset = bMultiRows
                                 ? chartDatasetEnd
                                 : chartDatasetStart;
                  dataset >= chartDatasetStart && dataset <= chartDatasetEnd;
                  bMultiRows
                                 ? --dataset
                                 : ++dataset ) {
                //qDebug("value   %u    dataset   %u   logHeight %f", value,dataset,logHeight);

                const bool bDataEntrySourceMode
                    = (params()->chartSourceMode( dataset ) == KDChartParams::DataEntry);

                const KDChartData& cell = data->cell( dataset, value );

                if ( cell.isDouble() ) {
                    //qDebug("b");
                    // there is a numeric value
                    double cellValue;
                    if( ordinateIsLogarithmic )
                        cellValue = log10(cell.doubleValue());
                    else
                        cellValue = cell.doubleValue();

                    double barHeight;
                    if ( params()->barChartSubType() == KDChartParams::BarPercent )
                        //     barHeight = ( cellValue / valueTotal ) * logHeight;
                        barHeight =   ( cellValue / valueTotal )
                                      * fabs( zeroXAxisI - logHeight + sideBarHeight );
                    else{
                        barHeight = pixelsPerUnit * cellValue;
                        if( 0.0 <= barHeight )
                            barHeight = QMAX(0.0, barHeight - sideBarHeight);
                        else
                            barHeight -= sideBarHeight;
                    }
                    // This little adjustment avoids a crash when trying
                    // to retrieve the bounding rect of a zero size region...
                    if( 0 == barHeight )
                        barHeight = 1; // makes sense to have a min size anyway


                    // draw only the bars belonging to the axis
                    // which we are processing currently
                    if( dataset >= datasetStart && dataset <= datasetEnd ) {
                        //qDebug("b2");

                        // calculate Abscissa axis value, if there are X coordinates
                        // ---------------------------------------------------------
                        bool skipMe = false;
                        if( ai.bCellsHaveSeveralCoordinates )
                            skipMe = !calculateAbscissaAxisValue( cell, ai, 0,
                                                                  xpos );

                        if( !skipMe ){
                            // Configure colors
                            QColor myBarColor(     params()->dataColor(        dataset ) );
                            QColor myShadow1Color( params()->dataShadow1Color( dataset ) );
                            QColor myShadow2Color( params()->dataShadow2Color( dataset ) );

                            // Get default values for extra lines and their markers
                            const KDChartParams::LineMarkerStyle
                                defaultMarkerStyle = params()->lineMarkerStyle( dataset );
                            const QPen defaultPen(   params()->lineColor().isValid()
                                                     ? params()->lineColor()
                                                     : params()->dataColor( dataset ),
                                                     params()->lineWidth(),
                                                     params()->lineStyle() );

                            // --------------------------------------------------------
                            // determine any 'extra' properties assigned to this cell
                            // by traversing the property set chain (if necessary)
                            // --------------------------------------------------------
                            KDChartPropertySet propSet;
                            bool bCellPropertiesFound =
                                params()->calculateProperties( cell.propertySet(),
                                                               propSet );
                            bool bShowThisBar = bDataEntrySourceMode;
                            if( bCellPropertiesFound ){
                                if( bShowThisBar && !bDrawExtraLines ){
                                    int  iDummy;
                                    if( propSet.hasOwnShowBar( iDummy, bShowThisBar ) ){
                                        // NOP
                                    }
                                    if( propSet.hasOwnBarColor( iDummy, myBarColor ) ){
                                        // adjust the shadow colors
                                        params()->calculateShadowColors( myBarColor,
                                                                         myShadow1Color,
                                                                         myShadow2Color );
                                    }
                                }
                            }

                            if( !bDrawExtraLines || bCellPropertiesFound ){

                                QRegion* region = 0;
                                if( bDataEntrySourceMode && !bDrawExtraLines ){
                                    painter->setPen( defaultOutlinePen );
                                    if ( myBarColor.isValid() )
                                        painter->setBrush( myBarColor );
                                    else
                                        painter->setBrush( NoBrush );

                                    // Prepare region for detection of mouse clicks
                                    // and for finding anchor positions of data value texts
                                    if ( regions )
                                        region = new QRegion();
                                }

                                // Start drawing
                                int frontX1 = 0;
                                int frontX2 = 0;
                                if ( barHeight < 0 ) {
                                    //
                                    //  Negative values:
                                    //
                                    double yZero = yposNegatives - zeroXAxisI;
                                    bool tooLow = (barHeight - yZero + logHeight) < - 1.01;
                                    //qDebug("\nbarHeight - yZero + logHeight = : %f - %f + %f = %f",
                                    //barHeight,yZero,logHeight,
                                    //barHeight - yZero + logHeight);
                                    if ( tooLow && bNormalMode ) {
                                        //qDebug("toooo low");
                                        double height  = -1.0 * ( yZero - logHeight );
                                        double delta   = 0.0125 * logHeight;
                                        double height1 = height - 3.0 * delta;
                                        int yArrowGap = static_cast < int > ( 2.5 * delta );
                                        frontX1 = static_cast<int>( xpos );
                                        frontX2 = static_cast < int > ( xpos + frontBarWidth );
                                        int xm = static_cast < int > ( xpos + frontBarWidth / 2.0 );
                                        QRect rect( ourClipRect );
                                        rect.setHeight( static_cast<int>( rect.height() + 3.0 * delta ) );
                                        painter->setClipRect( rect );
                                        if( bDrawExtraLines ){
                                            drawExtraLinesAndMarkers(
                                                propSet,
                                                defaultPen,
                                                defaultMarkerStyle,
                                                xm, static_cast<int>( yZero + height1 ),
                                                painter,
                                                ai.abscissaPara,
                                                ordinatePara,
                                                areaWidthP1000,
                                                logHeight/1000.0,
                                                bDrawExtraLinesInFront );
                                        }else if( bShowThisBar ){
                                            if( params()->drawSolidExcessArrows() ) {
                                                // Draw solid excess arrows
                                                QPointArray points( 5 );
                                                points.setPoint( 0, frontX1, static_cast<int>( yZero ) );
                                                points.setPoint( 1, frontX2, static_cast<int>( yZero ) );
                                                points.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta + 2 * yArrowGap ) );
                                                points.setPoint( 3, xm, static_cast<int>( yZero + height1 + 2 * yArrowGap ) );
                                                points.setPoint( 4, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta + 2 * yArrowGap ) );
                                                painter->drawPolygon( points );

                                                // Don't use points for drawing after this!
                                                if( region ) {
                                                    points.translate( _dataRect.x(), _dataRect.y() );
                                                    *region += QRegion( points );
                                                }
                                            } else {
                                                // Draw split excess arrows
                                                QPointArray points( 5 );
                                                points.setPoint( 0, frontX1, static_cast<int>( yZero ) );
                                                points.setPoint( 1, frontX2, static_cast<int>( yZero ) );
                                                points.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points.setPoint( 3, xm, static_cast<int>( yZero + height1 ) );
                                                points.setPoint( 4, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                painter->drawPolygon( points );
                                                // Don't use points for drawing after this!
                                                if ( region ) {
                                                    points.translate( _dataRect.x(), _dataRect.y() );
                                                    *region += QRegion( points );
                                                }

                                                QPointArray points2( 6 );
                                                points2.setPoint( 0, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points2.setPoint( 1, xm, static_cast<int>( yZero + height1 ) );
                                                points2.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points2.setPoint( 3, frontX2, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                                points2.setPoint( 4, xm, static_cast<int>( yZero + height1 - 0.75 * delta ) );
                                                points2.setPoint( 5, frontX1, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                                points2.translate( 0, yArrowGap );
                                                painter->drawPolygon( points2 );
                                                if ( region ) {
                                                    QPointArray points2cpy( points2 );
                                                    points2cpy.detach();
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
                                            }
                                        }
                                        painter->setClipRect( ourClipRect );
                                    } else {
                                        //
                                        // old code (sometimes not touching the grid):
                                        // QRect rec( xpos, yZero, frontBarWidth, -barHeight );
                                        // painter->drawRect( rec );
                                        //
                                        //bool fromBottom = bNormalMode && !_bThreeDBars;
                                        double pt1Y = yZero - barHeight;
                                        frontX1 = static_cast<int>( xpos );
                                        frontX2 = static_cast < int > ( xpos + frontBarWidth );
                                        QPoint pt1( frontX1,
                                                    static_cast < int > ( pt1Y ) );
                                        QPoint pt2( frontX2,
                                                    static_cast < int > ( yZero ) );
                                        if( pt2.y() < pt1Y ) {
                                            pt1.setY( pt2.y() );
                                            pt2.setY( static_cast<int>( pt1Y ) );
                                        }
                                        if( pt2.x() < frontX1 ) {
                                            pt1.setX( frontX2 );
                                            pt2.setX( frontX1 );
                                        }
                                        if( bDrawExtraLines ){
                                            int y = pt2.y();
                                            if( _bThreeDBars )
                                                y -= static_cast < int > ( sideBarHeight );
                                            drawExtraLinesAndMarkers(
                                                propSet,
                                                defaultPen,
                                                defaultMarkerStyle,
                                                (frontX1+frontX2)/2, y,
                                                painter,
                                                ai.abscissaPara,
                                                ordinatePara,
                                                areaWidthP1000,
                                                logHeight/1000.0,
                                                bDrawExtraLinesInFront );
                                        }else if( bShowThisBar ){
                                            QSize siz( pt2.x() - pt1.x() + 1,
                                                       pt2.y() - pt1.y() );
                                            QRect rect( pt1, siz );
                                            painter->drawRect( rect );

                                            // Don't use rect for drawing after this!
                                            if ( region ) {
                                                rect.moveBy( _dataRect.x(), _dataRect.y() );
                                                *region += QRegion( rect );
                                            }
                                        }
                                    }
                                } else {
                                    //
                                    //  Positive values:
                                    //
                                    double yZero = yposPositives - zeroXAxisI;
                                    bool tooHigh = (barHeight - yZero) > 1.01;
                                    if ( tooHigh && bNormalMode ) {
                                        double delta   = -0.0125 * logHeight;
                                        double height  = -1.0 * static_cast < int > ( yZero )
                                                         -2.0 * delta;
                                        double height1 = height + -3.0 * delta;
                                        int yArrowGap = static_cast < int > ( 2.5 * delta );
                                        frontX1 = static_cast<int>( xpos );
                                        frontX2 = static_cast < int > ( xpos + frontBarWidth );
                                        int xm = static_cast < int > ( xpos + frontBarWidth / 2.0 );
                                        QRect rect( ourClipRect );
                                        rect.setTop( static_cast<int>( rect.top() + 3 * delta ) );
                                        rect.setHeight( static_cast<int>( rect.height() - 3 * delta ) );
                                        painter->setClipRect( rect );
                                        if( bDrawExtraLines ){
                                            drawExtraLinesAndMarkers(
                                                propSet,
                                                defaultPen,
                                                defaultMarkerStyle,
                                                xm, static_cast<int>( yZero + height1 ),
                                                painter,
                                                ai.abscissaPara,
                                                ordinatePara,
                                                areaWidthP1000,
                                                logHeight/1000.0,
                                                bDrawExtraLinesInFront );
                                        }else if( bShowThisBar ){
                                            if( params()->drawSolidExcessArrows() ) {
                                                // Draw solid excess arrows
                                                QPointArray points( 5 );
                                                points.setPoint( 0, frontX1, static_cast<int>( yZero ) );
                                                points.setPoint( 1, frontX2, static_cast<int>( yZero ) );
                                                points.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta + 2 * yArrowGap ) );
                                                points.setPoint( 3, xm, static_cast<int>( yZero + height1 + 2 * yArrowGap ) );
                                                points.setPoint( 4, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta + 2 * yArrowGap ) );
                                                painter->drawPolygon( points );

                                                // Don't use points for drawing after this!
                                                if( region ) {
                                                    points.translate( _dataRect.x(), _dataRect.y() );
                                                    *region += QRegion( points );
                                                }
                                            } else {
                                                // Draw split excess arrows (default)
                                                QPointArray points( 5 );
                                                points.setPoint( 0, frontX1, static_cast<int>( yZero ) );
                                                points.setPoint( 1, frontX2, static_cast<int>( yZero ) );
                                                points.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points.setPoint( 3, xm, static_cast<int>( yZero + height1 ) );
                                                points.setPoint( 4, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                painter->drawPolygon( points );

                                                // Don't use points for drawing after this!
                                                if ( region ) {
                                                    points.translate( _dataRect.x(), _dataRect.y() );
                                                    *region += QRegion( points );
                                                }

                                                QPointArray points2( 6 );
                                                points2.setPoint( 0, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points2.setPoint( 1, xm, static_cast<int>( yZero + height1 ) );
                                                points2.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points2.setPoint( 3, frontX2, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                                points2.setPoint( 4, xm, static_cast<int>( yZero + height1 - 0.75 * delta ) );
                                                points2.setPoint( 5, frontX1, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                                points2.translate( 0, yArrowGap );
                                                painter->drawPolygon( points2 );
                                                if ( region ) {
                                                    QPointArray points2cpy( points2 );
                                                    points2cpy.detach();
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
                                            }
                                        }
                                        painter->setClipRect( ourClipRect );
                                    } else {
                                        //bool fromBottom = bNormalMode && !_bThreeDBars;
                                        double y0 = yposPositives - zeroXAxisI;
                                        double pt1Y =   y0
                                                        - barHeight;
                                        frontX1 = static_cast<int>( xpos );
                                        frontX2 = static_cast < int > ( xpos + frontBarWidth );
                                        QPoint pt1( frontX1,
                                                    static_cast < int > ( pt1Y ) );
                                        QPoint pt2( frontX2,
                                                    static_cast < int >
                                                    ( y0 + shiftUpperBars ) );
                                        if( pt2.y() < pt1Y ) {
                                            pt1.setY( pt2.y() );
                                            pt2.setY( static_cast<int>( pt1Y ) );
                                        }
                                        if( pt2.x() < frontX1 ) {
                                            pt1.setX( frontX2 );
                                            pt2.setX( frontX1 );
                                        }
                                        if( bDrawExtraLines ){
                                            int y = pt1.y();
                                            if( _bThreeDBars )
                                                y -= static_cast < int > ( sideBarHeight );
                                            drawExtraLinesAndMarkers(
                                                propSet,
                                                defaultPen,
                                                defaultMarkerStyle,
                                                (frontX1+frontX2)/2, y,
                                                painter,
                                                ai.abscissaPara,
                                                ordinatePara,
                                                areaWidthP1000,
                                                logHeight/1000.0,
                                                bDrawExtraLinesInFront );
                                        }else if( bShowThisBar ){
                                            QSize siz( pt2.x() - pt1.x() + 1,
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
                                }

                                if ( bShowThisBar && _bThreeDBars &&  !bDrawExtraLines ) {
                                    const int maxY = 2*devRect.height();
                                    // drawing the right side
                                    QPointArray points( 4 );
                                    if ( barHeight < 0 ) {
                                        points.setPoint( 0,
                                                         frontX2-1,
                                                         QMIN( static_cast<int>( yposNegatives - zeroXAxisI ),
                                                               maxY ) );
                                        points.setPoint( 1,
                                                         frontX2-1,
                                                         QMIN( static_cast<int>( yposNegatives - zeroXAxisI - barHeight ),
                                                               maxY ) );
                                        points.setPoint( 2,
                                                         static_cast<int>( frontX1 + totalThreeDBarWidth ),
                                                         QMIN( static_cast<int>( yposNegatives - zeroXAxisI - barHeight - sideBarHeight ),
                                                               maxY ) );
                                        points.setPoint( 3,
                                                         static_cast<int>( frontX1 + totalThreeDBarWidth ),
                                                         QMIN( static_cast<int>( yposNegatives - zeroXAxisI - sideBarHeight ),
                                                               maxY ) );
                                    } else {
                                        //qDebug("yposPositives: %f", yposPositives);
                                        //qDebug("zeroXAxisI:    %f", zeroXAxisI);
                                        //qDebug("barHeight:     %f", barHeight);
                                        //qDebug("sideBarHeight: %f", sideBarHeight);
                                        //qDebug("totalThreeDBarWidth: %f", totalThreeDBarWidth);
                                        points.setPoint( 0,
                                                         frontX2-1,
                                                         QMAX( static_cast<int>( yposPositives - zeroXAxisI ),
                                                               0 ) );
                                        points.setPoint( 1,
                                                         frontX2-1,
                                                         QMAX( static_cast<int>( yposPositives - zeroXAxisI - barHeight ),
                                                               0 ) );
                                        points.setPoint( 2,
                                                         static_cast<int>( frontX1 + totalThreeDBarWidth ),
                                                         QMAX( static_cast<int>( yposPositives - zeroXAxisI - barHeight - sideBarHeight ),
                                                               0 ) );
                                        points.setPoint( 3,
                                                         static_cast<int>( frontX1 + totalThreeDBarWidth ),
                                                         QMAX( static_cast<int>( yposPositives - zeroXAxisI - sideBarHeight ),
                                                               0 ) );
                                    }

                                    if ( myShadow2Color.isValid() )
                                        painter->setBrush( QBrush( myShadow2Color, params()->shadowPattern() ) );
                                    else
                                        painter->setBrush( NoBrush );
                                    painter->drawPolygon( points );
                                    if ( region ) {
                                        //qDebug("g");
                                        QPointArray points2cpy( points.copy() );
                                        //qDebug("g2");
                                        points2cpy.translate( _dataRect.x(), _dataRect.y() );
                                        //qDebug("dataRect XY: %i / %i",_dataRect.x(), _dataRect.y());
                                        //qDebug("g3");
                                        *region += QRegion( points2cpy );
                                        //qDebug("g4");
                                    }

                                    // drawing the top, but only for the topmost piece for stacked and percent
                                    if ( bNormalMode || bMultiRows ||
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
                                            points.setPoint( 0, frontX1,
                                                             static_cast<int>( yposNegatives - zeroXAxisI ) );
                                            points.setPoint( 1, static_cast<int>( frontX1 + sideBarWidth ),
                                                             static_cast<int>( yposNegatives - zeroXAxisI - sideBarHeight ) );
                                            points.setPoint( 2, static_cast<int>( frontX1 + totalThreeDBarWidth - 1 ),
                                                             static_cast<int>( yposNegatives - zeroXAxisI - sideBarHeight ) );
                                            points.setPoint( 3, frontX2-1,
                                                             static_cast<int>( yposNegatives - zeroXAxisI ) );
                                        } else {
                                            points.setPoint( 0, frontX1,
                                                             static_cast<int>( yposPositives - zeroXAxisI - barHeight ) );
                                            points.setPoint( 1, static_cast<int>( frontX1 + sideBarWidth ),
                                                             static_cast<int>( yposPositives - zeroXAxisI - barHeight - sideBarHeight ) );
                                            points.setPoint( 2, static_cast<int>( frontX1 + totalThreeDBarWidth - 1 ),
                                                             static_cast<int>( yposPositives - zeroXAxisI - barHeight - sideBarHeight ) );
                                            points.setPoint( 3, frontX2-1,
                                                             static_cast<int>( yposPositives - zeroXAxisI - barHeight ) );
                                        }
                                        if ( barHeight < 0 )
                                            painter->setBrush( bMultiRows ? myBarColor : black );
                                        else
                                            painter->setBrush( QBrush( myShadow1Color, params()->shadowPattern() ) );
                                        if ( !myShadow1Color.isValid() )
                                            painter->setBrush( NoBrush ); // override prev. setting
                                        painter->drawPolygon( points );
                                        // Don't use points for drawing after this!
                                        if ( region ) {
                                            points.translate( _dataRect.x(), _dataRect.y() );
                                            *region += QRegion( points );
                                        }
                                    }
                                }//if ( _bThreeDBars )

                                if ( regions && region ) {
                                    if( bShowThisBar && !bDrawExtraLines )
                                        regions->append( new KDChartDataRegion( *region, dataset, value, chart ) );
                                    delete region;
                                }

                            }// if( !bDrawExtraLines || bCellPropertiesFound )
                        }// if( !skipMe )
                    }// if( dataset >= datasetStart && dataset <= datasetEnd )


                    // Vertical advancement in stacked or percent only if there was a value
                    if ( params()->barChartSubType() == KDChartParams::BarStacked ||
                         params()->barChartSubType() == KDChartParams::BarPercent )
                        if ( barHeight < 0.0 )
                            yposNegatives -= barHeight;
                        else
                            yposPositives -= barHeight;

                } // no else, we simply do not paint if there is nothing to paint


                // advance only if the next dataset has DataEntry mode
                bool bAdvanceToNextValue =
                    (    bMultiRows ? (dataset == chartDatasetStart) : (dataset == chartDatasetEnd)
                         || (    params()->chartSourceMode( bMultiRows ? dataset-1 : dataset+1 )
                                 == KDChartParams::DataEntry ) );
                // Advance to next value; only for normal bars
                if ( bNormalMode ) {
                    if( bAdvanceToNextValue )
                        xpos += barWidth;
                    // skip gap between datasets, unless last dataset
                    if ( dataset < myLastDataEntryDataset )
                        xpos += datasetGap;
                }
                if( bAdvanceToNextValue || bMultiRows  ){
                    //qDebug("shiftMyPainter( -nShiftX, nShiftY );");
                    shiftMyPainter( -nShiftX, nShiftY );
                }
            }


            // Advancement between value blocks
            if ( bNormalMode )
                // skip gap between value blocks, don't worry about last one here
                xpos += valueBlockGap;
            else {
                // skip gap between value blocks
                xpos += valueBlockGap + barWidth;
                // start at bottom with next value group
                yposPositives = yposPositivesStart;
                yposNegatives = yposNegativesStart;
            }
            //qDebug("shiftMyPainterBack");
            shiftMyPainterBack();
        }
    }


    if( bMultiRows )
        painter->setClipping( bHadClipping );
}
