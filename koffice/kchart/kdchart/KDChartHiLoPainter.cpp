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
#include "KDChartHiLoPainter.h"
#include <KDChartParams.h>
#include "KDChartTextPiece.h"

#include <qpainter.h>

#include <stdlib.h>

/**
  \class KDChartHiLoPainter KDChartHiLoPainter.h

  \brief A chart painter implementation that can paint HiLo charts.
  */

/**
  Constructor. Sets up internal data structures as necessary.

  \param params the KDChartParams structure that defines the chart
  */
    KDChartHiLoPainter::KDChartHiLoPainter( KDChartParams* params ) :
KDChartAxesPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
  Destructor.
  */
KDChartHiLoPainter::~KDChartHiLoPainter()
{
    // intentionally left blank
}


/**
  This method is a specialization that returns a fallback legend text
  appropriate for HiLo that do not have the same notion of a dataset like
  e.g. bars.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \param uint dataset the dataset number for which to generate a
  fallback text
  \return the fallback text to use for describing the specified
  dataset in the legend
  */
QString KDChartHiLoPainter::fallbackLegendText( uint dataset ) const
{
    return QObject::tr( "Value " ) + QString::number( dataset + 1 );
}


/**
  This methods returns the number of elements to be shown in the
  legend in case fallback texts are used.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \return the number of fallback texts to use
  */
uint KDChartHiLoPainter::numLegendFallbackTexts( KDChartTableDataBase* data ) const
{
    return data->usedRows();
}


bool KDChartHiLoPainter::isNormalMode() const
{
    return KDChartParams::HiLoNormal == params()->hiLoChartSubType();
}

int KDChartHiLoPainter::clipShiftUp( bool, double ) const
{
    return 0;
}

void KDChartHiLoPainter::specificPaintData( QPainter* painter,
        const QRect& ourClipRect,
        KDChartTableDataBase* data,
        KDChartDataRegionList* /*regions*/,
        const KDChartAxisParams* axisPara,
        bool /*bNormalMode*/,
        uint /*chart*/,
        double logWidth,
        double areaWidthP1000,
        double logHeight,
        double axisYOffset,
        double /*minColumnValue*/,
        double /*maxColumnValue*/,
        double /*columnValueDistance*/,
        uint chartDatasetStart,
        uint chartDatasetEnd,
        uint datasetStart,
        uint datasetEnd )
{
    double areaHeightP1000 = logHeight / 1000.0;
    double averageValueP1000 = ( areaWidthP1000 + areaHeightP1000 ) / 2.0;
    int datasetNum=abs(static_cast<int>(chartDatasetEnd-chartDatasetStart))+1;

    painter->setPen( params()->outlineDataColor() );

    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    int numValues = 0;
    if ( params()->numValues() != -1 )
        numValues = params()->numValues();
    else
        numValues = data->usedCols();

    // We need to make sure that we have a certain number of
    // cells in the dataset(s), depending on the sub type to display.
    if( (numValues < 2) ||
        ((params()->hiLoChartSubType() == KDChartParams::HiLoClose)     && (numValues < 3)) ||
        ((params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose) && (numValues < 4)) ){
        qDebug( "\nNot enough data to display a High/Low Chart!\n" );
        qDebug( "type                 requiring" );
        qDebug( "----                 ---------" );
        qDebug( "High/Low             2 data cells per series" );
        qDebug( "High/Low/Close       3 data cells per series" );
        qDebug( "High/Low/open/Close  4 data cells per series\n" );
        return; // PENDING(kalle) Throw exception?
    }

    double pixelsPerUnit = 0.0;
    if( 0.0 != axisPara->trueAxisHigh() - axisPara->trueAxisLow() )
        pixelsPerUnit = logHeight / (axisPara->trueAxisHigh() - axisPara->trueAxisLow());
    else
        pixelsPerUnit = logHeight / 10;

    // Distance between the individual "stocks"
    double pointDist = logWidth / (double)datasetNum;

    // compute the position of the 0 axis
    double zeroXAxisI = axisPara->axisZeroLineStartY() - _dataRect.y();

    const int nLineWidth = params()->lineWidth();

    // Loop over the datasets, draw one "stock" line for each series.
    for ( uint dataset  = chartDatasetStart;
            dataset <= chartDatasetEnd;
            ++dataset ) {
        // The first and the second col are always high and low; we sort them
        // accordingly.
        QVariant valueA;
        QVariant valueB;
        if( dataset >= datasetStart &&
            dataset <= datasetEnd &&
            data->cellCoord( dataset, 0, valueA, 1 ) &&
            data->cellCoord( dataset, 1, valueB, 1 ) &&
            QVariant::Double == valueA.type() &&
            QVariant::Double == valueB.type() ){
            const double cellValue1 = valueA.toDouble();
            const double cellValue2 = valueB.toDouble();
            const double lowValue  = QMIN( cellValue1, cellValue2 );
            const double highValue = QMAX( cellValue1, cellValue2 );
            const double lowDrawValue = lowValue * pixelsPerUnit;
            const double highDrawValue = highValue * pixelsPerUnit;

            painter->setPen( QPen( params()->dataColor( dataset ),
                             nLineWidth ) );
            // draw line from low to high
            int xpos = static_cast<int>(
                        pointDist * ( (double)(dataset-chartDatasetStart) + 0.5 ) );
            int lowYPos  = static_cast<int>( zeroXAxisI - lowDrawValue );
            int highYPos = static_cast<int>( zeroXAxisI - highDrawValue );

            painter->drawLine( xpos, lowYPos, xpos, highYPos );

            // Find out how long open/close lines need to be in case we
            // need them. We make them 1/10 of the space available for one
            // "stock".
            int openCloseTickLength = static_cast<int>( pointDist * 0.1 );
            // we need these here because we might need to consider these
            // later when drawing the low and high labels
            bool hasOpen = false, hasClose = false;
            double openValue = 0.0, openDrawValue = 0.0,
            closeValue = 0.0, closeDrawValue = 0.0;

            // if we have an open/close chart, show the open value
            if( params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) {
                // Only do this if there is a value in the third col.
                if( data->cellCoord( dataset, 2, valueA, 1 ) &&
                    QVariant::Double == valueA.type() ) {
                    hasOpen = true;
                    openValue = valueA.toDouble();
                    openDrawValue = openValue * pixelsPerUnit;
                    painter->drawLine( xpos - openCloseTickLength,
                            static_cast<int>( zeroXAxisI - openDrawValue ),
                            xpos,
                            static_cast<int>( zeroXAxisI - openDrawValue ) );
                }
            }

            // If we have an open/close chart or a close chart, show the
            // close value, but only if there is a value in the
            // corresponding column (2 for HiLoClose, 3 for
            // HiLoOpenClose).
            if( ( params()->hiLoChartSubType() == KDChartParams::HiLoClose &&
                  data->cellCoord( dataset, 2, valueA, 1 ) &&
                  QVariant::Double == valueA.type() ) ||
                ( params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose &&
                  data->cellCoord( dataset, 3, valueB, 1 ) &&
                  QVariant::Double == valueB.type() ) ) {
                hasClose = true;
                closeValue = ( params()->hiLoChartSubType() == KDChartParams::HiLoClose )
                           ? valueA.toDouble() 
                           : valueB.toDouble();
                closeDrawValue = closeValue * pixelsPerUnit;
                painter->drawLine( xpos,
                        static_cast<int>( zeroXAxisI - closeDrawValue ),
                        xpos + openCloseTickLength,
                        static_cast<int>( zeroXAxisI - closeDrawValue ) );
            }

            // Draw the low value, if requested.
            if( params()->hiLoChartPrintLowValues() ) {
                // PENDING(kalle) Number formatting?
                QFont theFont( params()->hiLoChartLowValuesFont() );
                if ( params()->hiLoChartLowValuesUseFontRelSize() ) {
                    int nTxtHeight =
                        static_cast < int > ( params()->hiLoChartLowValuesFontRelSize()
                                * averageValueP1000 );
                    theFont.setPointSizeFloat( nTxtHeight );
                }
                KDChartTextPiece lowText( painter, QString::number( lowValue ),
                        theFont );
                int width = lowText.width();
                int height = lowText.height();

                // Check whether there is enough space below the data display
                int valX = 0, valY = 0;
                //qDebug("\nzeroXAxisI %f   lowDrawValue %f   height %i   logHeight %f   _dataRect.y() %i   axisYOffset %f",zeroXAxisI,highDrawValue,height,logHeight,_dataRect.y(),axisYOffset);
                //qDebug("zeroXAxisI - lowDrawValue + height %f    <   axisYOffset + logHeight %f",
                //zeroXAxisI - lowDrawValue + height, axisYOffset+logHeight);
                if( zeroXAxisI - lowDrawValue + height < axisYOffset+logHeight ) {
                    // enough space
                    valX = xpos - ( width / 2 );
                    valY = (int)lowDrawValue - lowText.fontLeading();
                } else {
                    // not enough space - move to left
                    if( !hasOpen || height < openDrawValue ) {
                        // Either there is no open value or it is high enough
                        // that we can put the low value to the left.
                        valX = xpos - width - nLineWidth;
                        valY = static_cast<int>(zeroXAxisI)
                                - lowYPos
                                + height/2
                                + nLineWidth/2;
                    }
                }
                lowText.draw( painter,
                              valX, static_cast<int>( zeroXAxisI - valY ),
                              ourClipRect,
                              params()->hiLoChartLowValuesColor() );
            }

            // Draw the high value, if requested.
            if( params()->hiLoChartPrintHighValues() ) {
                // PENDING(kalle) Number formatting?
                QFont theFont( params()->hiLoChartHighValuesFont() );
                if ( params()->hiLoChartHighValuesUseFontRelSize() ) {
                    int nTxtHeight =
                        static_cast < int > ( params()->hiLoChartHighValuesFontRelSize()
                                * averageValueP1000 );
                    theFont.setPointSizeFloat( nTxtHeight );
                }
                KDChartTextPiece highText( painter, QString::number( highValue ),
                        theFont );
                int width = highText.width();
                int height = highText.height();

                // Check whether there is enough space above the data display
                int valX = 0, valY = 0;
                if( zeroXAxisI - highDrawValue - height > axisYOffset ) {
                    // enough space
                    valX = xpos - ( width / 2 );
                    valY = (int)highDrawValue + highText.fontLeading() + height;
                } else {
                    // not enough space - move to right
                    if( !hasClose ||
                        height < ( _dataRect.height() - closeDrawValue ) ) {
                        // Either there is no close value or it is low enough
                        // that we can put the high value to the right.
                        valX = xpos + nLineWidth;
                        valY = static_cast<int>(zeroXAxisI)
                                - highYPos
                                + height/2
                                - nLineWidth/2;
                    }
                }
                highText.draw( painter,
                               valX, static_cast<int>( zeroXAxisI - valY ),
                               ourClipRect,
                               params()->hiLoChartHighValuesColor() );
            }

            // Draw the open value, if requested.
            if( params()->hiLoChartPrintOpenValues() &&
                    params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) {
                // PENDING(kalle) Number formatting?
                QFont theFont( params()->hiLoChartOpenValuesFont() );
                if ( params()->hiLoChartOpenValuesUseFontRelSize() ) {
                    int nTxtHeight =
                        static_cast < int > ( params()->hiLoChartOpenValuesFontRelSize()
                                * averageValueP1000 );
                    theFont.setPointSizeFloat( nTxtHeight );
                }
                KDChartTextPiece openText( painter, QString::number( openValue ),
                        theFont );
                int width = openText.width();
                int height = openText.height();

                // We can pretty safely assume that there is always enough
                // space to the left and right of the data display.
                int valX = 0, valY = 0;
                valX = xpos - openCloseTickLength - width;
                valY = (int)openDrawValue + ( height / 2 );
                openText.draw( painter,
                        valX, static_cast<int>( zeroXAxisI - valY ),
                        ourClipRect,
                        params()->hiLoChartOpenValuesColor() );
            }

            // Draw the close value, if requested.
            if( params()->hiLoChartPrintCloseValues() &&
                    ( params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose
                      ||
                      params()->hiLoChartSubType() == KDChartParams::HiLoClose ) ) {
                // PENDING(kalle) Number formatting?
                QFont theFont( params()->hiLoChartCloseValuesFont() );
                if ( params()->hiLoChartCloseValuesUseFontRelSize() ) {
                    int nTxtHeight =
                        static_cast < int > ( params()->hiLoChartCloseValuesFontRelSize()
                                * averageValueP1000 );
                    theFont.setPointSizeFloat( nTxtHeight );
                }
                KDChartTextPiece closeText( painter, QString::number( closeValue ),
                        theFont );
                //int width = closeText.width();
                int height = closeText.height();

                // We can pretty safely assume that there is always enough
                // space to the left and right of the data display.
                int valX = 0, valY = 0;
                valX = xpos + openCloseTickLength;
                valY = (int)closeDrawValue + ( height / 2 );
                closeText.draw( painter,
                        valX, static_cast<int>( zeroXAxisI - valY ),
                        ourClipRect,
                        params()->hiLoChartCloseValuesColor() );
            }

        } else
            continue; // we cannot display this value
    }
}
