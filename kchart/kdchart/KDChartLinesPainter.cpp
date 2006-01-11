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
#include "KDChartLinesPainter.h"
#include <KDChartParams.h>
#include <KDChartPropertySet.h>

#include <qpainter.h>

#if COMPAT_QT_VERSION >= 0x030000
#include <qvaluevector.h>
#else
#include <qarray.h>
#endif

#include <stdlib.h>

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
  regions is not 0.

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
            true,   // center points
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
    mCenterThePoints = centerThePoints;
    mDrawMarkers = drawMarkers;
    mIsArea = isArea;
    mChartType = paint2nd ? params()->additionalChartType()
        : params()->chartType();

    KDChartAxesPainter::paintData( painter, data, paint2nd, regions );
}


#define DEGTORAD(d) (d)*M_PI/180

/*!
  Projects a point in a space defined by its x, y, and z coordinates
  into a point onto a plane, given two rotation angles around the x
  resp. y axis.
  */
QPoint KDChartLinesPainter::project( int x, int y, int z )
{
    double xrad = DEGTORAD( params()->threeDLineXRotation() );
    double yrad = DEGTORAD( params()->threeDLineYRotation() );
    QPoint ret( static_cast<int>( x*cos( yrad ) + z * sin( yrad ) ),
            static_cast<int>( y*cos( xrad ) - z * sin( xrad ) ) );
    return ret;
}

bool KDChartLinesPainter::isNormalMode() const
{
    return KDChartParams::LineNormal == params()->lineChartSubType();
}

int KDChartLinesPainter::clipShiftUp( bool, double ) const
{
    return 0;
}


class MyPoint
{
    public:
        MyPoint() : bValid( false ), bSkipThis( false ), cellValue( 0.0 ) {}
        void set( int x, int y, double value ) {
            bValid = true;
            p.setX( x );
            p.setY( y );
            cellValue = value;
        }
        void setSkipThis( bool skipThis ) {
            bSkipThis = skipThis;
        }
        QPoint p;
        bool   bValid;
        bool   bSkipThis;
        double cellValue;
};


void KDChartLinesPainter::specificPaintData( QPainter* painter,
        const QRect& /*ourClipRect*/,
        KDChartTableDataBase* data,
        KDChartDataRegionList* regions,
        const KDChartAxisParams* ordinatePara,
        bool /*bNormalMode*/,
        uint chart,
        double logWidth,
        double /*areaWidthP1000*/,
        double logHeight,
        double axisYOffset,
        double minColumnValue,
        double maxColumnValue,
        double columnValueDistance,
        uint /*chartDatasetStart*/,
        uint /*chartDatasetEnd*/,
        uint datasetStart,
        uint datasetEnd )
{
    if( !data ) return;

    abscissaInfos ai;
    ai.bCenterThePoints = mCenterThePoints;
    calculateAbscissaInfos( *params(), *data,
                            datasetStart, datasetEnd,
                            logWidth, _dataRect,
                            ai );
    mCenterThePoints = ai.bCenterThePoints;

    bool bOrdinateDecreasing = ordinatePara
                                ? ordinatePara->axisValuesDecreasing()
                                : false;
    bool bOrdinateIsLogarithmic
        = ordinatePara
        ? (KDChartAxisParams::AxisCalcLogarithmic == ordinatePara->axisCalcMode())
        : false;

    //const double ordinatePixelsPerUnit = logHeight / columnValueDistance;
    const double ordinatePixelsPerUnit
        = (    ordinatePara
                && (0.0 != ordinatePara->trueAxisDeltaPixels())
                && (0.0 != ordinatePara->trueAxisDelta()))
        ? ordinatePara->trueAxisDeltaPixels() / ordinatePara->trueAxisDelta()
        : logHeight / columnValueDistance;;
    //qDebug("ordinatePixelsPerUnit: %f",ordinatePixelsPerUnit);


    const bool showThreeDLines = !mIsArea && params()->threeDLines();

    enum { Normal, Stacked, Percent } mode = Normal;
    if (    (    ( mChartType                   == KDChartParams::Line )
                && ( params()->lineChartSubType() == KDChartParams::LineNormal ) )
            || (    ( mChartType                   == KDChartParams::Area )
                && ( params()->areaChartSubType() == KDChartParams::AreaNormal ) ) )
        mode = Normal;
    else if (    (    ( mChartType                   == KDChartParams::Line )
                && ( params()->lineChartSubType() == KDChartParams::LineStacked ) )
            || (    ( mChartType                   == KDChartParams::Area )
                && ( params()->areaChartSubType() == KDChartParams::AreaStacked ) ) )
        mode = Stacked;
    else if (    (    ( mChartType                   == KDChartParams::Line )
                && ( params()->lineChartSubType() == KDChartParams::LinePercent ) )
            || (    ( mChartType                   == KDChartParams::Area )
                && ( params()->areaChartSubType() == KDChartParams::AreaPercent ) ) )
        mode = Percent;
    else
        qDebug( "Internal error in KDChartLinesPainter::paintDataInternal(): Unknown subtype" );


    QMap < int, double > currentValueSums;
    if ( mode == Stacked || mode == Percent ) {
        // this array is only used for stacked and percent lines, no need
        // to waste time initializing it for normal types
        for ( int value = 0; value < ai.numValues; ++value )
            currentValueSums[ value ] = 0.0;
    }
    QMap < int, double > totalValueSums;

    // compute the position of the 0 axis
    double zeroXAxisI;
    if ( mode == Percent ) {
        if ( minColumnValue == 0.0 )
            zeroXAxisI = logHeight + axisYOffset;
        else if( maxColumnValue == 0.0 )
            zeroXAxisI = _dataRect.y() + axisYOffset;
        else
            zeroXAxisI = logHeight / 2.0 + _dataRect.y();
    } else
        zeroXAxisI = ordinatePara->axisZeroLineStartY() - _dataRect.y();


    // compute how to shift of the points in case we want them in the
    // middle of their respective columns
    int xShift = mCenterThePoints ? static_cast < int > ( ai.pointDist * 0.5 ) : 0;


    // calculate all points' positions
    // ===============================
    int arrayNumDatasets = 0;
    int arrayNumValues   = ai.bAbscissaHasTrueAxisDtValues
        ? data->cols()
        : ai.numValues;
    int dataset;
    for( dataset = datasetEnd;
         ( dataset >= static_cast < int > ( datasetStart ) && dataset >= 0 );
         --dataset )
        ++arrayNumDatasets;
#if COMPAT_QT_VERSION >= 0x030000
        QValueVector<MyPoint> allPoints(
#else
        QArray<MyPoint> allPoints(
#endif
            arrayNumDatasets * arrayNumValues );

    KDChartPropertySet curPropSet;
    int curPropSetId = KDChartPropertySet::UndefinedID;

    for( dataset = datasetEnd; ( dataset >= (int)datasetStart && dataset >= 0 ); --dataset ) {

        int prevPointX = -1;
        int prevPointY = -1;

        const KDChartParams::LineMarkerStyle
            defaultMarkerStyle = params()->lineMarkerStyle( dataset );
        const QPen default2DPen(   params()->lineColor().isValid()
                                 ? params()->lineColor()
                                 : params()->dataColor( dataset ),
                                 params()->lineWidth(),
                                 params()->lineStyle( dataset ) );

        if( ai.bAbscissaHasTrueAxisDtValues )
            ai.numValues = data->cols();

        QVariant vValY;
        QVariant vValX;
        int cellPropID;
        for( int value = 0; value < ai.numValues; ++value ) {
        //if ( mode == Percent )
        //    valueTotal = data->colAbsSum( value );
            double valueTotal = 0.0; // Will only be used for Percent
            if( mode == Percent ) {
                valueTotal = 0.0;
                // iterate over datasets of this axis only:
                for ( uint dataset2  = datasetStart;
                        dataset2 <= datasetEnd;
                        ++dataset2 ) {
                    if( data->cellCoord( dataset2, value, vValY, 1 ) &&
                        QVariant::Double == vValY.type() )
                        valueTotal += vValY.toDouble();
                }
            }

            if( data->cellContent( dataset, value, vValY, vValX, cellPropID ) &&
                QVariant::Double == vValY.type() &&
                ( !ai.bCellsHaveSeveralCoordinates || QVariant::Invalid != vValX.type() ) ){
//qDebug("a. cellPropID: %i",cellPropID);

                // calculate Ordinate axis value
                // -----------------------------
                double cellValue = vValY.toDouble();
                double drawValue = 0.0;
                // PENDING(kalle) This does not work for AreaPercent yet
                if ( mode == Stacked )
                    drawValue = ( cellValue + currentValueSums[ value ] ) * ordinatePixelsPerUnit;
                else if ( mode == Percent )
                    drawValue = ( ( cellValue + currentValueSums[ value ] ) / valueTotal ) * 100.0 * ordinatePixelsPerUnit;
                else {
                    // LineNormal or AreaNormal
                    if( bOrdinateIsLogarithmic ){
                        if( 0.0 < cellValue )
                            drawValue = log10( cellValue ) * ordinatePixelsPerUnit;
                        else
                            drawValue = -10250.0;
                        //qDebug("\nlogarithmic calc  -  cellValue: %f   drawValue: %f",
                        //        cellValue, drawValue );
                    }else{
                        drawValue = cellValue * ordinatePixelsPerUnit * (bOrdinateDecreasing ? -1.0 : 1.0);
                        //qDebug("\nlinear calc  -  cellValue: %f\n             -  drawValue: %f",
                        //        cellValue, drawValue );
                    }
                }


                // calculate Abscissa axis value
                // -----------------------------
                double xValue;
                bool skipMe = !calculateAbscissaAxisValue( vValX, ai, value,
                                                           xValue );


                // calculate and store the point and region / draw the marker
                // ----------------------------------------------------------
                if( !skipMe ){
                    // prevent the point from being toooo far
                    // below the bottom (or above the top, resp.)
                    // of the cliprect
                    double pY = QMIN( zeroXAxisI - drawValue,
                            (logHeight + axisYOffset) * 3 );
                    pY = QMAX( pY, -(logHeight + axisYOffset) * 3 );
                    // specify the Point
                    int myPointX = static_cast < int > ( xValue ) + xShift;
                    int myPointY = static_cast < int > ( pY );

                    if( cellPropID == curPropSetId &&
                        myPointX == prevPointX &&
                        myPointY == prevPointY ){
                        allPoints[   static_cast < int > ( datasetEnd-dataset )
                                   * arrayNumValues + value ].setSkipThis( true );
                        skipMe = true;
//qDebug("skipped");
                    }else{
                        // use typecast to make it compile on windows using qt232
                        allPoints[   static_cast < int > ( datasetEnd-dataset )
                                   * arrayNumValues + value ].set( myPointX, myPointY, cellValue );
//qDebug("ok");
                    }
                    if( !skipMe ){
                        // --------------------------------------------------------
                        // determine any 'extra' properties assigned to this cell
                        // by traversing the property set chain (if necessary)
                        // --------------------------------------------------------
                        if( cellPropID != curPropSetId ){
//qDebug("b. ( curPropSetId: %i )",curPropSetId);
//qDebug("b. cellPropID: %i",cellPropID);
//qDebug(curPropSet.name().latin1());
                            if( cellPropID != KDChartPropertySet::UndefinedID &&
                                params()->calculateProperties( cellPropID,
                                                               curPropSet ) ){
                                curPropSetId = cellPropID;
//qDebug("c. curPropSetId: %i",curPropSetId);
//qDebug(curPropSet.name().latin1());
                            }else{
                                curPropSetId = KDChartPropertySet::UndefinedID;
                            }
                        }
                        // make sure any extra horiz. and/or vert. lines and/or markers
                        // are drawn *before* the data lines and/or markers are painted
                        if( mChartType == KDChartParams::Line ){
                            if( curPropSetId != KDChartPropertySet::UndefinedID ){
                                drawExtraLinesAndMarkers(
                                    curPropSet,
                                    default2DPen,
                                    defaultMarkerStyle,
                                    myPointX, myPointY,
                                    painter,
                                    ai.abscissaPara,
                                    ordinatePara,
                                    logWidth/1000.0,
                                    logHeight/1000.0,
                                    false );
                            }
                        }
                        prevPointX = myPointX;
                        prevPointY = myPointY;
                    }
                }
                // calculate running sum for stacked and percent
                if ( mode == Stacked || mode == Percent ) {
                    if( cellValue == KDCHART_POS_INFINITE )
                        currentValueSums[ value ] = KDCHART_POS_INFINITE;
                    else if( currentValueSums[ value ] != KDCHART_POS_INFINITE )
                        currentValueSums[ value ] += cellValue;
                }
            }
        }
    }



    QPointArray previousPoints; // no vector since only areas need it,
    // and these do not support 3d yet

    // Store some (dataset-independend) default values
    // to be used unless other properties
    // have been specified for the respective data cell:
    //
    const bool defaultDrawMarkers = mDrawMarkers;

    for ( dataset = datasetEnd; ( dataset >= (int)datasetStart && dataset >= 0 ); --dataset ) {

        // Store some (dataset-dependend) default values
        // to be used unless other properties
        // have been specified for the respective data cell:
        //
        const QPen default2DPen(   params()->lineColor().isValid()
                ? params()->lineColor()
                : params()->dataColor( dataset ),
                params()->lineWidth(),
                params()->lineStyle( dataset ) );
        bool currentDrawMarkers = defaultDrawMarkers;
        const KDChartParams::LineMarkerStyle markerStyle = params()->lineMarkerStyle( dataset );

        // the +2 is for the areas (if any)
        QPtrVector< QPointArray > points( 2 );
        points.setAutoDelete( true );
        int i = 0;
        for( i = 0; i < 2; ++i )
            points.insert( i, new QPointArray( ai.numValues + 2 ) );

        if( ai.bAbscissaHasTrueAxisDtValues )
            ai.numValues = data->cols();

        int point = 0;

        for ( int value = 0; value < ai.numValues; ++value ) {

            // determine and store marker properties assigned to this cell
            // -----------------------------------------------------------
            currentDrawMarkers = defaultDrawMarkers;
            int cellPropID;
            if( data->cellProp( dataset, value, cellPropID ) &&
                cellPropID != curPropSetId ){
                if( cellPropID != KDChartPropertySet::UndefinedID &&
                    params()->calculateProperties( cellPropID,
                                                   curPropSet ) )
                    curPropSetId = cellPropID;
                else
                    curPropSetId = KDChartPropertySet::UndefinedID;
            }
            if( curPropSetId != KDChartPropertySet::UndefinedID ){
                // we can safely call the following functions and ignore their
                // return values since they will touch the parameters' values
                // if the propSet *contains* corresponding own values only.
                int iDummy;
                curPropSet.hasOwnShowMarker( iDummy, currentDrawMarkers );
            }


            int iVec = static_cast < int > ( datasetEnd-dataset ) * arrayNumValues + value;
            if( allPoints[ iVec ].bValid && !allPoints[ iVec ].bSkipThis ){
                const MyPoint& mp = allPoints[iVec];
                //qDebug("\np.x() %i        p.y() %i", p.x(), p.y() );

                // For 2D lines, we need two points (that lie
                // behind each other on the Z axis). For 2D lines and
                // areas, we need only one point.
                if( showThreeDLines ) {
                  //Pending Michel fix
		  //points[0]->setPoint( point, project( mp.p.x(), mp.p.y(),
		  //            (datasetStart+dataset)*params()->threeDLineDepth() ) );
		  points[0]->setPoint( point,  mp.p.x(), mp.p.y() );
                  points[1]->setPoint( point, project( mp.p.x(), mp.p.y(),
							 (datasetStart+dataset+1)*params()->threeDLineDepth() ) );
                    
                } else
                    // 2D lines or areas
                    points[0]->setPoint( point, mp.p );
                ++point;

                int x = mp.p.x();
                int y = QMAX(QMIN(mp.p.y(),
                            static_cast < int > (logHeight +axisYOffset)),
                        0);
                bool markerIsOutside = y != mp.p.y();
                // draw the marker and store the region
                if ( currentDrawMarkers ){
                    uint   theAlignment = Qt::AlignCenter;
                    bool   hasOwnSize = false;
                    int    theWidth  = 0;
                    int    theHeight = 0;
                    QColor theColor(params()->dataColor( dataset ));
                    int    theStyle = markerStyle;
                    if( curPropSetId != KDChartPropertySet::UndefinedID ){
                        // we can safely call the following functions and ignore their
                        // return values since they will touch the parameters' values
                        // if the propSet *contains* corresponding own values only.
                        int iDummy;
                        curPropSet.hasOwnMarkerAlign( iDummy, theAlignment );
                        curPropSet.hasOwnMarkerColor( iDummy, theColor );
                        curPropSet.hasOwnMarkerStyle( iDummy, theStyle );
                        QSize size(theWidth, theHeight);
                        hasOwnSize = curPropSet.hasOwnMarkerSize(iDummy, size);
                        if( hasOwnSize ){
                            theWidth  = size.width();
                            theHeight = size.height();
                        }
                    }

                    drawMarker( painter,
                                params(),
                                _areaWidthP1000, _areaHeightP1000,
                                _dataRect.x(),
                                _dataRect.y(),
                                  markerIsOutside
                                ? KDChartParams::LineMarker1Pixel
                                : theStyle,
                                theColor,
                                QPoint(x,y),
                                dataset, value, chart, regions,
                                hasOwnSize ? &theWidth  : 0,
                                hasOwnSize ? &theHeight : 0,
                                theAlignment );

                }
                // store the region
                else if( regions ) {
                    QRect rect( QPoint( x-1, y-1 ), QPoint( x+1, y+1 ) );
                    rect.moveBy( _dataRect.x(), _dataRect.y() );
                    regions->append(
                        new KDChartDataRegion(dataset, value, chart, rect) );
                }

            }
        }
        if ( point ) {
            bool bDrawLines = (0 != params()->lineWidth());
            if ( mIsArea ) {
                // first draw with the fill brush, no pen, with the
                // zero axis points or upper border points added for the first
                // dataset or with the previous points reversed for all other
                // datasets.
                painter->setPen( QPen( Qt::NoPen ) );
                const QBrush datasetBrush( params()->dataColor( dataset ), Qt::SolidPattern );
                painter->setBrush( datasetBrush );
                QBrush currentBrush( datasetBrush );

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
                        yCoord = static_cast<int>(zeroXAxisI);
                    else
                        yCoord = static_cast<int>(axisYOffset);

                    // old: draw the complete area in on go:
                    /*
                    // no 3d handling for areas yet
                    points[0]->setPoint( point, lastPoint.x(), yCoord );
                    point++;

                    QPoint firstPoint = points[0]->point( 0 );
                    points[0]->setPoint( point, firstPoint.x(), yCoord );
                    point++;

                    painter->drawPolygon( *points[0], false, 0, point );
                    */

                    // new: draw individual area segments:
                    curPropSetId = KDChartPropertySet::UndefinedID;
                    for( int value = 0; value < point-1; ++value ) {

                        int cellPropID;
                        if( data->cellProp( dataset, value, cellPropID ) &&
                            cellPropID != curPropSetId ){

                            if( cellPropID != KDChartPropertySet::UndefinedID &&
                                params()->calculateProperties( cellPropID,
                                                              curPropSet ) ){
                                curPropSetId = cellPropID;
                            }else{
                                curPropSetId = KDChartPropertySet::UndefinedID;
                            }
                            // preset with default value
                            QBrush theAreaBrush = datasetBrush;

                            if( curPropSetId != KDChartPropertySet::UndefinedID ){
                                // we can safely call the following functions and ignore their
                                // return values since they will touch the parameters' values
                                // if the propSet *contains* corresponding own values only.
                                int iDummy;
                                curPropSet.hasOwnAreaBrush( iDummy, theAreaBrush );
                            }
                            painter->setBrush( theAreaBrush );

                        }
                        QPointArray segment( 4 );
                        segment.setPoint( 0, points[0]->point( value                 ) );
                        segment.setPoint( 1, points[0]->point( value+1               ) );
                        segment.setPoint( 2, points[0]->point( value+1 ).x(), yCoord );
                        segment.setPoint( 3, points[0]->point( value   ).x(), yCoord );
                        //Pending Michel: drawing a segment
                        painter->drawPolygon( segment );
                    }

                    // old: draw the complete area in on go:
                    /*
                    // remove the last two points added
                    point -= 2;
                    */
                    //qDebug("\n111");
                } //  if ( mode == Normal || dataset == (int)datasetEnd )
		else {
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
                    //Pending Michel: Drawing a section
                    painter->drawPolygon( thisSection );
                }
                // draw the line with no brush and outline color
                painter->setBrush( Qt::NoBrush );
                painter->setPen( QPen( params()->outlineDataColor(),
                                 params()->outlineDataLineWidth() ) );
            } else {
                // line
                if( showThreeDLines ) {
                    // This is a 3D line:
                    // We draw the line with the data color brush
                    //                   and the outline data pen.
                    painter->setBrush( params()->dataColor( dataset ) );
                    painter->setPen( QPen( params()->outlineDataColor(),
                                params()->outlineDataLineWidth() ) );
                } else {
                    // This is a 2D line:
                    // We draw the line with the no brush
                    // and the data color if no special line color was specified.
                    painter->setBrush( Qt::NoBrush );
                    painter->setPen( default2DPen );
                }
            }

            // Neither draw the contour line if this is a pure Point chart
            // nor draw it for the last row of a percent area chart.
            if( bDrawLines &&
                    ( (mode != Percent) || !mIsArea || (dataset != (int)datasetEnd) ) ){
                if( showThreeDLines ) {
                    // A 3D line needs to be drawn piece-wise
                    for ( int value = 0; value < point-1; ++value ) {
                        //          if( data->cell( dataset, value ).hasValue() &&
                        //              data->cell( dataset, value+1 ).hasValue() ) {
                        //              qDebug( "Draw a segment in dataset %d from %d to %d", dataset, value, value+1 );
                        QPointArray segment( 4 );
                        segment.setPoint( 0, points[0]->point( value ) );
                        segment.setPoint( 1, points[0]->point( value+1 ) );
                        segment.setPoint( 2, points[1]->point( value+1 ) );
                        segment.setPoint( 3, points[1]->point( value ) );
                        //PENDING Michel drawing a segment with showThreeDLines                       
                        painter->drawPolygon( segment );
                        //          } else
                        //              qDebug( "Can't draw a segment in dataset %d from %d to %d", dataset, value, value+1 );
                    }
                } else {
                    QPoint p1, p2;
                    // Note: If markers are drawn very near to each other
                    //       and tiny markers are used
                    //       we don't draw the connecting lines.
                    bool b4PMarkers = KDChartParams::LineMarker4Pixels == markerStyle;
                    bool bTinyMarkers =
                        KDChartParams::LineMarker1Pixel  == markerStyle || b4PMarkers;
                    curPropSetId = KDChartPropertySet::UndefinedID;
                    painter->setPen( default2DPen );
                    for ( int value = 0; value < point-1; ++value ) {
                        p1 = points[0]->point( value   );
                        p2 = points[0]->point( value+1 );

                        // Determine properties assigned to this cell
                        // and change the painter if necessarry:
                        currentDrawMarkers = defaultDrawMarkers;
                        int cellPropID;
                        if( data->cellProp( dataset, value, cellPropID ) &&
                            cellPropID != curPropSetId ){
                            if( cellPropID != KDChartPropertySet::UndefinedID &&
                                params()->calculateProperties( cellPropID,
                                                               curPropSet ) ){
                                curPropSetId = cellPropID;
                            }else{
                                curPropSetId = KDChartPropertySet::UndefinedID;
                            }
                            // preset with default values
                            int          theLineWidth = default2DPen.width();
                            QColor       theLineColor = default2DPen.color();
                            Qt::PenStyle theLineStyle = default2DPen.style();
                            if( curPropSetId != KDChartPropertySet::UndefinedID ){
                                // we can safely call the following functions and ignore their
                                // return values since they will touch the parameters' values
                                // if the propSet *contains* corresponding own values only.
                                int iDummy;
                                curPropSet.hasOwnLineWidth ( iDummy, theLineWidth );
                                curPropSet.hasOwnLineColor ( iDummy, theLineColor );
                                curPropSet.hasOwnLineStyle ( iDummy, theLineStyle );
                                curPropSet.hasOwnShowMarker( iDummy, currentDrawMarkers );
                            }
                            painter->setPen( QPen( theLineColor,
                                                              theLineWidth,
                                                              theLineStyle ) );
                        }

                        if( !currentDrawMarkers ){
			  //PENDING Michel: drawing a line - not currentMarkers
                            painter->drawLine( p1, p2 );
                        }else{
                            int dx = p2.x() - p1.x();
                            int dy = p2.y() - p1.y();
                            if( !bTinyMarkers || (abs(dx) > 4) || (abs(dy) > 4) ){
                                if( bTinyMarkers ) {
                                    double m  = !dx ? 100.0
                                        : !dy ? 0.01
                                        : ((double)dy / (double)dx);
                                    double am = fabs(m);
                                    int dxx;
                                    int dyy;
                                    if( 0.25 > am ){
                                        dxx = 3;
                                        dyy = 0;
                                    }else if( 0.67 > am ){
                                        dxx = 3;
                                        dyy = 1;
                                    }else if( 1.33 > am ){
                                        dxx = 2;
                                        dyy = 2;
                                    }else if( 4.0 > am ){
                                        dxx = 1;
                                        dyy = 3;
                                    }else{
                                        dxx = 0;
                                        dyy = 3;
                                    }
                                    if( 0 > dx )
                                        dxx *= -1;
                                    if( 0 > dy )
                                        dyy *= -1;
                                    if( b4PMarkers ){
                                        if( 0 < dx )
                                            ++p1.rx();
                                        else if( 0 > dx )
                                            ++p2.rx();
                                        if( 0 < dy )
                                            ++p1.ry();
                                        else if( 0 > dy )
                                            ++p2.ry();
                                    }
                                    p1.rx() += dxx; p1.ry() += dyy;
                                    p2.rx() -= dxx; p2.ry() -= dyy;
                                }
                                 //PENDING Michel: drawing a line - currentMarkers
                                painter->drawLine( p1, p2 );
                            }
                        }
                    }
                }
            }
        }

        // Save point array for next way through (needed for e.g. stacked
        // areas), not for 3D currently
        points[0]->resize( point );
        previousPoints = points[0]->copy();
    }


    // Now draw any extra lines (and/or their markers, resp.) that
    // are to be printed IN FRONT of the normal lines:
    if( mChartType == KDChartParams::Line ){
        for( dataset = datasetEnd; ( dataset >= (int)datasetStart && dataset >= 0 ); --dataset ) {

            const KDChartParams::LineMarkerStyle
                defaultMarkerStyle = params()->lineMarkerStyle( dataset );
            const QPen default2DPen(   params()->lineColor().isValid()
                                    ? params()->lineColor()
                                    : params()->dataColor( dataset ),
                                    params()->lineWidth(),
                                    params()->lineStyle( dataset ) );

            if( ai.bAbscissaHasTrueAxisDtValues )
                ai.numValues = data->cols();

            for ( int value = 0; value < ai.numValues; ++value ) {
                int iVec = static_cast < int > ( datasetEnd-dataset ) * arrayNumValues + value;
                if( allPoints[ iVec ].bValid ){
                    const MyPoint& mp = allPoints[iVec];
                    //qDebug("\np.x() %i        p.y() %i", p.x(), p.y() );

                    // --------------------------------------------------------
                    // determine any 'extra' properties assigned to this cell
                    // by traversing the property set chain (if necessary)
                    // --------------------------------------------------------
                    int cellPropID;
                    if( data->cellProp( dataset, value, cellPropID ) &&
                        cellPropID != curPropSetId ){
                        if( cellPropID != KDChartPropertySet::UndefinedID &&
                            params()->calculateProperties( cellPropID,
                                                           curPropSet ) )
                            curPropSetId = cellPropID;
                        else
                            curPropSetId = KDChartPropertySet::UndefinedID;
                    }
                    if( curPropSetId != KDChartPropertySet::UndefinedID ){
                        drawExtraLinesAndMarkers(
                            curPropSet,
                            default2DPen,
                            defaultMarkerStyle,
                            mp.p.x(), mp.p.y(),
                            painter,
                            ai.abscissaPara,
                            ordinatePara,
                            logWidth/1000.0,
                            logHeight/1000.0,
                            true );
                    }
                }
            }
        }
    }
//qDebug(const_cast < KDChartParams* > ( params() )->properties( KDCHART_PROPSET_NORMAL_DATA )->name().latin1());
//qDebug(const_cast < KDChartParams* > ( params() )->properties( KDCHART_PROPSET_TRANSPARENT_DATA )->name().latin1());
//qDebug(const_cast < KDChartParams* > ( params() )->properties( KDCHART_PROPSET_HORI_LINE )->name().latin1());
//qDebug(const_cast < KDChartParams* > ( params() )->properties( KDCHART_PROPSET_VERT_LINE )->name().latin1());
//qDebug("--");
}
