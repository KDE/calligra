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

#include <qpainter.h>

#include "KDChartAxesPainter.h"
#include "KDChartAxisParams.h"
#include "KDChartParams.h"
#include "KDChartData.h"

#ifdef __WINDOWS__
#include <math.h>
#else
#include <cmath>
#include <stdlib.h>
#endif

#if defined __WINDOWS__ || defined SUN7 || ( defined HP11_aCC && defined HP1100 )
#define std
#endif

/**
   \class KDChartAxesPainter KDChartAxesPainter.h

   \brief A common base class for classes that implement chart
   painters for chart types ith axes.
*/

/**
   Constructor. Sets up internal data structures as necessary.

   \param params the KDChartParams structure that defines the chart
*/
KDChartAxesPainter::KDChartAxesPainter( KDChartParams* params ) :
KDChartPainter( params )
{
    // Intentionally left blank.
    // We cannot setup the geometry yet
    // since we do not know the size of the painter.
}

/**
   Destructor.
*/
KDChartAxesPainter::~KDChartAxesPainter()
{
    // intentionally left blank
}


/**
    Provides some information needed twice by ::paintAxes()  (see below)
*/
void KDChartAxesPainter::findInfos( double averageValueP1000,
                                    const KDChartAxisParams& para,
                                    uint axisPos,
                                    KDChartAxisParams::AxisPos& basicPos,
                                    QPoint& orig,
                                    QPoint& dest )
{

    basicPos = KDChartAxisParams::basicAxisPos( axisPos );

    switch ( basicPos ) {
    case KDChartAxisParams::AxisPosBottom: {
            orig = para.axisTrueAreaRect().topLeft();
            dest = para.axisTrueAreaRect().topRight();
        }
        break;
    case KDChartAxisParams::AxisPosLeft: {
            orig = para.axisTrueAreaRect().bottomRight();
            dest = para.axisTrueAreaRect().topRight();
        }
        break;
    case KDChartAxisParams::AxisPosTop: {
            orig = para.axisTrueAreaRect().bottomLeft();
            dest = para.axisTrueAreaRect().bottomRight();
        }
        break;
    case KDChartAxisParams::AxisPosRight: {
            orig = para.axisTrueAreaRect().bottomLeft();
            dest = para.axisTrueAreaRect().topLeft();
        }
        break;
    }
}


/**
   Paints the actual axes areas.

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
*/
void KDChartAxesPainter::paintAxes( QPainter* painter,
                                    KDChartTableData* data )
{
    if ( !painter || !data || 0 == params() )
        return ;

    double areaWidthP1000 = _logicalWidth / 1000.0;
    double areaHeightP1000 = _logicalHeight / 1000.0;
    double averageValueP1000 = ( areaWidthP1000 + areaHeightP1000 ) / 2.0;
    painter->setPen( Qt::NoPen );

    /*
    // show complete rect areas of all active axes (for debugging)
    for( uint i = 0;  i < KDChartParams::KDCHART_MAX_AXES;  ++i )
        if(    params()->axisParams( i ).axisVisible()
            &&    KDChartAxisParams::AxisTypeUnknown
               != params()->axisParams( i ).axisType() ){
            const KDChartAxisParams& para( params()->axisParams( i ) );
            painter->fillRect(para.axisTrueAreaRect(),QBrush(Qt::white));
}
    */


    for ( uint i = 0; i < KDChartParams::KDCHART_MAX_AXES; ++i )
        if ( params()->axisParams( i ).axisVisible()
                && KDChartAxisParams::AxisTypeUnknown
                != params()->axisParams( i ).axisType() ) {

            const KDChartAxisParams & para = params()->axisParams( i );

            // length of little delimiter-marks indicating axis scaling
            double delimLen = 20.0 * averageValueP1000; // per mille of area

            uint lineWidth = 0 <= para.axisLineWidth()
                             ? para.axisLineWidth()
                             : -1 * static_cast < int > ( para.axisLineWidth()
                                                          * averageValueP1000 );
            ( ( KDChartAxisParams& ) para ).setAxisTrueLineWidth( lineWidth );

            uint gridLineWidth
                 = ( KDChartAxisParams::AXIS_GRID_AUTO_LINEWIDTH
                     == para.axisGridLineWidth() )
                 ? lineWidth
                 : (   ( 0 <= para.axisGridLineWidth() )
                     ? para.axisGridLineWidth()
                     : -1 * static_cast < int > ( para.axisGridLineWidth()
                                                  * averageValueP1000 ) );

            KDChartAxisParams::AxisPos basicPos;
            QPoint orig, dest;
            findInfos( averageValueP1000, para, i,
                       basicPos,
                       orig,
                       dest );

            // Magic to find out axis scaling factors and labels text height
            // =============================================================
            //                                             - khz, 02/24/2001
            //
            // 1st Calculate the axis label texts height regarding to
            //     user-defined per-axis settings.
            //
            // 2nd This height is given to calculateLabelTexts() to
            //     calculate the delimiter and sub-delimiter distances as
            //     well as the axis scaling factors.
            //     If neccessary and possible the short replacement strings
            //     are taken that might have been specified by the user.
            //     - see KDChartAxisParams::setAxisLabelStringLists() -
            //
            // 3rd Before displaying the texts we make sure they fit into
            //     their space, if needed we will further reduce their
            //     font height in order to avoid clipping of text parts.
            //
            // If the texts *still* don't fit into their space, we are lost
            // and they will be clipped. Such is live.
            //
            // Why all this?
            //
            // Because I do not believe in axis areas growing and shrinking
            // regarding to long or short label texts: start such behaviour
            // and become mad.
            //
            // Better plan: ask the user to specify a way how to abbreviate
            //              label texts (e.g. by writing "200" instead
            //              of that wide and unreadable  "200,000.00")
            //
            //
            //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
            //
            //

            double nSubDelimFactor = 0.0;
            double pDelimDelta = 0.0;
            double nTxtHeight = 0.0;
            double pTextsX = 0.0;
            double pTextsY = 0.0;
            double pTextsW = 0.0;
            double pTextsH = 0.0;
            int textAlign = Qt::AlignHCenter | Qt::AlignVCenter;

            calculateLabelTexts( *painter,
                                 *data,
                                 *params(),
                                 i,
                                 averageValueP1000,
                                 basicPos,
                                 orig,
                                 delimLen,
                                 // start of reference parameters
                                 nSubDelimFactor,
                                 pDelimDelta,
                                 nTxtHeight,
                                 pTextsX,
                                 pTextsY,
                                 pTextsW,
                                 pTextsH,
                                 textAlign );

            QStringList* labelTexts = ( QStringList* ) para.axisLabelTexts();
            uint nLabels = ( 0 != labelTexts && labelTexts->count() )
                           ? labelTexts->count()
                           : 0;

            // start point of 1st delimiter on the axis-line == grid-start
            QPoint p1( orig );
            // end point of 1st delimiter near the label text
            QPoint p2( orig );
            // end point of small sub-delimiter
            QPoint p2a( orig );
            // start point of 1st grid-line (beginnig at the axis)
            QPoint pGA( orig );
            // end point of 1st grid-line at the other side of the chart
            QPoint pGZ( orig );
            // start point of zero-line, this is often identical with p1
            // but will be different in case of shifted zero-line
            double axisZeroLineStartX = p1.x();
            double axisZeroLineStartY = p1.y();

            double pXDeltaFactor = 0.0;
            double pYDeltaFactor = 0.0;

            switch ( basicPos ) {
            case KDChartAxisParams::AxisPosBottom: {
                    p2.setY( p2.y() + delimLen );
                    p2a.setY( p2a.y() + delimLen * 2 / 3 );
                    pGZ.setY( pGZ.y() - _dataRect.height() + 1 );
                    pXDeltaFactor = 1.0;
                    pYDeltaFactor = 0.0;
                }
                break;
            case KDChartAxisParams::AxisPosLeft: {
                    p2.setX( p2.x() - delimLen );
                    p2a.setX( p2a.x() - delimLen * 2 / 3 );
                    pGZ.setX( pGZ.x() + _dataRect.width() - 1 );
                    pXDeltaFactor = 0.0;
                    pYDeltaFactor = -1.0;
                }
                break;
            case KDChartAxisParams::AxisPosTop: {
                    p2.setY( p2.y() - delimLen );
                    p2a.setY( p2a.y() - delimLen * 2 / 3 );
                    pGZ.setY( pGZ.y() + _dataRect.height() - 1 );
                    pXDeltaFactor = 1.0;
                    pYDeltaFactor = 0.0;
                }
                break;
            case KDChartAxisParams::AxisPosRight: {
                    p2.setX( p2.x() + delimLen );
                    p2a.setX( p2a.x() + delimLen * 2 / 3 );
                    pGZ.setX( pGZ.x() - _dataRect.width() + 1 );
                    pXDeltaFactor = 0.0;
                    pYDeltaFactor = -1.0;
                }
                break;
            }

            bool bOrdinate = para.axisSteadyValueCalc();
            bool bTouchEdges = para.axisLabelsTouchEdges();

            if ( nLabels ) {
                // draw label texts and delimiters and grid
                painter->setPen( QPen( para.axisLineColor(),
                                       lineWidth ) );
                // calculate font size
                QFont actFont( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() ) {
                    actFont.setPointSizeFloat( nTxtHeight );
                }
                // bad days: in case of labels being too wide
                //           to fit into the available space
                //           we try to reduce the font size
                //
                //
                // NOTE:  LABEL TEXT ROTATION COULD ALSO HELP HERE
                //
                //
                //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                //
                //
                QFontMetrics fm( actFont );
                QString strMax;
                int maxLabelsWidth = 0;
                for ( QStringList::Iterator it = labelTexts->begin();
                        it != labelTexts->end();
                        ++it ) {
                    if ( fm.width( *it ) > maxLabelsWidth ) {
                        maxLabelsWidth = fm.width( *it );
                        strMax = *it;
                    }
                }
                while ( fm.width( strMax ) > pTextsW
                        && 6.0 < nTxtHeight ) {
                    nTxtHeight -= 0.5;
                    actFont.setPointSizeFloat( nTxtHeight );
                    fm = QFontMetrics( actFont );
                }
                painter->setFont( actFont );

                // set colour of grid pen
                // (used for grid lines and for sub-grid lines)
                QPen gridPen;
                if ( para.axisShowGrid() )
                    gridPen.setColor( para.axisGridColor() );

                // draw delimiters and labels
                const double pXDelta = pXDeltaFactor * pDelimDelta;
                const double pYDelta = pYDeltaFactor * pDelimDelta;

                //qDebug("pYDelta = %f",pYDelta);

                double pGXMicroAdjust = 0.0;
                double pGYMicroAdjust = 0.0;
                if ( !bTouchEdges ) {
                    p1.setX( p1.x() + pXDelta / 2 );
                    p1.setY( p1.y() + pYDelta / 2 );
                    p2.setX( p2.x() + pXDelta / 2 );
                    p2.setY( p2.y() + pYDelta / 2 );
                    p2a.setX( p2a.x() + pXDelta / 2 );
                    p2a.setY( p2a.y() + pYDelta / 2 );
                    pGA.setX( pGA.x() + pXDelta );
                    pGA.setY( pGA.y() + pYDelta );
                    pGZ.setX( pGZ.x() + pXDelta );
                    pGZ.setY( pGZ.y() + pYDelta );
                    pGXMicroAdjust = pXDeltaFactor * lineWidth / 2.0;
                    pGYMicroAdjust = pYDeltaFactor * lineWidth / 2.0;
                }
                double x1, y1, x2, y2, xGA, yGA, xGZ, yGZ,
                p1X, p1Y, p2X, p2Y, pGAX, pGAY, pGZX, pGZY, xT, yT;
                if ( 0.0 != nSubDelimFactor
                        && para.axisShowSubDelimiters() ) {
                    QPen pen( para.axisLineColor(), 0.5 * lineWidth );
                    uint penWidth = pen.width();
                    bool bOk = true;
                    while ( std::fabs( ( pXDelta + pYDelta ) * nSubDelimFactor / 6.0 )
                            <= 1.0 + penWidth
                            && bOk ) {
                        if ( 0 < penWidth ) {
                            --penWidth;
                            pen.setWidth( penWidth );
                        } else
                            if ( 0.5 != nSubDelimFactor ) {
                                // emercency: reduce number of sub-scaling
                                // delimiters
                                nSubDelimFactor = 0.5;
                            } else
                                bOk = false;
                    }
                    if ( bOk ) {
                        x1 = p1.x();
                        y1 = p1.y();
                        x2 = p2a.x();
                        y2 = p2a.y();
                        xGA = pGA.x();
                        yGA = pGA.y();
                        xGZ = pGZ.x();
                        yGZ = pGZ.y();
                        p1X = x1;
                        p1Y = y1;
                        p2X = x2;
                        p2Y = y2;
                        pGAX = xGA;
                        pGAY = yGA;
                        pGZX = xGZ;
                        pGZY = yGZ;
                        // set up grid pen for drawnig the sub-grid lines
                        if ( para.axisShowGrid() ) {
                            gridPen.setWidth( penWidth );
                            gridPen.setStyle( para.axisGridSubStyle() );
                        }
                        const QPen oldPen( painter->pen() );
                        painter->setPen( pen );
                        double nSubDelim = ( labelTexts->count() - 1 )
                                           / nSubDelimFactor;
                        modf( nSubDelim, &nSubDelim );
                        double pXSubDelimDelta = pXDelta * nSubDelimFactor;
                        double pYSubDelimDelta = pYDelta * nSubDelimFactor;
                        for ( double i = 1.0; i <= nSubDelim + 1.0; i += 1.0 ) {
                            if ( para.axisShowGrid() ) {
                                saveDrawLine( *painter,
                                              QPoint( pGAX, pGAY ),
                                              QPoint( pGZX, pGZY ), gridPen );
                                pGAX = xGA + i * pXSubDelimDelta;
                                pGAY = yGA + i * pYSubDelimDelta;
                                pGZX = xGZ + i * pXSubDelimDelta;
                                pGZY = yGZ + i * pYSubDelimDelta;
                            }
                            painter->drawLine( QPoint( p1X, p1Y ),
                                               QPoint( p2X, p2Y ) );
                            p1X = x1 + i * pXSubDelimDelta;
                            p1Y = y1 + i * pYSubDelimDelta;
                            p2X = x2 + i * pXSubDelimDelta;
                            p2Y = y2 + i * pYSubDelimDelta;
                        }
                        painter->setPen( oldPen );
                    }
                }
                x1 = p1.x();
                y1 = p1.y();
                x2 = p2.x();
                y2 = p2.y();
                xGA = pGA.x();
                yGA = pGA.y();
                xGZ = pGZ.x();
                yGZ = pGZ.y();
                p1X = x1;
                p1Y = y1;
                p2X = x2;
                p2Y = y2;
                pGAX = xGA;
                pGAY = yGA;
                pGZX = xGZ;
                pGZY = yGZ;
                xT = pTextsX;
                yT = pTextsY;
                // set up grid pen for drawing the normal grid lines
                if ( para.axisShowGrid() ) {
                    gridPen.setWidth( gridLineWidth );
                    gridPen.setStyle( para.axisGridStyle() );
                }
                double i = 0.0;
                for ( QStringList::Iterator labelit = labelTexts->begin();
                        labelit != labelTexts->end();
                        ++labelit ) {
                    i += 1.0;
                    if ( para.axisShowGrid() ) {
                        saveDrawLine( *painter,
                                      QPoint( pGAX - pGXMicroAdjust,
                                              pGAY - pGYMicroAdjust ),
                                      QPoint( pGZX - pGXMicroAdjust,
                                              pGZY - pGYMicroAdjust ),
                                      gridPen );
                        pGAX = xGA + i * pXDelta;
                        pGAY = yGA + i * pYDelta;
                        pGZX = xGZ + i * pXDelta;
                        pGZY = yGZ + i * pYDelta;
                    }
                    painter->drawLine( QPoint( p1X, p1Y ), QPoint( p2X, p2Y ) );
                    painter->drawText( pTextsX, pTextsY, pTextsW, pTextsH,
                                       textAlign, *labelit );
                    /* for debugging:
                    painter->drawRoundRect(pTextsX,pTextsY,pTextsW,pTextsH);
                    */
                    p1X = x1 + i * pXDelta;
                    p1Y = y1 + i * pYDelta;
                    p2X = x2 + i * pXDelta;
                    p2Y = y2 + i * pYDelta;
                    pTextsX = xT + i * pXDelta;
                    pTextsY = yT + i * pYDelta;
                }
                // adjust zero-line start, if not starting at origin
                if ( bOrdinate
                        && 0.0 != para.trueAxisLow() ) {
                    double x = p1.x();
                    double y = p1.y();
                    double mult = para.trueAxisLow() / para.trueAxisDelta();
                    x -= mult * pXDelta;
                    y -= mult * pYDelta;
                    axisZeroLineStartX = x;
                    axisZeroLineStartY = y;
                }
            } // if( nLabels )

            // draw zero-line (Ok, this might be overwritten by axes
            //  cause those are drawn after all labels and grid and
            //  zero-line(s) has been painted, see code 15 lines below.)
            if ( bOrdinate ) {
                ( ( KDChartAxisParams& ) para ).
                setAxisZeroLineStart( axisZeroLineStartX, axisZeroLineStartY );
                if ( ( KDChartAxisParams::AxisPosLeft == i )
                        || ( ( !params()->axisParams(
                                   KDChartAxisParams::AxisPosLeft
                               ).axisVisible() )
                             && ( KDChartAxisParams::AxisPosRight == i ) ) ) {
                    double xFactor = KDChartAxisParams::AxisPosRight == i
                                     ? -1.0
                                     : 1.0;
                    QPoint pZ0( para.axisZeroLineStartX(),
                                para.axisZeroLineStartY() );
                    QPoint pZ(  para.axisZeroLineStartX()
                              + xFactor * _dataRect.width(),
                                para.axisZeroLineStartY()
                              - std::fabs( pXDeltaFactor ) * _dataRect.height() );
                    saveDrawLine( *painter,
                                  pZ0,
                                  pZ,
                                  QPen( para.axisZeroLineColor(),
                                        lineWidth ) );
                }
            }
        }

    // draw all the axes
    for ( uint i2 = 0; i2 < KDChartParams::KDCHART_MAX_AXES; ++i2 )
        if ( params()->axisParams( i2 ).axisVisible()
                && KDChartAxisParams::AxisTypeUnknown
                != params()->axisParams( i2 ).axisType() ) {
            const KDChartAxisParams & para = params()->axisParams( i2 );
            KDChartAxisParams::AxisPos basicPos;
            QPoint orig, dest;
            findInfos( averageValueP1000, para, i2,
                       basicPos,
                       orig,
                       dest );
            painter->setPen( QPen( para.axisLineColor(),
                                   para.axisTrueLineWidth() ) );
            painter->drawLine( orig, dest );
        }
}


/**
   Calculates the actual label texts for one axis.

   \note When calling this function the actual area size for this
axis must be set, this means you may only call it when
\c KDChartPainter::setupGeometry() has been called before.

   \param painter the QPainter onto which the chart should be painted
   \param data the data that will be displayed as a chart
   \param params the KDChartParams that were specified globally
   \param axisNumber the number of this axis (used in some params structures)
   \param averageValueP1000 (average height+width of the prtbl. area) / 1000
   \param basicPos the basic axis position returned by
    KDChartAxisParams::basicAxisPos()
   \param orig the axis start point
   \param delimLen the length of one delimiter mark
   \param (all others) the reference parameters to be returned
    by this function
*/
void KDChartAxesPainter::calculateLabelTexts( QPainter& painter,
        const KDChartTableData& data,
        const KDChartParams& params,
        uint axisNumber,
        double averageValueP1000,
        KDChartAxisParams::AxisPos basicPos,
        const QPoint& orig,
        double delimLen,
        // start of return parameters
        double& nSubDelimFactor,
        double& pDelimDelta,
        double& nTxtHeight,
        double& pTextsX,
        double& pTextsY,
        double& pTextsW,
        double& pTextsH,
        int& textAlign )
{
    /*
        ((KDChartAxisParams&)para).
            setTrueAxisLowHighDelta( QMIN( data.minValue(), 0.0  ),
                                     QMAX( data.maxValue(), 0.0 ),
                                     10.0 );
    */
    const KDChartAxisParams & para = params.axisParams( axisNumber );

    // which dataset(s) is/are represented by this axis?
    uint dataset, dataset2, chart;
    if ( !params.axisDatasets( axisNumber, dataset, dataset2, chart ) ) {
        dataset = KDChartParams::KDCHART_ALL_DATASETS;
        dataset2 = KDChartParams::KDCHART_ALL_DATASETS;
        chart = 0;
    }
    // which dataset(s) with mode DataEntry is/are represented by this axis?
    uint dataDataset, dataDataset2;
    if( params.findDataset( KDChartParams::DataEntry,
                            dataDataset,
                            dataDataset2,
                            chart ) ) {
        // adjust dataDataset in case MORE THAN ONE AXIS
        //                    is representing THIS CHART
        if(    KDChartParams::KDCHART_ALL_DATASETS != dataset
            && KDChartParams::KDCHART_NO_DATASET   != dataset
            && KDChartParams::KDCHART_ALL_DATASETS != dataDataset
            && KDChartParams::KDCHART_NO_DATASET   != dataDataset )
            dataDataset  = QMAX( dataDataset,  dataset  );
        if(    KDChartParams::KDCHART_ALL_DATASETS != dataset2
            && KDChartParams::KDCHART_NO_DATASET   != dataset2
            && KDChartParams::KDCHART_ALL_DATASETS != dataDataset2
            && KDChartParams::KDCHART_NO_DATASET   != dataDataset2 )
            dataDataset2 = QMIN( dataDataset2, dataset2 );
    }
    else {
        // Should not happen
        qDebug( "IMPLEMENTATION ERROR: findDataset( DataEntry, ... ) should *always* return true." );
        dataDataset = KDChartParams::KDCHART_ALL_DATASETS;
    }
    /*
    qDebug("\nchart: %u,\ndataset: %u,  dataset2: %u,\ndataDataset: %u,  dataDataset2: %u",
    chart, dataset, dataset2, dataDataset, dataDataset2);
    */
    if ( para.axisLabelsFontUseRelSize() )
        nTxtHeight = para.axisLabelsFontRelSize()
                     * averageValueP1000;
    else {
        QFontInfo info( para.axisLabelsFont() );
        nTxtHeight = info.pointSize();
    }

    int behindComma = para.axisDigitsBehindComma();
    bool bOrdinate = para.axisSteadyValueCalc();

    QStringList labelTexts;
    int colNum = para.labelTextsDataRow();
    bool bDone = true;
    switch ( para.axisLabelTextsFormDataRow() ) {
    case KDChartAxisParams::LabelsFromDataRowYes: {
            // Take whatever is in the specified column (even if not a string)
            QString sVal;
            int trueBehindComma = -1;
            for ( uint iDataset = 0; iDataset < data.usedRows(); iDataset++ ) {
                const KDChartData& cell = data.cell( iDataset, colNum );
                if ( cell.isString() )
                    labelTexts.append( cell.stringValue() );
                else
                    labelTexts.append(
                        trunctateBehindComma( cell.doubleValue(),
                                              behindComma,
                                              para.axisValueDelta(),
                                              trueBehindComma ) );
            }
            break;
        }
    case KDChartAxisParams::LabelsFromDataRowGuess: {
            for ( uint iDataset = 0; iDataset < data.usedRows(); iDataset++ ) {
                const KDChartData& cell = data.cell( iDataset, colNum );
                if ( cell.isString()
                        && !cell.stringValue().isEmpty()
                        && !cell.stringValue().isNull() )
                    labelTexts.append( cell.stringValue() );
                else {
                    labelTexts.clear();
                    bDone = false;
                    break;
                }
            }
            break;
        }
    case KDChartAxisParams::LabelsFromDataRowNo: {
            bDone = false;
            break;
        }
    default:
        // Should not happen
        qDebug( "KDChart: Unknown label texts source" );
    }

    if ( !bDone ) {
        bDone = true;
        // look if a string list was specified
        if ( para.axisLabelStringList()
                && para.axisLabelStringList()->count() ) {
            uint nLabels = bOrdinate
                           ? para.axisLabelStringList()->count()
                           : data.usedCols();
            calculateBasicTextFactors( nTxtHeight, para, averageValueP1000,
                                       basicPos, orig, delimLen, nLabels,
                                       // start of return parameters
                                       pDelimDelta,
                                       pTextsX, pTextsY, pTextsW, pTextsH,
                                       textAlign );
            bool useShortLabels = false;
            QStringList* tmpList = para.axisLabelStringList();

            // find start- and/or end-entry
            uint iStart = 0;
            uint iEnd = para.axisLabelStringList()->count() - 1;
            if(    ! ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() )
                || ! ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) ) {
                bool testStart = !( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT
                                    == para.axisValueStart()
                                    && para.axisValueStart().isString() );
                bool testEnd = !( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT
                                  == para.axisValueEnd()
                                  && para.axisValueEnd().isString() );
                QString sStart = testStart
                                 ? para.axisValueStart().stringValue().upper()
                                 : QString::null;
                QString sEnd = testEnd
                               ? para.axisValueEnd().stringValue().upper()
                               : QString::null;

                uint i = 0;
                for ( QStringList::Iterator it = tmpList->begin();
                        it != tmpList->end(); ++it, ++i ) {
                    if ( 0 == iStart &&
                            0 == QString::compare( sStart, ( *it ).upper() ) ) {
                        iStart = i;
                    }
                    if ( 0 == QString::compare( sEnd, ( *it ).upper() ) ) {
                        iEnd = i;
                    }
                }
            }

            // check text widths to ensure all the entries will fit
            // into the available space
            if ( para.axisShortLabelsStringList()
                    && ( para.axisLabelStringList()
                         != para.axisShortLabelsStringList() )
                    && para.axisShortLabelsStringList()->count() ) {
                QFont font( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() )
                    font.setPointSizeFloat( nTxtHeight );
                QFontMetrics fm( font );

                QStringList::Iterator it = tmpList->begin();
                for ( uint i = 0; i < nLabels; ++i ) {
                    if ( it != tmpList->end() ) {
                        if ( fm.width( *it ) > pTextsW ) {
                            useShortLabels = true;
                            break;
                        }
                        ++it;
                    }
                }
            }
            if ( useShortLabels )
                tmpList = para.axisShortLabelsStringList();
            else
                tmpList = para.axisLabelStringList();

            // prepare transfering the strings into the labelTexts list
            double ddelta
            = ( KDChartAxisParams::AXIS_LABELS_AUTO_DELTA == para.axisValueDelta() )
              ? 1.0
              : para.axisValueDelta();
            modf( ddelta, &ddelta );
            bool positive = ( 0.0 <= ddelta );
            int delta = static_cast < int > ( std::fabs( ddelta ) );
            // find 1st significant entry
            QStringList::Iterator it = positive
                                       ? tmpList->begin()
                                       : tmpList->fromLast();
            if ( positive )
                for ( uint i = 0; i < tmpList->count(); ++i ) {
                    if ( i >= iStart )
                        break;
                    ++it;
                }
            else
                for ( uint i = tmpList->count() - 1; i >= 0; --i ) {
                    if ( i <= iEnd )
                        break;
                    --it;
                }
            // transfer the strings
            int meter = delta;
            uint i2 = positive ? iStart : iEnd;
            for ( uint i = 0; i < nLabels; ) {
                if ( positive ) {
                    if ( it == tmpList->end() ) {
                        it = tmpList->begin();
                        i2 = 0;
                    }
                } else {
                    if ( it == tmpList->begin() ) {
                        it = tmpList->end();
                        i2 = tmpList->count();
                    }
                }
                if ( ( positive && i2 >= iStart )
                        || ( !positive && i2 <= iEnd ) ) {
                    if ( meter >= delta ) {
                        labelTexts << *it;
                        ++i;
                        meter = 1;
                    } else {
                        meter += 1;
                    }
                }
                if ( positive ) {
                    if ( i2 == iEnd ) {
                        it = tmpList->begin();
                        i2 = 0;
                    } else {
                        ++it;
                        ++i2;
                    }
                } else {
                    if ( i2 == iStart ) {
                        it = tmpList->end();
                        i2 = tmpList->count();
                    } else {
                        --it;
                        --i2;
                    }
                }
            }
        } else {
            // find out if the associated dataset contains only strings
            // if yes, we will take these as label texts
            uint dset = ( dataset == KDChartParams::KDCHART_ALL_DATASETS ) ? 0 : dataset;
            for ( uint col = 0; col < data.usedCols(); ++col ) {
                const KDChartData& cell = data.cell( dset, col );
                if ( cell.isString()
                        && !cell.stringValue().isEmpty()
                        && !cell.stringValue().isNull() )
                    labelTexts.append( cell.stringValue() );
                else {
                    labelTexts.clear();
                    bDone = false;
                    break;
                }
            }
        }
    }

    if ( bDone ) {
        // Some strings were found, now let us see which of them are
        // actually to be taken right now.
        //
        //
        //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
        //
        //

    }
    else {
        // No strings were found, so let us either calculate the texts
        // based upon the numerical values of the associated dataset(s)
        // or just compose some default texts...
        if ( bOrdinate && data.usedCols() ) {
            double nLow = 0.0;
            double nHigh = 0.0;
            double orgLow = 0.0;
            double orgHigh = 0.0;
            double nDelta = 0.0;
            double nDist = 0.0;

            const KDChartParams::ChartType params_chartType
            = ( 0 == chart ) ? params.chartType()
              : params.additionalChartType();
            enum { Normal, Stacked, Percent } mode;
            switch ( params_chartType ) {
            case KDChartParams::Bar:
                if ( KDChartParams::BarStacked
                        == params.barChartSubType() )
                    mode = Stacked;
                else if ( KDChartParams::BarPercent
                          == params.barChartSubType() )
                    mode = Percent;
                else
                    mode = Normal;
                break;
            case KDChartParams::Line:
                if ( KDChartParams::LineStacked
                        == params.lineChartSubType() )
                    mode = Stacked;
                else if ( KDChartParams::LinePercent
                          == params.lineChartSubType() )
                    mode = Percent;
                else
                    mode = Normal;
                break;
            case KDChartParams::Area:
                if ( KDChartParams::AreaStacked
                        == params.areaChartSubType() )
                    mode = Stacked;
                else if ( KDChartParams::AreaPercent
                          == params.areaChartSubType() )
                    mode = Percent;
                else
                    mode = Normal;
                break;
            default: {
                    // Should not happen
                    qDebug( "IMPLEMENTATION ERROR: Unknown params_chartType in calculateLabelTexts()" );
                    mode = Normal;
                }
            }

            uint nLabels = 200;

            // find highest and lowest value of associated dataset(s)
            bool bOrdFactorsOk = false;
            bool bAutoCalcStart =
                   ( Normal == mode )
                && ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() );
            bool bAutoCalcEnd =
                   ( Normal == mode )
                && ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() );
            if ( ! bAutoCalcStart && ! bAutoCalcEnd ) {
                switch ( mode ) {
                case Normal: {
                        nLow = para.axisValueStart().doubleValue();
                        nHigh = para.axisValueEnd().doubleValue();
                        nDist = nHigh - nLow;
                        break;
                    }
                case Stacked: {
                        if ( dataDataset == KDChartParams::KDCHART_ALL_DATASETS ) {
                            nLow  = QMIN( data.minColSum(), 0.0 );
                            nHigh = QMAX( data.maxColSum(), 0.0 );
                        } else {
                            nLow  = QMIN( data.minColSum( dataDataset, dataDataset2 ), 0.0 );
                            nHigh = QMAX( data.maxColSum( dataDataset, dataDataset2 ), 0.0 );
                        }
                        break;
                    }
                case Percent: {
                        nLow   =   0.0;
                        nHigh  = 100.0;
                        nDist  = 100.0;
                        nDelta =  10.0;
                        nSubDelimFactor = 0.25;
                        bOrdFactorsOk = true;
                        break;
                    }
                }
                // qDebug("\n[A] nLow: %f,  nHigh: %f", nLow, nHigh);
            } else {
                // Usage of this additional bool parameter instead of
                // just initializing nLow and nHigh with values taken
                // from col zero enabels us to compute rows with
                // missing entries!  :-)
                bool bInit = true;

                if ( dataDataset == KDChartParams::KDCHART_ALL_DATASETS ) {
                    nLow  = data.minValue();
                    nHigh = data.maxValue();
                } else {
                    nLow  = data.minInRows( dataDataset, dataDataset2 );
                    nHigh = data.maxInRows( dataDataset, dataDataset2 );
                }
                /*
                qDebug("\n[A] chart: %u,\ndataset: %u,  dataset2: %u,\ndataDataset: %u,  dataDataset2: %u",
                chart, dataset, dataset2, dataDataset, dataDataset2);
                qDebug("nLow: %f,  nHigh: %f", nLow, nHigh);
                */
                // Note: a double that was initialized with 0.0
                //       is NOT equal a double initialized with 0 !!!
                if(    MAXDOUBLE == nLow
                    || (    ( 0.0 == nHigh || 0 == nHigh )
                         && ( 0.0 == nLow  || 0 == nLow ) ) ) {
                    // qDebug("NO values or all values have ZERO value, showing 0.0 - 1.0 span");
                    nLow   = 0.0;
                    nHigh  = 1.0;
                    nDist  = 1.0;
                    nDelta = 0.5;
                    nSubDelimFactor = 0.2;
                    bOrdFactorsOk = true;
                }
                // ignore one of the above found values?
                if ( !bAutoCalcStart ) {
                    nLow = para.axisValueStart().doubleValue();
                    bOrdFactorsOk = false;
                }
                if ( !bAutoCalcEnd ) {
                    nHigh = para.axisValueEnd().doubleValue();
                    bOrdFactorsOk = false;
                }

                if ( !bOrdFactorsOk ) {
                    // adjust one or both of our limit values
                    // according to first two digits of (nHigh - nLow) delta
                    double nDivisor;
                    double nRound;
                    nDist = nHigh - nLow;
                    // replace nLow (or nHigh, resp.) by zero if NOT ALL OF
                    // our values are located outside of the 'max. empty
                    //  inner space' (i.e. percentage of the y-axis range
                    // that may to contain NO data entries)
                    int maxEmpty = para.axisMaxEmptyInnerSpan();
                    if( bAutoCalcStart ) {
                    // qDebug("\nbAutoCalcStart:\n  nLow:  %f\n  nHigh: %f", nLow, nHigh );
                        if( 0.0 < nLow ) {
                            if(    maxEmpty == KDChartAxisParams::AXIS_IGNORE_EMPTY_INNER_SPAN
                                || maxEmpty > ( nLow / nHigh * 100.0 ) )
                               nLow = 0.0;
                            else if( nDist / 100.0 < nLow )
                                nLow -= nDist / 100.0; // shift lowest value

                        }
                        else if( nDist / 100.0 < std::fabs( nLow ) )
                            nLow -= nDist / 100.0; // shift lowest value
                        nDist = nHigh - nLow;
                    // qDebug("* nLow:  %f\n  nHigh: %f", nLow, nHigh );
                    }
                    if( bAutoCalcEnd ) {
                    // qDebug("\nbAutoCalcEnd:\n  nLow:  %f\n  nHigh: %f", nLow, nHigh );
                        if( 0.0 > nHigh ) {
                            if(    maxEmpty == KDChartAxisParams::AXIS_IGNORE_EMPTY_INNER_SPAN
                                || maxEmpty > ( nHigh / nLow * 100.0 ) )
                                nHigh = 0.0;
                            else if( nDist / 100.0 > nHigh )
                                nHigh += nDist / 100.0; // shift highest value
                        }
                        else if( nDist / 100.0 < std::fabs( nHigh ) )
                            nHigh += nDist / 100.0; // shift highest value
                        nDist = nHigh - nLow;
                    // qDebug("* nLow:  %f\n  nHigh: %f", nLow, nHigh );
                    }
                    // find out factors and adjust nLow and nHigh
                    orgLow = nLow;
                    orgHigh = nHigh;
                    calculateOrdinateFactors(
                        para, nDist, nDivisor, nRound,
                        nDelta, nSubDelimFactor, nLow, nHigh );
                    nLabels = params.roundVal( nDist / nDelta );
                    /*
                    qDebug("\n0.  nDist: %f\n    nHigh: %f\n    nLow %f",
                    nDist, nHigh, nLow);
                    qDebug("    nDelta: %f", nDelta);
                    qDebug("    nRound: %f", nRound);
                    qDebug("    nLabels: %u", nLabels);
                    */
                    if( para.axisSteadyValueCalc() ) {
                        ++nLabels;
                        // qDebug("*   nLabels: %u", nLabels );
                    }
                }
            }

            // calculate the amount of nLabels to be written we could take
            // based on the space we have for writing the label texts
            if ( ! ( KDChartAxisParams::AXIS_LABELS_AUTO_DELTA
                     == para.axisValueDelta() ) ) {
                nDist = nHigh - nLow;
                nDelta = para.axisValueDelta();
                nLabels = params.roundVal( nDist / nDelta );
                /*
                qDebug("\nI nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u",
                        nLow, nHigh, nDelta, nLabels );
                */
                if( para.axisSteadyValueCalc() ) {
                    ++nLabels;
                    // qDebug("* nLabels: %u", nLabels );
                }
            }

            double areaHeight = para.axisTrueAreaRect().height();
            double nDivisor;
            double nRound;
            orgLow = nLow;
            orgHigh = nHigh;
            bool bTryNext = false;
            while ( ( 2 < nLabels )
                    && ( areaHeight < ( nTxtHeight * 1.5 ) * nLabels ) ) {
                nDist = nHigh - nLow;
                nLow = orgLow;
                nHigh = orgHigh;
                /*
                qDebug("\n1.  nDist: %f\n  nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u",
                        nDist, nLow, nHigh, nDelta, nLabels );
                */
                calculateOrdinateFactors( para,
                                          nDist, nDivisor, nRound,
                                          nDelta,
                                          nSubDelimFactor, nLow, nHigh,
                                          bTryNext );
                nLabels = params.roundVal( nDist / nDelta );
                /*
                qDebug("\n2.  nDist: %f\n+ nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u",
                        nDist, nLow, nHigh, nDelta, nLabels );
                */
                if( para.axisSteadyValueCalc() ) {
                    ++nLabels;
                    // qDebug("\nnLabels: %u", nLabels );
                }
                /*
                qDebug("n.  nDist = nHigh - nLow: %f = %f - %f",nDist, nHigh, nLow);
                qDebug("    nRound: %f", nRound);
                */
                bTryNext = true;
            }

            // finally we can build the texts
            int trueBehindComma = -1;
            double nVal = nLow;
            for ( uint i = 0; i < nLabels; ++i ) {
                labelTexts.append( trunctateBehindComma( nVal,
                                   behindComma,
                                   nDelta,
                                   trueBehindComma ) );
                nVal += nDelta;
            }

            // save our true Low and High value
            if ( para.axisSteadyValueCalc() ) {
                nHigh = nVal - nDelta;
            }
            ( ( KDChartAxisParams& ) para ).setTrueAxisLowHighDelta(
                                                nLow, nHigh, nDelta );
            // qDebug("\n[Z] nLow: %f,  nHigh: %f,  nDelta: %f", nLow, nHigh, nDelta );
            bDone = true;
        }

        // let's generate some strings
        if ( !bDone ) {
            // default scenario
            uint count = data.usedCols() ? data.usedCols() : 1;
            KDChartData start( 1.0 );
            double delta( 1.0 );
            KDChartData finis( start.doubleValue()
                               + delta * ( count - 1 ) );

            bool deltaIsAuto = true;
            if ( !( KDChartAxisParams::AXIS_LABELS_AUTO_DELTA == para.axisValueDelta() ) ) {
                delta = para.axisValueDelta();
                deltaIsAuto = false;
            }

            if ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() ) {
                if ( ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) ) {
                    finis = start.doubleValue()
                            + delta * ( count - 1 );
                } else {
                    if ( para.axisValueEnd().isDouble() ) {
                        finis = para.axisValueEnd();
                        start = finis.doubleValue()
                                - delta * ( count - 1 );
                    } else {
                        //
                        //
                        //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                        //
                        //
                    }
                }
            }
            else {
                if ( para.axisValueStart().isDouble() ) {
                    start = para.axisValueStart();
                } else {
                    //
                    //
                    //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                    //
                    //
                }
                if ( !( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) ) {
                    if ( para.axisValueEnd().isDouble() ) {
                        finis = para.axisValueEnd();
                        if ( deltaIsAuto ) {
                            delta = ( finis.doubleValue()
                                      - start.doubleValue() ) / count;
                        } else {
                            count = static_cast < uint > (
                                        ( finis.doubleValue()
                                          - start.doubleValue() ) / delta );
                        }
                    } else {
                        // auto-rows like
                        // sunday, monday, tuesday, ...
                        //
                        //
                        //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                        //
                        //
                    }
                }
                else {
                    finis = start.doubleValue()
                            + delta * ( count - 1 );
                }
            }
            QString prefix( QObject::tr( "Item " ) );
            QString postfix;


            if ( start.isDouble() && finis.isDouble() ) {
                int precis =
                    KDChartAxisParams::AXIS_LABELS_AUTO_DIGITS == para.axisDigitsBehindComma()
                    ? 0
                    : para.axisDigitsBehindComma();
                double s = start.doubleValue();
                double f = finis.doubleValue();
                bool up = ( 0.0 < delta );

                // check the text widths of one large(?) entry
                // and hope all the entries will
                // fit into the available space
                double value = up ? s : f;
                uint nLabels = 0;
                while ( up ? ( value <= f ) : ( value >= s ) ) {
                    ++nLabels;
                    value += delta * up ? 1.0 : -1.0;
                }
                calculateBasicTextFactors( nTxtHeight, para,
                                           averageValueP1000,
                                           basicPos, orig, delimLen, nLabels,
                                           // start of return parameters
                                           pDelimDelta,
                                           pTextsX, pTextsY, pTextsW, pTextsH,
                                           textAlign );
                QFont font( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() )
                    font.setPointSizeFloat( nTxtHeight );
                QFontMetrics fm( font );
                if ( fm.width( prefix +
                               QString::number( -std::fabs( ( s + f ) / 2.0 + delta ),
                                                'f', precis ) )
                        > pTextsW ) {
                    prefix = "[ ";
                    postfix = " ]";
                }

                // now transfer the strings into labelTexts
                value = up ? s : f;
                while ( up ? ( value <= f ) : ( value >= s ) ) {
                    labelTexts.append(
                        prefix + QString::number( value, 'f', precis )
                        + postfix );
                    value += delta * up ? 1.0 : -1.0;
                }
            } else {

                // check the text widths largest entry
                // to make sure it will fit into the available space
                calculateBasicTextFactors( nTxtHeight, para,
                                           averageValueP1000,
                                           basicPos, orig, delimLen,
                                           data.usedCols(),
                                           // start of return parameters
                                           pDelimDelta,
                                           pTextsX, pTextsY, pTextsW, pTextsH,
                                           textAlign );
                QFont font( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() )
                    font.setPointSizeFloat( nTxtHeight );
                QFontMetrics fm( font );
                if ( fm.width( prefix + QString::number( data.usedCols() - 1 ) )
                        > pTextsW ) {
                    prefix = "[ ";
                    postfix = " ]";
                }

                // now transfer the strings into labelTexts
                for ( uint col = 1; col <= data.usedCols(); ++col )
                    labelTexts.append(
                        prefix + QString::number( col ) + postfix );
            }
        }
    }

    uint nLabels = labelTexts.count()
                   ? labelTexts.count()
                   : 0;

    calculateBasicTextFactors( nTxtHeight, para, averageValueP1000,
                               basicPos, orig, delimLen, nLabels,
                               // start of return parameters
                               pDelimDelta,
                               pTextsX, pTextsY, pTextsW, pTextsH,
                               textAlign );





    ( ( KDChartAxisParams& ) para ).setAxisLabelTexts( &labelTexts );
    /*
    	qDebug( "Found label texts:" );
        for ( QStringList::Iterator it = labelTexts.begin();
                                it != labelTexts.end(); ++it )
            qDebug( ">>>  %s", (*it).latin1() );
        qDebug( "\n" );
    */
}


/**
   Calculates some label text factors needed
   by function \c calculateLabelTexts()

   \note When calling this function the actual area size for this
axis must be set, this means you may only call it when
\c KDChartPainter::setupGeometry() has been called before.

   \param nTxtHeight the text height to be used for calculating
    the return values
   \param para the KDChartAxisParams that were specified for this axis
   \param averageValueP1000 (average height+width of the prtbl. area) / 1000
   \param basicPos the basic axis position returned by
    KDChartAxisParams::basicAxisPos()
   \param orig the axis start point
   \param delimLen the length of one delimiter mark
   \param nLabels the number of labels to be shown at this axis
   \param (all others) the reference parameters to be returned
    by this function
*/
void KDChartAxesPainter::calculateBasicTextFactors( double nTxtHeight,
        const KDChartAxisParams& para,
        double averageValueP1000,
        KDChartAxisParams::AxisPos basicPos,
        const QPoint& orig,
        double delimLen,
        uint nLabels,
        // start of return params
        double& pDelimDelta,
        double& pTextsX,
        double& pTextsY,
        double& pTextsW,
        double& pTextsH,
        int& textAlign )
{
    switch ( basicPos ) {
    case KDChartAxisParams::AxisPosBottom: {
            bool bTouchEdges = para.axisLabelsTouchEdges();
            double wid = para.axisTrueAreaRect().width();
            double divi = bTouchEdges
                          ? ( 1 < nLabels ? nLabels - 1 : 1 )
              : ( nLabels ? nLabels : 10 );
            pDelimDelta = wid / divi;

            pTextsW = pDelimDelta - 4.0;
            pTextsX = orig.x() + 2.0
                      - ( bTouchEdges
                          ? pDelimDelta / 2.0
                          : 0.0 );
            pTextsH = para.axisTrueAreaRect().height() - delimLen * 1.33;
            pTextsY = orig.y()
                      + delimLen * 1.33;
            textAlign = Qt::AlignHCenter | Qt::AlignTop;
        }
        break;
    case KDChartAxisParams::AxisPosLeft: {
            double hig = para.axisTrueAreaRect().height();
            pDelimDelta = hig / ( 1 < nLabels ? nLabels - 1 : 1 );

            pTextsX = para.axisTrueAreaRect().bottomLeft().x()
                      + 2.0;
            pTextsY = orig.y() - nTxtHeight / 2;
            pTextsW = para.axisTrueAreaRect().width()
                      - delimLen * 1.33 - 2.0;
            pTextsH = nTxtHeight;
            textAlign = Qt::AlignRight | Qt::AlignVCenter;
        }
        break;
    case KDChartAxisParams::AxisPosTop: {
            bool bTouchEdges = para.axisLabelsTouchEdges();
            double wid = para.axisTrueAreaRect().width();
            double divi = bTouchEdges
                          ? ( 1 < nLabels ? nLabels - 1 : 1 )
              : ( nLabels ? nLabels : 10 );
            pDelimDelta = wid / divi;

            pTextsW = pDelimDelta - 4.0;
            pTextsX = orig.x() + 2.0
                      - ( bTouchEdges
                          ? pDelimDelta / 2.0
                          : 0.0 );
            pTextsH = para.axisTrueAreaRect().height() - delimLen * 1.33;
            pTextsY = para.axisTrueAreaRect().topLeft().y();

            textAlign = Qt::AlignHCenter | Qt::AlignBottom;
        }
        break;
    case KDChartAxisParams::AxisPosRight: {
            double hig = para.axisTrueAreaRect().height();
            pDelimDelta = hig / ( 1 < nLabels ? nLabels - 1 : 1 );

            pTextsX = para.axisTrueAreaRect().bottomLeft().x()
                      + delimLen * 1.33;
            pTextsY = orig.y() - nTxtHeight / 2;
            pTextsW = para.axisTrueAreaRect().width()
                      - delimLen * 1.33 - 2.0;
            pTextsH = nTxtHeight;
            textAlign = Qt::AlignLeft | Qt::AlignVCenter;
        }
        break;
    }
}


/**
   Takes double \c nVal and returns a QString showing the amount of digits
   behind the comma that was specified by \c behindComma (or calculated
   automatically by removing trailing zeroes, resp.).
   To make sure the resulting string looks fine together with other strings
   of the same label row please specify \c nDelta indicating the step width
   from one label text to the other.
   To prevent the function from having to re-calculate the number of
   digits to keep behind the comma, provide it with a temporary helper
   variable that has to be initialized with a value smaller zero.

   \note This function is reserved for internal use.
*/
QString KDChartAxesPainter::trunctateBehindComma( const double nVal,
        const double behindComma,
        const double nDelta,
        int& trueBehindComma )
{
    QString sVal = QString::number( nVal, 'f',
                                    KDChartAxisParams::AXIS_LABELS_AUTO_DIGITS == behindComma
                                    ? 10 : behindComma );
    if ( KDChartAxisParams::AXIS_LABELS_AUTO_DIGITS == behindComma ) {
        int comma = sVal.find( '.' );
        if ( -1 < comma ) {
            if ( KDChartAxisParams::AXIS_LABELS_AUTO_DELTA == nDelta ) {
                int i = sVal.length();
                while ( 1 < i
                        && '0' == sVal[ i - 1 ] )
                    --i;
                sVal.truncate( i );
                if ( '.' == sVal[ i - 1 ] )
                    sVal.truncate( i - 1 );
            } else {
                if ( 0 > trueBehindComma ) {
                    QString sDelta = QString::number( nDelta, 'f', 10 );
                    int i = sDelta.length();
                    while ( 1 < i
                            && '0' == sDelta[ i - 1 ] )
                        --i;
                    sDelta.truncate( i );
                    if ( '.' == sDelta[ i - 1 ] )
                        trueBehindComma = 0;
                    else {
                        int deltaComma = sDelta.find( '.' );
                        if ( -1 < deltaComma )
                            trueBehindComma = sDelta.length() - deltaComma - 1;
                        else
                            trueBehindComma = 0;
                    }
                }
                int nPos = comma + ( trueBehindComma ? trueBehindComma + 1 : 0 );
                sVal.truncate( nPos );
            }
        }
    }
    return sVal;
}

/**
   Calculates the factors to be used for calculating ordinate labels texts.

   \note This function is reserved for internal use.
*/
void KDChartAxesPainter::calculateOrdinateFactors(
    const KDChartAxisParams& para,
    double& nDist,
    double& nDivisor,
    double& nRound,
    double& nDelta,
    double& nSubDelimFactor,
    double& nLow,
    double& nHigh,
    bool findNextRound )
{
    if ( findNextRound ) {
        if ( 1.0 > nRound )
            nRound = 1.0;
        else
            if ( 2.0 > nRound )
                nRound = 2.0;
            else
                if ( 2.5 > nRound )
                    nRound = 2.5;
                else
                    if ( 5.0 > nRound )
                        nRound = 5.0;
                    else {
                        nDivisor *= 10.0;
                        nRound = 1.0;
                    }
    } else {
        nDivisor = 1.0;
        QString sDistDigis2;
        sDistDigis2.setNum( nDist, 'f' );
        if ( 1.0 > nDist ) {
            sDistDigis2.remove( 0, 2 );
            nDivisor = 0.01;
            while ( 0 < sDistDigis2.length()
                    && '0' == sDistDigis2[ 0 ] ) {
                nDivisor *= 0.1;
                sDistDigis2.remove( 0, 1 );
            }
        } else {
            if ( 10.0 > nDist ) {
                nDivisor = 0.1;
                // remove comma, if present
                sDistDigis2.remove( 1, 1 );
            } else
                if ( 100.0 > nDist )
                    nDivisor = 1.0;
                else {
                    int comma = sDistDigis2.find( '.' );
                    if ( -1 < comma )
                        sDistDigis2.truncate( comma );
                    nDivisor = pow( 10.0, ( int ) sDistDigis2.length() - 2 );
                }
        }
        sDistDigis2.truncate( 2 );
        bool bOk;
        double nDistDigis2( sDistDigis2.toDouble( &bOk ) );
        if ( !bOk )
            nDistDigis2 = 10.0;
        if ( 75.0 <= nDistDigis2 )
            nRound = 5.0;
        else
            if ( 40.0 <= nDistDigis2 )
                nRound = 2.5;
            else
                if ( 20.0 <= nDistDigis2 )
                    nRound = 2.0;
                else
                    nRound = 1.0;
    }

    nDelta = nRound * nDivisor;
    /*
    qDebug("  n D i s t       :  %f", nDist   );
    qDebug("  n D i v i s o r :  %f", nDivisor);
    qDebug("  n R o u n d     :  %f", nRound  );
    qDebug("  n D e l t a     :  %f", nDelta  );
    qDebug("  nHigh           :  %f", nHigh   );
    qDebug("  nLow            :  %f", nLow    );
    */
    if ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() ) {
        double orgLow( nLow );
        modf( nLow / nDivisor, &nLow );
        double nMod = modf( nLow / nRound, &nLow );
        nLow *= nRound * nDivisor;
        if ( nLow > orgLow )
            nLow -= nRound * nDivisor;
        if ( 0.0 < nLow && 0.0 >= orgLow )
            nLow = 0.0;
    }
    if ( KDChartAxisParams::AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) {
        double orgHigh( nHigh );
        //nHigh += 1.1 * nDelta;
        modf( nHigh / nDivisor, &nHigh );
        double nMod = modf( nHigh / nRound, &nHigh );
        nHigh *= nRound * nDivisor;
        if ( nHigh < orgHigh )
            nHigh += nRound * nDivisor;
        if ( 0.0 > nHigh && 0.0 <= orgHigh )
            nHigh = 0.0;
    }
    /*
    qDebug("  n H i g h       :  %f", nHigh   );
    qDebug("  n L o w         :  %f\n", nLow    );
    */
    if ( 1.0 == nRound )
        nSubDelimFactor = 0.5;
    else
        if ( 2.0 == nRound )
            nSubDelimFactor = 0.25;
        else
            if ( 2.5 == nRound )
                nSubDelimFactor = 0.2;
            else
                if ( 5.0 == nRound )
                    nSubDelimFactor = 0.2;
                else {
                    // Should not happen
                    qDebug( "IMPLEMENTATION ERROR: Unknown nRound in calculateOrdinateFactors()" );
                    nSubDelimFactor = 1.0;
                }

    nDist = nHigh - nLow;
}

/**** static ****/
void KDChartAxesPainter::saveDrawLine( QPainter& painter,
                                       QPoint pA,
                                       QPoint pZ,
                                       QPen pen )
{
    const QPen oldPen( painter.pen() );
    bool bNice =    ( pen.color() == oldPen.color() )
                 && ( pen.width() == oldPen.width() )
                 && ( pen.style() == oldPen.style() );
    if ( !bNice )
        painter.setPen( pen );
    painter.drawLine( pA, pZ );
    if ( !bNice )
        painter.setPen( oldPen );
}
