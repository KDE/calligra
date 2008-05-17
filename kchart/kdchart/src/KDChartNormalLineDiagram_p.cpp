/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2005-2007 Klarälvdalens Datakonsult AB.  All rights reserved.
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
 **   information about KD Chart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include <limits>

#include <QAbstractItemModel>

#include "KDChartBarDiagram.h"
#include "KDChartLineDiagram.h"
#include "KDChartTextAttributes.h"
#include "KDChartAttributesModel.h"
#include "KDChartAbstractCartesianDiagram.h"
#include "KDChartNormalLineDiagram_p.h"

using namespace KDChart;

NormalLineDiagram::NormalLineDiagram( LineDiagram* d )
    : LineDiagramType( d )
{
}

LineDiagram::LineType NormalLineDiagram::type() const
{
    return LineDiagram::Normal;
}

const QPair< QPointF, QPointF > NormalLineDiagram::calculateDataBoundaries() const
{
    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();
    double xMin = 0.0;
    double xMax = diagram()->model() ? diagram()->model()->rowCount( diagram()->rootIndex() ) - 1 : 0;
    double yMin = std::numeric_limits< double >::quiet_NaN();
    double yMax = std::numeric_limits< double >::quiet_NaN();

    for( int column = 0; column < colCount; ++column )
    {
        for ( int row = 0; row < rowCount; ++row )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, column );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            const double value = ISNAN( point.value ) ? 0.0 : point.value;
            
            if ( ISNAN( yMin ) ) {
                    yMin = value;
                    yMax = value;
            } else {
                yMin = qMin( yMin, value );
                yMax = qMax( yMax, value );
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

void NormalLineDiagram::paint( PaintContext* ctx )
{
    reverseMapper().clear();
    Q_ASSERT( dynamic_cast<CartesianCoordinatePlane*>( ctx->coordinatePlane() ) );
    CartesianCoordinatePlane* plane = static_cast<CartesianCoordinatePlane*>( ctx->coordinatePlane() );
    const int columnCount = compressor().modelDataColumns();
    const int rowCount = compressor().modelDataRows();
    if ( columnCount == 0 || rowCount == 0 ) return; // maybe blank out the area?

// FIXME integrate column index retrieval to compressor:
// the compressor should only pass through visiblel columns
    int maxFound = 0;
//     {   // find the last column number that is not hidden
//         for( int column =  datasetDimension() - 1;
//              column <  columnCount;
//              column += datasetDimension() )
//             if( ! diagram()->isHidden( column ) )
//                 maxFound = column;
//     }
    maxFound = columnCount;
    // ^^^ temp

    DataValueTextInfoList textInfoList;
    LineAttributesInfoList lineList;

    LineAttributes::MissingValuesPolicy policy;

    for( int column  = 0; column < columnCount; ++column ) {
        LineAttributes laPreviousCell;
        CartesianDiagramDataCompressor::DataPoint lastPoint;

        CartesianDiagramDataCompressor::CachePosition previousCellPosition;
        for ( int row = 0; row < rowCount; ++row ) {
            const CartesianDiagramDataCompressor::CachePosition position( row, column );
            // get where to draw the line from:
            CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            
            const QModelIndex sourceIndex = attributesModel()->mapToSource( point.index );

            const LineAttributes laCell = diagram()->lineAttributes( sourceIndex );
            const LineAttributes::MissingValuesPolicy policy = laCell.missingValuesPolicy();
            
            if( ISNAN( point.value ) )
            {
                switch( policy )
                {
                case LineAttributes::MissingValuesAreBridged:
                    // we just bridge both values
                    continue;
                case LineAttributes::MissingValuesShownAsZero:
                    // set it to zero
                    point.value = 0.0;
                    break;
                case LineAttributes::MissingValuesHideSegments:
                    // they're just hidden
                    break;
                default:
                    break;
                    // hm....
                }
            }
            
            // area corners, a + b are the line ends:
            const QPointF a( plane->translate( QPointF( lastPoint.key, lastPoint.value ) ) );
            const QPointF b( plane->translate( QPointF( point.key, point.value ) ) );
            const QPointF c( plane->translate( QPointF( lastPoint.key, 0.0 ) ) );
            const QPointF d( plane->translate( QPointF( point.key, 0.0 ) ) );
            // add the line to the list:
           // add data point labels:
            const PositionPoints pts = PositionPoints( b, a, d, c );
            // if necessary, add the area to the area list:
            QList<QPolygonF> areas;
            if ( !ISNAN( point.value ) && !ISNAN( lastPoint.value ) && laCell.displayArea() ) {
                areas << ( QPolygonF() << a << b << d << c );//polygon;
            }
            // add the pieces to painting if this is not hidden:
            if ( ! point.hidden && !ISNAN( point.value ) )
            {
                appendDataValueTextInfoToList( diagram(), textInfoList, sourceIndex, pts,
                                               Position::NorthWest, Position::SouthWest,
                                               point.value );
                paintAreas( ctx, attributesModel()->mapToSource( lastPoint.index ), areas, laCell.transparency() );
                // position 0 is not really painted, since it takes two points to make a line :-)
                if( row > 0 && !ISNAN( lastPoint.value ) )
                    lineList.append( LineAttributesInfo( sourceIndex, a, b ) );
            }
            
            // wrap it up:
            previousCellPosition = position;
            laPreviousCell = laCell;
            lastPoint = point;
        }
    }

    paintElements( ctx, textInfoList, lineList, policy );
}
