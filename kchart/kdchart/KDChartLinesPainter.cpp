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

#include "KDChartLinesPainter.h"
#include <KDChartParams.h>

#include <qpainter.h>

/**
   \class KDChartLinesPainter KDChartLinesPainter.h

   \brief A chart painter implementation that can paint line charts.
*/

/**
   Constructor. Sets up internal data structures as necessary.

   \param params the KDChartParams structure that defines the chart
   \param data the data that will be displayed as a chart
*/
KDChartLinesPainter::KDChartLinesPainter( KDChartParams* params ) :
KDChartAxesPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
   Destructor.
*/
KDChartLinesPainter::~KDChartLinesPainter()
{
    // intentionally left blank
}


/**
   Paints the actual data area. Data regions will only be added if \a
   regions is not 0 and the chart is configured to be drawn with
   markers.

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
   \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
   \param regions a pointer to a list of regions that will be filled
   with regions representing the data segments, if not null
*/
void KDChartLinesPainter::paintData( QPainter* painter,
                                     KDChartTableData* data,
                                     bool paint2nd,
                                     KDChartDataRegionList* regions )
{
    paintDataInternal( painter, data,
                       true,  // center points
                       params()->lineMarker(),  // line markers yes/no
                       false,  // not an area
                       paint2nd,
                       regions );
}

/**
   \internal

   Does the actual painting of a line or an area chart and is provided
   with the appropriate parameters from \c
   KDChartLinesPainter::paintData() and
   KDChartAreaPainter::paintDat().

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
   \param centerThePoints if true, the value points will be centered in
   their segment (typically used for line charts), if false, the first
   value point will be on the ordinate, the second one at the right
   margin of the chart.
   \param isArea true if an area will be drawn, false for lines. Using
   this parameter should be avoided.
   \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
   \param regions a pointer to a list of regions that will be filled
   with regions representing the data segments, if not null
*/
void KDChartLinesPainter::paintDataInternal( QPainter* painter,
        KDChartTableData* data,
        bool centerThePoints,
        bool drawMarkers,
        bool isArea,
        bool paint2nd,
        KDChartDataRegionList* regions )
{
    uint chart = paint2nd ? 1 : 0;
    const KDChartAxisParams& axisPara
        = params()->axisParams( paint2nd
                                ? KDChartAxisParams::AxisPosRight
                                : KDChartAxisParams::AxisPosLeft );
    const KDChartParams::ChartType params_chartType
        = paint2nd ? params()->additionalChartType()
        : params()->chartType();

    enum { Normal, Stacked, Percent } mode;
    if ( ( ( params_chartType == KDChartParams::Line ) &&
            ( params()->lineChartSubType() == KDChartParams::LineNormal ) ) ||
            ( ( params_chartType == KDChartParams::Area ) &&
              ( params()->areaChartSubType() == KDChartParams::AreaNormal ) ) )
        mode = Normal;
    else if ( ( ( params_chartType == KDChartParams::Line ) &&
                ( params()->lineChartSubType() == KDChartParams::LineStacked ) ) ||
              ( ( params_chartType == KDChartParams::Area ) &&
                ( params()->areaChartSubType() == KDChartParams::AreaStacked ) ) )
        mode = Stacked;
    else if ( ( ( params_chartType == KDChartParams::Line ) &&
                ( params()->lineChartSubType() == KDChartParams::LinePercent ) ) ||
              ( ( params_chartType == KDChartParams::Area ) &&
                ( params()->areaChartSubType() == KDChartParams::AreaPercent ) ) )
        mode = Percent;
    else
        qDebug( "Internal error in KDChartLinesPainter::paintDataInternal(): Unknown subtype" );

    double logHeight = _dataRect.height();
    double logWidth = _dataRect.width();
    double areaWidthP1000 = logWidth / 1000.0;

    QRect ourClipRect( _dataRect );
    ourClipRect.setBottom( ourClipRect.bottom() - 1 ); // protect axes
    ourClipRect.setLeft( ourClipRect.left() + 1 );
    ourClipRect.setRight( ourClipRect.right() - 1 );
    //
    // PENDING(khz) adjust the clip rect if neccessary...
    //
    painter->setClipRect( ourClipRect );

    painter->translate( _dataRect.x(), _dataRect.y() );

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
        datasetStart = paint2nd ? maxRow
                       : 0;
        datasetEnd = paint2nd ? maxRow
                     : ( ( KDChartParams::NoType
                           == params()->additionalChartType() )
                         ? maxRow
                         : maxRowMinus1 );
    }
    uint datasetNum = static_cast < uint > ( abs( ( datasetEnd - datasetStart ) + 1.0 ) );

    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    int numValues = 0;
    if ( params()->numValues() != -1 )
        numValues = params()->numValues();
    else
        numValues = data->usedCols();


    double maxColumnValue = 0.0;
    double minColumnValue = 0.0;
    if ( mode == Normal ) {
        maxColumnValue = axisPara.trueAxisHigh();
        minColumnValue = axisPara.trueAxisLow();
    } else { // Stacked or Percent
        maxColumnValue = QMAX( data->maxColSum(), 0.0 );
        minColumnValue = QMIN( data->minColSum(), 0.0 );
    }
    double columnValueDistance = maxColumnValue - minColumnValue;

    double pixelsPerUnit = 0.0;
    if ( mode != Percent )  // not used for percent
        pixelsPerUnit = logHeight / columnValueDistance;
    double pointDist = logWidth / ( ( double ) numValues
                                    - ( centerThePoints ? 0.0 : 1.0 ) );

    QMap < int, double > currentValueSums;
    if ( mode == Stacked || mode == Percent ) {
        // this array is only used for stacked and percent lines, no need
        // to waste time initializing it for normal types
        for ( int value = 0; value < numValues; value++ )
            currentValueSums[ value ] = 0.0;
    }
    QMap < int, double > totalValueSums;

    // compute the position of the 0 axis
    double zeroXAxisI;
    if ( mode == Percent ) {
        if ( minColumnValue == 0.0 )
            zeroXAxisI = _dataRect.height();
        else if ( maxColumnValue = 0.0 )
            zeroXAxisI = 0.0;
        else
            zeroXAxisI = _dataRect.height() / 2.0;
    } else
        zeroXAxisI = axisPara.axisZeroLineStartY() - _dataRect.y();

    // compute how to shift of the points in case we want them in the
    // middle of their respective columns
    int xShift = centerThePoints ? static_cast < int > ( pointDist * 0.5 ) : 0;

    QPointArray previousPoints;
    for ( uint dataset = datasetStart; dataset <= datasetEnd; dataset++ ) {
        // the +2 is for the areas (if any)
        QPointArray points( numValues + 2 );
        int point = 0;
        double valueTotal = 0.0; // Will only be used for Percent
        for ( int value = 0; value < numValues; value++ ) {
            if ( mode == Percent )
                valueTotal = data->colAbsSum( value );
            if ( data->cell( dataset, value ).isDouble() ) {
                double cellValue = data->cell( dataset, value ).doubleValue();
                double drawValue = 0.0;
                // PENDING(kalle) This does not work for AreaPercent yet
                if ( mode == Stacked )
                    drawValue = ( cellValue + currentValueSums[ value ] ) * pixelsPerUnit;
                else if ( mode == Percent )
                    drawValue = ( ( cellValue + currentValueSums[ value ] ) / valueTotal ) * zeroXAxisI;
                else // LineNormal or AreaNormal
                    drawValue = cellValue * pixelsPerUnit;

                QPoint p( static_cast < int > ( pointDist * ( double ) value ) + xShift,
                          static_cast < int > ( zeroXAxisI - drawValue ) );
                points.setPoint( point, p );
                point++;

                // draw markers if necessary
                if ( drawMarkers ) {
                    drawMarker( painter, params()->lineMarkerStyle( dataset ),
                                params()->dataColor( dataset ), p,
                                dataset, value, regions );
                }

                // calculate running sum for stacked and percent
                if ( mode == Stacked || mode == Percent ) {
                    currentValueSums[ value ] += cellValue;
                }
            }
        }
        if ( point ) {
            if ( isArea ) {
                // first draw with the fill brush, no pen, with the
                // zero axis points or upper border points added for the first
                // dataset or with the previous points reversed for all other
                // datasets.
                painter->setPen( QPen( Qt::NoPen ) );
                painter->setBrush( QBrush( params()->dataColor( dataset ),
                                           Qt::SolidPattern ) );
                if ( mode == Normal || dataset == datasetStart ) {
                    /// first dataset (or any dataset in normal mode, where
                    /// the datasets overwrite each other)
                    QPoint lastPoint = points.point( point - 1 );
                    // zeroXAxisI can be too far below the abscissa, but it's
                    // the only thing we have. Likewise can 0 be too far above
                    // the upper boundary, but again it's the only thing we
                    // have, at the rest is clipped anyway.
                    int yCoord;
                    if ( params()->areaLocation() == KDChartParams::AreaBelow ||
                            mode == Percent )
                        yCoord = ( int ) zeroXAxisI;
                    else
                        yCoord = 0;
                    points.setPoint( point, lastPoint.x(), yCoord );
                    point++;
                    QPoint firstPoint = points.point( 0 );
                    points.setPoint( point, firstPoint.x(), yCoord );
                    point++;
                    painter->drawPolygon( points, false, 0, point );
                    // remove the last two points added
                    point -= 2;
                } else {
                    // don't mess around with the original array; we'll need
                    // that for the next time through.
                    QPointArray thisSection = points.copy();
                    thisSection.resize( point + previousPoints.size() );
                    // append the previous array (there is guaranteed to be
                    // one because we are at least the second time through
                    // here) in reverse order
                    for ( int i = 0; i < previousPoints.size(); i++ )
                        thisSection.setPoint( point + i,
                                              previousPoints.point( previousPoints.size() - i - 1 ) );
                    painter->drawPolygon( thisSection );
                }
                // draw the line with no brush and outline color
                painter->setBrush( Qt::NoBrush );
                painter->setPen( QPen( params()->outlineDataColor(),
                                       params()->outlineDataLineWidth() ) );
            } else {
                // line
                // draw the line with no brush and the data color
                painter->setBrush( Qt::NoBrush );
                painter->setPen( QPen( params()->dataColor( dataset ),
                                       params()->lineWidth() ) );
            }

            // Do not draw the contour line if this is the last row in a
            // percent chart.
            if ( mode != Percent || dataset != datasetEnd )
                painter->drawPolyline( points, 0, point );
        }

        // Save point array for next way through (needed for e.g. stacked
        // areas)
        points.resize( point );
        previousPoints = points;
    }
    painter->translate( - _dataRect.x(), - _dataRect.y() );
}


/*!
  Draws the marker for one data point according to the specified style.

  \param painter the painter to draw on
  \param style what kind of marker is drawn (square, diamond or circle)
  \param color the color in which to draw the marker
  \param p the center of the marker
  \param dataset the dataset which this marker represents
  \param value the value which this marker represents
  \param regions a list of regions for data points, a new region for the new
  marker will be appended to this list if it is not 0
*/
void KDChartLinesPainter::drawMarker( QPainter* painter,
                                      KDChartParams::LineMarkerStyle style,
                                      const QColor& color, const QPoint& p,
                                      uint dataset, uint value,
                                      KDChartDataRegionList* regions )
{
    uint xsize = params()->lineMarkerSize().width();
    uint ysize = params()->lineMarkerSize().height();
    uint xsize2 = xsize / 2;
    uint ysize2 = ysize / 2;
    painter->setPen( color );
    switch ( style ) {
    case KDChartParams::LineMarkerSquare: {
            painter->save();
            painter->setBrush( color );
            QRect rect( QPoint( p.x() - xsize2, p.y() - ysize2 ), QPoint( p.x() + xsize2, p.y() + ysize2 ) );
            painter->drawRect( rect );
            if ( regions )
                regions->append( new KDChartDataRegion( QRegion( rect ),
                                                        dataset, value ) );
            painter->restore();
            break;
        }
    case KDChartParams::LineMarkerDiamond: {
            painter->save();
            painter->setBrush( color );
            QPointArray points( 4 );
            points.setPoint( 0, p.x() - xsize2, p.y() );
            points.setPoint( 1, p.x(), p.y() - ysize2 );
            points.setPoint( 2, p.x() + xsize2, p.y() );
            points.setPoint( 3, p.x(), p.y() + ysize2 );
            painter->drawPolygon( points );
            if ( regions )
                regions->append( new KDChartDataRegion( QRegion( points ),
                                                        dataset, value ) );
            painter->restore();
            break;
        }
    case KDChartParams::LineMarkerCircle:
    default: {
            painter->save();
            painter->setBrush( color );
            painter->drawEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
            if ( regions ) {
                QPointArray points;
                points.makeEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                regions->append( new KDChartDataRegion( QRegion( points ),
                                                        dataset, value ) );
            }
            painter->restore();
        }
    };
}

