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

#include <QAbstractItemModel>

#include "KDChartBarDiagram.h"
#include "KDChartLineDiagram.h"
#include "KDChartTextAttributes.h"
#include "KDChartAttributesModel.h"
#include "KDChartAbstractCartesianDiagram.h"
#include "KDChartStackedLineDiagram_p.h"

using namespace KDChart;

StackedLineDiagram::StackedLineDiagram( LineDiagram* d )
    : LineDiagramType( d )
{
}

LineDiagram::LineType StackedLineDiagram::type() const
{
    return LineDiagram::Stacked;
}

const QPair<QPointF, QPointF> StackedLineDiagram::calculateDataBoundaries() const
{
    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();
    double xMin = 0;
    double xMax = diagram()->model() ? diagram()->model()->rowCount( diagram()->rootIndex() ) - 1 : 0;
    double yMin = 0, yMax = 0;

    bool bStarting = true;
    for( int row = 0; row < rowCount; ++row )
    {
        // calculate sum of values per column - Find out stacked Min/Max
        double stackedValues = 0;
        for( int col = datasetDimension() - 1; col < colCount; col += datasetDimension() ) {
            CartesianDiagramDataCompressor::CachePosition position( row, col );
            CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );

            stackedValues += point.value;
        }

        if( bStarting ){
            yMin = stackedValues;
            yMax = stackedValues;
            bStarting = false;
        }else{
            // Pending Michel:
            // I am taking in account all values negatives or positives
            // take in account all stacked values
            yMin = qMin( qMin( yMin,  0.0 ), stackedValues );
            yMax = qMax( yMax, stackedValues );
        }
    }

    QPointF bottomLeft( QPointF( xMin, yMin ) );
    QPointF topRight(   QPointF( xMax, yMax ) );
    return QPair<QPointF, QPointF> ( bottomLeft, topRight );
}

void StackedLineDiagram::paint(  PaintContext* ctx )
{
    reverseMapper().clear();

    const QPair<QPointF, QPointF> boundaries = diagram()->dataBoundaries();
    const QPointF bottomLeft = boundaries.first;
    const QPointF topRight = boundaries.second;
        
    const int columnCount = compressor().modelDataColumns();
    const int rowCount = compressor().modelDataRows();

// FIXME integrate column index retrieval to compressor:
    int maxFound = 0;
//    {   // find the last column number that is not hidden
//        for( int iColumn =  datasetDimension() - 1;
//             iColumn <  columnCount;
//             iColumn += datasetDimension() )
//            if( ! diagram()->isHidden( iColumn ) )
//                maxFound = iColumn;
//    }
    maxFound = columnCount;
    // ^^^ temp

    DataValueTextInfoList list;
    LineAttributesInfoList lineList;
    LineAttributes::MissingValuesPolicy policy;

    //FIXME(khz): add LineAttributes::MissingValuesPolicy support for LineDiagram::Stacked and ::Percent

    QVector <double > percentSumValues;

    QList<QPointF> bottomPoints;
    bool bFirstDataset = true;

    for( int column = 0; column < columnCount; ++column )
    {
        CartesianDiagramDataCompressor::CachePosition previousCellPosition;

        //display area can be set by dataset ( == column) and/or by cell
        LineAttributes laPreviousCell; // by default no area is drawn
        QModelIndex indexPreviousCell;
        QList<QPolygonF> areas;
        QList<QPointF> points;

        for ( int row = 0; row < rowCount; ++row ) {
            const CartesianDiagramDataCompressor::CachePosition position( row, column );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            const QModelIndex sourceIndex = attributesModel()->mapToSource( point.index );

            const LineAttributes laCell = diagram()->lineAttributes( sourceIndex );
            const bool bDisplayCellArea = laCell.displayArea();

            double stackedValues = 0, nextValues = 0, nextKey = 0;
            for ( int column2 = column; column2 >= 0; --column2 )
            {
                const CartesianDiagramDataCompressor::CachePosition position( row, column2 );
                const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
                const QModelIndex sourceIndex = attributesModel()->mapToSource( point.index );
                stackedValues += point.value;
                //qDebug() << valueForCell( iRow, iColumn2 );
                if ( row + 1 < rowCount ){
                    CartesianDiagramDataCompressor::CachePosition position( row + 1, column2 );
                    CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
                    nextValues += point.value;
                    nextKey = point.key;
                }
            }
            //qDebug() << stackedValues << endl;
            const QPointF nextPoint = ctx->coordinatePlane()->translate( QPointF( point.key, stackedValues ) );
            points << nextPoint;

            const QPointF ptNorthWest( nextPoint );
            const QPointF ptSouthWest(
                bDisplayCellArea
                ? ( bFirstDataset
                    ? ctx->coordinatePlane()->translate( QPointF( point.key, 0.0 ) )
                    : bottomPoints.at( row )
                    )
                : nextPoint );
            QPointF ptNorthEast;
            QPointF ptSouthEast;

            if ( row + 1 < rowCount ){
                QPointF toPoint = ctx->coordinatePlane()->translate( QPointF( nextKey, nextValues ) );
                lineList.append( LineAttributesInfo( sourceIndex, nextPoint, toPoint ) );
                ptNorthEast = toPoint;
                ptSouthEast =
                    bDisplayCellArea
                    ? ( bFirstDataset
                        ? ctx->coordinatePlane()->translate( QPointF( nextKey, 0.0 ) )
                        : bottomPoints.at( row + 1 )
                        )
                    : toPoint;
                if( areas.count() && laCell != laPreviousCell ){
                    paintAreas( ctx, indexPreviousCell, areas, laPreviousCell.transparency() );
                    areas.clear();
                }
                if( bDisplayCellArea ){
                    QPolygonF poly;
                    poly << ptNorthWest << ptNorthEast << ptSouthEast << ptSouthWest;
                    areas << poly;
                    laPreviousCell = laCell;
                    indexPreviousCell = sourceIndex;
                }else{
                    //qDebug() << "no area shown for row"<<iRow<<"  column"<<iColumn;
                }
            }else{
                ptNorthEast = ptNorthWest;
                ptSouthEast = ptSouthWest;
            }

            const PositionPoints pts( ptNorthWest, ptNorthEast, ptSouthEast, ptSouthWest );
            appendDataValueTextInfoToList( diagram(), list, sourceIndex, pts,
                                           Position::NorthWest, Position::SouthWest,
                                           point.value );
        }
        if( areas.count() ){
            paintAreas( ctx, indexPreviousCell, areas, laPreviousCell.transparency() );
            areas.clear();
        }
        bottomPoints = points;
        bFirstDataset = false;
    }
    paintElements( ctx, list, lineList, policy );
}
