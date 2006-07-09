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
#include "KDChartBWPainter.h"
#include <KDChartParams.h>
#include "KDChartTextPiece.h"

#include <qpainter.h>
#if COMPAT_QT_VERSION >= 0x030000
#include <qmemarray.h>
#else
#include <qarray.h>
#define QMemArray QArray
#endif

#include <stdlib.h>

/**
  \class KDChartBWPainter KDChartBWPainter.h

  \brief A chart painter implementation that can paint Box&Whisker charts.
  */

/**
  Constructor. Sets up internal data structures as necessary.

  \param params the KDChartParams structure that defines the chart
  */
    KDChartBWPainter::KDChartBWPainter( KDChartParams* params ) :
KDChartAxesPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
  Destructor.
  */
KDChartBWPainter::~KDChartBWPainter()
{
    // intentionally left blank
}


void quicksort( QMemArray<double>& a, int lo, int hi )
{
    int    i=lo, j=hi;
    double h;
    double x=a[(lo+hi)/2];
    do
    {
        while (a[i]<x) i++;
        while (a[j]>x) j--;
        if (i<=j)
        {
            h=a[i]; a[i]=a[j]; a[j]=h;
            i++; j--;
        }
    } while (i<=j);
    if (lo<j) quicksort(a, lo, j);
    if (i<hi) quicksort(a, i, hi);
}


// The following function returns the number of used cells containing a double.
int KDChartBWPainter::calculateStats( KDChartTableDataBase& data,
        uint dataset )
{
    const uint nMax = data.usedCols();
    int nUsed = 0;
    QMemArray<double> values( nMax );
    double sum = 0.0;
    QVariant vVal;
    if( data.sorted() ){
        for( uint i=0; i<nMax; ++i){
            if( data.cellCoord( dataset, i, vVal, 1 ) &&
                QVariant::Double == vVal.type() ) {
                values[nUsed] = vVal.toDouble();
                sum += values[nUsed];
                ++nUsed;
            }
        }
    }else{
        // make copy of the dataset and look if it is sorted
        bool sorted = true;
        double last = 0.0; //  <--  avoids an annoying compile-time warning
        for( uint i=0; i<nMax; ++i){
            if( data.cellCoord( dataset, i, vVal, 1 ) &&
                QVariant::Double == vVal.type() ) {
                values[nUsed] = vVal.toDouble();
                if(    nUsed // skip 1st value
                    && last > values[nUsed] )
                    sorted = false;
                last = values[nUsed];
                sum += last;
                ++nUsed;
            }
        }
        if( !sorted ){
            // sort our copy of the dataset
            quicksort( values, 0, nUsed-1 );
        }
    }

    // Values now contains all used values without empty gaps.
    // nUsed contains their number, so values[nUsed-1] is the last value.

    if( nUsed ){
        // store some values
        stats[ KDChartParams::MaxValue  ] = values[nUsed-1];
        stats[ KDChartParams::MinValue  ] = values[0];

        stats[ KDChartParams::MeanValue ] = sum / nUsed;
        // calculate statistics
        bool bOdd = 1 == nUsed % 2;
        // find median
        int nUd2 = nUsed/2;
        if( bOdd )
            stats[ KDChartParams::Median ] = values[ nUd2 ];
        else
            stats[ KDChartParams::Median ] =
                (values[ QMAX(nUd2-1, 0) ] + values[ nUd2 ]) /2;
        // find last value of lower quartile
        nLastQ1  = QMAX( nUd2-1, 0 );
        // find 1st value of lower quartile
        nFirstQ1 = nLastQ1 / 2;

        // determine how many values are below the median ( == how many are above it)
        int nLowerCount = nLastQ1 - nFirstQ1 + 1;

        // find 1st value of upper quartile
        nFirstQ3 = bOdd ? QMIN( nUd2+1, nUsed-1 ) : nUd2;
        // find last value of upper quartile
        nLastQ3  = nFirstQ3 + nLowerCount - 1;

        // find quartiles
        bool bOdd2 = 1 == nLowerCount % 2;
        // find lower quartile
        if( bOdd2 )
            stats[ KDChartParams::Quartile1 ] = values[ nFirstQ1 ];
        else
            stats[ KDChartParams::Quartile1 ] =
                (values[ QMAX(nFirstQ1-1, 0) ] + values[ nFirstQ1 ]) /2;
        // find upper quartile
        if( bOdd2 ){
            stats[ KDChartParams::Quartile3 ] = values[ nLastQ3 ];
}
        else{
            //qDebug("  "+QString::number(nLastQ3)+"  "+QString::number(KDChartParams::Quartile3)
            //      +"  "+QString::number(nUsed)+"  "+QString::number(QMIN(nLastQ3+1, nUsed-1)));
            stats[ KDChartParams::Quartile3 ] =
                (values[ nLastQ3 ] + values[ QMIN(nLastQ3+1, nUsed-1) ]) /2;
}
        // find the interquartile range (IQR)
        double iqr = stats[ KDChartParams::Quartile3 ] - stats[ KDChartParams::Quartile1 ];

        // calculate the fences
        double upperInner, lowerInner, upperOuter, lowerOuter;
        params()->bWChartFences( upperInner, lowerInner,
                upperOuter, lowerOuter );
        stats[ KDChartParams::UpperInnerFence ] =
            stats[ KDChartParams::Quartile3 ] + iqr * upperInner;
        stats[ KDChartParams::LowerInnerFence ] =
            stats[ KDChartParams::Quartile1 ] - iqr * lowerInner;
        stats[ KDChartParams::UpperOuterFence ] =
            stats[ KDChartParams::Quartile3 ] + iqr * upperOuter;
        stats[ KDChartParams::LowerOuterFence ] =
            stats[ KDChartParams::Quartile1 ] - iqr * lowerOuter;
    }
    return nUsed;
}


/**
  This method is a specialization that returns a fallback legend text
  appropriate for BW that do not have the same notion of a dataset like
  e.g. bars.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \param uint dataset the dataset number for which to generate a
  fallback text
  \return the fallback text to use for describing the specified
  dataset in the legend
  */
QString KDChartBWPainter::fallbackLegendText( uint dataset ) const
{
    return QObject::tr( "Series " ) + QString::number( dataset + 1 );
}


/**
  This methods returns the number of elements to be shown in the
  legend in case fallback texts are used.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \return the number of fallback texts to use
  */
uint KDChartBWPainter::numLegendFallbackTexts( KDChartTableDataBase* data ) const
{
    return data->usedRows();
}


bool KDChartBWPainter::isNormalMode() const
{
    return KDChartParams::BWNormal == params()->bWChartSubType();
}

int KDChartBWPainter::clipShiftUp( bool, double ) const
{
    return 0;
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
void KDChartBWPainter::specificPaintData( QPainter* painter,
        const QRect& ourClipRect,
        KDChartTableDataBase* data,
        KDChartDataRegionList* /*regions*/,
        const KDChartAxisParams* axisPara,
        bool /*bNormalMode*/,
        uint /*chart*/,
        double logWidth,
        double /*areaWidthP1000*/,
        double logHeight,
        double /*axisYOffset*/,
        double /*minColumnValue*/,
        double /*maxColumnValue*/,
        double /*columnValueDistance*/,
        uint chartDatasetStart,
        uint chartDatasetEnd,
        uint datasetStart,
        uint datasetEnd )
{
    //double areaHeightP1000 = logHeight / 1000.0;

    uint datasetNum = ( chartDatasetEnd - chartDatasetStart ) + 1;

    double pixelsPerUnit = 0.0;
    if( 0.0 != axisPara->trueAxisHigh() - axisPara->trueAxisLow() )
        pixelsPerUnit = logHeight / (axisPara->trueAxisHigh() - axisPara->trueAxisLow());
    else
        pixelsPerUnit = logHeight / 10;

    // Distance between the individual "stocks"
    double pointDist = logWidth / ( ( double ) datasetNum );

    // compute the position of the 0 axis
    double zeroXAxisI = axisPara->axisZeroLineStartY() - _dataRect.y();

    const int lineWidth   = static_cast<int>( pointDist / 66.0 ) * QMAX(params()->lineWidth(), 1);
    const int lineWidthD2 = lineWidth * 2 / 3;

    const bool noBrush = Qt::NoBrush == params()->bWChartBrush().style();

    // Loop over the datasets, draw one box and whisker shape for each series.
    for ( uint dataset  = chartDatasetStart;
            dataset <= chartDatasetEnd;
            ++dataset ) {

        if( dataset >= datasetStart &&
                dataset <= datasetEnd &&
                0 < calculateStats( *data, dataset ) ) {
            const QColor color( params()->dataColor( dataset ) );
            // transform calculated values
            double drawUOF = stats[ KDChartParams::UpperOuterFence ] * pixelsPerUnit;
            double drawUIF = stats[ KDChartParams::UpperInnerFence ] * pixelsPerUnit;
            double drawQu3 = stats[ KDChartParams::Quartile3       ] * pixelsPerUnit;
            double drawMed = stats[ KDChartParams::Median          ] * pixelsPerUnit;
            double drawQu1 = stats[ KDChartParams::Quartile1       ] * pixelsPerUnit;
            double drawLIF = stats[ KDChartParams::LowerInnerFence ] * pixelsPerUnit;
            double drawLOF = stats[ KDChartParams::LowerOuterFence ] * pixelsPerUnit;
            double drawMax = stats[ KDChartParams::MaxValue        ] * pixelsPerUnit;
            double drawMin = stats[ KDChartParams::MinValue        ] * pixelsPerUnit;
            double drawMean= stats[ KDChartParams::MeanValue       ] * pixelsPerUnit;
            // get whisker values
            double drawUWhisker = QMIN(drawUIF, drawMax);
            double drawLWhisker = QMAX(drawLIF, drawMin);
            // get the box width
            const int boxWidth = QMAX( 6, static_cast<int>( pointDist * 0.2 ) );
            // get marker size (for the outliers and/or for the median value)
            int markWidth = params()->bWChartOutValMarkerSize();
            bool drawOutliers = ( 0 != markWidth );
            if( drawOutliers ){
                if( 0 > markWidth)
                    markWidth = QMAX( 4, markWidth * boxWidth / -100 );
                else
                    markWidth = QMAX( 4, markWidth );
            }
            else
                markWidth = boxWidth * 25 / 100; // use the default for the Median marker

            painter->setPen( QPen( color, lineWidth ) );
            painter->setBrush( params()->bWChartBrush() );
            // draw the box
            int boxWidthD2 = boxWidth / 2;
            int xPos = static_cast<int>(
                    pointDist * ( (double)(dataset - chartDatasetStart) + 0.5 )
                    - lineWidth / 2);
            painter->drawRect( xPos - boxWidthD2,
                    static_cast<int>( zeroXAxisI - drawQu3 ),
                    boxWidth,
                    static_cast<int>( drawQu3 - drawQu1) + 1 );
            // draw the median
            painter->drawLine( xPos - boxWidthD2,
                    static_cast<int>( zeroXAxisI - drawMed ),
                    xPos - boxWidthD2 + boxWidth,
                    static_cast<int>( zeroXAxisI - drawMed ) );
            // draw the whisker
            painter->drawLine( xPos - boxWidthD2,
                    static_cast<int>( zeroXAxisI - drawUWhisker ),
                    xPos - boxWidthD2 + boxWidth,
                    static_cast<int>( zeroXAxisI - drawUWhisker ) );
            painter->drawLine( xPos,
                    static_cast<int>( zeroXAxisI - drawUWhisker ),
                    xPos,
                    static_cast<int>( zeroXAxisI - drawQu3 ) );
            painter->drawLine( xPos - boxWidthD2,
                    static_cast<int>( zeroXAxisI - drawLWhisker ),
                    xPos - boxWidthD2 + boxWidth,
                    static_cast<int>( zeroXAxisI - drawLWhisker ) );
            painter->drawLine( xPos,
                    static_cast<int>( zeroXAxisI - drawLWhisker ),
                    xPos,
                    static_cast<int>( zeroXAxisI - drawQu1 ) );
            // draw the values
            int xPos2 = static_cast<int>(
                    pointDist * ( (double)(dataset - chartDatasetStart) + 0.5 )
                    - lineWidthD2 / 2);
            int markWidthD2 =  QMAX(markWidth / 2, 2);
            int markWidthD25 = QMAX(static_cast<int>( 0.85 * markWidth / 2.0), 2);
            int markWidthD35 = QMAX(static_cast<int>( 0.85 * markWidth / 3.0), 2);
            // draw the outliers
            if( drawOutliers ){
                const uint nMax = data->usedCols();
                int drawVal;
                QVariant vVal;
                for( uint i=0; i<nMax; ++i)
                    if( data->cellCoord( dataset, i, vVal, 1 ) &&
                        QVariant::Double == vVal.type() ) {
                        drawVal = static_cast<int>( pixelsPerUnit * vVal.toDouble() );
                        if( drawLOF > drawVal || drawUOF < drawVal ) {
                            painter->setPen( Qt::NoPen );
                            painter->drawChord( xPos2 - markWidthD2,
                                    static_cast<int>(zeroXAxisI - drawVal) - markWidthD2,
                                    markWidth,
                                    markWidth,
                                    0, 5760 );
                            painter->setPen( QPen( color, lineWidthD2 ) );
                            painter->drawArc( xPos2 - markWidthD2,
                                    static_cast<int>(zeroXAxisI - drawVal) - markWidthD2,
                                    markWidth,
                                    markWidth,
                                    0, 5760 );
                        } else if( drawLIF > drawVal || drawUIF < drawVal ) {
                            painter->setPen( Qt::NoPen );
                            painter->drawChord( xPos2 - markWidthD2,
                                    static_cast<int>( zeroXAxisI - drawVal) - markWidthD2,
                                    markWidth,
                                    markWidth,
                                    0, 5760 );
                            painter->setPen( QPen( color, lineWidthD2 ) );
                            painter->drawLine( xPos2,
                                    static_cast<int>(zeroXAxisI - drawVal) - markWidthD2,
                                    xPos2,
                                    static_cast<int>(zeroXAxisI - drawVal) + markWidthD2 );
                            painter->drawLine( xPos2 - markWidthD25,
                                    static_cast<int>(zeroXAxisI - drawVal) - markWidthD35,
                                    xPos2 + markWidthD25,
                                    static_cast<int>(zeroXAxisI - drawVal) + markWidthD35 );
                            painter->drawLine( xPos2 + markWidthD25,
                                    static_cast<int>(zeroXAxisI - drawVal) - markWidthD35,
                                    xPos2 - markWidthD25,
                                    static_cast<int>(zeroXAxisI - drawVal) + markWidthD35 );
                        }
                    }
            }
            // draw the mean value
            bool evenLineWidthD2 = floor( ((double)lineWidthD2)/2.0 ) == ((double)lineWidthD2)/2.0;
            painter->setPen( params()->bWChartBrush().color() );
            painter->drawChord( xPos2 - markWidthD2-1 - (evenLineWidthD2 ? 0 : 1),
                    static_cast<int>( zeroXAxisI - drawMean ) - markWidthD2 - 1,
                    markWidthD2*2 + (evenLineWidthD2 ? 2 : 3),
                    markWidthD2*2 + (evenLineWidthD2 ? 2 : 3),
                    0, 5760 );
            if( noBrush ) {
                // use different color brightness for the Mean marker
                int h,s,v;
                color.hsv(&h,&s,&v);
                painter->setPen( QPen( 128 > v ? color.light(150) : color.dark(150),
                            lineWidthD2 ) );
            } else
                painter->setPen( QPen( color, lineWidthD2 ) );
            painter->drawLine( xPos2 - markWidthD2 - (evenLineWidthD2 ? 0 : 1),
                               static_cast<int>( zeroXAxisI - drawMean ),
                               xPos2 + markWidthD2,
                               static_cast<int>( zeroXAxisI - drawMean ) );
            painter->drawLine( xPos2 - (evenLineWidthD2 ? 0 : 1),
                               static_cast<int>( zeroXAxisI - drawMean ) - markWidthD2,
                               xPos2 - (evenLineWidthD2 ? 0 : 1),
                               static_cast<int>( zeroXAxisI - drawMean ) + markWidthD2 + (evenLineWidthD2 ? 0 : 1) );

            // draw the statistical value texts
            painter->setPen( Qt::NoPen );
            for( int ii =  KDChartParams::BWStatValSTART;
                    ii <= KDChartParams::BWStatValEND;
                    ++ii ){
                KDChartParams::BWStatVal i = (KDChartParams::BWStatVal)ii;
                if( params()->bWChartPrintStatistics( i ) ) {
                    QFont statFont( params()->bWChartStatisticsFont( i ) );
                    float nTxtHeight = statFont.pointSizeFloat();
                    if ( params()->bWChartStatisticsUseRelSize( i ) ) {
                        nTxtHeight = params()->bWChartStatisticsFontRelSize( i )
                            * boxWidth / 100;
                        statFont.setPointSizeFloat( nTxtHeight );
                    }
                    double drawStat = pixelsPerUnit * stats[i];
                    KDChartTextPiece statText( painter, QString::number( stats[i] ),
                            statFont );
                    int tw = statText.width();
                    int xDelta = (    KDChartParams::MaxValue  == i
                            || KDChartParams::MeanValue == i
                            || KDChartParams::MinValue  == i )
                        ? -1 * (tw + static_cast<int>( 1.3*boxWidthD2 ))
                        : static_cast<int>( 1.3*boxWidthD2 );
                    QBrush brush( params()->bWChartStatisticsBrush( i ) );
                    painter->setBrush( brush );
                    int y = static_cast<int>( zeroXAxisI - drawStat - nTxtHeight/2);
                    painter->drawRect( xPos + xDelta - 1,
                            y,
                            tw + 2,
                            QMAX(static_cast < int > ( nTxtHeight ), 8) + 1 );
                    statText.draw( painter,
                            xPos + xDelta,
                            y,
                            ourClipRect,
                            params()->bWChartStatisticsColor( i ),
                            0 );
                }
            }

        } else
            continue; // we cannot display this value
    }
}
