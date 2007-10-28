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
#include "KDChartPercentBarDiagram_p.h"

using namespace KDChart;

PercentBarDiagram::PercentBarDiagram( BarDiagram* d )
    : BarDiagramType( d )
{
}

BarDiagram::BarType PercentBarDiagram::type() const
{
    return BarDiagram::Percent;
}

const QPair<QPointF, QPointF> PercentBarDiagram::calculateDataBoundaries() const
{
    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();

    const double xMin = 0;
    const double xMax = diagram()->model() ? diagram()->model()->rowCount( diagram()->rootIndex() ) : 0;
    double yMin = 0.0, yMax = 0.0;
    for( int col = 0; col < colCount; ++col )
    {
        for( int row = 0; row < rowCount; ++row )
        {
            // Ordinate should begin at 0 the max value being the 100% pos
            const QModelIndex idx = diagram()->model()->index( row, col, diagram()->rootIndex() );
            // only positive values are handled
            double value = diagram()->model()->data( idx ).toDouble();
            if ( value > 0 )
                yMax = qMax( yMax, value );
        }
    }
    // special cases
    if (  yMax == yMin ) {
        if ( yMin == 0.0 )
            yMax = 0.1; //we need at list a range
        else
            yMax = 0.0; // they are the same but negative
    }
    const QPointF bottomLeft( QPointF( xMin, yMin ) );
    const QPointF topRight( QPointF( xMax, yMax ) );

    //qDebug() << "BarDiagram::calculateDataBoundaries () returns ( " << bottomLeft << topRight <<")";
    return QPair< QPointF, QPointF >( bottomLeft,  topRight );
}

void PercentBarDiagram::paint( PaintContext* ctx )
{
    reverseMapper().clear();

    const QPair<QPointF,QPointF> boundaries = diagram()->dataBoundaries(); // cached

    const QPointF boundLeft = ctx->coordinatePlane()->translate( boundaries.first ) ;
    const QPointF boundRight = ctx->coordinatePlane()->translate( boundaries.second );

    const int rowCount = compressor().modelDataRows();
    const int colCount = compressor().modelDataColumns();

    BarAttributes ba = diagram()->barAttributes( diagram()->model()->index( 0, 0, diagram()->rootIndex() ) );
    double barWidth = 0;
    double maxDepth = 0;
    double width = boundRight.x() - boundLeft.x();
    double groupWidth = width/ (rowCount + 2);
    double spaceBetweenBars = 0;
    double spaceBetweenGroups = 0;

    if ( ba.useFixedBarWidth() ) {
        barWidth = ba.fixedBarWidth();
        groupWidth += barWidth;

        // Pending Michel set a min and max value for the groupWidth
        // related to the area.width
        if ( groupWidth < 0 )
            groupWidth = 0;

        if ( groupWidth  * rowCount > ctx->rectangle().width() )
            groupWidth = ctx->rectangle().width() / rowCount;
    }

    // maxLimit: allow the space between bars to be larger until area.width()
    // is covered by the groups.
    double maxLimit = rowCount * (groupWidth + ((colCount-1) * ba.fixedDataValueGap()) );


    //Pending Michel: FixMe
    if ( ba.useFixedDataValueGap() ) {
        if ( ctx->rectangle().width() > maxLimit )
            spaceBetweenBars += ba.fixedDataValueGap();
        else
            spaceBetweenBars = ((ctx->rectangle().width()/rowCount) - groupWidth)/(colCount-1);
    }

    if ( ba.useFixedValueBlockGap() )
        spaceBetweenGroups += ba.fixedValueBlockGap();

    calculateValueAndGapWidths( rowCount, colCount,groupWidth,
                                barWidth, spaceBetweenBars, spaceBetweenGroups );

    DataValueTextInfoList list;
    const double maxValue = 100; // always 100 %
    double sumValues = 0;
    QVector <double > sumValuesVector;

    //calculate sum of values for each column and store
    for( int row = 0; row < rowCount; ++row )
    {
        for( int col = 0; col < colCount; ++col )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, col );
            const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
            if ( point.value > 0 )
                sumValues += point.value;
            if ( col == colCount - 1 ) {
                sumValuesVector <<  sumValues ;
                sumValues = 0;
            }
        }
    }

    // calculate stacked percent value
    for( int col = 0; col < colCount; ++col )
    {
        double offset = spaceBetweenGroups;
        if( ba.useFixedBarWidth() )
            offset -= ba.fixedBarWidth();
        
        if( offset < 0 )
            offset = 0;

        for( int row = 0; row < rowCount ; ++row )
        {
            const CartesianDiagramDataCompressor::CachePosition position( row, col );
            const CartesianDiagramDataCompressor::DataPoint p = compressor().data( position );
            QModelIndex sourceIndex = attributesModel()->mapToSource( p.index );
            ThreeDBarAttributes threeDAttrs = diagram()->threeDBarAttributes( sourceIndex );

            if ( threeDAttrs.isEnabled() ){
                if ( barWidth > 0 )
                    barWidth =  (width - ((offset+(threeDAttrs.depth()))*rowCount))/ rowCount;
                if ( barWidth <= 0 ) {
                    barWidth = 0;
                    maxDepth = offset - ( width/rowCount);
                }
            }else{
                barWidth = (ctx->rectangle().width() - (offset*rowCount))/ rowCount;
            }

            const double value = p.value;
            double stackedValues = 0.0;
            double key = 0.0;
            
            // calculate stacked percent value
            // we only take in account positives values for now.
            for( int k = col; k >= 0 ; --k )
            {
                const CartesianDiagramDataCompressor::CachePosition position( row, k );
                const CartesianDiagramDataCompressor::DataPoint point = compressor().data( position );
                if ( point.value > 0)
                    stackedValues += point.value;
                key = point.key;
            }

            QPointF point, previousPoint;
            if(  sumValuesVector.at( row ) != 0 && value > 0 ) {
                point = ctx->coordinatePlane()->translate( QPointF( key,  stackedValues / sumValuesVector.at( row ) * maxValue ) );
                point.rx() += offset / 2;

                previousPoint = ctx->coordinatePlane()->translate( QPointF( key, ( stackedValues - value)/sumValuesVector.at(row)* maxValue ) );
            }
            const double barHeight = previousPoint.y() - point.y();

            const QRectF rect( point, QSizeF( barWidth, barHeight ) );
            appendDataValueTextInfoToList( diagram(), list, sourceIndex, PositionPoints( rect ),
                                              Position::NorthWest, Position::SouthEast,
                                              p.value );
            paintBars( ctx, sourceIndex, rect, maxDepth );
        }
    }
    paintDataValueTextsAndMarkers(  diagram(),  ctx,  list,  false );
}
