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
   Library General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this library; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "KDChartRingPainter.h"
#include "KDChartParams.h"

#include <qpainter.h>
#include <qvaluestack.h>

#define DEGTORAD(d) (d)*M_PI/180

#ifdef __WINDOWS__
#include <math.h>
#else
#include <cmath>
#endif

#if defined __WINDOWS__ || defined SUN7 || ( defined HP11_aCC && defined HP1100 )
#define std
#endif


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
                                    KDChartTableData* data,
                                    bool paint2nd,
                                    KDChartDataRegionList* regions )
{
    painter->setClipRect( _dataRect );

    uint chart = paint2nd ? 1 : 0;
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
    QArray<double> rowsums;
    double totalSum = 0.0;
    rowsums.resize( datasetEnd+1 ); // not datasetNum!
    for( int d1 = datasetStart; d1 <= datasetEnd; d1++ ) {
        rowsums[d1] = data->rowAbsSum( d1 );
        totalSum += rowsums[d1];
    }

    QArray<uint> ringthicknesses;
    ringthicknesses.resize( datasetEnd+1 ); // not datasetNum!

    // constant ring thickness
    uint ringthickness = _size / ( datasetNum * 2 );
    // Never let the ring thickness be more than 1/10 of the size to
    // ensure "ringness"
    if( ringthickness > ( _size/10 ) )
        ringthickness = _size / 10;

    for( int d2 = datasetStart; d2 <= datasetEnd; d2++ )
        if( params()->relativeRingThickness() ) {
            // 50% should be the same thickness as the one used when ring
            // thickness is constant.
            ringthicknesses[d2] = (uint)std::floor( (rowsums[d2] / totalSum) *
                                                    ( 2.0 * (double)ringthickness ) + 0.5 );
        } else {
            ringthicknesses[d2] = ringthickness;
        }

    int currentouterradius = _size/2;

    // Loop through all the displayable datasets; each dataset is one ring
    for( int dataset = datasetStart; dataset <= datasetEnd; dataset++ ) {
        double sectorsPerValue = 5760.0 / rowsums[dataset]; // 5760 == 16*360, number of sections in Qt circle
        int sectorsPerValueI = static_cast<int>( sectorsPerValue );
        int currentstartpos = params()->ringStart()*16;
        // Loop through all the values; each value is one piece on the ring.
        for( uint value = 0; value < _numValues; value++ ) {
            // is there anything at all at this value?
            double cellValue = 0.0;
            if( data->cell( dataset, value ).isDouble() ) {
                cellValue = std::fabs( data->cell( dataset, value ).doubleValue() );
                // Explosion: Only explode if explosion is turned on generally
                // and we are on the first ring. Besides, if there is a list
                // of explodeable values, the current value must be on this
                // list.

                QValueList<int> explodeList = params()->explodeValues();
                bool explode = params()->explode() && // explosion is on at all
                   ( dataset == datasetStart ) && // outermost ring
     ( ( explodeList.count() == 0 ) || // either nothing on explode list
       ( explodeList.find( value ) != explodeList.end() ) ); // or pie is on it

                drawOneSegment( painter,
                                currentouterradius,
                                currentouterradius-ringthicknesses[dataset],
                                currentstartpos,
                                currentstartpos+sectorsPerValue*cellValue,
                                dataset, value, explode, regions );
            }

            currentstartpos += static_cast<uint>( sectorsPerValue*cellValue );
        }
        currentouterradius -= ringthicknesses[dataset];
    }
}



void KDChartRingPainter::drawOneSegment( QPainter* painter,
                                         uint outerRadius,
                                         uint innerRadius,
                                         uint startAngle,
                                         uint endAngle,
                                         uint dataset,
                                         uint value,
                                         bool explode,
                                         KDChartDataRegionList* regions )
{
    painter->setPen( QPen( params()->outlineDataColor(),
                           params()->outlineDataLineWidth() ) );
    painter->setBrush( params()->dataColor( value ) );

    uint outerRadius2 = outerRadius * 2;
    uint innerRadius2 = innerRadius * 2;

    QRect drawPosition = _position;
    if ( explode ) {
        // need to compute a new position for each pie
        double explodeAngle = ( startAngle + ( endAngle - startAngle + 1 ) / 2 ) / 16;
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
        drawPosition.moveBy( explodeX, explodeY );
    } else
        drawPosition = _position;

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
    innerArc.makeArc( innerRect.x(), innerRect.y(),
                      innerRect.width(), innerRect.height(),
                      startAngle, endAngle - startAngle + 1 );

    // And the points for the outer arc
    QPointArray outerArc;
    outerArc.makeArc( outerRect.x(), outerRect.y(),
                      outerRect.width(), outerRect.height(),
                      startAngle, endAngle - startAngle + 1 );

    // Now copy the points from the outer arc in the reverse order onto the
    // inner arc array and draw that.
    uint innerArcPoints = innerArc.size();
    uint outerArcPoints = outerArc.size();
    innerArc.resize( innerArcPoints + outerArcPoints );
    for ( int i = outerArc.size() - 1; i >= 0; i-- ) {
        innerArc.setPoint( innerArcPoints+outerArcPoints-i-1,
                           outerArc.point( i ) );
    }
    painter->drawPolygon( innerArc );
    if ( regions )
        regions->append( new KDChartDataRegion( QRegion( innerArc ),
                                                dataset, value ) );
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
uint KDChartRingPainter::numLegendFallbackTexts( KDChartTableData* data ) const
{
    return data->usedCols();
}
