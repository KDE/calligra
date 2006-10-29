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

    const bool bThreeDBars = params()->threeDBars() || (KDChartParams::BarMultiRows == params()->barChartSubType());
    return   ( normalMode && !bThreeDBars )
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


void KDChartBarPainter::calculateXFront1_2( bool bNormalMode, bool bIsVeryFirstBar, bool bIsFirstDataset, bool _bThreeDBars,
                                            double xpos, double valueBlockGap, double datasetGap, double frontBarWidth,
                                            int& frontX1, int& frontX2, int& prevFrontX2 ){

    if( _bThreeDBars || bIsVeryFirstBar || !bNormalMode )
        frontX1 = static_cast<int>( xpos );
    else if( bIsFirstDataset )
        frontX1 = prevFrontX2 +  static_cast <int> ( valueBlockGap ) + 1;
    else
        frontX1 = prevFrontX2 + static_cast < int > ( datasetGap ) + 1;

    // Note: frontX2 is calculated by using xpos but NOT by using frontX1.
    //       So we make sure that (a) the gaps between all bars of one dataset
    //       are exactly the same size, and that (b) the bars are automatically
    //       adjusted in their width: to match their very position within the abscissa range.
    //       Adjusting will result in them being ONE pixel wider sometimes.
    frontX2 = static_cast < int > ( xpos + frontBarWidth );
    prevFrontX2 = frontX2;
}


QPointArray rectToPointArray( const QRect& r )
{
    QPointArray a(4);
    a.putPoints( 0, 4, r.left(),r.top(), r.right(),r.top(), r.right(),r.bottom(), r.left(),r.bottom() );
    return a;
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
    /****************Pending Michel***********************
     *Rectangles are painted in the following order:     *
     *front , right side, top.                           *
     *Store the painted rectangles in order to paint     *
     *them in relation to each other for more precision. *
     ****************************************************/
    QRect frontRectPositive,frontRectNegative,
        rightRectPositive, rightRectNegative,
        excessRectPositive,excessRectNegative;

    _areaP1000 = areaWidthP1000;



    if( !data ) return;
    // qDebug("datasetStart: %i    datasetEnd: %i", datasetStart, datasetEnd);
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

    //this allow the height/size of the bar to be painted fully
    if( bMultiRows ) {
        painter->setClipping( false );
    }
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
                        * (   params()->datasetGapIsRelative()
                              ? areaWidthP1000
                              : 1.0 );
    double valueBlockGap = bMultiRows
                           ? 0.0
                           : params()->valueBlockGap()
                           * (   params()->valueBlockGapIsRelative()
                                 ? areaWidthP1000
                                 : 1.0 );

    // This is the same for all three bar types except for multi-bar Surface charts.
    double spaceBetweenValueBlocks = bMultiRows
                                     ? 0.0
                                     : static_cast<int>( valueBlockGap ) * numValues;

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

    double barWidth = 0.0;
    if( params()->barWidth() == KDCHART_AUTO_SIZE ) {

        //Default auto calc. to optimize when many bars datasetGap ==  blockGap/2 == barWidth/2
        //unless valueBlockGap or datasetGap are different from default values == users value
        barWidth = logWidth / totalNumberOfBars;
        do {
            barWidth -= 0.1;
            params()->valueBlockGap() != 24 ? spaceBetweenValueBlocks = params()->valueBlockGap() * numValues
                                            : spaceBetweenValueBlocks = ( barWidth/2 ) * numValues;
            if  ( spaceBetweenDatasets != 0 )
                params()->datasetGap()!= 6 ? spaceBetweenDatasets = params()->datasetGap() * ( totalNumberOfBars - numValues )
                                           : spaceBetweenDatasets = ( barWidth/4 ) * ( totalNumberOfBars - numValues );
        }
        while (  barWidth*totalNumberOfBars + spaceBetweenValueBlocks + spaceBetweenDatasets > logWidth );

        valueBlockGap = ( spaceBetweenValueBlocks )/ numValues;
        if ( spaceBetweenDatasets != 0 )
            datasetGap = spaceBetweenDatasets /  ( totalNumberOfBars - numValues );

        barWidth = (logWidth - ( valueBlockGap * numValues ) - ( datasetGap * ( totalNumberOfBars - numValues ) ) ) / totalNumberOfBars;
        ;

        /* Debug space available
           qDebug (  " logWidth %f" ,  logWidth );
           qDebug (  "compare value %f",  barWidth*totalNumberOfBars + spaceBetweenValueBlocks + datasetGap * ( totalNumberOfBars - numValues ) );
        */
    } else if( 0 > params()->barWidth() )
        barWidth = params()->barWidth() * -areaWidthP1000;
    else
        barWidth = params()->barWidth();

    //sideBarWidth: pre-calculate in order to have a reference
    //we will correct according to the available space later.
    //This in order to center the labels in relation to the front width
    //when painting 3DBars
    double sideBarWidth = _bThreeDBars
                          ? ( barWidth - barWidth / (1.0 + params()->cosThreeDBarAngle()) ) *
                          params()->threeDBarDepth()
                          : 0.0;

    const double frontBarWidth = _bThreeDBars && !bMultiRows
                                 ? barWidth - sideBarWidth
                                 : barWidth;

    //correct the width for the side of the bar
    double totalThreeDBarWidth = totalNumberOfBars*barWidth + sideBarWidth;
    double totalSpaceOccupied = totalThreeDBarWidth + spaceBetweenDatasets + spaceBetweenValueBlocks;
    if ( logWidth < totalSpaceOccupied) {
        sideBarWidth -= (totalSpaceOccupied /*+  spaceBetweenDatasets + spaceBetweenValueBlocks*/ - logWidth)/totalNumberOfBars;

        //qDebug ( "logWidth %f", logWidth );
        //qDebug ( "totalSpaceOccupied %f", totalSpaceOccupied );
    }

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
        zeroXAxisI = logHeight
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

        double xpos = 0.0;

        if ( _bThreeDBars && !bMultiRows  )
            xpos = 0.0 + (barWidth/2) + (valueBlockGap/2) - (frontBarWidth/2);
        else
            xpos = 0.0 + (valueBlockGap / 2.0);

        double yposPositives = yposPositivesStart;
        double yposNegatives = yposNegativesStart;

        /* Pending Michel: no need for this anymore */
        //double totalThreeDBarWidth = bMultiRows
        //? barWidth + sideBarWidth
        //	: barWidth;

        double nShiftX = bMultiRows
                         ? sideBarWidth
                         : 0.0;
        double nShiftY = bMultiRows
                         ? sideBarHeight
                         : 0.0;

        double valueTotal = 0.0; // valueTotal is used for percent bars only

        // iterate over all columns: item1, item2, item3 ...
        int prevFrontX2 = 0;
        bool bIsVeryFirstBar = true;
        for ( int value = 0; value < numValues; ++value ) {

            bool bFirstValidValueUnknown = true;
            uint firstValidValue = 0;
            uint lastValidPositiveValue  = 0;
            double maxValueInThisColumn = 0.0, minValueInThisColumn = 0.0;
            if ( params()->barChartSubType() == KDChartParams::BarStacked ||
                 params()->barChartSubType() == KDChartParams::BarPercent) {
                valueTotal = 0.0;
                // iterate over datasets of this axis only:
                for ( uint dataset  = datasetStart;
                      dataset <= datasetEnd;
                      ++dataset ) {

                    QVariant vVal;
                    if( data->cellCoord( dataset, value, vVal, 1 )
                        && params()->chartSourceMode( dataset ) == KDChartParams::DataEntry
                        && QVariant::Double == vVal.type() ){

                        const double cellValue
                            = ordinateIsLogarithmic
                            ? log10( vVal.toDouble() )
                            : vVal.toDouble();
                        //qDebug("value   %u    dataset   %u   logHeight %f", value,dataset,logHeight);

                        if( bFirstValidValueUnknown ){
                            firstValidValue = dataset;
                            bFirstValidValueUnknown = false;
                        }
                        if( 0.0 <= cellValue )
                            lastValidPositiveValue = dataset;

                        maxValueInThisColumn = QMAX( maxValueInThisColumn, cellValue );
                        minValueInThisColumn = QMIN( minValueInThisColumn, cellValue );
                        if( params()->barChartSubType() == KDChartParams::BarPercent /*||
                                                                                       params()->barChartSubType() == KDChartParams::BarStacked*/ )
                            valueTotal += cellValue;
                    }
                }
            }

            //qDebug("shiftMyPainter( (numChartDataEntryDatasets-1)*nShiftX, (numChartDataEntryDatasets-1)*-nShiftY );");
            shiftMyPainter( (numChartDataEntryDatasets-1)*nShiftX, (numChartDataEntryDatasets-1)*-nShiftY );

            // iterate over all datasets of this chart:
            // (but draw only the bars of this axis)
            bool bIsFirstDataset = true;
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


                QVariant coord1;
                QVariant coord2;
                int propID;
                if( data->cellContent( dataset, value, coord1, coord2, propID )
                    && QVariant::Double == coord1.type() ){

                    const double cellValue
                        = ordinateIsLogarithmic
                        ? log10( coord1.toDouble() )
                        : coord1.toDouble();
                    //qDebug("b");
                    // there is a numeric value

                    double barHeight;

                    if ( params()->barChartSubType() == KDChartParams::BarPercent )
                        barHeight = ( cellValue / valueTotal ) * fabs(zeroXAxisI - logHeight );
                    else {
                        barHeight = pixelsPerUnit * cellValue;

                        if( 0.0 <= barHeight )
                            //barHeight = QMAX(0.0, barHeight - sideBarHeight);
                            barHeight = barHeight - sideBarHeight;
                        else
                            barHeight -= sideBarHeight;
                    }

                    // This little adjustment avoids a crash when trying
                    // to retrieve the bounding rect of a zero size region...
                    if( 0 == barHeight || 0.0 == barHeight ) {

                        barHeight = 1.0; // makes sense to have a min size anyway
                    }

                    // draw only the bars belonging to the axis
                    // which we are processing currently
                    if( dataset >= datasetStart && dataset <= datasetEnd ) {
                        //qDebug("b2");

                        // calculate Abscissa axis value, if there are X coordinates
                        // ---------------------------------------------------------
                        bool skipMe = false;
                        if( ai.bCellsHaveSeveralCoordinates ){
                            skipMe = !calculateAbscissaAxisValue( coord2,
                                                                  ai, 0, xpos );
                            // adjust bar position to have it horizontally centered to the point
                            if( ai.bAbscissaHasTrueAxisDtValues &&
                                QVariant::DateTime == coord2.type() )
                                xpos -= frontBarWidth / 2.0;
                        }

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
                                                     params()->lineStyle( dataset ) );

                            // --------------------------------------------------------
                            // determine any 'extra' properties assigned to this cell
                            // by traversing the property set chain (if necessary)
                            // --------------------------------------------------------
                            KDChartPropertySet propSet;
                            bool bCellPropertiesFound =
                                params()->calculateProperties( propID,
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

                                //QRegion* region = 0;
                                KDChartDataRegion::PointArrayList * pointArrayList = 0;
                                if( bDataEntrySourceMode && !bDrawExtraLines ){
                                    painter->setPen( defaultOutlinePen );
                                    if ( myBarColor.isValid() )
                                        painter->setBrush( myBarColor );
                                    else
                                        painter->setBrush( NoBrush );

                                    // Prepare region for detection of mouse clicks
                                    // and for finding anchor positions of data value texts
                                    if ( regions )
                                        pointArrayList = new KDChartDataRegion::PointArrayList();
                                }

                                // Start drawing
                                int frontX1 = 0;
                                int frontX2 = 0;
                                bool tooLow = false;
                                bool tooHigh = false;

                                if ( cellValue < 0 || cellValue < minColumnValue ) {
                                    double maxValueYPos = maxColumnValue * pixelsPerUnit;
                                    double yZero = yposNegatives - zeroXAxisI - sideBarHeight;

                                    tooLow = (barHeight - yZero + logHeight + axisYOffset) < 0;

                                    /*we need to handle those values in case we have a negative view*/
                                    if ( cellValue <= 0 && cellValue > maxColumnValue && minColumnValue < 0 && maxColumnValue < 0 )
                                        tooLow = true;

                                    if ( tooLow && ( bNormalMode || bMultiRows) ) {
                                        double delta   = 0.0125 * logHeight;
                                        double height  = -1*(-1.0 * (yZero  + sideBarHeight) - 2 * delta);
                                        double height1 = height - 3.0 * delta;

                                        int yArrowGap = static_cast < int > ( 2.5 * delta );
                                        calculateXFront1_2( bNormalMode, bIsVeryFirstBar, bIsFirstDataset, _bThreeDBars,
                                                            xpos, valueBlockGap, datasetGap, frontBarWidth,
                                                            frontX1, frontX2, prevFrontX2 );

                                        const int xm = static_cast < int > ( (frontX1 + frontX2) / 2.0 );
                                        QRect rect( ourClipRect );

                                        rect.setHeight( static_cast<int>( rect.height() + 3.0 * delta ) );
                                        painter->setClipRect( rect );

                                        //Pending Michel: Make sure the point of the arrow is always at the same distance
                                        //from the X axis reference to the point of the arrow.
                                        int arrowXAxisGap;
                                        QPoint arrowTop(  xm,static_cast<int>( yZero + height1 + 2 * yArrowGap) );

                                        if ( arrowTop.y()== yposNegatives )
                                            arrowXAxisGap = -2;
                                        else
                                            arrowXAxisGap = static_cast <int> (yposNegatives - arrowTop.y() - 2);

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

                                                /* PENDING Michel:
                                                 * Here we have two situations.
                                                 * The value is too low because over the Min negative value
                                                 * or it is not within the configured view.
                                                 */
                                                // Draw solid excess arrows negatives

                                                QPointArray points( 5 );
                                                /*this works in a positive view -> 200 500*/
                                                points.setPoint( 0, frontX1, cellValue < 0  ?
                                                                               static_cast<int>(yZero+height1 - 3.0*delta)+(2*yArrowGap)+ arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2*yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 1, frontX2, cellValue < 0 ?
                                                                              static_cast<int>(yZero+height1 - 3.0*delta)+(2*yArrowGap)+ arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2*yArrowGap)+ arrowXAxisGap );
                                                points.setPoint( 2, frontX2, cellValue < 0 ?
                                                                               static_cast<int>(yZero+height1 - 3.0*delta)+(2*yArrowGap)+ arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2*yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 3, xm, cellValue < 0 ?
                                                                          static_cast<int>( yZero + height1)+(2*yArrowGap)+ arrowXAxisGap:
                                                                 static_cast<int>(yZero+height1 - 3.0*delta)+(2*yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 4, frontX1, cellValue < 0  ?
                                                                               static_cast<int>(yZero+height1 - 3.0*delta)+(2*yArrowGap)+ arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2*yArrowGap)+ arrowXAxisGap);

                                                /* 0 between start and end -> -500 500*/
                                                if ( minColumnValue < 0 && maxColumnValue > 0 ) {
                                                    points.setPoint(0, points.point(0).x(), static_cast <int> (yposNegatives - zeroXAxisI) );
                                                    points.setPoint(1, points.point(1).x(), static_cast <int> (yposNegatives - zeroXAxisI) );
                                                }

                                                /* negative view -> -200 -500 */
                                                if ( minColumnValue < 0 && maxColumnValue < 0 ) {
                                                    /*value negative or zero > maxColumnValue*/
                                                    if ( cellValue > maxColumnValue ) {
                                                        // the view is under Yaxis 0 level
                                                        // we need to show a symbol for the bars which are over the Yaxis.
                                                        // calculate the coordinate and direction for the arrow.
                                                        // arrow downward for negative values and upward for positives value
                                                        int diffArrowBase = points.point(2).y() - points.point(3).y();
                                                        double maxValueYPos = maxColumnValue * pixelsPerUnit;
                                                        double minValueYPos = minColumnValue * pixelsPerUnit;
                                                        double adjustedArrow = (cellValue == 0 ? minValueYPos - maxValueYPos + diffArrowBase + 2:
                                                                                minValueYPos - maxValueYPos - diffArrowBase + 1);
                                                        points.setPoint( 0, frontX1, points.point(0).y() + static_cast <int> (adjustedArrow));
                                                        points.setPoint( 1, frontX2, points.point(1).y() + static_cast <int> (adjustedArrow ));
                                                        points.setPoint( 2, frontX2, points.point(2).y() + static_cast <int> (adjustedArrow));
                                                        points.setPoint( 3, xm, points.point(3).y() + static_cast <int> (adjustedArrow));
                                                        points.setPoint( 4, frontX1, points.point(4).y() + static_cast <int> (adjustedArrow));
                                                    }
                                                    /*value < mincolumn value*/
                                                    if ( cellValue < minColumnValue ) {
                                                        points.setPoint( 0, frontX1, static_cast <int> ( yZero - maxValueYPos ) );
                                                        points.setPoint( 1, frontX2, static_cast <int> ( yZero - maxValueYPos ) );
                                                    }

                                                }

                                                /*Pending Michel: case setbarWidth */
                                                //adjust the painting in case we have a user given Width allow it
                                                //to be larger than the auto calculated width in case we want to overlap

                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    QRect tmpRect ( points.point(0), points.point(2));
                                                    points.setPoint(0, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(0).y());
                                                    points.setPoint(1, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(1).y());
                                                    points.setPoint(2, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(2).y());
                                                    points.setPoint(4, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(4).y());
                                                    //}
                                                }
                                                if ( bMultiRows )painter->setClipping(  false );
                                                painter->drawPolygon( points );

                                                //correct the y position: displaying position for the value label
                                                  QPoint tpLeft (points.point(4).x(), points.point(4).y() - 2 * yArrowGap );
                                                  QPoint tpRight(points.point(2).x(), points.point(2).y() - 2 * yArrowGap );

                                                //store the front rectangle
                                                excessRectNegative.setTopLeft(tpLeft);
                                                excessRectNegative.setTopRight(tpRight);
                                                excessRectNegative.setBottomRight(points.point(1));
                                                excessRectNegative.setBottomLeft(points.point(0));

                                                // Don't use points for drawing after this!
                                                if ( pointArrayList ) {
                                                    if (  cellValue < 0 ) {
                                                        //correction for labels vertical position
                                                        int size = static_cast <int> ( ( points.point( 3 ).y() - tpRight.y() + excessRectNegative.width() )/2 );
                                                        points.setPoint( 4 ,tpLeft );
                                                        points.setPoint( 2, tpRight );
                                                        if (  cellValue < maxColumnValue )
                                                            points.translate( _dataRect.x(), -_dataRect.y() - size );
                                                        else
                                                            points.translate( _dataRect.x(), _dataRect.y() );
                                                    } else
                                                        points.translate(  _dataRect.x(),  _dataRect.y() );

                                                    pointArrayList->append( points );
                                                }

                                            } else {
                                                // Draw split excess arrows negatives

                                                /* PENDING Michel:
                                                 * Here we have two situations.
                                                 * The value is too low because over the Min negative value
                                                 * The value is not within the configured view..
                                                 */
                                                QPointArray points( 5 );
                                                /*this works in a positive view -> 200 500*/
                                                points.setPoint( 0, frontX1, cellValue < 0  ?
                                                                               static_cast<int>(yZero+height1 - 3.0*delta) + arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2 * yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 1, frontX2, cellValue < 0 ?
                                                                              static_cast<int>(yZero+height1 - 3.0*delta) + arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2 * yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 2, frontX2, cellValue < 0 ?
                                                                               static_cast<int>(yZero+height1 - 3.0*delta) + arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2 * yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 3, xm, cellValue < 0 ?
                                                                          static_cast<int>( yZero + height1) + arrowXAxisGap:
                                                                 static_cast<int>(yZero+height1 - 3.0*delta)+(2 * yArrowGap)+ arrowXAxisGap);
                                                points.setPoint( 4, frontX1, cellValue < 0  ?
                                                                 static_cast<int>(yZero+height1 - 3.0*delta) + arrowXAxisGap:
                                                                 static_cast<int>( yZero + height1)+(2 * yArrowGap)+ arrowXAxisGap);

                                                /* 0 between start and end -> -500 500*/
                                                if ( minColumnValue < 0 && maxColumnValue > 0 ) {
                                                    points.setPoint(0, points.point(0).x(), static_cast <int> (yposNegatives - zeroXAxisI) );
                                                    points.setPoint(1, points.point(1).x(), static_cast <int> (yposNegatives - zeroXAxisI) );
                                                }

                                                /* negative view -> -200 -500 */
                                                if ( minColumnValue < 0 && maxColumnValue < 0 ) {
                                                    /*value negative or zero > maxColumnValue*/
                                                    if ( cellValue > maxColumnValue ) {
                                                        // the view is under Yaxis 0 level
                                                        // we need to show a symbol for the bars which are over the Yaxis.
                                                        // calculate the coordinate and direction for the arrow.
                                                        // arrow downward for negative values and upward for positives value
                                                        int diffArrowBase = points.point(2).y() - points.point(3).y();
                                                        double maxValueYPos = maxColumnValue * pixelsPerUnit;

                                                        points.setPoint( 0, frontX1, static_cast <int> ( yZero - maxValueYPos) );
                                                        points.setPoint( 1, frontX2, static_cast <int> ( yZero - maxValueYPos) );
                                                        points.setPoint( 2, frontX2, static_cast <int> ( yZero - maxValueYPos) );
                                                        points.setPoint( 3, xm, static_cast <int> ( yZero - maxValueYPos - diffArrowBase ) );
                                                        points.setPoint( 4, frontX1, static_cast <int> ( yZero - maxValueYPos) );
                                                    }
                                                    /*value < mincolumn value*/
                                                    if ( cellValue < minColumnValue ) {
                                                        points.setPoint( 0, frontX1, static_cast <int> ( yZero - maxValueYPos) );
                                                        points.setPoint( 1, frontX2, static_cast <int> ( yZero - maxValueYPos) );
                                                    }
                                                }

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //allow it to be larger than the auto calculated width in case we want
                                                //to overlap
                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    QRect tmpRect ( points.point(0), points.point(2));
                                                    points.setPoint(0, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(0).y());
                                                    points.setPoint(1, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(1).y());
                                                    points.setPoint(2, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(2).y());
                                                    points.setPoint(4, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(4).y());
                                                }
                                                if ( bMultiRows )painter->setClipping(  false );
                                                 painter->drawPolygon( points );

                                                 //correct the y position: displaying position for the value label
                                                 QPoint tpLeft (points.point(4).x(), points.point(4).y() - 2 * yArrowGap );
                                                 QPoint tpRight(points.point(2).x(), points.point(2).y() - 2 * yArrowGap );

                                                //store the excess front rectangle
                                                excessRectNegative.setTopLeft(tpLeft);
                                                excessRectNegative.setTopRight(tpRight);
                                                excessRectNegative.setBottomRight(points.point(1));
                                                excessRectNegative.setBottomLeft(points.point(0));

                                                // Don't use points for drawing after this!
                                                if ( pointArrayList ) {
                                                    if (  cellValue < 0 ) {
                                                        //calculate correction for labels vertical position
                                                        int size = static_cast <int> ( ( points.point( 3 ).y() - tpRight.y() + excessRectNegative.width() )/2 );
                                                        if (  cellValue < maxColumnValue )
                                                        points.translate( _dataRect.x(), -_dataRect.y() - size );
                                                        else
                                                            points.translate( _dataRect.x(), _dataRect.y() + ( 2 * yArrowGap ) );
                                                    } else
                                                        points.translate(  _dataRect.x(),  -_dataRect.y() );

                                                    pointArrayList->append( points );
                                                }

                                                QPointArray points2( 6 );
                                                points2.setPoint( 0, frontX1, cellValue < 0 ?
                                                                                static_cast<int>( yZero + height1 - 3.0 * delta ) + arrowXAxisGap:
                                                                  static_cast<int>(yZero + height1) + arrowXAxisGap);
                                                points2.setPoint( 1, xm,      cellValue < 0 ?
                                                                  static_cast<int>(yZero + height1) + arrowXAxisGap:
                                                                  static_cast<int>( yZero + height1 - 3.0 * delta ) + arrowXAxisGap);
                                                points2.setPoint( 2, frontX2, cellValue < 0 ?
                                                                  static_cast<int>(yZero + height1 - 3.0 * delta) + arrowXAxisGap:
                                                                  static_cast<int>(yZero + height1) + arrowXAxisGap);
                                                points2.setPoint( 3, frontX2, cellValue < 0 ?
                                                                  static_cast<int>(yZero + height1 - 3.75 * delta) + arrowXAxisGap :
                                                                  static_cast<int>(yZero + height1 - 0.75 * delta)  + arrowXAxisGap);
                                                points2.setPoint( 4, xm,      cellValue < 0 ?
                                                                  static_cast<int>(yZero + height1 - 0.75 * delta)  + arrowXAxisGap:
                                                                  static_cast<int>(yZero + height1 - 3.75 * delta) + arrowXAxisGap);
                                                points2.setPoint( 5, frontX1, cellValue < 0 ?
                                                                  static_cast<int>(yZero + height1 - 3.75 * delta)  + arrowXAxisGap:
                                                                  static_cast<int>(yZero + height1 - 0.75 * delta)  + arrowXAxisGap);
                                                points2.translate( 0, yArrowGap );

                                                if ( minColumnValue < 0 && maxColumnValue < 0 &&  cellValue > maxColumnValue ) {
                                                    // the view is under Yaxis 0 level
                                                    // we need to show a symbol for the bars which are over the Yaxis.
                                                    // calculate the coordinate and direction for the arrow.
                                                    // arrow downward for negative values and upward for positives value
                                                    int diffArrowBase = points.point(2).y() - points.point(3).y();
                                                    double maxValueYPos = maxColumnValue * pixelsPerUnit;
                                                    double minValueYPos = minColumnValue * pixelsPerUnit;
                                                    double adjustedArrow = cellValue == 0 ? minValueYPos - maxValueYPos + diffArrowBase:
                                                                           minValueYPos - maxValueYPos - diffArrowBase + 1;

                                                    points2.setPoint( 0, frontX1, points2.point(0).y()
                                                                      + static_cast <int> (adjustedArrow - diffArrowBase + yArrowGap) );
                                                    points2.setPoint( 1, xm,  points2.point(1).y()
                                                                      + static_cast <int> (adjustedArrow - diffArrowBase + yArrowGap) );
                                                    points2.setPoint( 2, frontX2, points2.point(2).y()
                                                                      +  static_cast <int> (adjustedArrow - diffArrowBase + yArrowGap) );
                                                    points2.setPoint( 3, frontX2, points2.point(3).y()
                                                                      + static_cast <int> (adjustedArrow - diffArrowBase + yArrowGap) );
                                                    points2.setPoint( 4, xm, points2.point(4).y()
                                                                      + static_cast <int> (adjustedArrow - diffArrowBase + yArrowGap) );
                                                    points2.setPoint( 5, frontX1, points2.point(5).y()
                                                                      + static_cast <int> (adjustedArrow - diffArrowBase + yArrowGap) );
                                                }

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //allow it to be larger than the auto calculated width in case we want
                                                //to overlap
                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {

                                                    points2.setPoint(0, excessRectNegative.topLeft().x(),points2.point(0).y());
                                                    points2.setPoint(2, excessRectNegative.topRight().x(),points2.point(2).y());
                                                    points2.setPoint(3, excessRectNegative.topRight().x(),points2.point(3).y());
                                                    points2.setPoint(5, excessRectNegative.topLeft().x(),points2.point(5).y());
                                                }

                                                painter->drawPolygon( points2 );

                                                /*
                                                  NOTE:  We do NOT want to store the region here, but
                                                  we use the front rectangle above in order to display the
                                                  data value labels inside the front rectangle.
                                                  Disadvantage: clicking onto these excess arrows, will not
                                                  result in a signal being sent, because KDChartWidget will
                                                  not notice, that the user has clicked onto the bar.
                                                  That's a minor drawback, compared to the gain of being
                                                  able to position the labels correctly.
                                                */

                                                if ( cellValue < 0  )
                                                    points2.translate( 0, yArrowGap );
                                                else
                                                    points2.translate( 0, -yArrowGap );

                                                painter->drawPolygon( points2 );


                                                /*
                                                  NOTE:  We do NOT want to store the region here
                                                  (see comment above)
                                                */
                                            }
                                        }
                                        painter->setClipRect( ourClipRect );
                                    } /*if (tooLow && bNormalMode)*/
                                    else {
                                        //
                                        // old code (sometimes not touching the grid):
                                        //QRect rec( xpos, yZero, frontBarWidth, -barHeight );
                                        //painter->drawRect( rec );
                                        //

                                        int pt1Y = static_cast < int > ( yZero - barHeight /*- sideBarsHeight*/ );

                                        /*
                                          if ( cellValue != 0 ) {
                                          pt1Y = static_cast <int> (cellValue * pixelsPerUnit * -2);
                                          qDebug( "value %s",QString::number(static_cast <int> (cellValue)).latin1());
                                          qDebug( "pt1Y %s", QString::number(static_cast <int> (cellValue * pixelsPerUnit * -2)).latin1());
                                          }
                                          else
                                          pt1Y = static_cast < int > ( yZero - barHeight);
                                        */
                                        calculateXFront1_2( bNormalMode, bIsVeryFirstBar, bIsFirstDataset, _bThreeDBars,
                                                            xpos, valueBlockGap, datasetGap, frontBarWidth,
                                                            frontX1, frontX2, prevFrontX2 );

                                        QPoint pt1( frontX1, pt1Y );
                                        QPoint pt2( frontX2,
                                                    static_cast < int > ( yZero + sideBarHeight) );

                                        if( 0.0 > maxColumnValue ){
                                            pt2.setY(pt2.y() - static_cast < int > (pixelsPerUnit * maxColumnValue));
                                        }
                                        if( pt2.y() < pt1Y ) {
                                            pt1.setY( pt2.y() );
                                            pt2.setY( pt1Y );
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

                                            QSize siz( pt2.x() - pt1.x(),
                                                       pt2.y() - pt1.y() );
                                            QRect rect( pt1, siz );

                                            if( 1.5 > frontBarWidth ){
                                                QPen oldPen( painter->pen() );
                                                painter->setPen( QPen(painter->brush().color(), 0) );
                                                painter->drawLine(pt1, QPoint(pt1.x(),pt2.y()));
                                                painter->setPen( oldPen );
                                            }else{
                                                // store the front rect negative
                                                if ( tooLow || cellValue < minColumnValue) {
                                                    frontRectNegative.setTopLeft(excessRectNegative.bottomLeft());
                                                    frontRectNegative.setTopRight(excessRectNegative.bottomRight());
                                                    frontRectNegative.setBottomRight(excessRectNegative.topRight());
                                                    frontRectNegative.setBottomLeft(excessRectNegative.topLeft());

                                                } else {
                                                    frontRectNegative.setTopLeft(rect.topLeft());
                                                    frontRectNegative.setTopRight(rect.topRight());
                                                    frontRectNegative.setBottomRight(rect.bottomRight());
                                                    frontRectNegative.setBottomLeft(rect.bottomLeft());

                                                    if ( cellValue == 0 && params()->barChartSubType() == KDChartParams::BarPercent)
                                                        rect.setTop( rect.bottom());
                                                }

                                                //debug  lines and points
                                                /*
                                                  painter->drawLine( rect.topLeft(), rect.topRight());
                                                  painter->drawLine( rect.topRight(), rect.bottomRight());
                                                  painter->drawLine( rect.bottomRight(), rect.bottomLeft());
                                                  painter->drawLine( rect.bottomLeft(), rect.topLeft());

                                                  painter->drawText( frontRectNegative.topLeft(), "0f");
                                                  painter->drawText( frontRectNegative.topRight(), "1f");
                                                  painter->drawText( frontRectNegative.bottomRight(), "2f");
                                                  painter->drawText( frontRectNegative.bottomLeft(), "3f");
                                                */

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //and it is not larger than the auto calculated width

                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    rect.setLeft( static_cast <int> (rect.center().x() - (userwidth/2)));
                                                    rect.setRight( static_cast <int> (rect.center().x() + (userwidth/2)));
                                                    rect.setWidth( static_cast <int> (userwidth) );
                                                    frontRectNegative.setLeft( tooLow || cellValue < minColumnValue ?
                                                                               static_cast <int> (excessRectNegative.center().x() - (userwidth/2)):rect.left());
                                                    frontRectNegative.setRight( tooHigh ? static_cast <int> (excessRectNegative.center().x() + (userwidth/2)):rect.right());
                                                }
                                                //drawing the front size negative values
                                                if ( cellValue != 0 && params()->barChartSubType() != KDChartParams::BarPercent) {
                                                    painter->setClipping( false );
                                                    painter->drawRect( rect );
                                                }
                                            }
                                            // Don't use rect for drawing after this!
                                            if ( pointArrayList ) {
                                                rect.moveBy( _dataRect.x(), _dataRect.y() );
                                                pointArrayList->append( rectToPointArray( rect ) );
                                            }
                                        }
                                    }
                                } else {
                                    //
                                    //  Positive values:
                                    //
                                    /*Pending Michel: all values under the min value are handled as negative*/

                                    double maxValueYPos = maxColumnValue * pixelsPerUnit;
                                    double minValueYPos = minColumnValue * pixelsPerUnit;
                                    double minDataValueYPos = maxValueYPos - minValueYPos;
                                    double yZero = yposPositives - zeroXAxisI;
                                    //qDebug( "yposPositives %f - zeroXAxisI %f = %f" ,  yposPositives,  zeroXAxisI,  yZero );
                                    //qDebug( "yZero %s", QString::number( yZero ).latin1());
                                    //qDebug( "minDataValueYPos = %s",  QString::number( minDataValueYPos).latin1());
                                    //qDebug( "positive value %s",  QString::number( cellValue).latin1());
                                    //Pending Michel: draw the default split excess arrows
                                    //when the front top of the 3d chart reach the max Y value

                                    if(!_bThreeDBars)
                                        tooHigh =  ( barHeight > maxValueYPos*1.001 ) || ( cellValue < minColumnValue );
                                    else {
                                        //calculate the Y position for the top front line
                                        //if it is over the max height pos - tooHigh becomes true
                                        if ( params()->barChartSubType()!= KDChartParams::BarStacked ) {
                                            int dataValueYPos = static_cast <int>( ( cellValue * pixelsPerUnit ) );
                                            tooHigh = dataValueYPos > maxValueYPos;
                                        } else {
                                            tooHigh = maxValueInThisColumn > maxColumnValue;
                                        }
                                    }

                                    if ( tooHigh && bNormalMode ||
                                         tooHigh && params()->barChartSubType()== KDChartParams::BarStacked
                                         || tooHigh && bMultiRows ) {

                                        double delta   = -0.0125 * logHeight;
                                        double height  = -1.0 * yZero
                                                         - 2.0 * delta;
                                        double height1 = height + -3.0 * delta;

                                        int yArrowGap = static_cast < int > ( 2.5 * delta );
                                        calculateXFront1_2( bNormalMode, bIsVeryFirstBar, bIsFirstDataset, _bThreeDBars,
                                                            xpos, valueBlockGap, datasetGap, frontBarWidth,
                                                            frontX1, frontX2, prevFrontX2 );

                                        const int xm = static_cast < int > ( ( frontX1 + frontX2 ) / 2.0 );
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
                                                /*this works for positive config and 0 include config*/
                                                points.setPoint( 0, frontX1,
                                                                 (minDataValueYPos < static_cast <int> (yZero))?
                                                                 static_cast <int> (minDataValueYPos-1):static_cast <int>(yZero));
                                                points.setPoint( 1, frontX2,
                                                                 (minDataValueYPos < static_cast <int> (yZero))?
                                                                 static_cast <int> (minDataValueYPos-1):static_cast <int>(yZero));
                                                points.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta )
                                                                 + 2 * yArrowGap );
                                                points.setPoint( 3, xm,      static_cast<int>( yZero + height1 )
                                                                 + 2 * yArrowGap );
                                                points.setPoint( 4, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta )
                                                                 + 2 * yArrowGap );

                                                /*case where start and end value are negatives */
                                                if ( cellValue > maxColumnValue && 0 >= maxColumnValue ) {
                                                    points.setPoint( 0, frontX1,static_cast<int>( yZero + height1 - 3.0 * delta ) + 2 * yArrowGap);
                                                    points.setPoint( 1, frontX2,static_cast<int>( yZero + height1 - 3.0 * delta ) + 2 * yArrowGap);
                                                }

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //allow it to be larger than the auto calculated width in case we want
                                                //to overlap
                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    QRect tmpRect ( points.point(0), points.point(2));
                                                    points.setPoint(0, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(0).y());
                                                    points.setPoint(1, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(1).y());
                                                    points.setPoint(2, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(2).y());
                                                    points.setPoint(4, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(4).y());
                                                    //}
                                                }
                                                if ( params()->barChartSubType() != KDChartParams::BarStacked ||
                                                     params()->barChartSubType() == KDChartParams::BarStacked && dataset != datasetEnd ) {
                                                    //drawing a single front in case it is too high
                                                    painter->setClipping(  false );
                                                    painter->drawPolygon( points );
                                                }
                                                //correct the y position: displaying position for the value label
                                                QPoint tpLeft (points.point(4).x(), static_cast <int> (points.point(4).y() -  yArrowGap));
                                                QPoint tpRight(points.point(2).x(), static_cast <int> (points.point(2).y() -  yArrowGap));

                                                //debugging points
                                                /*
                                                  painter->drawText( points.point(0), "p0");
                                                  painter->drawText( points.point(1), "p1");
                                                  painter->drawText( points.point(2), "p2");
                                                  painter->drawText( points.point(3), "p3");
                                                */
                                                //store the front rectangle
                                                excessRectPositive.setTopLeft(tpLeft);
                                                excessRectPositive.setTopRight(tpRight);
                                                excessRectPositive.setBottomRight(points.point(1));
                                                excessRectPositive.setBottomLeft(points.point(0));

                                                // Don't use points for drawing after this!
                                                if ( pointArrayList && params()->barChartSubType() != KDChartParams::BarStacked ) {
                                                    points.translate( _dataRect.x(), _dataRect.y()  + excessRectPositive.top() - yArrowGap );
                                                    pointArrayList->append( points );
                                                } else if ( params()->barChartSubType() == KDChartParams::BarStacked )  {
                                                    if ( dataset != datasetEnd ) {
                                                        points.translate( _dataRect.x(), _dataRect.y()  + excessRectPositive.top() );
                                                        pointArrayList->append( points );
                                                    } else {
                                                        //adjust the display of the value label under Y  max value level
                                                        points.translate( _dataRect.x(), _dataRect.y() - excessRectPositive.bottom() - yArrowGap);
                                                        pointArrayList->append( points );
                                                    }
                                                }
                                            } else {

                                                // Draw split excess arrows (default)
                                                /* PENDING Michel:
                                                 * Here we have two situations.
                                                 * The value is too high because over the Max positive value
                                                 * or it is not within the configured view.
                                                 */
                                                QPointArray points( 5 );
                                                /*this works for positive config and 0 include config*/
                                                points.setPoint( 0, frontX1,
                                                                 (minDataValueYPos < static_cast <int> (yZero))?
                                                                 static_cast <int> (minDataValueYPos - 1) : static_cast <int>(yZero));
                                                points.setPoint( 1, frontX2,
                                                                 (minDataValueYPos < static_cast <int> (yZero))?
                                                                 static_cast<int> ( minDataValueYPos - 1) : static_cast <int>(yZero));
                                                points.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points.setPoint( 3, xm,      static_cast<int>( yZero + height1 ) );
                                                points.setPoint( 4, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ) );

                                                /*case where start and end value are negatives */
                                                if ( cellValue > maxColumnValue && 0 >= maxColumnValue || cellValue == 0) {
                                                    points.setPoint( 0, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ));
                                                    points.setPoint( 1, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ));
                                                }

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //allow it to be larger than the auto calculated width in case we want
                                                //to overlap
                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    QRect tmpRect ( points.point(0), points.point(2));
                                                    points.setPoint(0, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(0).y());
                                                    points.setPoint(1, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(1).y());
                                                    points.setPoint(2, static_cast <int>(tmpRect.center().x() + userwidth/2),
                                                                    points.point(2).y());
                                                    points.setPoint(4, static_cast <int>(tmpRect.center().x() - userwidth/2),
                                                                    points.point(4).y());
                                                    //}
                                                }
                                                if ( params()->barChartSubType() != KDChartParams::BarStacked ||
                                                     params()->barChartSubType() == KDChartParams::BarStacked && dataset != datasetEnd ) {
                                                    painter->setClipping(  false );
                                                    painter->drawPolygon( points );
                                                }

                                                //store the front rectangle
                                                excessRectPositive.setTopLeft(points.point(4));
                                                excessRectPositive.setTopRight(points.point(2));
                                                excessRectPositive.setBottomRight(points.point(1));
                                                excessRectPositive.setBottomLeft(points.point(0));

                                                // Don't use points for drawing after this!
                                                if ( pointArrayList && params()->barChartSubType() != KDChartParams::BarStacked ) {
                                                    points.translate( _dataRect.x(), _dataRect.y()  + excessRectPositive.top() );
                                                    pointArrayList->append( points );
                                                } else if ( params()->barChartSubType() == KDChartParams::BarStacked )  {
                                                    if ( dataset != datasetEnd ) {
                                                        points.translate( _dataRect.x(), _dataRect.y()  + excessRectPositive.top() );
                                                        pointArrayList->append( points );
                                                    } else {
                                                        //adjust the display of the value label under Y  max value level
                                                        points.translate( _dataRect.x(), _dataRect.y() - excessRectPositive.bottom() - yArrowGap);
                                                        pointArrayList->append( points );
                                                    }
                                                }

                                                QPointArray points2( 6 );
                                                points2.setPoint( 0, frontX1, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points2.setPoint( 1, xm,      static_cast<int>( yZero + height1 ) );
                                                points2.setPoint( 2, frontX2, static_cast<int>( yZero + height1 - 3.0 * delta ) );
                                                points2.setPoint( 3, frontX2, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                                points2.setPoint( 4, xm,      static_cast<int>( yZero + height1 - 0.75 * delta ) );
                                                points2.setPoint( 5, frontX1, static_cast<int>( yZero + height1 - 3.75 * delta ) );
                                                points2.translate( 0, yArrowGap );

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //allow it to be larger than the auto calculated width in case we want
                                                //to overlap
                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    points2.setPoint(0, excessRectPositive.topLeft().x(),points2.point(0).y());
                                                    points2.setPoint(2, excessRectPositive.topRight().x(),points2.point(2).y());
                                                    points2.setPoint(3, excessRectPositive.topRight().x(),points2.point(3).y());
                                                    points2.setPoint(5, excessRectPositive.topLeft().x(),points2.point(5).y());
                                                    //}
                                                }

                                                painter->drawPolygon( points2 );
                                                /*
                                                  NOTE:  We do NOT want to store the region here, but
                                                  we use the front rectangle above in order to display the
                                                  data value labels inside the front rectangle.
                                                  Disadvantage: clicking onto these excess arrows, will not
                                                  result in a signal being sent, because KDChartWidget will
                                                  not notice, that the user has clicked onto the bar.
                                                  That's a minor drawback, compared to the gain of being
                                                  able to position the labels correctly.
                                                */
                                                points2.translate( 0, yArrowGap );
                                                painter->drawPolygon( points2 );
                                                /*
                                                  NOTE:  We do NOT want to store the region here
                                                  (see comment above)
                                                */
                                            } // draw split excess arrow
                                        } //if( bShowThisBar )

                                        painter->setClipRect( ourClipRect );
                                    } // not tooLow  && bNormalMode )
                                    else {
                                        //bool fromBottom = bNormalMode && !_bThreeDBars;
                                        double y0 = yposPositives - zeroXAxisI;

                                        int pt1Y = static_cast < int > ( y0 - barHeight - sideBarHeight);

                                        calculateXFront1_2( bNormalMode, bIsVeryFirstBar, bIsFirstDataset, _bThreeDBars,
                                                            xpos, valueBlockGap, datasetGap, frontBarWidth,
                                                            frontX1, frontX2, prevFrontX2 );

                                        QPoint pt1( frontX1, pt1Y );
                                        QPoint pt2( frontX2,
                                                    static_cast < int > ( y0 + shiftUpperBars ) );

                                        if( 0.0 < minColumnValue )
                                            pt2.setY(pt2.y() - static_cast < int > ( pixelsPerUnit * minColumnValue ));

                                        if( pt2.y() < pt1Y ) {
                                            pt1.setY( pt2.y() );
                                            pt2.setY( pt1Y );
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

                                            QSize siz( pt2.x() - pt1.x(),
                                                       pt2.y() - pt1.y());

                                            QRect rect(  pt1, siz );

                                            //adjust display for the 3d percent bars - last dataset.
                                            if ( params()->barChartSubType() == KDChartParams::BarPercent && cellValue != 0)
                                                rect.setTop( rect.top() + static_cast <int> (sideBarHeight) - 1);

                                            // store the front rect
                                            if( tooHigh  ) {
                                                frontRectPositive.setTopLeft(excessRectPositive.topLeft());
                                                frontRectPositive.setTopRight(excessRectPositive.topRight());
                                                frontRectPositive.setBottomRight(excessRectPositive.bottomRight());
                                                frontRectPositive.setBottomLeft(excessRectPositive.bottomLeft());
                                            } else {
                                                frontRectPositive.setTopLeft(rect.topLeft());
                                                frontRectPositive.setTopRight(rect.topRight());
                                                frontRectPositive.setBottomRight(rect.bottomRight());
                                                frontRectPositive.setBottomLeft(rect.bottomLeft());
                                                if( _bThreeDBars && cellValue == 0 )
                                                    frontRectNegative = frontRectPositive;
                                            }

                                            if( 1.5 > frontBarWidth ){
                                                //qDebug("1.5 > frontBarWidth ");
                                                QPen oldPen( painter->pen() );
                                                painter->setPen( QPen(painter->brush().color(), 0) );
                                                painter->drawLine(pt1, QPoint(pt1.x(),pt2.y()));
                                                painter->setPen( oldPen );
                                            }else{
                                                //debugging points and lines
                                                /*
                                                  painter->drawText( rect.topLeft(), QString::number(cellValue).latin1());
                                                  painter->drawText( rect.topRight(), "1f");
                                                  painter->drawText( rect.bottomRight(), "2f");
                                                  painter->drawText( rect.bottomLeft(), "3f");
                                                  painter->drawLine( rect.topLeft(), rect.topRight());
                                                  painter->drawLine( rect.topRight(), rect.bottomRight());
                                                  painter->drawLine( rect.bottomRight(), rect.bottomLeft());
                                                  painter->drawLine( rect.bottomLeft(), rect.topLeft());
                                                */

                                                //Pending Michel adjust the painting in case we have a user given Width
                                                //allow it to be larger than the auto calculated width in case we want
                                                //to overlap
                                                if ( params()->userWidth() != 0 ) {
                                                    int userwidth = 0;
                                                    if ( params()->userWidth() < 0 )
                                                        userwidth = static_cast <int> (params()->userWidth() * -areaWidthP1000);
                                                    else
                                                        userwidth = static_cast <int> (params()->userWidth() * areaWidthP1000);
                                                    //if ( userwidth < frontBarWidth ) {
                                                    rect.setLeft( static_cast <int> (rect.center().x() - (userwidth/2)));
                                                    rect.setRight( static_cast <int> (rect.center().x() + (userwidth/2)));
                                                    rect.setWidth( static_cast <int> (userwidth) );
                                                    //adjust the front rect
                                                    frontRectPositive.setLeft( tooHigh ? static_cast <int> (excessRectPositive.center().x() - (userwidth/2)):rect.left());
                                                    frontRectPositive.setRight( tooHigh ? static_cast <int> (excessRectPositive.center().x() + (userwidth/2)):rect.right());
                                                }

                                                // drawing the front side
                                                if (!tooHigh && !tooLow || params()->barChartSubType() == KDChartParams::BarPercent ) {
                                                    if ( bMultiRows )
                                                        painter->setClipping( false );
                                                    else
                                                        painter->setClipping( true );
                                                    painter->drawRect( rect );
                                                }
                                                // Don't use rect for drawing after this
                                                if ( pointArrayList ) {
                                                    rect.moveBy( _dataRect.x(), _dataRect.y());
                                                    pointArrayList->append( rectToPointArray( rect ) );
                                                }
                                            }
                                        } // bShowThisBar
                                    } // positive values
                                }
                                if ( bShowThisBar && _bThreeDBars &&  !bDrawExtraLines ) {
                                    //Pending Michel: no need to use that anymore
                                    //const int maxY = 2*devRect.height();
                                    QPointArray points( 4 );
                                    if (cellValue <= 0 || cellValue < minColumnValue) {
                                        if ( tooLow || cellValue < minColumnValue ) {
                                            points.setPoint(0, excessRectNegative.topRight());
                                            points.setPoint(1, excessRectNegative.topRight().x() +  static_cast<int>(sideBarHeight),
                                                            excessRectNegative.top() - static_cast<int>(sideBarHeight));
                                            points.setPoint(2, excessRectNegative.bottomRight().x() + static_cast<int>(sideBarHeight),
                                                            excessRectNegative.bottom() - static_cast<int>(sideBarHeight));
                                            points.setPoint(3, excessRectNegative.bottomRight());
                                        } else {
                                            points.setPoint( 0, frontRectNegative.bottomRight());
                                            points.setPoint( 1, frontRectNegative.bottomRight().x() +  static_cast<int>(sideBarHeight),
                                                             frontRectNegative.bottom() -  static_cast<int>(sideBarHeight) );
                                            points.setPoint(2,  frontRectNegative.bottomRight().x() +  static_cast<int>(sideBarHeight),
                                                            frontRectNegative.top() - static_cast<int>(sideBarHeight));
                                            points.setPoint(3, frontRectNegative.topRight() );
                                        }

                                        rightRectNegative.setTopLeft( points.point(0));
                                        rightRectNegative.setTopRight( points.point(2));
                                        rightRectNegative.setBottomRight(points.point(1));
                                        rightRectNegative.setBottomLeft(points.point(3));

                                    } else {
                                        // Pending Michel
                                        // Make sure to align the right side top and bottom points
                                        // to the front side points
                                        if ( tooHigh ) {
                                            points.setPoint(0, excessRectPositive.topRight());
                                            points.setPoint(1, excessRectPositive.topRight().x() + static_cast <int> (sideBarHeight),
                                                            excessRectPositive.top() - static_cast <int> (sideBarHeight) );
                                            points.setPoint(2, excessRectPositive.bottomRight().x() + static_cast <int> (sideBarHeight),
                                                            excessRectPositive.bottom() - static_cast <int> (sideBarHeight));
                                            points.setPoint(3, excessRectPositive.bottomRight());
                                        } else {
                                            points.setPoint(0, frontRectPositive.topRight());
                                            points.setPoint(1, frontRectPositive.topRight().x() + static_cast <int> (sideBarHeight),
                                                            frontRectPositive.top() - static_cast<int>(sideBarHeight));
                                            points.setPoint(2, frontRectPositive.bottomRight().x() + static_cast <int> (sideBarHeight),
                                                            frontRectPositive.bottom() - static_cast<int>(sideBarHeight));
                                            points.setPoint(3, frontRectPositive.bottomRight());
                                        }
                                        //register the right rect
                                        rightRectPositive.setTopLeft( points.point(0));
                                        rightRectPositive.setTopRight( points.point(1));
                                        rightRectPositive.setBottomLeft( points.point(3));
                                        rightRectPositive.setBottomRight(points.point(2));
                                    }

                                    if ( myShadow2Color.isValid() )
                                        painter->setBrush( QBrush( myShadow2Color, params()->shadowPattern() ) );
                                    else
                                        painter->setBrush( NoBrush );

                                    //debug points and lines
                                    /*
                                      painter->drawText( points.point(0), "0r");
                                      painter->drawText( points.point(1), "1r");
                                      painter->drawText( points.point(2), "2r");
                                      painter->drawText( points.point(3), "3r");

                                      painter->drawLine( points.point(0), points.point(1));
                                      painter->drawLine( points.point(1), points.point(2));
                                      painter->drawLine( points.point(2), points.point(3));
                                      painter->drawLine( points.point(3), points.point(0));
                                    */

                                    //drawing the right side
                                    if( (!tooHigh  && !tooLow)  || (tooHigh && cellValue <= 0 ) ) {
                                        if (( cellValue != 0 && params()->barChartSubType() == KDChartParams::BarPercent ) ||
                                            ( cellValue != 0 && params()->barChartSubType() == KDChartParams::BarStacked ) ||
                                            ( cellValue != 0 && bNormalMode ||
                                              cellValue != 0 && bMultiRows)) {
                                            painter->setClipping( false );
                                            painter->drawPolygon( points );
                                        }
                                    }

                                    // Dont override the region stored in case of excess values	or barPercent this in order to display
                                    // the value labels closer to the corner of the front bar.
                                    if ( pointArrayList  && !tooHigh && !tooLow && params()->barChartSubType() != KDChartParams::BarPercent ) {
                                        QPointArray points2cpy( points.copy() );
                                        //qDebug("g2");
                                        points2cpy.translate( _dataRect.x(), _dataRect.y());
                                        //qDebug("g3");
                                        pointArrayList->append( points2cpy );
                                        //qDebug("g4");
                                    }

                                    // drawing the top, but only for the topmost piece for stacked and percent
                                    if ( bNormalMode || bMultiRows ||
                                         // For stacked and percent bars, there are three ways to determine
                                         // the top:
                                         // 1. all values are negative: the top is the one in the first dataset
                                         ( maxValueInThisColumn <= 0.0 && dataset == firstValidValue ) ||
                                         // 2. all values are positive: the top is the one in the last dataset
                                         ( minValueInThisColumn >= 0.0 && dataset == lastValidPositiveValue ) ||
                                         // 3. some values are positive, some negative:
                                         // the top is the one in the last positive
                                         // dataset value
                                         ( dataset == lastValidPositiveValue ) ) {
                                        if (cellValue <= 0  || cellValue < minColumnValue) {
                                            if ( tooLow ) {
                                                points.setPoint(0,excessRectNegative.bottomLeft());
                                                points.setPoint(1,excessRectNegative.topLeft().x() + static_cast <int> (sideBarHeight),
                                                                excessRectNegative.bottom() - static_cast <int> (sideBarHeight));
                                                points.setPoint(2,excessRectNegative.bottomRight().x() + static_cast <int> (sideBarHeight),
                                                                excessRectNegative.bottom() - static_cast <int> (sideBarHeight));
                                                points.setPoint(3,excessRectNegative.bottomRight());
                                            }else {
                                                // Align the top to the front and the right side
                                                points.setPoint(0,frontRectNegative.topLeft() );
                                                points.setPoint(1,frontRectNegative.topLeft().x() + static_cast <int> (sideBarHeight), 						  			                        rightRectNegative.top());
                                                points.setPoint(2,rightRectNegative.topRight() );
                                                points.setPoint(3,rightRectNegative.topRight().x() - static_cast <int> (sideBarHeight),
                                                                frontRectNegative.topRight().y() );
                                                //make sure the top rect color is the right one - barStacked - Noll values
                                                if ( (params()->barChartSubType() == KDChartParams::BarStacked && cellValue == 0 && maxValueInThisColumn != 0 ) )
                                                    points.translate(0,  maxValueInThisColumn <= 0?-(static_cast <int> (1*pixelsPerUnit) + 1):
                                                                     -static_cast<int>(maxValueInThisColumn*pixelsPerUnit));

                                                if ( params()->barChartSubType() == KDChartParams::BarPercent && cellValue == 0 ) {
                                                    if ( dataset == datasetEnd && maxValueInThisColumn != 0)
                                                        points.translate(0,  -static_cast<int>( logHeight - sideBarHeight ));
                                                    else if ( maxValueInThisColumn == 0)
                                                        points.translate(0, static_cast <int> (logHeight + (sideBarHeight - sideBarWidth)));
                                                }
                                            }
                                        } else {
                                            if ( tooHigh ) {
                                                points.setPoint(0, excessRectPositive.topLeft());
                                                points.setPoint(1, excessRectPositive.topLeft().x() + static_cast <int> (sideBarHeight),
                                                                excessRectPositive.top() - static_cast <int> (sideBarHeight) );
                                                points.setPoint(2, excessRectPositive.topRight().x() +  static_cast <int> (sideBarHeight),
                                                                excessRectPositive.top() - static_cast <int> (sideBarHeight));
                                                points.setPoint(3, excessRectPositive.topRight());
                                            } else {
                                                // Pending Michel
                                                // Align the top to the front and the right side
                                                points.setPoint(0, frontRectPositive.topLeft());
                                                points.setPoint(1, frontRectPositive.topLeft().x() + static_cast <int> (sideBarHeight),
                                                                rightRectPositive.top() );
                                                points.setPoint(2, rightRectPositive.topRight());
                                                points.setPoint(3, rightRectPositive.topRight().x() - static_cast <int> (sideBarHeight),
                                                                frontRectPositive.topRight().y());
                                            }
                                        }

                                        if (cellValue < 0.0 && maxValueInThisColumn < 0)
                                            painter->setBrush( bMultiRows ? myBarColor : black );
                                        else
                                            painter->setBrush( QBrush( myShadow1Color, params()->shadowPattern() ) );

                                        if ( !myShadow1Color.isValid() )
                                            painter->setBrush( NoBrush ); // override prev. setting
                                        // debug points and lines
                                        /*
                                          painter->drawText( points.point(0), "0t");
                                          painter->drawText( points.point(1), "1t");
                                          painter->drawText( points.point(2), "2t");
                                          painter->drawText( points.point(3), "3t");

                                          painter->drawLine( points.point(0), points.point(1) );
                                          painter->drawLine( points.point(1),points.point(2) );
                                          painter->drawLine( points.point(2),points.point(3) );
                                          painter->drawLine( points.point(3),points.point(0) );
                                        */

                                        // drawing the top side
                                        if (!tooHigh && !tooLow  || (tooHigh && cellValue <= 0) )
                                            painter->drawPolygon( points );

                                        // dont override the region stored in case of excess values.
                                        if (pointArrayList && !tooHigh && !tooLow
                                            && params()->barChartSubType() != KDChartParams::BarPercent
                                            && params()->barChartSubType() != KDChartParams::BarStacked) {
                                            points.translate(  _dataRect.x(), _dataRect.y());
                                            pointArrayList->append( points );
                                        }
                                    }
                                }//if ( _bThreeDBars )

                                if( regions && pointArrayList && ! pointArrayList->empty() ) {
                                    if( bShowThisBar && !bDrawExtraLines ){
                                        KDChartDataRegion * region;
                                        if( _bThreeDBars ){
                                            region = new KDChartDataRegion( dataset, value, chart,
                                                                            pointArrayList, true );
                                        } else {
                                            // just store a rectangle if NOT in 3-D bar mode
                                            region = new KDChartDataRegion( dataset, value, chart,
                                                                            pointArrayList->first().boundingRect() );
                                            delete pointArrayList;
                                        }
                                        /*qDebug("KDChartDataRegion stored!  x: %i  y: %i  w: %i  h: %i",
                                          region->rect().x(),region->rect().y(),
                                          region->rect().width(),region->rect().height());*/
                                        regions->append( region );
                                    } else {
                                        delete pointArrayList;
                                    }
                                }
                            }// if( !bDrawExtraLines || bCellPropertiesFound )
                        }// if( !skipMe )
                    }else{
                        // Do not paint a bar, but update the position
                        // variable: to find the exact x1 position for
                        // the next bar that will be drawn.
                        int iDummy1, iDummy2;
                        calculateXFront1_2( bNormalMode, bIsVeryFirstBar, bIsFirstDataset, _bThreeDBars,
                                            xpos, valueBlockGap, datasetGap, frontBarWidth,
                                            iDummy1, iDummy2, prevFrontX2 );
                    }// if( dataset >= datasetStart && dataset <= datasetEnd )

                    // Vertical advancement in stacked or percent only if there was a value
                    // Pending Michel add sideBarHeight in case we are in 3D mode but not for Percent
                    // where we need to display the top rect but cant resize the YAxis.
                    if ( params()->barChartSubType() == KDChartParams::BarStacked ||
                         params()->barChartSubType() == KDChartParams::BarPercent )
                        if ( /*barHeight*/ cellValue < 0 )
                            (_bThreeDBars && params()->barChartSubType() != KDChartParams::BarPercent)?yposNegatives -= sideBarHeight + barHeight:
                                                                                                       yposNegatives -= barHeight;
                        else
                            (_bThreeDBars && params()->barChartSubType() != KDChartParams::BarPercent)?yposPositives -= sideBarHeight + barHeight:
                                                                                                       yposPositives -= barHeight;

                } else {
                    // Do not paint a bar, but update the position
                    // variable: to find the exact x1 position for
                    // the next bar that will be drawn.
                    int iDummy1, iDummy2;
                    calculateXFront1_2( bNormalMode, bIsVeryFirstBar, bIsFirstDataset, _bThreeDBars,
                                        xpos, valueBlockGap, datasetGap, frontBarWidth,
                                        iDummy1, iDummy2, prevFrontX2 );
                }


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
                bIsVeryFirstBar = false;
                bIsFirstDataset = false;
            }


            // Advancement between value blocks
            if ( bNormalMode ){
                // skip gap between value blocks, don't worry about last one here
                xpos += valueBlockGap;
                //qDebug("**************************** xpos: %f  +  valueBlockGap: %f  =  %f", xpos, valueBlockGap, xpos+valueBlockGap);
            }else{
                // skip gap between value blocks
                xpos += valueBlockGap + barWidth;
                //qDebug ( "2/ barWidth %f", barWidth );
                //qDebug ( " valueBlockGap %f", valueBlockGap );
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
