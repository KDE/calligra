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
#include "KDChartRingPainter.h"
#include "KDChartParams.h"

#include <qpainter.h>
#include <qvaluestack.h>

#include <stdlib.h>

#define DEGTORAD(d) (d)*M_PI/180

/**
  \class KDChartRingPainter KDChartRingPainter.h

  \brief A chart painter implementation that can paint pie charts.

  PENDING(kalle) Write more documentation.
  */

/**
  Constructor. Sets up internal data structures as necessary.

  \param params the KDChartParams structure that defines the chart
  */
    KDChartRingPainter::KDChartRingPainter( KDChartParams* params ) :
KDChartPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
  Destructor.
  */
KDChartRingPainter::~KDChartRingPainter()
{
    // intentionally left blank
}


/**
  Paints the actual data area.

  \param painter the QPainter onto which the chart should be painted
  \param data the data that will be displayed as a chart
  \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartRingPainter::paintData( QPainter* painter,
        KDChartTableDataBase* data,
        bool paint2nd,
        KDChartDataRegionList* regions )
{
    uint chart = paint2nd ? 1 : 0;

    QRect ourClipRect( _dataRect );

    const QWMatrix & world = painter->worldMatrix();
    ourClipRect =
#if COMPAT_QT_VERSION >= 0x030000
        world.mapRect( ourClipRect );
#else
    world.map( ourClipRect );
#endif

    ourClipRect.setTop(ourClipRect.top()-1);
    ourClipRect.setLeft(ourClipRect.left()-1);
    ourClipRect.setBottom(ourClipRect.bottom()+1);
    ourClipRect.setRight(ourClipRect.right()+1);
    painter->setClipRect( ourClipRect );

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
    uint datasetNum = abs( (int)( datasetEnd - datasetStart ) + 1 );


    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    if ( params()->numValues() != -1 )
        _numValues = params()->numValues();
    else
        _numValues = data->usedCols();

    // compute position
    _size = QMIN( _dataRect.width(), _dataRect.height() ); // initial size
    // if the rings explode, we need to give them additional space =>
    // make the basic size smaller
    if ( params()->explode() ) {
        double doubleSize = ( double ) _size;
        doubleSize /= ( 1.0 + params()->explodeFactor() * 2 );
        _size = ( int ) doubleSize;
    }

    int x = ( _dataRect.width() == _size ) ? 0 : ( ( _dataRect.width() - _size ) / 2 );
    int y = ( _dataRect.height() == _size ) ? 0 : ( ( _dataRect.height() - _size ) / 2 );
    _position = QRect( x, y, _size, _size );
    _position.moveBy( _dataRect.left(), _dataRect.top() );

    // We need the row sums anyway later, so we can just as well compute them
    // here, because we need them in case of relative ring thicknesses.
    QMemArray<double> rowsums;
    double totalSum = 0.0;
    rowsums.resize( datasetEnd+1 ); // not datasetNum!
    for( int d1 = (int)datasetStart; d1 <= (int)datasetEnd; d1++ ) {
        rowsums[d1] = data->rowAbsSum( d1 );
        totalSum += rowsums[d1];
    }

    QMemArray<int> ringthicknesses;
    ringthicknesses.resize( datasetEnd+1 ); // not datasetNum!

    // constant ring thickness
    int ringthickness = _size / ( datasetNum * 2 );
    // Never let the ring thickness be more than 1/10 of the size to
    // ensure "ringness"
    if( ringthickness > ( _size/10 ) )
        ringthickness = _size / 10;

    for( int d2 = (int)datasetStart; d2 <= (int)datasetEnd; d2++ )
        if( params()->relativeRingThickness() ) {
            // 50% should be the same thickness as the one used when ring
            // thickness is constant.
            ringthicknesses[d2] = (uint)floor( (rowsums[d2] / totalSum) *
                    ( 2.0 * (double)ringthickness ) + 0.5 );
        } else {
            ringthicknesses[d2] = ringthickness;
        }

    int currentouterradius = _size/2;

    // Loop through all the displayable datasets; each dataset is one ring
    for( int dataset = (int)datasetStart; dataset <= (int)datasetEnd; dataset++ ) {
        double sectorsPerValue = 5760.0 / rowsums[dataset]; // 5760 == 16*360, number of sections in Qt circle
        //int sectorsPerValueI = static_cast<int>( sectorsPerValue );
        double currentstartpos = (double)params()->ringStart() * 16.0;
        // Loop through all the values; each value is one piece on the ring.
        QVariant vValY;
        for( int value = 0; value < _numValues; value++ ) {
            // is there anything at all at this value?
            double cellValue = 0.0;
            if( data->cellCoord( dataset, value, vValY, 1 ) &&
                QVariant::Double == vValY.type() ){
                cellValue = fabs( vValY.toDouble() );
                // Explosion: Only explode if explosion is turned on generally
                // and we are on the first ring. Besides, if there is a list
                // of explodeable values, the current value must be on this
                // list.

                QValueList<int> explodeList = params()->explodeValues();
                bool explode = params()->explode() && // explosion is on at all
                    ( dataset == (int)datasetStart ) && // outermost ring
                    ( ( explodeList.count() == 0 ) || // either nothing on explode list
                      ( explodeList.find( value ) != explodeList.end() ) ); // or pie is on it

                drawOneSegment( painter,
                        currentouterradius,
                        currentouterradius-ringthicknesses[dataset],
                        currentstartpos,
                        sectorsPerValue * cellValue,
                        dataset, value, chart, explode, regions );
            }
            currentstartpos += sectorsPerValue * cellValue;
        }
        currentouterradius -= ringthicknesses[dataset];
    }
}



void KDChartRingPainter::drawOneSegment( QPainter* painter,
        uint outerRadius,
        uint innerRadius,
        double startAngle,
        double angles,
        uint dataset,
        uint value,
        uint chart,
        bool explode,
        KDChartDataRegionList* regions )
{
    // special case for full circle
    if( angles == 5760.0 )
        startAngle =  0.0;

    painter->setPen( QPen( params()->outlineDataColor(),
                params()->outlineDataLineWidth() ) );
    painter->setBrush( params()->dataColor( value ) );

    uint outerRadius2 = outerRadius * 2;
    uint innerRadius2 = innerRadius * 2;

    QRect drawPosition = _position;
    if ( explode ) {
        // need to compute a new position for each pie
        double explodeAngle = ( startAngle + angles / 2.0 ) / 16.0;
        double explodeAngleRad = DEGTORAD( explodeAngle );
        double cosAngle = cos( explodeAngleRad );
        double sinAngle = -sin( explodeAngleRad );

        // find the explode factor for this particular ring segment
        double explodeFactor = 0.0;
        QMap<int,double> explodeFactors = params()->explodeFactors();
        if( !explodeFactors.contains( value ) ) // not on factors list, use default
            explodeFactor = params()->explodeFactor();
        else // on factors list, use segment-specific value
            explodeFactor = explodeFactors[value];

        double explodeX = explodeFactor * _size * cosAngle;
        double explodeY = explodeFactor * _size * sinAngle;
        drawPosition.moveBy( static_cast<int>( explodeX ), static_cast<int>( explodeY ) );
    }

    QRect outerRect( drawPosition.x() +
            ( drawPosition.width() - outerRadius2 ) / 2,
            drawPosition.y() +
            ( drawPosition.height() - outerRadius2 ) / 2,
            outerRadius2, outerRadius2 );
    QRect innerRect( drawPosition.x() +
            ( drawPosition.width() - innerRadius2 ) / 2,
            drawPosition.y() +
            ( drawPosition.height() - innerRadius2 ) / 2,
            innerRadius2, innerRadius2 );

    // Start with getting the points for the inner arc.
    QPointArray innerArc;
    makeArc( innerArc, innerRect, startAngle, angles );

    // And the points for the outer arc
    QPointArray outerArc;
    makeArc( outerArc, outerRect, startAngle, angles );

    // Now copy the points from the outer arc in the reverse order onto the
    // inner arc array and draw that.
    uint innerArcPoints = innerArc.size();
    uint outerArcPoints = outerArc.size();
    innerArc.resize( innerArcPoints + outerArcPoints );
    for ( int i = outerArcPoints - 1; i >= 0; i-- ) {
        innerArc.setPoint( innerArcPoints+outerArcPoints-i-1,
                outerArc.point( i ) );
    }

    painter->drawPolygon( innerArc );
    if ( regions /* && ( innerArc.size() > 2 )*/ ) {
        KDChartDataRegion* datReg = new KDChartDataRegion( dataset,
                                                           value,
                                                           chart,
                                                           innerArc );

        const int aA = static_cast<int>( startAngle );
        const int aM = static_cast<int>( startAngle + angles / 2.0 );
        const int aZ = static_cast<int>( startAngle + angles );

        datReg->points[ KDChartEnums::PosTopLeft ]
            = pointOnCircle( outerRect, aZ );
        datReg->points[ KDChartEnums::PosTopCenter ]
            = pointOnCircle( outerRect, aM );
        datReg->points[ KDChartEnums::PosTopRight ]
            = pointOnCircle( outerRect, aA );

        datReg->points[ KDChartEnums::PosBottomLeft ]
            = pointOnCircle( innerRect, aZ );
        datReg->points[ KDChartEnums::PosBottomCenter ]
            = pointOnCircle( innerRect, aM );
        datReg->points[ KDChartEnums::PosBottomRight ]
            = pointOnCircle( innerRect, aA );

        datReg->points[ KDChartEnums::PosCenterLeft ]
            = QPoint( (   datReg->points[ KDChartEnums::PosTopLeft      ].x()
                        + datReg->points[ KDChartEnums::PosBottomLeft   ].x() ) / 2,
                      (   datReg->points[ KDChartEnums::PosTopLeft      ].y()
                        + datReg->points[ KDChartEnums::PosBottomLeft   ].y() ) / 2 );
        datReg->points[ KDChartEnums::PosCenter ]
            = QPoint( (   datReg->points[ KDChartEnums::PosTopCenter    ].x()
                        + datReg->points[ KDChartEnums::PosBottomCenter ].x() ) / 2,
                      (   datReg->points[ KDChartEnums::PosTopCenter    ].y()
                        + datReg->points[ KDChartEnums::PosBottomCenter ].y() ) / 2 );
        datReg->points[ KDChartEnums::PosCenterRight ]
            = QPoint( (   datReg->points[ KDChartEnums::PosTopRight     ].x()
                        + datReg->points[ KDChartEnums::PosBottomRight  ].x() ) / 2,
                      (   datReg->points[ KDChartEnums::PosTopRight     ].y()
                        + datReg->points[ KDChartEnums::PosBottomRight  ].y() ) / 2 );

        // test the 9 positions:
        /*
        painter->drawEllipse( datReg->points[ KDChartEnums::PosTopLeft     ].x() - 2,
                              datReg->points[ KDChartEnums::PosTopLeft     ].y() - 2,  5, 5);
        painter->drawEllipse( datReg->points[ KDChartEnums::PosCenterLeft  ].x() - 2,
                              datReg->points[ KDChartEnums::PosCenterLeft  ].y() - 2,  5, 5);
        painter->drawEllipse( datReg->points[ KDChartEnums::PosBottomLeft  ].x() - 2,
                              datReg->points[ KDChartEnums::PosBottomLeft  ].y() - 2,  5, 5);

        qDebug( "\ncenter: (%i, %i)",
                datReg->points[ KDChartEnums::PosCenter   ].x(),
                datReg->points[ KDChartEnums::PosCenter   ].y() );
        painter->drawEllipse( datReg->points[ KDChartEnums::PosTopCenter   ].x() - 2,
                              datReg->points[ KDChartEnums::PosTopCenter   ].y() - 2,  5, 5);
        painter->drawEllipse( datReg->points[ KDChartEnums::PosCenter      ].x() - 2,
                              datReg->points[ KDChartEnums::PosCenter      ].y() - 2,  5, 5);
        painter->drawEllipse( datReg->points[ KDChartEnums::PosBottomCenter].x() - 2,
                              datReg->points[ KDChartEnums::PosBottomCenter].y() - 2,  5, 5);

        painter->drawRect( datReg->points[ KDChartEnums::PosCenterRight ].x() - 2,
                              datReg->points[ KDChartEnums::PosCenterRight ].y() - 2,  5, 5);
        //painter->drawRect( datReg->points[ KDChartEnums::PosTopRight    ].x() - 2,
        //                    datReg->points[ KDChartEnums::PosTopRight    ].y() - 2,  5, 5);
        painter->drawRect( datReg->points[ KDChartEnums::PosBottomRight ].x() - 2,
                              datReg->points[ KDChartEnums::PosBottomRight ].y() - 2,  5, 5);
        */
        datReg->startAngle = static_cast<int>( startAngle );
        datReg->angleLen   = static_cast<int>( angles );
        regions->append( datReg );
    }
}


/**
  This method is a specialization that returns a fallback legend text
  appropriate for rings where the fallbacks should come from the values, not
  from the datasets.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \param uint dataset the dataset number for which to generate a
  fallback text
  \return the fallback text to use for describing the specified
  dataset in the legend
  */
QString KDChartRingPainter::fallbackLegendText( uint dataset ) const
{
    return QObject::tr( "Item " ) + QString::number( dataset + 1 );
}


/**
  This methods returns the number of elements to be shown in the
  legend in case fallback texts are used.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \return the number of fallback texts to use
  */
uint KDChartRingPainter::numLegendFallbackTexts( KDChartTableDataBase* data ) const
{
    return data->usedCols();
}
