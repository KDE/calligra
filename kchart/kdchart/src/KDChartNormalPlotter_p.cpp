/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2007 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#include "KDChartPlotter.h"
#include "KDChartNormalPlotter_p.h"

#include <limits>

using namespace KDChart;

NormalPlotter::NormalPlotter( Plotter* d )
    : PlotterType( d )
{
}

Plotter::PlotType NormalPlotter::type() const
{
    return Plotter::Normal;
}

const QPair< QPointF, QPointF > NormalPlotter::calculateDataBoundaries() const
{
    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();
    double xMin = std::numeric_limits< double >::quiet_NaN();
    double xMax = std::numeric_limits< double >::quiet_NaN();
    double yMin = std::numeric_limits< double >::quiet_NaN();
    double yMax = std::numeric_limits< double >::quiet_NaN();

    for( int column = 0; column < colCount; ++column )
    {
        for ( int row = 0; row < rowCount; ++row )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, column );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );

            const double valueX = ISNAN( point.key ) ? 0.0 : point.key;
            const double valueY = ISNAN( point.value ) ? 0.0 : point.value;

            if( ISNAN( xMin ) )
            {
                xMin = valueX;
                xMax = valueX;
                yMin = valueY;
                yMax = valueY;
            }
            else
            {
                xMin = qMin( xMin, valueX );
                xMax = qMax( xMax, valueX );
                yMin = qMin( yMin, valueY );
                yMax = qMax( yMax, valueY );
            }
        }
    }

    // NOTE: calculateDataBoundaries must return the *real* data boundaries!
    //       i.e. we may NOT fake yMin to be qMin( 0.0, yMin )
    //       (khz, 2008-01-24)
    const QPointF bottomLeft( QPointF( xMin, yMin ) );
    const QPointF topRight( QPointF( xMax, yMax ) );
    return QPair< QPointF, QPointF >( bottomLeft, topRight );
}

void NormalPlotter::paint( PaintContext* ctx )
{
    reverseMapper().clear();

    Q_ASSERT( dynamic_cast< CartesianCoordinatePlane* >( ctx->coordinatePlane() ) );
    const CartesianCoordinatePlane* const plane = static_cast< CartesianCoordinatePlane* >( ctx->coordinatePlane() );
    const int colCount = compressor().modelDataColumns();
    const int rowCount = compressor().modelDataRows();

    if( colCount == 0 || rowCount == 0 )
        return;

    DataValueTextInfoList textInfoList;
    LineAttributes::MissingValuesPolicy policy; // ???
    
    for( int column = 0; column < colCount; ++column )
    {
        LineAttributesInfoList lineList;
        LineAttributes laPreviousCell;
        CartesianDiagramDataCompressor::CachePosition previousCellPosition;

        for( int row = 0; row < rowCount; ++row )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, column );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            if( ISNAN( point.key ) || ISNAN( point.value ) )
            {
                previousCellPosition = CartesianDiagramDataCompressor::CachePosition();
                continue;
            }

            LineAttributes laCell;
            
            const QModelIndex sourceIndex = attributesModel()->mapToSource( point.index );
            const CartesianDiagramDataCompressor::DataPoint lastPoint = compressor().data( previousCellPosition );
            // area corners, a + b are the line ends:
            const QPointF a( plane->translate( QPointF( lastPoint.key, lastPoint.value ) ) );
            const QPointF b( plane->translate( QPointF( point.key, point.value ) ) );
            const QPointF c( plane->translate( QPointF( lastPoint.key, 0.0 ) ) );
            const QPointF d( plane->translate( QPointF( point.key, 0.0 ) ) );
            // add the line to the list:
            laCell = diagram()->lineAttributes( sourceIndex );
            // add data point labels:
            const PositionPoints pts = PositionPoints( b, a, d, c );
            // if necessary, add the area to the area list:
            QList<QPolygonF> areas;
            if ( laCell.displayArea() ) {
                QPolygonF polygon;
                polygon << a << b << d << c;
                areas << polygon;
            }
            // add the pieces to painting if this is not hidden:
            if ( !point.hidden /*&& !ISNAN( lastPoint.key ) && !ISNAN( lastPoint.value ) */) {
                appendDataValueTextInfoToList( diagram(), textInfoList, sourceIndex, pts,
                                               Position::NorthWest, Position::SouthWest,
                                               1.0 );//point.value );
                if( !ISNAN( lastPoint.key ) && !ISNAN( lastPoint.value ) )
                {
                    paintAreas( ctx, attributesModel()->mapToSource( lastPoint.index ), areas, laCell.transparency() );
                    lineList.append( LineAttributesInfo( sourceIndex, a, b ) );
                }
            }
            
            // wrap it up:
            previousCellPosition = position;
            laPreviousCell = laCell;
        }
        paintElements( ctx, textInfoList, lineList, policy );
    }
}
