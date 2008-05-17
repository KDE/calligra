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

#include <QModelIndex>

#include "KDChartBarDiagram.h"
#include "KDChartTextAttributes.h"
#include "KDChartAttributesModel.h"
#include "KDChartAbstractCartesianDiagram.h"
#include "KDChartNormalBarDiagram_p.h"

using namespace KDChart;

NormalBarDiagram::NormalBarDiagram( BarDiagram* d )
    : BarDiagramType( d )
{
}

BarDiagram::BarType NormalBarDiagram::type() const
{
    return BarDiagram::Normal;
}

const QPair<QPointF, QPointF> NormalBarDiagram::calculateDataBoundaries() const
{
    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();

    double xMin = 0.0;
    double xMax = diagram()->model() ? diagram()->model()->rowCount( diagram()->rootIndex() ) : 0;
    double yMin = 0.0, yMax = 0.0;

    bool bStarting = true;
    for ( int column = 0; column < colCount; ++column )
    {
        for ( int row = 0; row < rowCount; ++row )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, column );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            const double value = ISNAN( point.value ) ? 0.0 : point.value;
            // this is always true yMin can be 0 in case all values
            // are the same
            // same for yMax it can be zero if all values are negative
            if( bStarting ){
                yMin = value;
                yMax = value;
                bStarting = false;
            }else{
                yMin = qMin( yMin, value );
                yMax = qMax( yMax, value );
            }
        }
    }

    // special cases
    if (  yMax == yMin ) {
        if ( yMin == 0.0 )
            yMax = 0.1; //we need at least a range
        else if( yMax < 0.0 )
            yMax = 0.0; // they are the same and negative
        else if( yMin > 0.0 )
            yMin = 0.0; // they are the same but positive
    }
    const QPointF bottomLeft ( QPointF( xMin, yMin ) );
    const QPointF topRight ( QPointF( xMax, yMax ) );

    return QPair< QPointF, QPointF >( bottomLeft,  topRight );
}

void NormalBarDiagram::paint(  PaintContext* ctx )
{
    reverseMapper().clear();

    const QPair<QPointF,QPointF> boundaries = diagram()->dataBoundaries(); // cached

    const QPointF boundLeft = ctx->coordinatePlane()->translate( boundaries.first ) ;
    const QPointF boundRight = ctx->coordinatePlane()->translate( boundaries.second );

    const int rowCount = attributesModel()->rowCount(attributesModelRootIndex());
    const int colCount = attributesModel()->columnCount(attributesModelRootIndex());

    BarAttributes ba = diagram()->barAttributes( diagram()->model()->index( 0, 0, diagram()->rootIndex() ) );
    double barWidth = 0;
    double maxDepth = 0;
    double width = boundRight.x() - boundLeft.x();
    double groupWidth = width / (rowCount + 2);
    double spaceBetweenBars = 0;
    double spaceBetweenGroups = 0;

    if ( ba.useFixedBarWidth() ) {

        barWidth = ba.fixedBarWidth();
        groupWidth += barWidth;

        // Pending Michel set a min and max value for the groupWidth
        // related to the area.width
        if ( groupWidth < 0 )
            groupWidth = 0;

        if ( groupWidth  * rowCount > width )
            groupWidth = width / rowCount;
    }

    // maxLimit: allow the space between bars to be larger until area.width()
    // is covered by the groups.
    double maxLimit = rowCount * (groupWidth + ((colCount-1) * ba.fixedDataValueGap()) );

    //Pending Michel: FixMe
    if ( ba.useFixedDataValueGap() ) {
        if ( width > maxLimit )
            spaceBetweenBars += ba.fixedDataValueGap();
        else
            spaceBetweenBars = ((width/rowCount) - groupWidth)/(colCount-1);
    }

    if ( ba.useFixedValueBlockGap() ) {
        spaceBetweenGroups += ba.fixedValueBlockGap();
    }

    calculateValueAndGapWidths( rowCount, colCount,groupWidth,
                                barWidth, spaceBetweenBars, spaceBetweenGroups );

    DataValueTextInfoList list;

    for( int row = 0; row < rowCount; ++row )
    {
        double offset = -groupWidth/2 + spaceBetweenGroups/2;

        if ( ba.useFixedDataValueGap() )
        {
            if ( spaceBetweenBars > 0 )
            {
                if ( width > maxLimit )
                    offset -= ba.fixedDataValueGap();
                else
                    offset -= ((width/rowCount) - groupWidth)/(colCount-1);

            }
            else
            {
                offset += barWidth/2;
            }
        }

        for( int column=0; column< colCount; ++column )
        {
            // paint one group
            const CartesianDiagramDataCompressor::CachePosition position( row,  column );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            const QModelIndex sourceIndex = attributesModel()->mapToSource( point.index );
            const qreal value = point.value;//attributesModel()->data( sourceIndex ).toDouble();
            if ( !ISNAN( value ) ) {
                QPointF topPoint = ctx->coordinatePlane()->translate( QPointF( point.key + 0.5, value ) );
                QPointF bottomPoint =  ctx->coordinatePlane()->translate( QPointF( point.key, 0 ) );
                const double barHeight = bottomPoint.y() - topPoint.y();
                topPoint.setX( topPoint.x() + offset );
                const QRectF rect( topPoint, QSizeF( barWidth, barHeight ) );
                appendDataValueTextInfoToList( diagram(), list, sourceIndex, PositionPoints( rect ),
                                               Position::NorthWest, Position::SouthEast,
                                               point.value );
                paintBars( ctx, sourceIndex, rect, maxDepth );
            }
            offset += barWidth + spaceBetweenBars;
        }
    }
    paintDataValueTextsAndMarkers(  diagram(),  ctx,  list,  false );
}
