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
   Paints the actual data area and registers the region for the data
   points if \a regions is not 0.

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
   \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
   \param regions a pointer to a list of regions that will be filled
   with regions representing the data segments, if not null
*/
void KDChartHiLoPainter::paintData( QPainter* painter,
                                    KDChartTableData* data,
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

    const KDChartParams::ChartType params_chartType
        = paint2nd ? params()->additionalChartType() : params()->chartType();

    double logHeight = _dataRect.height();
    double logWidth = _dataRect.width();
    double areaWidthP1000 = logWidth / 1000.0;
    QRect ourClipRect( _dataRect );
    ourClipRect.setHeight( ourClipRect.height() - 1 );
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


    const KDChartAxisParams& axisPara
        = params()->axisParams( paint2nd
                                ? KDChartAxisParams::AxisPosRight
                                : KDChartAxisParams::AxisPosLeft );

    uint datasetStart, datasetEnd;
    if ( params()->neverUsedSetChartSourceMode()
         || !params()->findDataset( KDChartParams::DataEntry,
                                    datasetStart,
                                    datasetEnd,
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
        datasetStart = paint2nd ? maxRow : 0;
        datasetEnd = paint2nd ? maxRow
                     : ( ( KDChartParams::NoType
                           == params()->additionalChartType() )
                         ? maxRow
                         : maxRowMinus1 );
    }

    uint datasetNum = static_cast < uint > ( abs( ( chartDatasetEnd - chartDatasetStart ) + 1 ) );

    // We need to make sure that we have a certain number of
    // datasets, depending on the sub type to display.
    if( ( params()->hiLoChartSubType() == KDChartParams::HiLoSimple &&
          datasetNum < 2 ) ||
        ( params()->hiLoChartSubType() == KDChartParams::HiLoClose &&
          datasetNum < 3 ) ||
        ( params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose &&
          datasetNum < 4 ) ) {
        qDebug( "Not enough data to chart" );
        return; // PENDING(kalle) Throw exception?
    }

    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    int numValues = 0;
    if ( params()->numValues() != -1 )
        numValues = params()->numValues();
    else
        numValues = data->usedCols();

    double maxColumnValue = axisPara.trueAxisHigh();
    double minColumnValue = axisPara.trueAxisLow();
    double columnValueDistance = maxColumnValue - minColumnValue;

    double pixelsPerUnit = 0.0;
    pixelsPerUnit = logHeight / columnValueDistance;

    // Distance between the individual "stocks"
    double pointDist = logWidth / ( ( double ) numValues );

    // compute the position of the 0 axis
    double zeroXAxisI = axisPara.axisZeroLineStartY() - _dataRect.y();

    // Loop over the value, draw one "stock" line for each value.
    for( int value = 0; value < numValues; value++ ) {
        // We already know that we have enough datasets. The first and the
        // second dataset are always high and low; we sort them
        // accordingly.
        if( data->cell( 0, value ).isDouble() &&
            data->cell( 1, value ).isDouble() ) {
            double cellValue1 = data->cell( 0, value ).doubleValue();
            double cellValue2 = data->cell( 1, value ).doubleValue();
            double lowValue = QMIN( cellValue1, cellValue2 );
            double highValue = QMAX( cellValue1, cellValue2 );
            double lowDrawValue = lowValue * pixelsPerUnit;
            double highDrawValue = highValue * pixelsPerUnit;

            // It is correct that we use the value to index the data color
            // here, not the dataset as with the other charts.
            painter->setPen( QPen( params()->dataColor( value ),
                                   params()->lineWidth() ) );
            // draw line from low to high
            int xpos = static_cast<int>( pointDist * ( (double)value + 0.5
                                                       ) );
            painter->drawLine( xpos,
                               static_cast<int>( zeroXAxisI - lowDrawValue ),
                               xpos,
                               static_cast<int>( zeroXAxisI - highDrawValue ) );

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
                // Only do this if there is a value in the third dataset.
                if( data->cell( 2, value ).isDouble() ) {
                    hasOpen = true;
                    openValue = data->cell( 2, value ).doubleValue();
                    openDrawValue = openValue * pixelsPerUnit;
                    painter->drawLine( xpos - openCloseTickLength,
                                       static_cast<int>( zeroXAxisI - openDrawValue ),
                                       xpos,
                                       static_cast<int>( zeroXAxisI - openDrawValue ) );
                }
            }

            // If we have an open/close chart or a close chart, show the
            // close value, but only if there is a value in the
            // corresponding dataset (2 for HiLoClose, 3 for
            // HiLoOpenClose).
            if( ( params()->hiLoChartSubType() == KDChartParams::HiLoClose &&
                  data->cell( 2, value ).isDouble() ) ||
                ( params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose &&
                  data->cell( 3, value ).isDouble() ) ) {
                hasClose = true;
                closeValue = ( params()->hiLoChartSubType() == KDChartParams::HiLoClose ) ?
                             data->cell( 2, value ).doubleValue() :
                           data->cell( 3, value ).doubleValue();
                closeDrawValue = closeValue * pixelsPerUnit;
                painter->drawLine( xpos,
                                   static_cast<int>( zeroXAxisI - closeDrawValue ),
                                   xpos + openCloseTickLength,
                                   static_cast<int>( zeroXAxisI - closeDrawValue ) );
            }

            // Draw the low value, if requested.
            if( params()->hiLoChartPrintLowValues() ) {
                // PENDING(kalle) Number formatting?
                KDChartTextPiece lowText( QString::number( lowValue ),
                                          params()->hiLoChartLowValuesFont() );
                int width = lowText.width();
                int height = lowText.height();

                // Check whether there is enough space below the data display
                int valX = 0, valY = 0;
                if( ( height + lowText.fontLeading() ) < lowDrawValue ) {
                    // enough space
                    valX = xpos - ( width / 2 );
                    valY = (int)lowDrawValue - lowText.fontLeading();
                } else {
                    // not enough space - move to left
                    if( !hasOpen || height < openDrawValue ) {
                        // Either there is no open value or it is high enough
                        // that we can put the low value to the left.
                        valX = xpos - width;
                        valY = height;
                    } else
                        ; // no way to draw it (really?)
                }
                lowText.draw( painter,
                              valX, zeroXAxisI - valY,
                              ourClipRect,
                              params()->hiLoChartLowValuesColor() );
            }

            // Draw the high value, if requested.
            if( params()->hiLoChartPrintHighValues() ) {
                // PENDING(kalle) Number formatting?
                KDChartTextPiece highText( QString::number( highValue ),
                                           params()->hiLoChartHighValuesFont() );
                int width = highText.width();
                int height = highText.height();

                // Check whether there is enough space above the data display
                int valX = 0, valY = 0;
                if( ( height + highText.fontLeading() ) <
                    ( _dataRect.height() - highDrawValue ) ) {
                    // enough space
                    valX = xpos - ( width / 2 );
                    valY = (int)highDrawValue + highText.fontLeading() + height;
                } else {
                    // not enough space - move to right
                    if( !hasClose ||
                        height < ( _dataRect.height() - closeDrawValue ) ) {
                        // Either there is no close value or it is low enough
                        // that we can put the high value to the right.
                        valX = xpos;
                        valY = _dataRect.height();
                    } else
                        ; // no way to draw it (really?)
                }
                highText.draw( painter,
                               valX, zeroXAxisI - valY,
                               ourClipRect,
                               params()->hiLoChartHighValuesColor() );
            }

            // Draw the open value, if requested.
            if( params()->hiLoChartPrintOpenValues() &&
                params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose ) {
                // PENDING(kalle) Number formatting?
                KDChartTextPiece openText( QString::number( openValue ),
                                           params()->hiLoChartOpenValuesFont() );
                int width = openText.width();
                int height = openText.height();

                // We can pretty safely assume that there is always enough
                // space to the left and right of the data display.
                int valX = 0, valY = 0;
                valX = xpos - openCloseTickLength - width;
                valY = (int)openDrawValue + ( height / 2 );
                openText.draw( painter,
                               valX, zeroXAxisI - valY,
                               ourClipRect,
                               params()->hiLoChartOpenValuesColor() );
            }

            // Draw the close value, if requested.
            if( params()->hiLoChartPrintCloseValues() &&
                ( params()->hiLoChartSubType() == KDChartParams::HiLoOpenClose
                  ||
                  params()->hiLoChartSubType() == KDChartParams::HiLoClose ) ) {
                // PENDING(kalle) Number formatting?
                KDChartTextPiece closeText( QString::number( closeValue ),
                                           params()->hiLoChartCloseValuesFont() );
                int width = closeText.width();
                int height = closeText.height();

                // We can pretty safely assume that there is always enough
                // space to the left and right of the data display.
                int valX = 0, valY = 0;
                valX = xpos + openCloseTickLength;
                valY = (int)closeDrawValue + ( height / 2 );
                closeText.draw( painter,
                               valX, zeroXAxisI - valY,
                               ourClipRect,
                               params()->hiLoChartCloseValuesColor() );
            }

        } else
            continue; // we cannot display this value
    }
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
uint KDChartHiLoPainter::numLegendFallbackTexts( KDChartTableData* data ) const
{
    return data->usedCols();
}
