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
#include "KDChartLinesPainter.h"
#include <KDChartParams.h>

#include <qpainter.h>

#if defined( __WINDOWS__ ) || defined( _SGIAPI )
#include <math.h>
#else
#include <cmath>
#include <stdlib.h>
#endif

#if defined( __WINDOWS__ ) || defined( SUN7 ) || defined( _SGIAPI ) || ( defined HP11_aCC && defined HP1100 )
#define std
#endif

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
                                     KDChartTableDataBase* data,
                                     bool paint2nd,
                                     KDChartDataRegionList* regions )
{
    paintDataInternal( painter, data,
                       true,  // center points
                       params()->lineMarker() && !params()->threeDLines(),  // line markers yes/no, 3D lines have no markers
                       false,  // not an area
                       paint2nd,
                       regions );
}

/**
   \internal

   Does the actual painting of a line or an area chart and is provided
   with the appropriate parameters from \c
   KDChartLinesPainter::paintData() and
   KDChartAreaPainter::paintData().

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
        KDChartTableDataBase* data,
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
    const bool showThreeDLines = !isArea && params()->threeDLines();

    enum { Normal, Stacked, Percent } mode;
    if (    (    ( params_chartType             == KDChartParams::Line )
              && ( params()->lineChartSubType() == KDChartParams::LineNormal ) )
         || (    ( params_chartType             == KDChartParams::Area )
              && ( params()->areaChartSubType() == KDChartParams::AreaNormal ) ) )
        mode = Normal;
    else if (    (    ( params_chartType             == KDChartParams::Line )
                   && ( params()->lineChartSubType() == KDChartParams::LineStacked ) )
              || (    ( params_chartType             == KDChartParams::Area )
                   && ( params()->areaChartSubType() == KDChartParams::AreaStacked ) ) )
        mode = Stacked;
    else if (    (    ( params_chartType             == KDChartParams::Line )
                   && ( params()->lineChartSubType() == KDChartParams::LinePercent ) )
              || (    ( params_chartType             == KDChartParams::Area )
                   && ( params()->areaChartSubType() == KDChartParams::AreaPercent ) ) )
        mode = Percent;
    else
        qDebug( "Internal error in KDChartLinesPainter::paintDataInternal(): Unknown subtype" );

    double logHeight = _dataRect.height();
    double logWidth = _dataRect.width();
    //double areaWidthP1000 = logWidth / 1000.0;

    QRect ourClipRect( _dataRect );
    ourClipRect.setBottom( ourClipRect.bottom() - 1 ); // protect axes
    ourClipRect.setLeft( ourClipRect.left() + 1 );
    ourClipRect.setRight( ourClipRect.right() - 1 );
    //
    // PENDING(khz) adjust the clip rect if necessary...
    //

    const QWMatrix & world = painter->worldMatrix();
    ourClipRect = world.mapRect( ourClipRect );

    painter->setClipRect( ourClipRect );

    painter->translate( _dataRect.x(), _dataRect.y() );

    uint datasetStart, datasetEnd;
    if ( params()->neverUsedSetChartSourceMode()
            || !params()->findDataset( KDChartParams::DataEntry,
                                        datasetStart,
                                        datasetEnd,
                                        chart ) ) {
        int maxRow, maxRowMinus1;
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
    //uint datasetNum = static_cast < int > ( abs( ( datasetEnd - datasetStart ) + 1.0 ) );

    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    int numValues = 0;
    if ( params()->numValues() != -1 )
        numValues = params()->numValues();
    else
        numValues = data->usedCols();


    double maxColumnValue = 0.0;
    double minColumnValue = 0.0;
    switch ( mode ) {
    case Normal:
        maxColumnValue = axisPara.trueAxisHigh();
        minColumnValue = axisPara.trueAxisLow();
        break;
    case Stacked:
        maxColumnValue =  QMAX( data->maxColSum(), 0.0 );
        minColumnValue =  QMIN( data->minColSum(), 0.0 );
        break;
    case Percent:
        maxColumnValue =  100.0;
        minColumnValue =    0.0;
        break;
    }
//qDebug("\nmax: %f",maxColumnValue);
//qDebug("min: %f",minColumnValue);
    double columnValueDistance = maxColumnValue - minColumnValue;

    double pixelsPerUnit = logHeight / columnValueDistance;
//qDebug("pixelsPerUnit: %f",pixelsPerUnit);
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
        else if( maxColumnValue == 0.0 )
            zeroXAxisI = 0.0;
        else
            zeroXAxisI = _dataRect.height() / 2.0;
    } else
        zeroXAxisI = axisPara.axisZeroLineStartY() - _dataRect.y();
//qDebug("zeroXAxisI: %f",zeroXAxisI);

    // compute how to shift of the points in case we want them in the
    // middle of their respective columns
    int xShift = centerThePoints ? static_cast < int > ( pointDist * 0.5 ) : 0;

    QPointArray previousPoints; // no vector since only areas need it,
                                // and these do not support 3d yet
    for ( int dataset = datasetEnd; ( dataset >= (int)datasetStart && dataset >= 0 ); --dataset ) {
        // the +2 is for the areas (if any)
        QPtrVector< QPointArray > points( 2 );
        points.setAutoDelete( true );
        int i = 0;
        for( i = 0; i < 2; ++i )
            points.insert( i, new QPointArray( numValues + 2 ) );
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
                    drawValue = ( ( cellValue + currentValueSums[ value ] ) / valueTotal ) * 100.0 * pixelsPerUnit;
                else // LineNormal or AreaNormal
                    drawValue = cellValue * pixelsPerUnit;

                QPoint p( static_cast < int > ( pointDist * ( double ) value ) + xShift,
                          static_cast < int > ( zeroXAxisI - drawValue ) );

                // For 2D lines, we need two points (that lie
                // behind each other on the Z axis). For 2D lines and
                // areas, we need only one point.
                if( showThreeDLines ) {
                    points[0]->setPoint( point, project( p.x(), p.y(),
                                                         (datasetStart+dataset)*params()->threeDLineDepth() ) );
                    points[1]->setPoint( point, project( p.x(), p.y(),
                                                         (datasetStart+dataset+1)*params()->threeDLineDepth() ) );
                } else
                    // 2D lines or areas
                    points[0]->setPoint( point, p );
                point++;

                // draw markers if necessary
                if ( drawMarkers )
                    drawMarker( painter, params()->lineMarkerStyle( dataset ),
                                params()->dataColor( dataset ), p,
                                dataset, value, chart, regions );
                else if( regions ) {
                    QRect rect( QPoint( p.x() - 1, p.y() - 1 ), QPoint( p.x() + 1, p.y() + 1 ) );
                    rect.moveBy( _dataRect.x(), _dataRect.y() );
                    regions->append( new KDChartDataRegion( QRegion( rect ),
                                                            dataset,
                                                            value,
                                                            chart ) );
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
                if ( mode == Normal || dataset == (int)datasetEnd ) {
                    /// first dataset (or any dataset in normal mode, where
                    /// the datasets overwrite each other)

                    // no 3d handling for areas yet
                    QPoint lastPoint = points[0]->point( point - 1 );

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

                    // no 3d handling for areas yet
                    points[0]->setPoint( point, lastPoint.x(), yCoord );
                    point++;

                    QPoint firstPoint = points[0]->point( 0 );
                    points[0]->setPoint( point, firstPoint.x(), yCoord );
                    point++;
                    painter->drawPolygon( *points[0], false, 0, point );
                    // remove the last two points added
                    point -= 2;
//qDebug("\n111");
                } else {
                    // don't mess around with the original array; we'll need
                    // that for the next time through.

//qDebug("222");
                    // no 3d handling for areas yet
                    QPointArray thisSection = points[0]->copy();

                    thisSection.resize( point + previousPoints.size() );
                    // append the previous array (there is guaranteed to be
                    // one because we are at least the second time through
                    // here) in reverse order
                    for ( unsigned int i = 0; i < previousPoints.size(); ++i ) {
                        thisSection.setPoint( point + i,
                                              previousPoints.point( previousPoints.size() - i - 1 ) );
//qDebug("\nx: %i",previousPoints.point( previousPoints.size() - i - 1 ).x());
//qDebug("y: %i",previousPoints.point( previousPoints.size() - i - 1 ).y());
}
                    painter->drawPolygon( thisSection );
                }
                // draw the line with no brush and outline color
                painter->setBrush( Qt::NoBrush );
                painter->setPen( QPen( params()->outlineDataColor(),
                                       params()->outlineDataLineWidth() ) );
            } else {
                // line
                if( showThreeDLines ) {
                    // draw the line with the data color brush and the
                    // outline data pen if it is 3D
                    painter->setBrush( params()->dataColor( dataset ) );
                    painter->setPen( QPen( params()->outlineDataColor(),
                                           params()->outlineDataLineWidth() ) );
                } else {
                    // draw the line with no brush and the data color if
                    // it is 2D
                    painter->setBrush( Qt::NoBrush );
                    painter->setPen( QPen( params()->dataColor( dataset ),
                                           params()->lineWidth() ) );
                }
            }

            // Do not draw the contour line if this is the last row in a
            // percent chart.
            if ( mode != Percent || dataset != (int)datasetEnd )
                if( showThreeDLines ) {
                    // A 3D line needs to be drawn piece-wise
                    for ( int value = 0; value < numValues-1; value++ ) {
                        QPointArray segment( 4 );
                        segment.setPoint( 0, points[0]->point( value ) );
                        segment.setPoint( 1, points[0]->point( value+1 ) );
                        segment.setPoint( 2, points[1]->point( value+1 ) );
                        segment.setPoint( 3, points[1]->point( value ) );
                        painter->drawPolygon( segment );
                    }
                } else
                    painter->drawPolyline( *points[0], 0, point );
        }

        // Save point array for next way through (needed for e.g. stacked
        // areas), not for 3D currently
        points[0]->resize( point );
        previousPoints = points[0]->copy();
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
                                      uint dataset, uint value, uint chart,
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
            // Don't use rect for drawing after this!
            rect.moveBy( _dataRect.x(), _dataRect.y() );
            if ( regions )
                regions->append( new KDChartDataRegion( QRegion( rect ),
                                                        dataset,
                                                        value,
                                                        chart ) );
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
            // Don't use points for drawing after this!
            points.translate( _dataRect.x(), _dataRect.y() );
            if ( regions )
                regions->append( new KDChartDataRegion( QRegion( points ),
                                                        dataset,
                                                        value,
                                                        chart ) );
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
            // Don't use points for drawing after this!
            points.translate( _dataRect.x(), _dataRect.y() );
                if( points.size() > 0 )
                    regions->append( new KDChartDataRegion( QRegion( points ),
                                                            dataset,
                                                            value,
                                                            chart ) );
            }
            painter->restore();
        }
    };
}

#define DEGTORAD(d) (d)*M_PI/180

/*!
  Projects a point in a space defined by its x, y, and z coordinates
  into a point onto a plane, given two rotation angles around the x
  resp. y axis.
*/
QPoint KDChartLinesPainter::project( int x, int y, int z ) {
    double xrad = DEGTORAD( params()->threeDLineXRotation() );
    double yrad = DEGTORAD( params()->threeDLineYRotation() );
    QPoint ret( static_cast<int>( x*cos( yrad ) + z * sin( yrad ) ),
                static_cast<int>( y*cos( xrad ) - z * sin( xrad ) ) );
    return ret;
  }
