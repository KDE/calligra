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
#include "KDChartPolarPainter.h"
#include <KDChartParams.h>
#include <KDChartAxisParams.h>
#include "KDChartAxesPainter.h"
#include "KDDrawText.h"

#include <qpainter.h>
//Added by qt3to4:
#include <Q3PointArray>

/**
  \class KDChartPolarPainter KDChartPolarPainter.h

  \brief A chart painter implementation that can paint polar charts.
*/

/**
  Constructor. Sets up internal data structures as necessary.

  \param params the KDChartParams structure that defines the chart
  \param data the data that will be displayed as a chart
*/
KDChartPolarPainter::KDChartPolarPainter( KDChartParams* params ) :
    KDChartPainter( params )
{
    // This constructor intentionally left blank so far; we cannot setup the
    // geometry yet since we do not know the size of the painter.
}


/**
  Destructor.
*/
KDChartPolarPainter::~KDChartPolarPainter()
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
void KDChartPolarPainter::paintData( QPainter* painter,
                                     KDChartTableDataBase* data,
                                     bool paint2nd,
                                     KDChartDataRegionList* regions )
{
    uint chart = paint2nd ? 1 : 0;

    QRect ourClipRect( _dataRect );
    ourClipRect.setBottom( ourClipRect.bottom() - 1 ); // protect axes
    ourClipRect.setLeft( ourClipRect.left() + 1 );
    ourClipRect.setRight( ourClipRect.right() - 1 );
    //
    // PENDING(khz) adjust the clip rect if neccessary...
    //

    const QMatrix & world = painter->worldMatrix();
    ourClipRect =
#if COMPAT_QT_VERSION >= 0x030000
        world.mapRect( ourClipRect );
#else
    world.map( ourClipRect );
#endif

    painter->setClipRect( ourClipRect );


    uint datasetStart, datasetEnd;
    findChartDatasets( data, paint2nd, chart, datasetStart, datasetEnd );


    painter->translate( _dataRect.x(), _dataRect.y() );


    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    int numValues = 0;
    if ( params()->numValues() != -1 )
        numValues = params()->numValues();
    else
        numValues = data->usedCols();

    // compute position
    int size = qMin( _dataRect.width(), _dataRect.height() ); // initial size

    const double minSizeP1000 = size / 1000.0;

    int x = ( _dataRect.width() == size ) ? 0 : ( ( _dataRect.width() - size ) / 2 );
    int y = ( _dataRect.height() == size ) ? 0 : ( ( _dataRect.height() - size ) / 2 );
    QRect position( x, y, size, size );

    QPoint center( position.width() / 2 + position.x(),
                   position.height() / 2 + position.y() );


    double maxValue;
    switch ( params()->polarChartSubType() ) {
        case KDChartParams::PolarNormal:
            maxValue = data->maxValue();
            break;
        case KDChartParams::PolarPercent:
            maxValue = 100.0;
            break;
        default:
            maxValue = qMax( data->maxColSum(), 0.0 );
    }

    double pixelsPerUnit = 0.0;
    // the / 2 in the next line is there because we need the space in
    // both directions
    pixelsPerUnit = (position.height() / maxValue / 2) * 1000 / 1250;

    QMap < int, double > currentValueSums;
    if (    params()->polarChartSubType() == KDChartParams::PolarStacked
            || params()->polarChartSubType() == KDChartParams::PolarPercent )
        // this array is only used for stacked and percent polar
        // charts, no need to waste time initializing it for normal
        // ones
        for ( int value = 0; value < numValues; value++ )
            currentValueSums[ value ] = 0.0;
    QMap < int, double > totalValueSums;


    /*
       axes schema: use AxisPosSagittal for sagittal 'axis' lines
       use AxisPosCircular for circular 'axis'
       */
    const KDChartAxisParams & paraSagittal = params()->axisParams( KDChartAxisParams::AxisPosSagittal );
    const KDChartAxisParams & paraCircular = params()->axisParams( KDChartAxisParams::AxisPosCircular );

    int sagittalLineWidth = 0 <= paraSagittal.axisLineWidth()
        ? paraSagittal.axisLineWidth()
        : -1 * static_cast < int > (   paraSagittal.axisLineWidth()
                * minSizeP1000 );
    ( ( KDChartAxisParams& ) paraSagittal ).setAxisTrueLineWidth( sagittalLineWidth );
    int sagittalGridLineWidth
        = (    KDCHART_AXIS_GRID_AUTO_LINEWIDTH
                == paraSagittal.axisGridLineWidth() )
        ? sagittalLineWidth
        : (   ( 0 <= paraSagittal.axisGridLineWidth() )
                ? paraSagittal.axisGridLineWidth()
                : -1 * static_cast < int > (   paraSagittal.axisGridLineWidth()
                    * minSizeP1000 ) );

    int circularLineWidth = 0 <= paraCircular.axisLineWidth()
        ? paraCircular.axisLineWidth()
        : -1 * static_cast < int > (   paraCircular.axisLineWidth()
                * minSizeP1000 );
    ( ( KDChartAxisParams& ) paraCircular ).setAxisTrueLineWidth( circularLineWidth );
    int circularGridLineWidth
        = (    KDCHART_AXIS_GRID_AUTO_LINEWIDTH
                == paraCircular.axisGridLineWidth() )
        ? circularLineWidth
        : (   ( 0 <= paraCircular.axisGridLineWidth() )
                ? paraCircular.axisGridLineWidth()
                : -1 * static_cast < int > (   paraCircular.axisGridLineWidth()
                    * minSizeP1000 ) );

    QFont actFont;
    int labels = 0;
    double currentRadiusPPU = position.height() / 2.0;

    // draw the "axis" circles
    if( paraCircular.axisShowGrid()
            || paraCircular.axisVisible()
            || paraCircular.axisLabelsVisible() ) {

        double radiusPPU = maxValue * pixelsPerUnit;
        double pDelimDelta = 0.0;

        // calculate label texts
        QStringList* labelTexts = 0;
        ((KDChartParams*)params())->setAxisArea( KDChartAxisParams::AxisPosCircular,
                                                 QRect( 0,
                                                     0,
                                                     static_cast<int>( radiusPPU ),
                                                     static_cast<int>( radiusPPU ) ) );

        double delimLen = 20.0 * minSizeP1000; // per mille of area
        KDChartAxisParams::AxisPos basicPos;
        QPoint orig, dest;
        double dDummy;
        double nSubDelimFactor = 0.0;
        double nTxtHeight = 0.0;
        double pTextsX = 0.0;
        double pTextsY = 0.0;
        double pTextsW = 0.0;
        double pTextsH = 0.0;
        int textAlign = Qt::AlignHCenter | Qt::AlignVCenter;
        bool isLogarithmic = false;
        bool isDateTime = false;
        bool autoDtLabels = false;
        QDateTime dtLow;
        QDateTime dtHigh;
        KDChartAxisParams::ValueScale dtDeltaScale;
        KDChartAxesPainter::calculateLabelTexts(
                painter,
                *data,
                *params(),
                KDChartAxisParams::AxisPosCircular,
                minSizeP1000,
                delimLen,
                // start of reference parameters
                basicPos,
                orig,
                dest,
                dDummy,dDummy,dDummy,dDummy,
                nSubDelimFactor,
                pDelimDelta,
                nTxtHeight,
                pTextsX,
                pTextsY,
                pTextsW,
                pTextsH,
                textAlign,
                isLogarithmic,
                isDateTime,
                autoDtLabels,
                dtLow,
                dtHigh,
                dtDeltaScale );
                labelTexts = ( QStringList* ) paraCircular.axisLabelTexts();
        if( paraCircular.axisLabelsVisible() ) {
//qDebug("\nnTxtHeight: "+QString::number(nTxtHeight));
            // calculate font size
            actFont = paraCircular.axisLabelsFont();
            if ( paraCircular.axisLabelsFontUseRelSize() ) {
//qDebug("paraCircular.axisLabelsFontUseRelSize() is TRUE");
                actFont.setPointSizeFloat( nTxtHeight );
            }
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
        }

        double radiusDelta = pDelimDelta;

        labels = labelTexts
            ? labelTexts->count()
            : 0;
        if( labels )
            currentRadiusPPU = -radiusDelta;
        for( int iLabel = 0; iLabel < labels; ++iLabel ) {
            //while( currentRadius < maxValue ) {
            //double currentRadiusPPU = currentRadius;
            currentRadiusPPU += radiusDelta;
            double currentRadiusPPU2 = currentRadiusPPU * 2;
            int circularAxisAngle = ( currentRadiusPPU != 0.0 ) ? ( static_cast < int > (4.0 * radiusPPU / currentRadiusPPU) ) : 0;
            if( paraCircular.axisShowGrid() ) {
                painter->setPen( QPen( paraCircular.axisGridColor(),
                            circularGridLineWidth ) );
                painter->drawEllipse( static_cast<int>( center.x() - currentRadiusPPU ),
                        static_cast<int>( center.y() - currentRadiusPPU ),
                        static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ) );
            }
            if( paraCircular.axisVisible() ) {
                painter->setPen( QPen( paraCircular.axisLineColor(),
                            circularLineWidth ) );
                if( params()->polarDelimAtPos( KDChartEnums::PosTopCenter ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (90 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );
                if( params()->polarDelimAtPos( KDChartEnums::PosBottomCenter ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (270 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );

                if( params()->polarDelimAtPos( KDChartEnums::PosCenterRight ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (0 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );
                if( params()->polarDelimAtPos( KDChartEnums::PosCenterLeft ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (180 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );

                if( params()->polarDelimAtPos( KDChartEnums::PosTopRight ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (45 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );
                if( params()->polarDelimAtPos( KDChartEnums::PosBottomLeft ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (225 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );

                if( params()->polarDelimAtPos( KDChartEnums::PosBottomRight ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (315 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );
                if( params()->polarDelimAtPos( KDChartEnums::PosTopLeft ) )
                    painter->drawArc( static_cast<int>( center.x() - currentRadiusPPU ),
                            static_cast<int>( center.y() - currentRadiusPPU ),
                            static_cast<int>( currentRadiusPPU2 ), static_cast<int>( currentRadiusPPU2 ),
                            (135 - circularAxisAngle/2) * 16,
                            circularAxisAngle * 16 );
            }
            if( paraCircular.axisLabelsVisible() ) {
                const bool rotate = params()->polarRotateCircularLabels();
                painter->setPen( QPen( paraCircular.axisLabelsColor(),
                                       circularLineWidth ) );
                const QString& txt = (*labelTexts)[ iLabel ];
                if( params()->polarLabelsAtPos( KDChartEnums::PosTopCenter ) )
                    paintCircularAxisLabel( painter, rotate, 90, center, currentRadiusPPU, txt,
                            Qt::AlignBottom | Qt::AlignHCenter, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosBottomCenter ) )
                    paintCircularAxisLabel( painter, rotate, 270, center, currentRadiusPPU, txt,
                            Qt::AlignTop | Qt::AlignHCenter, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosCenterRight ) )
                    paintCircularAxisLabel( painter, rotate,   0, center, currentRadiusPPU, txt,
                            Qt::AlignVCenter | Qt::AlignRight, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosCenterLeft ) )
                    paintCircularAxisLabel( painter, rotate, 180, center, currentRadiusPPU, txt,
                            Qt::AlignVCenter | Qt::AlignLeft, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosTopRight ) )
                    paintCircularAxisLabel( painter, rotate,  45, center, currentRadiusPPU, txt,
                            Qt::AlignBottom | Qt::AlignRight, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosBottomLeft ) )
                    paintCircularAxisLabel( painter, rotate, 225, center, currentRadiusPPU, txt,
                            Qt::AlignTop | Qt::AlignLeft, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosBottomRight ) )
                    paintCircularAxisLabel( painter, rotate, 315, center, currentRadiusPPU, txt,
                            Qt::AlignTop | Qt::AlignRight, iLabel );

                if( params()->polarLabelsAtPos( KDChartEnums::PosTopLeft ) )
                    paintCircularAxisLabel( painter, rotate, 135, center, currentRadiusPPU, txt,
                            Qt::AlignBottom | Qt::AlignLeft, iLabel );
            }
        }
    }


    double circularSpan = params()->polarChartSubType() == KDChartParams::PolarPercent
        ? 100.0
        : paraCircular.trueAxisHigh() - paraCircular.trueAxisLow();
    double radius = currentRadiusPPU;
    if(    !labels
            || params()->polarChartSubType() == KDChartParams::PolarPercent )
        radius = (position.width() / 2.0) * 1000.0 / 1250.0;

    if( params()->polarChartSubType() != KDChartParams::PolarPercent )
        pixelsPerUnit = labels ? currentRadiusPPU / circularSpan
            : (position.height() / maxValue / 2.0) * 1000.0 / 1250.0;
    else
        pixelsPerUnit = (position.height() / 100.0 / 2.0) * 1000.0 / 1250.0;

    // draw the sagittal grid and axis lines
    if(    paraSagittal.axisShowGrid()
            || paraSagittal.axisVisible()
            || paraSagittal.axisLabelsVisible() ) {

        // calculate label texts
        QStringList* labelTexts = 0;
        bool onlyDefaultLabels = true;
        if( paraSagittal.axisLabelsVisible() ) {
            ((KDChartParams*)params())->setAxisArea( KDChartAxisParams::AxisPosSagittal,
                                                    QRect( 0,
                                                        0,
                                                        static_cast < int > ( 2.0 * M_PI * radius ),
                                                        static_cast < int > ( 0.5 * radius ) ) );
            double delimLen = 20.0 * minSizeP1000; // per mille of area
            KDChartAxisParams::AxisPos basicPos;
            QPoint orig, dest;
            double dDummy;
            double nSubDelimFactor = 0.0;
            double pDelimDelta = 0.0;
            double nTxtHeight = 0.0;
            double pTextsX = 0.0;
            double pTextsY = 0.0;
            double pTextsW = 0.0;
            double pTextsH = 0.0;
            int textAlign = Qt::AlignCenter;
            bool isLogarithmic = false;
            bool isDateTime = false;
            bool autoDtLabels = false;
            QDateTime dtLow;
            QDateTime dtHigh;
            KDChartAxisParams::ValueScale dtDeltaScale;
            KDChartAxesPainter::calculateLabelTexts(
                    painter,
                    *data,
                    *params(),
                    KDChartAxisParams::AxisPosSagittal,
                    minSizeP1000,
                    delimLen,
                    // start of reference parameters
                    basicPos,
                    orig,
                    dest,
                    dDummy,dDummy,dDummy,dDummy,
                    nSubDelimFactor,
                    pDelimDelta,
                    nTxtHeight,
                    pTextsX,
                    pTextsY,
                    pTextsW,
                    pTextsH,
                    textAlign,
                    isLogarithmic,
                    isDateTime,
                    autoDtLabels,
                    dtLow,
                    dtHigh,
                    dtDeltaScale );
                    labelTexts = ( QStringList* ) paraSagittal.axisLabelTexts();
                    // calculate font size
                    actFont = paraSagittal.axisLabelsFont();
                    if ( paraSagittal.axisLabelsFontUseRelSize() ) {
                        actFont.setPointSizeFloat( nTxtHeight );
                    }
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
                        if ( !(*it).startsWith( "Item ") )
                            onlyDefaultLabels = false;
                    }
                    while ( fm.width( strMax ) > pTextsW && 6.0 < nTxtHeight ) {
                        nTxtHeight -= 0.5;
                        actFont.setPointSizeFloat( nTxtHeight );
                        fm = QFontMetrics( actFont );
                    }
                    painter->setFont( actFont );
        }

        int currentAngle = params()->polarZeroDegreePos();
        if(    -360 > currentAngle
                ||  360 < currentAngle )
            currentAngle = 0;
        if( 0 > currentAngle )
            currentAngle += 360;
        int r1 = static_cast < int > ( radius * 1050 / 1000 );
        int r2 = static_cast < int > ( radius * 1100 / 1000 );
        int r3 = static_cast < int > ( radius * 1175 / 1000 );
        QPoint pt1, pt2, pt3;
        uint nLabels = labelTexts ? labelTexts->count() : 0;
        int angleBetweenRays = 360 / nLabels;
        for( uint value = 0; value < nLabels; ++value ) {
            pt1 = center + polarToXY( r1, currentAngle );
            pt2 = center + polarToXY( r2, currentAngle );
            pt3 = center + polarToXY( r3, currentAngle );

            //pt3 = painter->worldMatrix().map( pt3 );

            if( paraSagittal.axisShowGrid() ) {
                painter->setPen( QPen( paraSagittal.axisGridColor(),
                            sagittalGridLineWidth ) );
                painter->drawLine( center, pt1 );
            }
            if( paraSagittal.axisVisible() ) {
                painter->setPen( QPen( paraSagittal.axisLineColor(),
                            sagittalLineWidth ) );
                painter->drawLine( pt1, pt2 );
            }
            if(    paraSagittal.axisLabelsVisible()
                    && labelTexts
                    && labelTexts->count() > value ) {
                painter->setPen( QPen( paraSagittal.axisLabelsColor(),
                            sagittalLineWidth ) );
                QString label(   onlyDefaultLabels
                        ? QString::number( currentAngle )
                        : (*labelTexts)[ value ] );

                KDDrawText::drawRotatedText( painter,
                        currentAngle+90,
                        painter->worldMatrix().map(pt3),
                        label,
                        0,
                        Qt::AlignCenter );
            }
            currentAngle += angleBetweenRays;
        }
    }


    // Now draw the data
    int dataLinesWidth = 0 <= params()->polarLineWidth()
        ? params()->polarLineWidth()
        : -1 * static_cast < int > (   params()->polarLineWidth()
                * minSizeP1000 );
    painter->setBrush( Qt::NoBrush );
    for ( unsigned int dataset = datasetStart; dataset <= datasetEnd; dataset++ ) {
        painter->setPen( QPen( params()->dataColor( dataset ),
                    dataLinesWidth ) );
        Q3PointArray points( numValues );
        int totalPoints = 0;
        double valueTotal = 0.0; // Will only be used for Percent
        int angleBetweenRays = 360 / numValues;
        QVariant vValY;
        for ( int value = 0; value < numValues; value++ ) {
            if( params()->polarChartSubType() == KDChartParams::PolarPercent )
                valueTotal = data->colAbsSum( value );
            // the value determines the angle, the dataset only the color
            if( data->cellCoord( dataset, value, vValY, 1 ) &&
                QVariant::Double == vValY.type() ){
                const double cellValue = vValY.toDouble();
                double drawValue;
                if ( params()->polarChartSubType() == KDChartParams::PolarStacked )
                    drawValue = ( cellValue + currentValueSums[ value ] ) * pixelsPerUnit;
                else if( params()->polarChartSubType() == KDChartParams::PolarPercent ) {
                    drawValue = (   ( cellValue + currentValueSums[ value ] )
                            / valueTotal * static_cast<double>( radius ) );
                } else
                    drawValue = cellValue * pixelsPerUnit;

                // record the point for drawing the polygon later
                int drawAngle = value * angleBetweenRays;
                QPoint drawPoint( center + polarToXY( static_cast<int>( drawValue ),
                                  drawAngle ) );
                points.setPoint( totalPoints, drawPoint );
                totalPoints++;
                KDChartDataRegion* datReg = 0;
                // the marker can be drawn now
                if( params()->polarMarker() ) {
                    int xsize = params()->polarMarkerSize().width();
                    int ysize = params()->polarMarkerSize().height();
                    datReg = drawMarker( painter,
                                        params(),
                                        _areaWidthP1000, _areaHeightP1000,
                                        _dataRect.x(), _dataRect.y(),
                                        params()->polarMarkerStyle( dataset ),
                                        params()->dataColor( dataset ),
                                        drawPoint,
                                        dataset, value, chart,
                                        regions,
                                        xsize ? &xsize : 0,
                                        ysize ? &ysize : 0 );
                    painter->setPen( QPen( params()->dataColor( dataset ),
                                    dataLinesWidth ) );
                }
                if ( regions ) {
                    bool bMustAppendDatReg = 0 == datReg;
                    if( bMustAppendDatReg ){
                        QRect rect( QPoint( drawPoint.x() - 1,
                                            drawPoint.y() - 1 ),
                                    QSize( 3, 3 ) );
                        datReg = new KDChartDataRegion( dataset,
                                                        value,
                                                        chart,
                                                        rect );
                    }
                    datReg->points[ KDChartEnums::PosTopLeft ] =
                        drawPoint + _dataRect.topLeft();

                    datReg->points[     KDChartEnums::PosTopCenter ]    =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosTopRight ]     =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosBottomLeft ]   =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosBottomCenter ] =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosBottomRight ]  =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosCenterLeft ]   =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosCenter ]       =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    datReg->points[     KDChartEnums::PosCenterRight ]  =
                        datReg->points[ KDChartEnums::PosTopLeft ];
                    /*
                    // test the center positions:
                    painter->drawEllipse( datReg->points[ KDChartEnums::PosCenterLeft ].x() - 2,
                    datReg->points[ KDChartEnums::PosCenterLeft ].y() - 2,  5, 5);
                    */
                    datReg->startAngle = drawAngle;
                    datReg->angleLen   = drawAngle;
                    if( bMustAppendDatReg )
                        regions->append( datReg );
                }
                // calculate running sum for stacked and percent
                if ( params()->polarChartSubType() == KDChartParams::PolarStacked ||
                        params()->polarChartSubType() == KDChartParams::PolarPercent )
                    currentValueSums[ value ] += cellValue;
            }
        }
        painter->drawPolygon( points );
    }

    painter->translate( -_dataRect.x(), -_dataRect.y() );
}


/*
  Helper methode being called by KDChartPolarPainter::paintData()
*/
void KDChartPolarPainter::paintCircularAxisLabel( QPainter* painter,
        bool rotate,
        int txtAngle,
        QPoint center,
        double currentRadiusPPU,
        const QString& txt,
        int align,
        int step )
{
    if( !rotate && (0 != (align & (Qt::AlignLeft | Qt::AlignRight) ) ) )
        currentRadiusPPU += center.x()*0.01;
    KDDrawText::drawRotatedText(
            painter,
            rotate ? txtAngle - 90 : 0,
            painter->worldMatrix().map(center - polarToXY( static_cast<int>( currentRadiusPPU ), txtAngle )),
            txt,
            0,
            step
            ? (rotate ? (int)(Qt::AlignBottom | Qt::AlignHCenter) : align)
            : Qt::AlignCenter,
            false,0,false,
            false );
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
*//*
void KDChartPolarPainter::drawMarker( QPainter* painter,
        KDChartParams::PolarMarkerStyle style,
        const QColor& color,
        const QPoint& p,
        uint, //dataset,
        uint, //value,
        uint, //chart,
        double minSizeP1000,
        QRegion& region )
{
    int xsize = params()->polarMarkerSize().width();
    if ( 0 > xsize )
        xsize = -1 * static_cast < int > ( xsize * minSizeP1000 );
    int ysize = params()->polarMarkerSize().height();
    if ( 0 > ysize )
        ysize = -1 * static_cast < int > ( ysize * minSizeP1000 );
    int xsize2 = xsize / 2;
    int ysize2 = ysize / 2;
    painter->setPen( color );
    switch ( style ) {
        case KDChartParams::PolarMarkerSquare: {
                                                  painter->save();
                                                  painter->setBrush( color );
                                                  QRect rect( QPoint( p.x() - xsize2, p.y() - ysize2 ), QPoint( p.x() + xsize2, p.y() + ysize2 ) );
                                                  painter->drawRect( rect );
                                                  // Don't use rect for drawing after this!
                                                  rect.moveBy( _dataRect.x(), _dataRect.y() );
                                                  region = QRegion( rect );
                                                  painter->restore();
                                                  break;
                                              }
        case KDChartParams::PolarMarkerDiamond: {
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
                                                    region = QRegion( points );
                                                    painter->restore();
                                                    break;
                                                }
        case KDChartParams::PolarMarkerCircle:
        default: {
                    painter->save();
                    painter->setBrush( color );
                    painter->drawEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                    QPointArray points;
                    points.makeEllipse( p.x() - xsize2, p.y() - ysize2, xsize, ysize );
                    // Don't use points for drawing after this!
                    points.translate( _dataRect.x(), _dataRect.y() );
                    if( points.size() > 0 )
                        region = QRegion( points );
                    else
                        region = QRegion();
                    painter->restore();
                }
    };
}*/

#define DEGTORAD(d) (d)*M_PI/180

QPoint KDChartPolarPainter::polarToXY( int radius, int angle )
{
    double anglerad = DEGTORAD( static_cast<double>( angle ) );
    QPoint ret( static_cast<int>( cos( anglerad ) * radius ),
            static_cast<int>( sin( anglerad ) * radius ) );
    return ret;
}


/**
  This method is a specialization that returns a fallback legend text
  appropriate for polar charts where the fallbacks should come from
  the values, not from the datasets.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \param uint dataset the dataset number for which to generate a
  fallback text
  \return the fallback text to use for describing the specified
  dataset in the legend
*/
QString KDChartPolarPainter::fallbackLegendText( uint dataset ) const
{
    return QObject::tr( "Series " ) + QString::number( dataset + 1 );
}


/**
  This methods returns the number of elements to be shown in the
  legend in case fallback texts are used.

  This method is only used when automatic legends are used, because
  manual and first-column legends do not need fallback texts.

  \return the number of fallback texts to use
*/
uint KDChartPolarPainter::numLegendFallbackTexts( KDChartTableDataBase* data ) const
{
    return data->usedRows();
}
