/* -*- Mode: C++ -*-
   KDChart - a multi-platform charting engine
   */

/****************************************************************************
 ** Copyright (C) 2001-2003 Klar�vdalens Datakonsult AB.  All rights reserved.
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
#include <qpainter.h>
#include <qlabel.h>

#include <KDDrawText.h>
#include "KDChartAxesPainter.h"
#include "KDChartAxisParams.h"
#include "KDChartParams.h"

#include <stdlib.h>


/**
  Little helper function returning the number of seconds
  between UTC start date 1970/01/01 00:00 and a given date \c dt.
  The return value is negative for \c dt < 1970/01/01.
  */
int secondsSinceUTCStart( const QDateTime& dt )
{
    QDateTime dtStart( QDate( 1970, 1, 1 ) );
    return dtStart.secsTo( dt );
}


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


#if COMPAT_QT_VERSION < 0x030000
QDateTime dateTimeFromString( const QString& s ) // only ISODate is allowed
{
    int year(  s.mid(  0, 4 ).toInt() );
    int month( s.mid(  5, 2 ).toInt() );
    int day(   s.mid(  8, 2 ).toInt() );
    QString t( s.mid( 11 ) );
    int hour(   t.mid( 0, 2 ).toInt() );
    int minute( t.mid( 3, 2 ).toInt() );
    int second( t.mid( 6, 2 ).toInt() );
    int msec(   t.mid( 9, 3 ).toInt() );
    if ( year && month && day )
        return QDateTime( QDate( year, month, day ),
                QTime( hour, minute, second, msec ) );
    else
        return QDateTime();
}
QString dateTimeToString( const QDateTime& dt )  // ISODate is returned
{
    QString date;
    QString month(
            QString::number( dt.date().month() ).rightJustify( 2, '0' ) );
    QString day(
            QString::number( dt.date().day() ).rightJustify( 2, '0' ) );
    date = QString::number( dt.date().year() ) + "-" + month + "-" + day;
    QString time;
    time.sprintf( "%.2d:%.2d:%.2d",
            dt.time().hour(), dt.time().minute(), dt.time().second() );
    return date + "T" + time;
}
#endif


/**
  ReCalculate the labels based upon given nDelta and nDeltaPix.

  This is necessary to build isometric axes.
  */
void reCalculateLabelTexts(
        QPainter* painter,
        const KDChartTableDataBase& data,
        const KDChartParams& params,
        uint axisNumber,
        double averageValueP1000,
        double delimLen,
        internal__KDChart__CalcValues& cv )
{
    KDChartAxesPainter::calculateLabelTexts(
            painter,
            data,
            params,
            axisNumber,
            averageValueP1000,
            delimLen,
            // start of reference parameters
            cv.basicPos,
            cv.orig,
            cv.dest,
            cv.pXDeltaFactor,
            cv.pYDeltaFactor,
            cv.pXDelimDeltaFaktor,
            cv.pYDelimDeltaFaktor,
            cv.nSubDelimFactor,
            cv.pDelimDelta,
            cv.nTxtHeight,
            cv.pTextsX,
            cv.pTextsY,
            cv.pTextsW,
            cv.pTextsH,
            cv.textAlign,
            cv.bLogarithmic,
            cv.isDateTime,
            cv.autoDtLabels,
            cv.dtLow,
            cv.dtHigh,
            cv.dtDeltaScale,
            true,
            cv.nDelta,
            cv.nDeltaPix );
            const KDChartAxisParams & para = params.axisParams( axisNumber );
            cv.bSteadyCalc = para.axisSteadyValueCalc();
            cv.bDecreasing = para.axisValuesDecreasing();
            cv.nLow        = para.trueAxisLow();
            cv.nHigh       = para.trueAxisHigh();
}


bool KDChartAxesPainter::calculateAllAxesLabelTextsAndCalcValues(
        QPainter* painter,
        KDChartTableDataBase* data,
        double areaWidthP1000,
        double areaHeightP1000,
        double& delimLen)
{
    uint iAxis;

    double averageValueP1000 = QMIN(areaWidthP1000, areaHeightP1000);//( areaWidthP1000 + areaHeightP1000 ) / 2.0;
    // length of little delimiter-marks indicating axis scaling
    delimLen = 20.0 * averageValueP1000; // per mille of area

    // Determine axes calculation values and labels before drawing the axes.

    // step #1: calculate all values independendly from the other axes' values
    for( iAxis = 0;  iAxis < KDCHART_MAX_AXES;  ++iAxis )
    {
        internal__KDChart__CalcValues& cv = calcVal[iAxis];
        cv.processThisAxis = (    params()->axisParams( iAxis ).axisVisible()
                               && KDChartAxisParams::AxisTypeUnknown
                                  != params()->axisParams( iAxis ).axisType() );
        if( cv.processThisAxis ){
            cv.nSubDelimFactor = 0.0;
            cv.pDelimDelta     = 0.0;
            cv.nTxtHeight      = 0.0;
            cv.pTextsX         = 0.0;
            cv.pTextsY         = 0.0;
            cv.pTextsW         = 0.0;
            cv.pTextsH         = 0.0;
            cv.textAlign       = Qt::AlignHCenter | Qt::AlignVCenter;
            cv.isDateTime      = false;
            cv.autoDtLabels    = false;
            calculateLabelTexts( painter,
                                 *data,
                                 *params(),
                                 iAxis,
                                 averageValueP1000,
                                 delimLen,
                                 // start of reference parameters
                                 cv.basicPos,
                                 cv.orig,
                                 cv.dest,
                                 cv.pXDeltaFactor,
                                 cv.pYDeltaFactor,
                                 cv.pXDelimDeltaFaktor,
                                 cv.pYDelimDeltaFaktor,
                                 cv.nSubDelimFactor,
                                 cv.pDelimDelta,
                                 cv.nTxtHeight,
                                 cv.pTextsX,
                                 cv.pTextsY,
                                 cv.pTextsW,
                                 cv.pTextsH,
                                 cv.textAlign,
                                 cv.bLogarithmic,
                                 cv.isDateTime,
                                 cv.autoDtLabels,
                                 cv.dtLow,
                                 cv.dtHigh,
                                 cv.dtDeltaScale );
            const KDChartAxisParams & para = params()->axisParams( iAxis );
            cv.bSteadyCalc = para.axisSteadyValueCalc();
            cv.bDecreasing = para.axisValuesDecreasing();
            cv.nLow        = para.trueAxisLow();
            cv.nHigh       = para.trueAxisHigh();
            cv.nDelta      = para.trueAxisDelta();
            cv.nDeltaPix   = para.trueAxisDeltaPixels();
            cv.pLastX      = cv.dest.x();
            cv.pLastY      = cv.dest.y();
        }
    }

    // step #2: if isometric axes are desired adjust/re-calculate some values
    for ( iAxis = 0; iAxis < KDCHART_MAX_AXES; ++iAxis ){
        internal__KDChart__CalcValues& cv = calcVal[iAxis];
        if(    cv.processThisAxis
                && cv.bSteadyCalc ){
            const KDChartAxisParams & para = params()->axisParams( iAxis );
            const uint isoRef = para.isometricReferenceAxis();
            if(    KDCHART_NO_AXIS != isoRef
                    && iAxis != isoRef
                    && (    KDCHART_MAX_AXES  > isoRef
                         || KDCHART_ALL_AXES == isoRef ) ){
                if( KDCHART_ALL_AXES == isoRef ){
                    uint iAxis2;
                    // first find the axis values to be taken as reference
                    double nDelta          = cv.nDelta;
                    double nDeltaPix       = cv.nDeltaPix;
                    double nSubDelimFactor = cv.nSubDelimFactor;
                    for ( iAxis2 = 0;
                          iAxis2 < KDCHART_MAX_AXES;
                          ++iAxis2 ){
                        internal__KDChart__CalcValues& cv2 = calcVal[iAxis2];
                        if(    cv2.processThisAxis
                            && cv2.bSteadyCalc
                            && (0.0 != cv2.nDelta)
                            && (fabs(cv2.nDeltaPix / cv2.nDelta) < fabs(nDeltaPix / nDelta)) ){
                            if( (nDelta >= 0.0) == (cv2.nDelta >= 0.0) )
                                nDelta = cv2.nDelta;
                            else
                                nDelta = cv2.nDelta * -1.0;
                            if( (nDeltaPix >= 0.0) == (cv2.nDeltaPix >= 0.0) )
                                nDeltaPix = cv2.nDeltaPix;
                            else
                                nDeltaPix = cv2.nDeltaPix * -1.0;
                            if( (nSubDelimFactor >= 0.0) == (cv2.nSubDelimFactor >= 0.0) )
                                nSubDelimFactor = cv2.nSubDelimFactor;
                            else
                                nSubDelimFactor = cv2.nSubDelimFactor * -1.0;
                        }
                    }
                    // now adjust all axes (if necessary)
                    for ( iAxis2 = 0;
                          iAxis2 < KDCHART_MAX_AXES;
                          ++iAxis2 ){
                        internal__KDChart__CalcValues& cv2 = calcVal[iAxis2];
                        if(    cv2.processThisAxis
                            && cv2.bSteadyCalc
                            && (    fabs(cv2.nDelta)    != fabs(nDelta)
                                 || fabs(cv2.nDeltaPix) != fabs(nDeltaPix) ) ){
                            //qDebug("\nrecalculating scale for axis %x", iAxis2);
                            //qDebug("cv2.nDelta %f   cv2.nDeltaPix %f       nDelta %f   nDeltaPix %f\n",
                            //        cv2.nDelta,cv2.nDeltaPix,nDelta,nDeltaPix);
                            if( (cv2.nDelta >= 0.0) == (nDelta >= 0.0) )
                                cv2.nDelta = nDelta;
                            else
                                cv2.nDelta = nDelta * -1.0;
                            if( (cv2.nDeltaPix >= 0.0) == (nDeltaPix >= 0.0) )
                                cv2.nDeltaPix = nDeltaPix;
                            else
                                cv2.nDeltaPix = nDeltaPix * -1.0;
                            reCalculateLabelTexts( painter,
                                                   *data,
                                                   *params(),
                                                   iAxis2,
                                                   averageValueP1000,
                                                   delimLen,
                                                   cv2 );
                            if( (cv2.nSubDelimFactor >= 0.0) == (nSubDelimFactor >= 0.0) )
                                cv2.nSubDelimFactor = nSubDelimFactor;
                            else
                                cv2.nSubDelimFactor = nSubDelimFactor * -1.0;
                        }
                    }
                }else{
                    internal__KDChart__CalcValues& cv2 = calcVal[isoRef];
                    // adjust this axis or the other axis (if necessary)
                    if(    cv2.processThisAxis
                            && cv2.bSteadyCalc
                            && (    cv2.nDelta    != cv.nDelta
                                || cv2.nDeltaPix != cv.nDeltaPix ) ){
                        if(    cv2.nDelta > cv.nDelta
                                || (     cv2.nDelta   == cv.nDelta
                                    && cv2.nDeltaPix < cv.nDeltaPix ) ){
                            // adjust this axis
                            //qDebug("recalculating scale for this axis %x", iAxis);
                            cv.nDelta    = cv2.nDelta;
                            cv.nDeltaPix = cv2.nDeltaPix;
                            reCalculateLabelTexts(
                                    painter,
                                    *data,
                                    *params(),
                                    iAxis,
                                    averageValueP1000,
                                    delimLen,
                                    cv );
                            cv.nSubDelimFactor = cv2.nSubDelimFactor;
                        }else{
                            // adjust the other axis
                            //qDebug("\nrecalculating scale for other axis %x", isoRef);
                            //qDebug("cv2.nDelta %f   cv2.nDeltaPix %f       cv.nDelta %f   cv.nDeltaPix %f",
                            //        cv2.nDelta,cv2.nDeltaPix,cv.nDelta,cv.nDeltaPix);
                            cv2.nDelta    = cv.nDelta;
                            cv2.nDeltaPix = cv.nDeltaPix;
                            reCalculateLabelTexts(
                                    painter,
                                    *data,
                                    *params(),
                                    isoRef,
                                    averageValueP1000,
                                    delimLen,
                                    cv2 );
                            cv2.nSubDelimFactor = cv.nSubDelimFactor;
                        }
                    }
                }
            }
        }
    }
    return true;
}


/**
  Paints the actual axes areas.

  \param painter the QPainter onto which the chart should be painted
  \param data the data that will be displayed as a chart
  */
void KDChartAxesPainter::paintAxes( QPainter* painter,
        KDChartTableDataBase* data )
{
    if ( !painter || !data || 0 == params() )
        return ;
    
    const bool bMultiRowBarChart = KDChartParams::Bar == params()->chartType() &&
                                   KDChartParams::BarMultiRows == params()->barChartSubType();

    double areaWidthP1000 = _logicalWidth / 1000.0;
    double areaHeightP1000 = _logicalHeight / 1000.0;
    double averageValueP1000 = QMIN(areaWidthP1000, areaHeightP1000);//( areaWidthP1000 + areaHeightP1000 ) / 2.0;
    // length of little delimiter-marks indicating axis scaling
    double delimLen;

    calculateAllAxesLabelTextsAndCalcValues( painter, data, areaWidthP1000, areaHeightP1000, delimLen );


    // Now the labels are known, so let us paint the axes...
    painter->save();
    painter->setPen( Qt::NoPen );

    bool screenOutput = params()->optimizeOutputForScreen();
    uint iAxis;

    for ( iAxis = 0; iAxis < KDCHART_MAX_AXES; ++iAxis ){
        internal__KDChart__CalcValues& cv = calcVal[iAxis];
        if( cv.processThisAxis ){

            const KDChartAxisParams & para = params()->axisParams( iAxis );
           
            internal__KDChart__CalcValues& cv = calcVal[iAxis];

            const QColor labelsColor( para.axisLabelsColor() );

            // Debugging axis areas:
            //painter->fillRect(para.axisTrueAreaRect(), Qt::yellow);

            uint lineWidth = 0 <= para.axisLineWidth()
                ? para.axisLineWidth()
                : -1 * static_cast < int > ( para.axisLineWidth()
                        * averageValueP1000 );
            ( ( KDChartAxisParams& ) para ).setAxisTrueLineWidth( lineWidth );

            uint gridLineWidth
                = ( KDCHART_AXIS_GRID_AUTO_LINEWIDTH
                        == para.axisGridLineWidth() )
                ? lineWidth
                : (   ( 0 <= para.axisGridLineWidth() )
                        ? para.axisGridLineWidth()
                        : -1 * static_cast < int > ( para.axisGridLineWidth()
                            * averageValueP1000 ) );

            uint gridSubLineWidth
                = ( KDCHART_AXIS_GRID_AUTO_LINEWIDTH
                        == para.axisGridSubLineWidth() )
                ? lineWidth
                : (   ( 0 <= para.axisGridSubLineWidth() )
                        ? para.axisGridSubLineWidth()
                        : -1 * static_cast < int > ( para.axisGridSubLineWidth()
                            * averageValueP1000 ) );

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
            //     their space, if needed we will do the following
            //     in order to avoid clipping of text parts:
            //
            //     (a) ABSCISSA axes only: rotate the texts in 5 steps
            //                             until they are drawn vertically
            //
            //     (b) further reduce the texts' font height down to 6pt
            //         .
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

            // Note: The labels-touch-edges flag may have been set to true
            //       inside the calculateLabelTexts() function.
            bool bTouchEdges = para.axisLabelsTouchEdges();

            // NOTE: The steady-value-calc flag may have been set to true
            //       inside the calculateLabelTexts() function
            //       by a special setAxisLabelTextParams() call,
            //       therefor we do not store its value before calling that function.
            if( cv.bLogarithmic )
                cv.nSubDelimFactor = 0.1;

            const double nUsableAxisHeight = cv.pTextsH;
            const double nUsableAxisWidth  = cv.pTextsW;

            const bool isHorizontalAxis
                = (KDChartAxisParams::AxisPosBottom == cv.basicPos) ||
                (KDChartAxisParams::AxisPosTop    == cv.basicPos);

            QStringList* labelTexts = ( QStringList* ) para.axisLabelTexts();
            uint nLabels = ( 0 != labelTexts )
                ? labelTexts->count()
                : 0;
            // start point of 1st delimiter on the axis-line == grid-start
            QPoint p1( cv.orig );
            // end point of 1st delimiter near the label text
            QPoint p2( cv.orig );
            // end point of small sub-delimiter
            QPoint p2a( cv.orig );
            // start point of 1st grid-line (beginnig at the axis)
            QPoint pGA( cv.orig );
            // end point of 1st grid-line at the other side of the chart
            QPoint pGZ( cv.orig );
            // start point of zero-line, this is often identical with p1
            // but will be different in case of shifted zero-line
            double axisZeroLineStartX = p1.x();
            double axisZeroLineStartY = p1.y();

            p2.setX(  p2.x()  + static_cast < int > ( cv.pXDelimDeltaFaktor * delimLen ) );
            p2.setY(  p2.y()  + static_cast < int > ( cv.pYDelimDeltaFaktor * delimLen ) );
            p2a.setX( p2a.x() + static_cast < int > ( cv.pXDelimDeltaFaktor * delimLen * 2.0 / 3.0 ) );
            p2a.setY( p2a.y() + static_cast < int > ( cv.pYDelimDeltaFaktor * delimLen * 2.0 / 3.0 ) );
            pGZ.setX( pGZ.x() - static_cast < int > ( cv.pXDelimDeltaFaktor * (_dataRect.width()  - 1) ) );
            pGZ.setY( pGZ.y() - static_cast < int > ( cv.pYDelimDeltaFaktor * (_dataRect.height() - 1) ) );

            if ( nLabels ) {
                // Sometimes the first or last labels partially reach out of
                // their axis area: we allow this
                const bool oldClippingFlag = painter->hasClipping();
                painter->setClipping( false );

                if( para.hasAxisFirstLabelText() )
                    labelTexts->first() = para.axisFirstLabelText();
                if( para.hasAxisLastLabelText() )
                    labelTexts->last() = para.axisLastLabelText();

                const double pXDelta = cv.pXDeltaFactor * cv.pDelimDelta;
                const double pYDelta = cv.pYDeltaFactor * cv.pDelimDelta;

                // draw label texts and delimiters and grid
                painter->setPen( QPen( para.axisLineColor(),
                                 lineWidth ) );

                const QString formatDT = cv.isDateTime
                    ? para.axisLabelsDateTimeFormat()
                    : QString();

                // calculate font size
                const double minTextHeight = para.axisLabelsFontMinSize();
                if ( minTextHeight > cv.nTxtHeight )
                    cv.nTxtHeight = minTextHeight;
                QFont actFont( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() ) {
                    actFont.setPixelSize( static_cast < int > ( cv.nTxtHeight ) );
                }
                painter->setFont( actFont );
                QFontMetrics fm( painter->fontMetrics() );

                int nLeaveOut = 0;
                int nRotation = 0;

                // Draw simple string labels
                // or calculate and draw nice Date/Time ruler?
                QString commonDtHeader;
                if( cv.autoDtLabels ){
                    cv.textAlign = Qt::AlignCenter;
                    //qDebug(dtLow.toString("\nd.MM.yyyy  -  h:mm:ss" ));
                    //qDebug(dtHigh.toString( "d.MM.yyyy  -  h:mm:ss" ));
                    const QDate& dLow  = cv.dtLow.date();
                    const QTime& tLow  = cv.dtLow.time();
                    const QDate& dHigh = cv.dtHigh.date();
                    const QTime& tHigh = cv.dtHigh.time();
                    bool sameYear   = dLow.year() == dHigh.year();
                    bool sameMonth  = sameYear   && (dLow.month()  == dHigh.month() );
                    bool sameDay    = sameMonth  && (dLow.day()    == dHigh.day()   );
                    bool sameHour   = sameDay    && (tLow.hour()   == tHigh.hour()  );
                    bool sameMinute = sameHour   && (tLow.minute() == tHigh.minute());
                    bool sameSecond = sameMinute && (tLow.second() == tHigh.second());
                    if( sameDay ){
                        commonDtHeader = QString::number( dLow.day() )
                            + ". "
#if COMPAT_QT_VERSION >= 0x030000
                            + QDate::longMonthName( dLow.month() )
#else
                            + dLow.monthName( dLow.month() )
#endif
                            + ' '
                            + QString::number( dLow.year() );
                        if( sameHour ){
                            commonDtHeader += "  /  "
                                + QString::number( tLow.hour() )
                                + ':';
                            if( sameMinute ){
                                if( 10 > tLow.minute() )
                                    commonDtHeader += '0';
                                commonDtHeader += QString::number( tLow.minute() )
                                    + ':';
                                if( sameSecond ){
                                    if( 10 > tLow.second() )
                                        commonDtHeader += '0';
                                    commonDtHeader += QString::number( tLow.second() );
                                    //
                                    // " Huston, we have a problem! "
                                    //
                                    // Currently we don't support milli secs
                                    // since they will not fit into a double
                                    // when looking at years...
                                    //
                                    // This will be improved in release 2.0.
                                    //                     (khz, 2002/07/12)
                                }
                                else
                                    commonDtHeader += "00";
                            }
                            else
                                commonDtHeader += "00";
                        }
                    }else if( sameMonth )
#if COMPAT_QT_VERSION >= 0x030000
                        commonDtHeader = QDate::longMonthName( dLow.month() )
#else
                            commonDtHeader = dLow.monthName( dLow.month() )
#endif
                            + ' '
                            + QString::number( dLow.year() );
                    else if( sameYear )
                        commonDtHeader = QString::number( dLow.year() );
                    //if( !commonDtHeader.isEmpty() )
                    //    qDebug(commonDtHeader);
                }else{
                    // make sure all label texts fit into their space
                    // by rotating and/or shrinking the texts
                    // or by leaving out some of the labels
                    QRegion unitedRegions;

                    const bool tryLeavingOut =
                        ( para.axisValueLeaveOut() == KDCHART_AXIS_LABELS_AUTO_LEAVEOUT )
                        || ( 0 < para.axisValueLeaveOut() );
                    if( tryLeavingOut ) {
                        if( para.axisValueLeaveOut()
                                == KDCHART_AXIS_LABELS_AUTO_LEAVEOUT )
                            nLeaveOut = 0;
                        else 
                            nLeaveOut = para.axisValueLeaveOut();
                            
                    }
                    else
                        nLeaveOut = 0;
                    int stepWidthLeaveOut = nLeaveOut+1;
                    int iStepsLeaveOut = 0;

                    const bool tryShrinking = !para.axisLabelsDontShrinkFont();
                    const double nInitialTxtHeight = cv.nTxtHeight;

                    const bool tryRotating = isHorizontalAxis
                        && !para.axisLabelsDontAutoRotate();
                    const int nInitialRotation = (    (360 >  para.axisLabelsRotation())
                            && (270 <= para.axisLabelsRotation()) )
                        ? para.axisLabelsRotation()
                        : 0;
                    nRotation = nInitialRotation;

                    bool textsDontFitIntoArea;
                    bool textsOverlapping;
                    bool textsMatching;
                    do {
                        textsDontFitIntoArea = false;
                        textsOverlapping = false;
                        textsMatching = true;
                        // test if all texts match without mutually overlapping
                        unitedRegions = QRegion();
                        int align = nRotation
                            ? (Qt::AlignRight | Qt::AlignVCenter) // adjusting for rotation
                            : cv.textAlign;
                        QPoint anchor(200,200);
                        int iLeaveOut = 0;
                        double iLabel=0.0;
                        for ( QStringList::Iterator it = labelTexts->begin();
                                it != labelTexts->end();
                                ++it ) {
                            iLabel += 1.0;
                            if( iLeaveOut < nLeaveOut ) {
                                ++iLeaveOut;
                            } else {
                                iLeaveOut = 0;
                                anchor.setX( p2.x() + static_cast < int > ( pXDelta * (iLabel - 0.5) ) );
                                anchor.setY( p2.y() + static_cast < int > ( pYDelta * (iLabel - 0.5) ) );

                                // allow for shearing and/or scaling of the painter
                                anchor = painter->worldMatrix().map( anchor );

                                QString text;
                                if( cv.isDateTime ){
#if COMPAT_QT_VERSION >= 0x030000
                                    QDateTime dt( QDateTime::fromString( *it,
                                                Qt::ISODate ) );
                                    text = dt.toString( formatDT );
#else
                                    QDateTime dt( dateTimeFromString( *it ) );
                                    text = dt.toString();
#endif
                                }else{
                                    text = *it;
                                }
                                KDDrawTextRegionAndTrueRect infosKDD =
                                    KDDrawText::measureRotatedText( painter,
                                            nRotation,
                                            anchor,
                                            text,
                                            0,
                                            align,
                                            &fm,
                                            false,
                                            false,
                                            15 );
                                if( infosKDD.region.boundingRect().left()
                                        < params()->globalLeadingLeft()+1 ){
                                    textsMatching = false;
                                    textsDontFitIntoArea = true;
                                    //qDebug("too wide");
                                    break;
                                }
                                else{
                                    QRegion sectReg( infosKDD.region.intersect( unitedRegions ) );
                                    if ( sectReg.isEmpty() )
                                        unitedRegions = unitedRegions.unite( infosKDD.region );
                                    else {
                                        textsMatching = false;
                                        textsOverlapping = true;
                                        //qDebug("label regions are intersecting");
                                        break;
                                    }
                                }
                            }
                        }
                        /*if(!iAxis){
                          qDebug("nTxtHeight: "+QString::number(nTxtHeight)+"   nRotation: "+QString::number(nRotation)+
                          "   matching: "+QString(textsMatching ? "TRUE":"FALSE"));
                          }*/
                        if( isHorizontalAxis ) {
                            if( nUsableAxisHeight < unitedRegions.boundingRect().height() ){
                                textsMatching = false;
                                textsDontFitIntoArea = true;
                                //qDebug("too high");
                            }
                        } else {
                            if( nUsableAxisWidth < unitedRegions.boundingRect().width() ){
                                //qDebug("textsMatching: %s",textsMatching ? "TRUE" : "FALSE");
                                textsMatching = false;
                                textsDontFitIntoArea = true;
                                //qDebug("too wide");
                            }
                            //else qDebug("not too wide");
                        }
                        /*if(textsMatching && !iAxis){
                          qDebug("--------------------------");
                          qDebug("nTxtHeight: "+QString::number(nTxtHeight)+"   nRotation: "+QString::number(nRotation));
                          qDebug("matching");
                          }*/
                        if( !textsMatching ) {
                            bool rotatingDoesNotHelp = false;
                            // step 1: In case of labels being too wide
                            //         to fit into the available space
                            //         we try to rotate the texts in 5 steps.
                            //         This is done for Abscissa axes only.
                            if ( tryRotating ) {
                                //qDebug("try rotating");
                                // The following is designed for horizontal axes
                                // since we currently don't support label rotating
                                // on vertical axes.             (khz, 2002/08/15)
                                if( textsDontFitIntoArea  ){
                                    if( nRotation != nInitialRotation ){
                                      textsDontFitIntoArea = false;
                                      nRotation = nInitialRotation;
                                    }
                                    rotatingDoesNotHelp = true;
                                }
                                else{
                                    if( nRotation ) {
                                        if( 270 < nRotation ) {
                                            nRotation -= 5;
                                            if( 270 > nRotation )
                                                nRotation = 270; // drawing vertically now
                                        } else {
                                            if( nInitialRotation )
                                                nRotation = nInitialRotation;
                                            else
                                                nRotation = 0; // reset rotation to ZERO
                                            rotatingDoesNotHelp = true;
                                        }
                                    } else {
                                        if( nInitialRotation )
                                            nRotation = nInitialRotation;
                                        else
                                            nRotation = 350; // (re-)start rotating with -10
                                    }
                                }
                            }
                            if ( !tryRotating || rotatingDoesNotHelp ) {

                                // step 2: In case of labels being too wide and
                                //         rotating them did not help or is forbidden
                                //         we try to reduce the font size.
                                if ( tryShrinking && (minTextHeight < cv.nTxtHeight) ) {
                                    cv.nTxtHeight -= 1.0;
                                    if ( minTextHeight > cv.nTxtHeight )
                                        cv.nTxtHeight = minTextHeight;
                                } else {

                                    // step 3: In case reducing the font size is not possible
                                    //         any further (or is not allowed at all) we try
                                    //         to leave out some of the labels.
                                    if(    tryLeavingOut
                                        && textsOverlapping
                                        && (nLeaveOut+1 < static_cast < int > ( nLabels ) ) ) {
                                        ++iStepsLeaveOut;
                                        //if(!iAxis)qDebug("iStepsLeaveOut: %i", iStepsLeaveOut);
                                        nLeaveOut =
                                            iStepsLeaveOut*stepWidthLeaveOut - 1;
                                        if( tryShrinking )
                                            cv.nTxtHeight = nInitialTxtHeight;
                                    }
                                    else
                                        break;
                                }
                                if( tryShrinking ) {
                                    actFont.setPixelSize( static_cast < int > ( cv.nTxtHeight ) );
                                    //qDebug("axis:     cv.nTxtHeight: %f", iAxis, cv.nTxtHeight);
                                    painter->setFont( actFont );
                                    fm = painter->fontMetrics();
                                }
                            }
                        }
                    } while( !textsMatching );

                    if( nRotation ){
                        // The following is designed for horizontal axes
                        // since we currently don't support label rotating
                        // on vertical axes.             (khz, 2002/08/15)
                        //int oldVert = textAlign & (Qt::AlignTop | Qt::AlignBottom);
                        //int steepness = abs(270-nRotation);
                        //bool steep = (30 > steepness);
                        cv.textAlign = Qt::AlignRight | Qt::AlignVCenter;  // adjusting for rotation
                        /*textAlign = Qt::AlignRight |
                          ( steep ? Qt::AlignVCenter : oldVert);*/
                        //int dx = pXDelta / 2 - steep ? (nTxtHeight / 4) : 0;
                        double dx = (pXDelta / 2) - (cv.nTxtHeight / 4);
                        double dy = /*steep ? 0 : */(cv.nTxtHeight / 2.0);
                        cv.pTextsX += dx;
                        cv.pTextsY += dy;
                    }
                    /*
                       QBrush oldBrush = painter->brush();
                       QRegion oldReg = painter->clipRegion();//QPainter::CoordPainter);
                       painter->setBrush(Qt::Dense4Pattern);
                       painter->setClipRegion(unitedRegions);//,QPainter::CoordPainter);
                       painter->drawRect(0,0,2000,1500);
                       painter->setClipRegion(oldReg);//,QPainter::CoordPainter);
                       painter->setBrush(oldBrush);
                       */
                    /*if(!iAxis){
                      qDebug("==========================");
                      qDebug("nTxtHeight: "+QString::number(nTxtHeight)+"   nRotation: "+QString::number(nRotation));
                      qDebug(textsMatching ? "matching":"not matching");
                      }*/
                }

                painter->setFont( actFont );
                fm = QFontMetrics( painter->fontMetrics() );

                // set colour of grid pen
                QPen gridPen, leaveOutGridPen;
                if( para.axisShowGrid() && !bMultiRowBarChart )
                    gridPen.setColor( para.axisGridColor() );

                const int pXDeltaDiv2 = static_cast < int > ( pXDelta / 2.0 );
                const int pYDeltaDiv2 = static_cast < int > ( pYDelta / 2.0 );

                bool bDrawAdditionalSubGridLine = false;
                double pGXMicroAdjust = 0.0;
                double pGYMicroAdjust = 0.0;
                if ( !bTouchEdges ) {
                    // adjust the data values pos
                    p1.setX( p1.x() + pXDeltaDiv2 );
                    p1.setY( p1.y() + pYDeltaDiv2 );
                    p2.setX( p2.x() + pXDeltaDiv2 );
                    p2.setY( p2.y() + pYDeltaDiv2 );
                    // adjust the short delimiter lines pos
                    p2a.setX( p2a.x() + pXDeltaDiv2 );
                    p2a.setY( p2a.y() + pYDeltaDiv2 );
                    // adjust grid lines pos
                    bDrawAdditionalSubGridLine =
                        isHorizontalAxis && !
                        params()->axisParams(
                                KDChartAxisParams::AxisPosRight ).axisVisible() &&
                        !bMultiRowBarChart;
                    pGA.setX( pGA.x() + pXDeltaDiv2 );
                    pGA.setY( pGA.y() + pYDeltaDiv2 );
                    pGZ.setX( pGZ.x() + pXDeltaDiv2 );
                    pGZ.setY( pGZ.y() + pYDeltaDiv2 );
                    // fine-tune grid line pos for grid of vertical axis
                    if( KDChartAxisParams::AxisTypeNORTH == para.axisType() ) {
                        pGXMicroAdjust = cv.pXDeltaFactor * lineWidth / 2.0;
                        pGYMicroAdjust = cv.pYDeltaFactor * lineWidth / 2.0;
                    }
                }
                double x1, y1, x2, y2, xGA, yGA, xGZ, yGZ,
                p1X, p1Y, p2X, p2Y, pGAX, pGAY, pGZX, pGZY, xT, yT;

                double pXSubDelimDelta = pXDelta * cv.nSubDelimFactor;
                double pYSubDelimDelta = pYDelta * cv.nSubDelimFactor;

                if (    !cv.autoDtLabels
                        && 0.0 != cv.nSubDelimFactor
                        && para.axisShowSubDelimiters()
                        && para.axisLabelsVisible()
                        && !nLeaveOut ) {
                    QPen pen( para.axisLineColor(), static_cast < int > ( 0.5 * lineWidth ) );
                    uint penWidth = pen.width();
                    bool bOk = true;

                    if( cv.bLogarithmic )
                        cv.nSubDelimFactor = 0.1;

                    while ( fabs( ( pXDelta + pYDelta ) * cv.nSubDelimFactor / 6.0 )
                            <= 1.0 + penWidth
                            && bOk ) {
                        if ( 0 < penWidth ) {
                            --penWidth;
                            pen.setWidth( penWidth );
                        }else{
                            if( cv.bLogarithmic ){
                                break; // there is nothing we can do: we allways
                                // want 10 sub-delims per logarithmic step
                            }else{
                                if ( 0.5 != cv.nSubDelimFactor ) {
                                    // emercency: reduce number of sub-scaling
                                    cv.nSubDelimFactor = 0.5;

                                    pXSubDelimDelta = pXDelta * cv.nSubDelimFactor;
                                    pYSubDelimDelta = pYDelta * cv.nSubDelimFactor;
                                } else
                                    bOk = false;
                            }
                        }
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

                        // set up grid pen for drawing the sub-grid lines
                        const QPen oldGridPen( gridPen );
                        if ( para.axisShowGrid() ) {
                            gridPen.setColor( para.axisGridSubColor() );
                            gridPen.setWidth( gridSubLineWidth );
                            gridPen.setStyle( para.axisGridSubStyle() );
                        }
                        const QPen oldPen( painter->pen() );
                        painter->setPen( pen );
                        double nSubDelim = ( labelTexts->count() - 1 )
                            / cv.nSubDelimFactor;
                       
                        //qDebug("subDelim: %f", 
                        modf( nSubDelim, &nSubDelim );

                        int logarithCnt = 1;
                        double xLogarithOffs = 0;
                        double yLogarithOffs = 0;
                        double dDummy;
                        double mainDelim = 0.0;
                        bool paint = true;

                        for ( double iDelim = 1.0;
                                iDelim <= nSubDelim + 1.0;
                                iDelim += 1.0, logarithCnt++ ) {
                          // test if it is a sub or a main delimiter
                            if ( mainDelim > 0.0 )
                              paint = true;
                            else
                              paint = false;

                            if ( cv.bLogarithmic )
                            {
                                if ( logarithCnt == 11 )
                                {
                                    xLogarithOffs +=
                                        pXDelta * log10( 10*cv.nSubDelimFactor*10 );
                                    yLogarithOffs +=
                                        pYDelta * log10( 10*cv.nSubDelimFactor*10 );
                                    logarithCnt=1;
                                }

                                pXSubDelimDelta =
                                    pXDelta * log10( 10*cv.nSubDelimFactor*logarithCnt );
                                pYSubDelimDelta =
                                    pYDelta * log10( 10*cv.nSubDelimFactor*logarithCnt );
                            }

                            if ( para.axisShowGrid() && !bMultiRowBarChart) {
                                // draw the sub grid line
                                if( 0.0 != modf((iDelim-1.0) * cv.nSubDelimFactor, &dDummy) )
				  
                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pGAX - pGXMicroAdjust ),
                                                          static_cast<int>( pGAY - pGYMicroAdjust ) ),
                                                  QPoint( static_cast<int>( pGZX - pGXMicroAdjust ),
                                                          static_cast<int>( pGZY - pGYMicroAdjust ) ),
                                                  gridPen );
				  
                                if( cv.bLogarithmic ){
                                    pGAX = xGA + pXSubDelimDelta + xLogarithOffs;
                                    pGAY = yGA + pYSubDelimDelta + yLogarithOffs;
                                    pGZX = xGZ + pXSubDelimDelta + xLogarithOffs;
                                    pGZY = yGZ + pYSubDelimDelta + yLogarithOffs;
                                }else{
                                    pGAX = xGA + iDelim * pXSubDelimDelta;
                                    pGAY = yGA + iDelim * pYSubDelimDelta;
                                    pGZX = xGZ + iDelim * pXSubDelimDelta;
                                    pGZY = yGZ + iDelim * pYSubDelimDelta;
                                    /*
                                    if( !modf(iDelim * cv.nSubDelimFactor, &dDummy) ){
                                       pGAX = xGA + (iDelim * cv.nSubDelimFactor) * pXDelta;
                                       pGAY = yGA + (iDelim * cv.nSubDelimFactor) * pYDelta;
                                       pGZX = xGZ + (iDelim * cv.nSubDelimFactor) * pXDelta;
                                       pGZY = yGZ + (iDelim * cv.nSubDelimFactor) * pYDelta;
                                    }
                                    */
                                }
                            }
                           
                       
                            // draw the short delimiter line
                            // PENDING: Michel - make sure not to draw the sub-delimiters over the main ones.
                            // by testing if it is a sub delimiter or a main one 
                            if ( paint ) 			   	   
                            painter->drawLine( QPoint( static_cast<int>( p1X ), static_cast<int>( p1Y ) ),
                                               QPoint( static_cast<int>( p2X ), static_cast<int>( p2Y ) ) );
			   
                            mainDelim += 1.0;


                            if( cv.bLogarithmic ){
                                p1X = x1 + pXSubDelimDelta + xLogarithOffs;
                                p1Y = y1 + pYSubDelimDelta + yLogarithOffs;
                                p2X = x2 + pXSubDelimDelta + xLogarithOffs;
                                p2Y = y2 + pYSubDelimDelta + yLogarithOffs;
                            }else{
                                p1X = x1 + iDelim * pXSubDelimDelta;
                                p1Y = y1 + iDelim * pYSubDelimDelta;
                                p2X = x2 + iDelim * pXSubDelimDelta;
                                p2Y = y2 + iDelim * pYSubDelimDelta;
                            }

                            if ( mainDelim >= nSubDelim/(labelTexts->count() -1) )
                              mainDelim = 0.0;


                        } // for
                        // draw additional sub grid line
                        if( bDrawAdditionalSubGridLine
                                && para.axisShowGrid() ) {
			  
                            saveDrawLine( *painter,
                                          QPoint( static_cast<int>( pGAX - pGXMicroAdjust ),
                                                  static_cast<int>( pGAY - pGYMicroAdjust ) ),
                                          QPoint( static_cast<int>( pGZX - pGXMicroAdjust ),
                                                  static_cast<int>( pGZY - pGYMicroAdjust ) ),
                                                  gridPen );
			  
                        }
                        painter->setPen( oldPen );
                        gridPen = oldGridPen;
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
                xT = cv.pTextsX;
                yT = cv.pTextsY;
                // set up grid pen for drawing the normal grid lines
                if ( para.axisShowGrid() ) {
                    gridPen.setWidth( gridLineWidth );
                    gridPen.setStyle( para.axisGridStyle() );
                    // if axis not visible draw the 1st grid line too
                    if( !para.axisLineVisible() )		      
		      saveDrawLine( *painter, cv.orig, cv.dest, gridPen );
                }
                if( nLeaveOut ) {
                    leaveOutGridPen = gridPen;
                    leaveOutGridPen.setWidth( gridLineWidth / 2 );
                    leaveOutGridPen.setStyle( Qt::DotLine );
                }
                //  =========================================================
                //  ||  The labels and delimiters and grid printing loops  ||
                //  =========================================================
                //
                double iLabel = 0.0;
                if( cv.autoDtLabels )
                {
                    /*
                       qDebug("\ndtLow: %i %i %i    %i:%i:%i",
                       dtLow.date().year(),
                       dtLow.date().month(),
                       dtLow.date().day(),
                       dtLow.time().hour(),
                       dtLow.time().minute(),
                       dtLow.time().second());
                       qDebug("dtHigh: %i %i %i    %i:%i:%i",
                       dtHigh.date().year(),
                       dtHigh.date().month(),
                       dtHigh.date().day(),
                       dtHigh.time().hour(),
                       dtHigh.time().minute(),
                       dtHigh.time().second());
                       */
                    int pXD = static_cast <int> (cv.pXDelimDeltaFaktor * 1.25 * (cv.nTxtHeight+4));
                    int pYD = static_cast <int> (cv.pYDelimDeltaFaktor * 1.25 * (cv.nTxtHeight+4));
                    int orgXD = pXD;
                    int orgYD = pYD;
                    cv.pTextsW = fabs( (0.0 == pXDelta) ? pXD : pXDelta );
                    cv.pTextsH = fabs( (0.0 == pYDelta) ? pYD : pYDelta );

                    double pSecX     = x1;
                    double pSecY     = y1;
                    bool   secPaint= false;
                    double pMinX     = x1;
                    double pMinY     = y1;
                    bool   minPaint= false;
                    double pHourX    = x1;
                    double pHourY    = y1;
                    bool   hourPaint= false;
                    double pDayX     = x1;
                    double pDayY     = y1;
                    bool   dayPaint= false;
                    /* khz: currently not used
                       double pWeekX    = x1;
                       double pWeekY    = y1;
                       bool   weekPaint= false;
                       */
                    double pMonthX   = x1;
                    double pMonthY   = y1;
                    bool   monthPaint= false;
                    /*double pQuarterX = x1;
                      double pQuarterY = y1;
                      bool   minPaint= false;
                      */
                    double pYearX    = x1;
                    double pYearY    = y1;
                    bool   yearPaint= false;

                    double pXYDelta = fabs( pXDelta ) + fabs( pYDelta );

                    if( 0.0 == para.trueAxisDeltaPixels() )
                        ( ( KDChartAxisParams& ) para ).setTrueAxisDeltaPixels( QMIN(_logicalWidth, _logicalHeight) / 150 );

                    bool dtGoDown = cv.dtLow > cv.dtHigh;
                    int  mult = dtGoDown ? -1 : 1;
                    const QDateTime& startDt = dtGoDown ? cv.dtHigh : cv.dtLow;

                    ( ( KDChartAxisParams& ) para ).setAxisDtLowPos( x1, y1 );
                    // adjust stored dt-low and scale settings
                    ( ( KDChartAxisParams& ) para ).setTrueAxisDtLow( startDt );
                    ( ( KDChartAxisParams& ) para ).setTrueAxisDtScale( cv.dtDeltaScale );

                    int gridDX = pGZ.x() - pGA.x();
                    int gridDY = pGZ.y() - pGA.y();
                    if ( para.axisShowGrid() ) {
                        gridPen.setColor( para.axisGridColor() );
                        gridPen.setWidth( gridLineWidth );
                        gridPen.setStyle( para.axisGridStyle() );
                    }
                    QPen subGridPen(    gridPen.color(), 1, para.axisGridStyle() );
                    QPen subSubGridPen( gridPen.color(), 1, para.axisGridSubStyle() );
                    QPen pen = subGridPen;

                    QDateTime dt(    startDt );
                    QDateTime newDt( startDt );
                    for( uint i=1; i <= nLabels; ++i ){
                        switch( cv.dtDeltaScale ) {
                            case KDChartAxisParams::ValueScaleSecond:
                                dtAddSecs( dt, 1 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleMinute:
                                dtAddSecs( dt, 60 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleHour:
                                dtAddSecs( dt, 3600 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleDay:
                                dtAddDays( dt, 1 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleWeek:
                                dtAddDays( dt, 7 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleMonth:
                                dtAddMonths( dt,1 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleQuarter:
                                dtAddMonths( dt,3 * mult, newDt );
                                break;
                            case KDChartAxisParams::ValueScaleYear:
                                dtAddYears( dt, 1 * mult, newDt );
                                break;
                            default:
                                dtAddDays( dt, 1 * mult, newDt );
                                break;
                        }
                        const QDateTime& testDt
                            = dtGoDown
                            ? (   ( newDt < cv.dtLow )
                                    ? cv.dtLow
                                    : newDt )
                            : (   ( newDt > cv.dtHigh )
                                    ? cv.dtHigh
                                    : newDt );
                        /*
                           qDebug("    dt: %i %i %i    %i:%i:%i",
                           newDt.date().year(),newDt.date().month(),newDt.date().day(),
                           newDt.time().hour(),newDt.time().minute(),newDt.time().second());
                           qDebug("testDt: %i %i %i    %i:%i:%i",
                           testDt.date().year(),testDt.date().month(),testDt.date().day(),
                           testDt.time().hour(),testDt.time().minute(),testDt.time().second());
                           */
                        bool endLoop = (i == nLabels) || (&testDt != &newDt);

                        secPaint = ( KDChartAxisParams::ValueScaleSecond >= cv.dtDeltaScale ) &&
                            ( testDt.time().second() != dt.time().second() ||
                              ( endLoop && ((pSecX != x1) || (pSecY != y1))));
                        minPaint = ( KDChartAxisParams::ValueScaleMinute >= cv.dtDeltaScale ) &&
                            ( testDt.time().minute() != dt.time().minute() ||
                              ( endLoop && ((pMinX != x1) || (pMinY != y1))));
                        hourPaint = ( KDChartAxisParams::ValueScaleHour >= cv.dtDeltaScale ) &&
                            ( testDt.time().hour() != dt.time().hour() ||
                              ( endLoop && ((pHourX != x1) || (pHourY != y1))));
                        dayPaint = ( KDChartAxisParams::ValueScaleDay >= cv.dtDeltaScale ) &&
                            ( testDt.date().day() != dt.date().day() ||
                              ( endLoop && ((pDayX != x1) || (pDayY != y1))));
                        /* khz: currently not used
                           weekPaint = ( KDChartAxisParams::ValueScaleWeek >= cv.dtDeltaScale ) &&
                           ( testDt.date().week() != dt.date().week() ||
                           ( endLoop && ((pWeekX != x1) || (pWeekY != y1))));
                           */
                        monthPaint = ( KDChartAxisParams::ValueScaleMonth >= cv.dtDeltaScale ) &&
                            ( testDt.date().month() != dt.date().month() ||
                              ( endLoop && ((pMonthX != x1) || (pMonthY != y1))));
                        yearPaint = ( KDChartAxisParams::ValueScaleYear >= cv.dtDeltaScale ) &&
                            ( testDt.date().year() != dt.date().year() ||
                              ( endLoop && ((pYearX != x1) || (pYearY != y1))));

                        p1X = x1 + iLabel * pXDelta;
                        p1Y = y1 + iLabel * pYDelta;
                        p2X = p1X + pXDelta;
                        p2Y = p1Y + pYDelta;
                        pXD = orgXD;
                        pYD = orgYD;

                        if( endLoop ){
                            ( ( KDChartAxisParams& ) para ).setAxisDtHighPos( p1X, p1Y );
                            // adjust stored dt-high settings
                            ( ( KDChartAxisParams& ) para ).setTrueAxisDtHigh( dt );
                        }
                        pen = subGridPen;
                        /*
                        // old code: just draw the seconds without any tests
                        // (not wise to do that when supporting sec1000
                        //  and the like some day...)
                        if( newDt.time().second() != dt.time().second() ){
                        painter->drawLine( QPoint( p1X, p1Y ), QPoint( p1X+pXD, p1Y+pYD ) );
                        painter->drawLine( QPoint( p1X+pXD, p1Y+pYD ),
                        QPoint( p1X+pXD + pXDelta, p1Y+pYD + pYDelta ) );
                        painter->drawText( p1X+pXD-orgXD, p1Y+pYD-orgYD,
                        pTextsW, pTextsH,
                        textAlign | Qt::DontClip,
                        QString::number( dt.time().second() ) );
                        pXD += orgXD;
                        pYD += orgYD;
                        }
                        */
                        if( secPaint ){
                            painter->drawLine( QPoint( static_cast<int>( pSecX+pXD ),
                                                       static_cast<int>( pSecY+pYD ) ),
                                               QPoint( static_cast<int>( p2X + pXD ),
                                                       static_cast<int>( p2Y + pYD ) ) );
                            if( (pXDelta/2.0 < p2X - pSecX) || (pYDelta/2.0 < p2Y - pSecY) ){
                                QPen oldPen( painter->pen() );
                                painter->setPen( QPen( labelsColor ) );
                                painter->drawText( static_cast<int>( pSecX+pXD-orgXD ),
                                                   static_cast<int>( pSecY+pYD-orgYD ),
                                                   static_cast<int>( fabs((0.0 == pXDelta) ? cv.pTextsW : (p2X - pSecX))),
                                                   static_cast<int>( fabs((0.0 == pYDelta) ? cv.pTextsH : (p2Y - pSecY))),
                                        cv.textAlign | Qt::DontClip,
                                        QString::number( dt.time().second() ) );
                                painter->setPen( oldPen );
                                if ( para.axisShowGrid() ){

                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pSecX ),
                                                          static_cast<int>( pSecY ) ),
                                                  QPoint( static_cast<int>( pSecX + gridDX ),
                                                          static_cast<int>( pSecY + gridDY ) ),
                                            pen );
                                    pen = gridPen;
                                }
                                if( !minPaint || pMinX != pSecX || pMinY != pSecY ){
                                    painter->drawLine( QPoint( static_cast<int>( pSecX ),
                                                               static_cast<int>( pSecY ) ),
                                                       QPoint( static_cast<int>( pSecX+pXD ),
                                                               static_cast<int>( pSecY+pYD ) ) );
                                }
                            }
                            if( endLoop && !minPaint )
                                painter->drawLine( QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ),
                                                   QPoint( static_cast<int>( p2X+pXD ),
                                                           static_cast<int>( p2Y+pYD ) ) );
                            pSecX = p1X + pXDelta;
                            pSecY = p1Y + pYDelta;
                            pXD += orgXD;
                            pYD += orgYD;
                        }
                        if( minPaint ){
                            painter->drawLine( QPoint( static_cast<int>( pMinX+pXD ),
                                                       static_cast<int>( pMinY+pYD ) ),
                                               QPoint( static_cast<int>( p2X + pXD ),
                                                       static_cast<int>( p2Y + pYD ) ) );
                            if( (pXDelta/2.0 < p2X - pMinX) || (pYDelta/2.0 < p2Y - pMinY) ){
                                QPen oldPen( painter->pen() );
                                painter->setPen( QPen( labelsColor ) );
                                painter->drawText( static_cast<int>( pMinX+pXD-orgXD ),
                                                   static_cast<int>( pMinY+pYD-orgYD ),
                                                   static_cast<int>( fabs((0.0 == pXDelta) ? cv.pTextsW : (p2X - pMinX)) ),
                                                   static_cast<int>( fabs((0.0 == pYDelta) ? cv.pTextsH : (p2Y - pMinY)) ),
                                                   cv.textAlign | Qt::DontClip,
                                                   QString::number( dt.time().minute() ) );
                                painter->setPen( oldPen );
                                if ( para.axisShowGrid() ){
                                    if( !secPaint && 10 < pXYDelta  ){
                                        saveDrawLine( *painter,
                                                      QPoint( static_cast<int>( pMinX+pXDelta/2 ),
                                                              static_cast<int>( pMinY+pYDelta/2 ) ),
                                                      QPoint( static_cast<int>( pMinX+pXDelta/2 + gridDX ),
                                                              static_cast<int>( pMinY+pYDelta/2 + gridDY ) ),
                                                      subSubGridPen );
                                    }
                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pMinX ),
                                                          static_cast<int>( pMinY ) ),
                                                  QPoint( static_cast<int>( pMinX + gridDX ),
                                                          static_cast<int>( pMinY + gridDY ) ),
                                                  pen );
                                    pen = gridPen;
                                }
                                if( !hourPaint || pHourX != pMinX || pHourY != pMinY ){
                                    painter->drawLine( QPoint( static_cast<int>( pMinX ),
                                                               static_cast<int>( pMinY ) ),
                                                       QPoint( static_cast<int>( pMinX+pXD ),
                                                               static_cast<int>( pMinY+pYD ) ) );
                                }
                            }
                            if( endLoop && !hourPaint )
                                painter->drawLine( QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ),
                                                   QPoint( static_cast<int>( p2X+pXD ),
                                                           static_cast<int>( p2Y+pYD ) ) );
                            pMinX = p1X + pXDelta;
                            pMinY = p1Y + pYDelta;
                            pXD += orgXD;
                            pYD += orgYD;
                        }
                        if( hourPaint ){
                            painter->drawLine( QPoint( static_cast<int>( pHourX+pXD ),
                                                       static_cast<int>( pHourY+pYD ) ),
                                               QPoint( static_cast<int>( p2X + pXD ),
                                                       static_cast<int>( p2Y + pYD ) ) );
                            /*
                               qDebug("line");
                               qDebug("pXDelta / 2.0 : %f", pXDelta/2.0);
                               qDebug("p2X - pHourX  : %f", p2X - pHourX);
                               */
                            if( (pXDelta/2.0 < p2X - pHourX) || (pYDelta/2.0 < p2Y - pHourY) ){
                                /*
                                   qDebug("pHourX              %f", pHourX          );
                                   qDebug("      +pXD          %i",        pXD      );
                                   qDebug("          -orgXD    %i",            orgXD);
                                   qDebug("pHourY              %f", pHourY          );
                                   qDebug("      +pYD          %i",        pYD      );
                                   qDebug("          -orgYD    %i",            orgYD);
                                   */
                                QPen oldPen( painter->pen() );
                                painter->setPen( QPen( labelsColor ) );
                                painter->drawText( static_cast<int>( pHourX+pXD-orgXD ),
                                                   static_cast<int>( pHourY+pYD-orgYD ),
                                                   static_cast<int>( fabs((0.0 == pXDelta) ? cv.pTextsW : (p2X - pHourX))),
                                                   static_cast<int>( fabs((0.0 == pYDelta) ? cv.pTextsH : (p2Y - pHourY))),
                                                   cv.textAlign | Qt::DontClip,
                                                   QString::number( dt.time().hour() ) );
                                painter->setPen( oldPen );
                                if ( para.axisShowGrid() ){
                                    if( !minPaint && 10 < pXYDelta  ){
                                        saveDrawLine( *painter,
                                                      QPoint( static_cast<int>( pHourX+pXDelta/2 ),
                                                              static_cast<int>( pHourY+pYDelta/2 ) ),
                                                      QPoint( static_cast<int>( pHourX+pXDelta/2 + gridDX ),
                                                              static_cast<int>( pHourY+pYDelta/2 + gridDY ) ),
                                                      subSubGridPen );
                                    }
                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pHourX ),
                                                          static_cast<int>( pHourY ) ),
                                                  QPoint( static_cast<int>( pHourX + gridDX ),
                                                          static_cast<int>( pHourY + gridDY ) ),
                                                  pen );
                                    pen = gridPen;
                                }
                                if( !dayPaint || pDayX != pHourX || pDayY != pHourY ){
                                    painter->drawLine( QPoint( static_cast<int>( pHourX ),
                                                               static_cast<int>( pHourY ) ),
                                                       QPoint( static_cast<int>( pHourX+pXD ),
                                                               static_cast<int>( pHourY+pYD ) ) );
                                }
                            }
                            if( endLoop && !dayPaint )
                                painter->drawLine( QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ),
                                                   QPoint( static_cast<int>( p2X+pXD ),
                                                           static_cast<int>( p2Y+pYD ) ) );
                            pHourX = p1X + pXDelta;
                            pHourY = p1Y + pYDelta;
                            pXD += orgXD;
                            pYD += orgYD;
                        }
                        if( dayPaint ){
                            painter->drawLine( QPoint( static_cast<int>( pDayX+pXD ),
                                                       static_cast<int>( pDayY+pYD ) ),
                                               QPoint( static_cast<int>( p2X + pXD ),
                                                       static_cast<int>( p2Y + pYD ) ) );
                            if( (pXDelta/2.0 < p2X - pDayX) || (pYDelta/2.0 < p2Y - pDayY) ){
                                QPen oldPen( painter->pen() );
                                painter->setPen( QPen( labelsColor ) );
                                painter->drawText( static_cast<int>( pDayX+pXD-orgXD ),
                                                   static_cast<int>( pDayY+pYD-orgYD ),
                                                   static_cast<int>( fabs((0.0 == pXDelta) ? cv.pTextsW : (p2X - pDayX)) ),
                                                   static_cast<int>( fabs((0.0 == pYDelta) ? cv.pTextsH : (p2Y - pDayY)) ),
                                        cv.textAlign | Qt::DontClip,
                                        QString::number( dt.date().day() ) );
                                painter->setPen( oldPen );
                                /* khz: currently not used
                                   if( !weekPaint || pWeekX != pDayX || pWeekY != pDayY )
                                   */
                                if ( para.axisShowGrid() ){
                                    if( !hourPaint && 10 < pXYDelta  ){
                                        saveDrawLine( *painter,
                                                      QPoint( static_cast<int>( pDayX+pXDelta/2 ),
                                                              static_cast<int>( pDayY+pYDelta/2 ) ),
                                                      QPoint( static_cast<int>( pDayX+pXDelta/2 + gridDX ),
                                                              static_cast<int>( pDayY+pYDelta/2 + gridDY ) ),
                                                subSubGridPen );
                                    }
                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pDayX ),
                                                          static_cast<int>( pDayY ) ),
                                                  QPoint( static_cast<int>( pDayX + gridDX ),
                                                          static_cast<int>( pDayY + gridDY ) ),
                                            pen );
                                    pen = gridPen;
                                }
                                if( !monthPaint || pMonthX != pDayX || pMonthY != pDayY ){
                                    painter->drawLine( QPoint( static_cast<int>( pDayX ),
                                                               static_cast<int>( pDayY ) ),
                                                       QPoint( static_cast<int>( pDayX+pXD ),
                                                               static_cast<int>( pDayY+pYD ) ) );
                                }
                            }
                            /* khz: currently not used
                               if( endLoop && !weekPaint )
                               */
                            if( endLoop && !monthPaint )
                                painter->drawLine( QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ),
                                                   QPoint( static_cast<int>( p2X+pXD ),
                                                           static_cast<int>( p2Y+pYD ) ) );
                            pDayX = p1X + pXDelta;
                            pDayY = p1Y + pYDelta;
                            pXD += orgXD;
                            pYD += orgYD;
                        }
                        /* khz: currently unused
                           if( weekPaint ){
                           painter->drawLine( QPoint( pWeekX+pXD, pWeekY+pYD ),
                            QPoint( p2X + pXD, p2Y + pYD ) );
                           if( (pXDelta/2.0 < p2X - pWeekX) || (pYDelta/2.0 < p2Y - pWeekY) ){
                           QPen oldPen( painter->pen() );
                           painter->setPen( QPen( labelsColor ) );
                           painter->drawText( pWeekX+pXD-orgXD, pWeekY+pYD-orgYD,
                           painter->setPen( oldPen );
                           fabs((0.0 == pXDelta) ? pTextsW : (p2X - pWeekX)),
                           fabs((0.0 == pYDelta) ? pTextsH : (p2Y - pWeekY)),
                           textAlign | Qt::DontClip,
                           QString::number( dt.date().week() ) );
                           if ( para.axisShowGrid() ){
                           if( !dayPaint && 40 < pXYDelta  ){
                        // draw 7 lines:
                        //saveDrawLine( *painter,
                        //            QPoint( pWeekX+pXDelta/2,
                        //                    pWeekY+pYDelta/2 ),
                        //            QPoint( pWeekX+pXDelta/2 + gridDX,
                        //                    pWeekY+pYDelta/2 + gridDY ),
                        //            subSubGridPen );
                        }
                        saveDrawLine( *painter,
                        QPoint( pWeekX,
                        pWeekY ),
                        QPoint( pWeekX + gridDX,
                        pWeekY + gridDY ),
                        pen );
                        pen = gridPen;
                        }
                        if( !monthPaint || pMonthX != pDayX || pMonthY != pDayY ){
                        painter->drawLine( QPoint( pWeekX, pWeekY ), QPoint( pWeekX+pXD, pWeekY+pYD ) );
                        }
                        }
                        if( endLoop && !monthPaint )
                        painter->drawLine( QPoint( p2X, p2Y ), QPoint( p2X+pXD, p2Y+pYD ) );
                        pWeekX = p1X + pXDelta;
                        pWeekY = p1Y + pYDelta;
                        pXD += orgXD;
                        pYD += orgYD;
                        }
                        */
                        if( monthPaint ){
                            painter->drawLine( QPoint( static_cast<int>( pMonthX+pXD ),
                                                       static_cast<int>( pMonthY+pYD ) ),
                                               QPoint( static_cast<int>( p2X + pXD ),
                                                       static_cast<int>( p2Y + pYD ) ) );
                            if( (pXDelta/2.0 < p2X - pMonthX) || (pYDelta/2.0 < p2Y - pMonthY) ){
                                QPen oldPen( painter->pen() );
                                painter->setPen( QPen( labelsColor ) );
                                painter->drawText( static_cast<int>( pMonthX+pXD-orgXD ),
                                                   static_cast<int>( pMonthY+pYD-orgYD ),
                                                   static_cast<int>( fabs((0.0 == pXDelta) ? cv.pTextsW : (p2X - pMonthX)) ),
                                                   static_cast<int>( fabs((0.0 == pYDelta) ? cv.pTextsH : (p2Y - pMonthY)) ),
                                        cv.textAlign | Qt::DontClip,
                                        QString::number( dt.date().month() ) );
                                painter->setPen( oldPen );
                                if ( para.axisShowGrid() ){
                                    /* khz: currently unused
                                       if( !weekPaint &&
                                       && 10 < pXYDelta  ){
                                       saveDrawLine( *painter,
                                       QPoint( pMonthX+pXDelta/2,
                                       pMonthY+pYDelta/2 ),
                                       QPoint( pMonthX+pXDelta/2 + gridDX,
                                       pMonthY+pYDelta/2 + gridDY ),
                                       subSubGridPen );
                                       }
                                       */
                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pMonthX ),
                                                          static_cast<int>( pMonthY ) ),
                                                  QPoint( static_cast<int>( pMonthX + gridDX ),
                                                          static_cast<int>( pMonthY + gridDY ) ),
                                            pen );
                                    pen = gridPen;
                                }
                                if( !yearPaint || pYearX != pMonthX || pYearY != pMonthY ){
                                    painter->drawLine( QPoint( static_cast<int>( pMonthX ),
                                                               static_cast<int>( pMonthY ) ),
                                                       QPoint( static_cast<int>( pMonthX+pXD ),
                                                               static_cast<int>( pMonthY+pYD ) ) );
                                }
                            }
                            if( endLoop && !yearPaint )
                                painter->drawLine( QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ),
                                                   QPoint( static_cast<int>( p2X+pXD ),
                                                           static_cast<int>( p2Y+pYD ) ) );
                            pMonthX = p1X + pXDelta;
                            pMonthY = p1Y + pYDelta;
                            pXD += orgXD;
                            pYD += orgYD;
                        }
                        if( yearPaint ){
                            painter->drawLine( QPoint( static_cast<int>( pYearX+pXD ),
                                                       static_cast<int>( pYearY+pYD  ) ),
                                               QPoint( static_cast<int>( p2X + pXD ),
                                                       static_cast<int>( p2Y + pYD ) ) );
                            if( (pXDelta/2.0 < p2X - pYearX) || (pYDelta/2.0 < p2Y - pYearY) ){
                                QPen oldPen( painter->pen() );
                                painter->setPen( QPen( labelsColor ) );
                                painter->drawText( static_cast<int>( pYearX+pXD-orgXD ),
                                                   static_cast<int>( pYearY+pYD-orgYD ),
                                                   static_cast<int>( fabs((0.0 == pXDelta) ? cv.pTextsW : (p2X - pYearX)) ),
                                                   static_cast<int>( fabs((0.0 == pYDelta) ? cv.pTextsH : (p2Y - pYearY)) ),
                                        cv.textAlign | Qt::DontClip,
                                        QString::number( dt.date().year() ) );
                                painter->setPen( oldPen );
                                if ( para.axisShowGrid() ){
                                    if( !monthPaint && 10 < pXYDelta  ){
                                        saveDrawLine( *painter,
                                                      QPoint( static_cast<int>( pYearX+pXDelta/2 ),
                                                              static_cast<int>( pYearY+pYDelta/2 ) ),
                                                      QPoint( static_cast<int>( pYearX+pXDelta/2 + gridDX ),
                                                              static_cast<int>( pYearY+pYDelta/2 + gridDY ) ),
                                                subSubGridPen );
                                    }
                                    saveDrawLine( *painter,
                                                  QPoint( static_cast<int>( pYearX ),
                                                          static_cast<int>( pYearY ) ),
                                                  QPoint( static_cast<int>( pYearX + gridDX ),
                                                          static_cast<int>( pYearY + gridDY ) ),
                                            pen );
                                    pen = gridPen;
                                }
                                painter->drawLine( QPoint( static_cast<int>( pYearX ),
                                                           static_cast<int>( pYearY ) ),
                                                   QPoint( static_cast<int>( pYearX+pXD ),
                                                           static_cast<int>( pYearY+pYD ) ) );
                            }
                            if( endLoop )
                                painter->drawLine( QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ),
                                                   QPoint( static_cast<int>( p2X+pXD ),
                                                           static_cast<int>( p2Y+pYD ) ) );
                            pYearX = p1X + pXDelta;
                            pYearY = p1Y + pYDelta;
                            pXD += orgXD;
                            pYD += orgYD;
                        }
                        if( &testDt != &newDt )
                            break;
                        dt = newDt;
                        iLabel += 1.0;
                    }
                    if( !commonDtHeader.isEmpty() ){
                        QPen oldPen( painter->pen() );
                        painter->setPen( QPen( labelsColor ) );
                        painter->drawText( static_cast<int>( x1 + pXD ), static_cast<int>( y1 + pYD ),
                                           commonDtHeader );
                        painter->setPen( oldPen );
                    }
                }else{
                    int iLeaveOut = nLeaveOut;
                    QString label;
                    for ( QStringList::Iterator labelIter = labelTexts->begin();
                          labelIter != labelTexts->end();
                          ++labelIter ) {
                        QDateTime dt;
                        if( cv.isDateTime ){
#if COMPAT_QT_VERSION >= 0x030000
                            dt = QDateTime::fromString( *labelIter,
                                    Qt::ISODate );
                            label = dt.toString( formatDT );
#else
                            dt = dateTimeFromString( *labelIter );
                            label = dt.toString();
#endif
                        }else{
                            label = *labelIter;
                        }

                        if( iLeaveOut < nLeaveOut )
                            ++iLeaveOut;
                        else 
                            iLeaveOut = 0;
                        //Pending Michel: test if the user implicitely wants to get rid 
                        //of the non fractional values delimiters and grid lines.
                        // axisDigitsBehindComma == 0 and the user implicitely  
                        // setAxisShowFractionalValuesDelimiters() to false
                        bool showDelim =  para.axisShowFractionalValuesDelimiters();     
                        if ( para.axisShowGrid() && !bMultiRowBarChart ) {
			  if ( !label.isNull() || showDelim ){
                            if( !iLeaveOut )
                                // draw the main grid line
			      
                                saveDrawLine( *painter,
                                              QPoint( static_cast<int>( pGAX - pGXMicroAdjust ),
                                                      static_cast<int>( pGAY - pGYMicroAdjust ) ),
                                              QPoint( static_cast<int>( pGZX - pGXMicroAdjust ),
                                                      static_cast<int>( pGZY - pGYMicroAdjust ) ),
                                              gridPen );
			      
                            else if( para.axisShowSubDelimiters()  )
                                // draw a thin sub grid line instead of main line
                                saveDrawLine( *painter,
                                              QPoint( static_cast<int>( pGAX - pGXMicroAdjust ),
                                                      static_cast<int>( pGAY - pGYMicroAdjust ) ),
                                              QPoint( static_cast<int>( pGZX - pGXMicroAdjust ),
                                                      static_cast<int>( pGZY - pGYMicroAdjust ) ),
                                              leaveOutGridPen );
			  }
                        }
                        if ( para.axisLabelsVisible() ) {
                            if( !iLeaveOut ) {			     
                              /*PENDING Michel: those points should not be redrawn if sub-delimiters are drawn 
			       *drawing the submarkers	
			       * make it visible or not		
			       *In the case we have a null label - axisDigitsBehindComma is implicitely set to 0 -
			       *also paint or dont paint the delimiter corresponding to this label - default is paint. 
			       */                             
			      if ( !label.isNull() || showDelim )
                                painter->drawLine( QPoint( static_cast<int>( p1X ),
                                                           static_cast<int>( p1Y ) ),
                                                   QPoint( static_cast<int>( p2X ),
                                                           static_cast<int>( p2Y ) ) );			    
  
			      cv.pLastX = p1X;
			      cv.pLastY = p1Y;
			      QPen oldPen( painter->pen() );
			      painter->setPen( QPen( labelsColor ) );
			      if(    para.axisLabelsDontShrinkFont()
				     && isHorizontalAxis
				     && (Qt::AlignHCenter == (cv.textAlign & Qt::AlignHCenter)) ) {
				double w = fm.width( label ) + 4.0;
				double x0 = cv.pTextsX + cv.pTextsW / 2.0;
				
				painter->drawText( static_cast<int>( x0 - w / 2.0 ),
						   static_cast<int>( cv.pTextsY ),
						   static_cast<int>( w ),
						   static_cast<int>( cv.pTextsH ),
						   cv.textAlign, label );
			      } else {                                
				if( nRotation ){
				  KDDrawText::drawRotatedText(
						painter,
						nRotation,
						painter->worldMatrix().map(
                                                QPoint( static_cast<int>( cv.pTextsX ),
                                                        static_cast<int>( cv.pTextsY ) ) ),
                                                label,
                                                0,
                                                cv.textAlign,
                                                false,
                                                &fm,
                                                screenOutput,screenOutput,0,
                                                screenOutput );
                                    } else {
				      // Pending Michel draw the axis labels
                                        painter->drawText( static_cast<int>( cv.pTextsX ),
                                                           static_cast<int>( cv.pTextsY ),
                                                           static_cast<int>( cv.pTextsW ),
                                                           static_cast<int>( cv.pTextsH ),
                                                           cv.textAlign | Qt::DontClip,
                                                           label );
				      
                                        // debugging text rect
                                        /*
                                        painter->drawRect(static_cast <int>(cv.pTextsX),
							  static_cast <int>(cv.pTextsY),
							  static_cast <int> (nUsableAxisWidth),
							  static_cast <int> (nUsableAxisHeight));
					*/
                                    }
                                }
                                painter->setPen( oldPen );
                            }
                        }
                       

                        if( cv.isDateTime ){
                            if( labelTexts->begin() == labelIter ){
                                ((KDChartAxisParams&)para).setAxisDtLowPos(
                                                                           pGAX - pGXMicroAdjust,
                                                                           pGAY - pGYMicroAdjust );
                                // adjust stored dt-low settings
                                ( ( KDChartAxisParams& ) para ).setTrueAxisDtLow( dt );
                            }else{
                                ((KDChartAxisParams&)para).setAxisDtHighPos(
                                                                            pGAX - pGXMicroAdjust,
                                                                            pGAY - pGYMicroAdjust );
                                // adjust stored dt-high settings
                                ( ( KDChartAxisParams& ) para ).setTrueAxisDtHigh( dt );
                            }
                        }

                        iLabel += 1.0;
                        p1X = x1 + iLabel * pXDelta;
                        p1Y = y1 + iLabel * pYDelta;
                        p2X = x2 + iLabel * pXDelta;
                        p2Y = y2 + iLabel * pYDelta;
                        cv.pTextsX = xT + iLabel * pXDelta;
                        cv.pTextsY = yT + iLabel * pYDelta;

                        pGAX = xGA + iLabel * pXDelta;
                        pGAY = yGA + iLabel * pYDelta;
                        pGZX = xGZ + iLabel * pXDelta;
                        pGZY = yGZ + iLabel * pYDelta;
                        /*
                        pGAX = xGA + iLabel * pXSubDelimDelta / cv.nSubDelimFactor;
                        pGAY = yGA + iLabel * pYSubDelimDelta / cv.nSubDelimFactor;
                        pGZX = xGZ + iLabel * pXSubDelimDelta / cv.nSubDelimFactor;
                        pGZY = yGZ + iLabel * pYSubDelimDelta / cv.nSubDelimFactor;
                        */
                    }
                }


                // adjust zero-line start, if not starting at origin
                if ( cv.bSteadyCalc &&
                     ( para.axisValuesDecreasing() ||
                       (0.0 != para.trueAxisLow())      ) ) {
                    double x = p1.x();
                    double y = p1.y();
                    double mult = para.trueAxisLow() / para.trueAxisDelta();
                    x -= mult * pXDelta;
                    y -= mult * pYDelta;
                    axisZeroLineStartX = x;
                    axisZeroLineStartY = y;
                    //qDebug( "axisZeroLineStartX %f,  axisZeroLineStartY %f",
                    //        axisZeroLineStartX, axisZeroLineStartY );
                }

                painter->setClipping( oldClippingFlag );
            } // if( nLabels )

            // draw zero-line (Ok, this might be overwritten by axes
            //  cause those are drawn after all labels and grid and
            //  zero-line(s) has been painted, see code below, starting
            // with "// draw all the axes".
            if ( cv.bSteadyCalc && !cv.isDateTime ) {
                ( ( KDChartAxisParams& ) para ).setAxisZeroLineStart( axisZeroLineStartX, axisZeroLineStartY );
                double axisZeroLineStart;
                int minCoord, maxCoord;
                double xFactor, yFactor;
                switch( cv.basicPos ){
                    case KDChartAxisParams::AxisPosLeft:
                        xFactor =  1.0;
                        yFactor =  0.0;
                        axisZeroLineStart = axisZeroLineStartY;
                        minCoord = QMIN( cv.orig.y(), cv.dest.y() );
                        maxCoord = QMAX( cv.orig.y(), cv.dest.y() );

                        break;
                    case KDChartAxisParams::AxisPosRight:
                        xFactor = -1.0;
                        yFactor =  0.0;
                        axisZeroLineStart = axisZeroLineStartY;
                        minCoord = QMIN( cv.orig.y(), cv.dest.y() );
                        maxCoord = QMAX( cv.orig.y(), cv.dest.y() );
                        break;
                    case KDChartAxisParams::AxisPosTop:
                        xFactor =  0.0;
                        yFactor =  1.0;
                        axisZeroLineStart = axisZeroLineStartX;
                        minCoord = QMIN( cv.orig.x(), cv.dest.x() );
                        maxCoord = QMAX( cv.orig.x(), cv.dest.x() );
                        break;
                    case KDChartAxisParams::AxisPosBottom:
                        xFactor =  0.0;
                        yFactor = -1.0;
                        axisZeroLineStart = axisZeroLineStartX;
                        minCoord = QMIN( cv.orig.x(), cv.dest.x() );
                        maxCoord = QMAX( cv.orig.x(), cv.dest.x() );
                        break;
                    default:
                        xFactor =  0.0;
                        yFactor =  0.0;
                        axisZeroLineStart = 0.0;
                        minCoord = 0;
                        maxCoord = 0;
                }
                if( axisZeroLineStart >= minCoord &&
                        axisZeroLineStart <= maxCoord ){
                    QPoint pZ0( static_cast<int>( para.axisZeroLineStartX() ),
                                static_cast<int>( para.axisZeroLineStartY() ) );
                    QPoint pZ(  static_cast<int>( para.axisZeroLineStartX()
                                + xFactor * _dataRect.width() ),
                                static_cast<int>( para.axisZeroLineStartY()
                                + yFactor * _dataRect.height() ) );
                    //qDebug("------");
                    saveDrawLine( *painter,
                            pZ0,
                            pZ,
                            QPen( para.axisZeroLineColor(),
                                lineWidth ) );
                }
            }
	    
        }
 
    }

    // Drawing all the axes lines:
/*
    // 1. test if the standard axes are share one or several corner points
    //    if yes, we first draw a polyline using a "Qt::MiterJoin" PenJoinStyle
    //    to make sure the corners are filled
    internal__KDChart__CalcValues& cv1 = calcVal[ KDChartAxisParams::AxisPosLeft   ];
    internal__KDChart__CalcValues& cv2 = calcVal[ KDChartAxisParams::AxisPosBottom ];
    const KDChartAxisParams& pa1 = params()->axisParams( KDChartAxisParams::AxisPosLeft );
    const KDChartAxisParams& pa2 = params()->axisParams( KDChartAxisParams::AxisPosBottom );
qDebug("\n\nx1: %i   y1: %i   w1: %i\nx2: %i   y2: %i   w2: %i",
cv1.orig.x(), cv1.orig.y(), pa1.axisTrueLineWidth(),
cv2.orig.x(), cv2.orig.y(), pa2.axisTrueLineWidth() );
    if( cv1.orig == cv2.orig ){
        const QColor c1( pa1.axisLineColor() );
        const QColor c2( pa2.axisLineColor() );
        const QPoint pA( cv1.dest );
        const QPoint pB( cv1.orig );
        const QPoint pC( cv2.dest );
        QPen pen( QColor( (c1.red()   + c2.red())  /2,
                          (c1.green() + c2.green())/2,
                          (c1.blue()  + c2.blue()) /2 ),
                  QMIN(pa1.axisTrueLineWidth(), pa2.axisTrueLineWidth()) );
        pen.setJoinStyle( Qt::MiterJoin );
        painter->setPen( pen );
        QPointArray a;
        a.putPoints( 0, 3, pA.x(),pA.y(), pB.x(),pB.y(), pC.x(),pC.y() );
        painter->drawPolyline( a );
qDebug("done\n" );
    }
*/
    // 2. draw the axes using their normal color
    for( iAxis = 0; iAxis < KDCHART_MAX_AXES; ++iAxis ){
        internal__KDChart__CalcValues& cv = calcVal[iAxis];
        const KDChartAxisParams & para = params()->axisParams( iAxis );
        if( cv.processThisAxis && para.axisLineVisible() ){
            painter->setPen( QPen( para.axisLineColor(),
                             para.axisTrueLineWidth() ) );
            int x =         cv.dest.x();
            if( 2.0 >= QABS(cv.pLastX - x) )
                x = static_cast < int > ( cv.pLastX );
            int y =         cv.dest.y();
            if( 2.0 >= QABS(cv.pLastY - y) )
                y = static_cast < int > ( cv.pLastY );
            painter->drawLine( cv.orig, QPoint(x,y) );
        }
    }
   
    painter->restore(); 
}


double fastPow10( int x )
{
    double res = 1.0;
    if( 0 <= x ){
        for( int i = 1; i <= x; ++i )
            res *= 10.0;
    }else{
        for( int i = -1; i >= x; --i )
            res /= 10.0;
    }
    return res;
}
double fastPow10( double x )
{
    return pow(10.0, x);
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
/**** static ****/
void KDChartAxesPainter::calculateLabelTexts(
        QPainter* painter,
        const KDChartTableDataBase& data,
        const KDChartParams& params,
        uint axisNumber,
        double averageValueP1000,
        double delimLen,
        // start of return parameters
        KDChartAxisParams::AxisPos& basicPos,
        QPoint& orig,
        QPoint& dest,
        double& pXDeltaFactor,
        double& pYDeltaFactor,
        double& pXDelimDeltaFaktor,
        double& pYDelimDeltaFaktor,
        double& nSubDelimFactor,
        double& pDelimDelta,
        double& nTxtHeight,
        double& pTextsX,
        double& pTextsY,
        double& pTextsW,
        double& pTextsH,
        int& textAlign,
        bool& isLogarithmic,
        bool& isDateTime,
        bool& autoDtLabels,
        QDateTime& dtLow,
        QDateTime& dtHigh,
        KDChartAxisParams::ValueScale& dtDeltaScale,
        bool adjustTheValues,
        double trueDelta,
        double trueDeltaPix )
{
//qDebug("\nentering KDChartAxesPainter::calculateLabelTexts() :   nTxtHeight: "+QString::number(nTxtHeight));
    const KDChartAxisParams & para = params.axisParams( axisNumber );

    // store whether the labels are to be drawn in reverted order
    const bool bDecreasing = para.axisValuesDecreasing();

    basicPos = KDChartAxisParams::basicAxisPos( axisNumber );

    pXDeltaFactor = 0.0;
    pYDeltaFactor = 0.0;
    pXDelimDeltaFaktor = 0.0;
    pYDelimDeltaFaktor = 0.0;
    int axisLength;
    switch ( basicPos ) {
        case KDChartAxisParams::AxisPosBottom: {
            axisLength = para.axisTrueAreaRect().width();
            orig = bDecreasing
                 ? para.axisTrueAreaRect().topRight()
                 : para.axisTrueAreaRect().topLeft();
            dest = bDecreasing
                 ? para.axisTrueAreaRect().topLeft()
                 : para.axisTrueAreaRect().topRight();
            pYDelimDeltaFaktor = 1.0;
            pXDeltaFactor      = bDecreasing ? -1.0 : 1.0;
            //qDebug("\nsetting pXDeltaFactor for axis %x", axisNumber);
            //qDebug(bDecreasing ? "bDecreasing =  TRUE" : "bDecreasing = FALSE");
            //qDebug("pXDeltaFactor = %f\n",pXDeltaFactor);
        }
        break;
        case KDChartAxisParams::AxisPosLeft: {
            axisLength = para.axisTrueAreaRect().height();
            orig = bDecreasing
                 ? para.axisTrueAreaRect().topRight()
                 : para.axisTrueAreaRect().bottomRight();
            dest = bDecreasing
                 ? para.axisTrueAreaRect().bottomRight()
                 : para.axisTrueAreaRect().topRight();
            pXDelimDeltaFaktor = -1.0;
            pYDeltaFactor      = bDecreasing ? 1.0 : -1.0;
        }
        break;
        case KDChartAxisParams::AxisPosTop: {
            axisLength = para.axisTrueAreaRect().width();
            orig = bDecreasing
                 ? para.axisTrueAreaRect().bottomRight()
                 : para.axisTrueAreaRect().bottomLeft();
            dest = bDecreasing
                 ? para.axisTrueAreaRect().bottomLeft()
                 : para.axisTrueAreaRect().bottomRight();
            pYDelimDeltaFaktor = -1.0;
            pXDeltaFactor      =  bDecreasing ? -1.0 : 1.0;
        }
        break;
        case KDChartAxisParams::AxisPosRight: {
            axisLength = para.axisTrueAreaRect().height();
            orig = bDecreasing
                 ? para.axisTrueAreaRect().topLeft()
                 : para.axisTrueAreaRect().bottomLeft();
            dest = bDecreasing
                 ? para.axisTrueAreaRect().bottomLeft()
                 : para.axisTrueAreaRect().topLeft();
            pXDelimDeltaFaktor = 1.0;
            pYDeltaFactor      = bDecreasing ? 1.0 : -1.0;
        }
        break;
        default: {
            axisLength = 0;
            qDebug( "IMPLEMENTATION ERROR: KDChartAxesPainter::paintAxes() unhandled enum value." );
        }
        break;
    }

    // which dataset(s) is/are represented by this axis?
    uint dataset, dataset2, chart;
    if ( !params.axisDatasets( axisNumber, dataset, dataset2, chart ) ) {
        dataset = KDCHART_ALL_DATASETS;
        dataset2 = KDCHART_ALL_DATASETS;
        chart = 0;
        //qDebug("\nautomatic set values:   chart: %u,\ndataset: %u,  dataset2: %u",
        //chart, dataset, dataset2);
    }
    // which dataset(s) with mode DataEntry (or mode ExtraLinesAnchor, resp.)
    // is/are represented by this axis?
    uint dataDataset, dataDataset2;
    if( params.findDatasets( KDChartParams::DataEntry,
                             KDChartParams::ExtraLinesAnchor,
                             dataDataset,
                             dataDataset2,
                             chart ) ) {
        // adjust dataDataset in case MORE THAN ONE AXIS
        //                    is representing THIS CHART
        if(    (    KDCHART_ALL_DATASETS != dataset
                 && KDCHART_NO_DATASET   != dataset )
            || (    KDCHART_ALL_DATASETS != dataDataset
                 && KDCHART_NO_DATASET   != dataDataset ) ){
            int ds = (KDCHART_ALL_DATASETS != dataset)
                   ? dataset
                   : 0;
            int dds = (KDCHART_ALL_DATASETS != dataDataset)
                    ? dataDataset
                    : 0;
            dataDataset  = QMAX( ds, dds );
        }
        if(    (    KDCHART_ALL_DATASETS != dataset2
                 && KDCHART_NO_DATASET   != dataset2 )
            || (    KDCHART_ALL_DATASETS != dataDataset2
                 && KDCHART_NO_DATASET   != dataDataset2 ) ){
            int ds2 = (KDCHART_ALL_DATASETS != dataset2)
                    ? dataset2
                    : KDCHART_MAX_AXES-1;
            int dds2 = (KDCHART_ALL_DATASETS != dataDataset2)
                     ? dataDataset2
                     : KDCHART_MAX_AXES-1;
            dataDataset2  = QMIN( ds2, dds2 );
        }
    }
    else {
        // Should not happen
        qDebug( "IMPLEMENTATION ERROR: findDatasets( DataEntry, ExtraLinesAnchor ) should *always* return true. (b)" );
        dataDataset = KDCHART_ALL_DATASETS;
    }
    //qDebug("\naxisNumber: %x\nchart: %x\ndataset: %x,  dataset2: %x,\ndataDataset: %x,  dataDataset2: %x",
    //axisNumber, chart, dataset, dataset2, dataDataset, dataDataset2);

    if ( para.axisLabelsFontUseRelSize() ){
        nTxtHeight = para.axisLabelsFontRelSize()
            * averageValueP1000;
//qDebug("using rel. size in KDChartAxesPainter::calculateLabelTexts() :   nTxtHeight: "+QString::number(nTxtHeight));
    }else {
        QFontInfo info( para.axisLabelsFont() );
        nTxtHeight = info.pointSize();
//qDebug("using FIXED size in KDChartAxesPainter::calculateLabelTexts() :   nTxtHeight: "+QString::number(nTxtHeight));
    }

    const int     behindComma    = para.axisDigitsBehindComma();
    const int     divPow10       = para.axisLabelsDivPow10();
    const QString decimalPoint   = para.axisLabelsDecimalPoint();
    const QString thousandsPoint = para.axisLabelsThousandsPoint();
    const QString prefix         = para.axisLabelsPrefix();
    const QString postfix        = para.axisLabelsPostfix();
    const int     totalLen       = para.axisLabelsTotalLen();
    const QChar   padFill        = para.axisLabelsPadFill();
    const bool    blockAlign     = para.axisLabelsBlockAlign();

    QStringList labelTexts;
    int colNum = para.labelTextsDataRow();
    bool bDone = true;
    switch ( para.axisLabelTextsFormDataRow() ) {
        case KDChartAxisParams::LabelsFromDataRowYes: {
            // Take whatever is in the specified column (even if not a string)
            int trueBehindComma = -1;
            QVariant value;
            for ( uint iDataset = 0; iDataset < data.usedRows(); iDataset++ ) {
                if( data.cellCoord( iDataset, colNum, value, 1 ) ){
                    if( QVariant::String == value.type() )
                        labelTexts.append( value.toString() );
                    else {
                        labelTexts.append( applyLabelsFormat( value.toDouble(),
                                                              divPow10,
                                                              behindComma,
                                                              para.axisValueDelta(),
                                                              trueBehindComma,
                                                              decimalPoint,
                                                              thousandsPoint,
                                                              prefix,
                                                              postfix,
                                                              totalLen,
                                                              padFill,
                                                              blockAlign ) );
			
		    }
                }
            }
            break;
        }
        case KDChartAxisParams::LabelsFromDataRowGuess: {
            QVariant value;
            for ( uint iDataset = 0; iDataset < data.usedRows(); iDataset++ ) {
                if( data.cellCoord( iDataset, colNum, value, 1 ) ){
                    if( QVariant::String == value.type() ){
                        const QString sVal( value.toString() );
                        if( !sVal.isEmpty() && !sVal.isNull() )
                            labelTexts.append( sVal );
                    }
                }else{
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

    // if necessary adjust text params *including* the steady value calc setting
    const bool dataCellsHaveSeveralCoordinates =
        (KDCHART_ALL_DATASETS == dataDataset)
        ? data.cellsHaveSeveralCoordinates()
        : data.cellsHaveSeveralCoordinates( dataDataset, dataDataset2 );
    if( dataCellsHaveSeveralCoordinates && !para.axisSteadyValueCalc() )
        ((KDChartParams&)params).setAxisLabelTextParams(
            axisNumber,
            true,
            KDCHART_AXIS_LABELS_AUTO_LIMIT,
            KDCHART_AXIS_LABELS_AUTO_LIMIT,
            KDCHART_AXIS_LABELS_AUTO_DELTA,
            para.axisLabelsDigitsBehindComma() );// NOTE: This sets MANY other params to default values too!


    const KDChartParams::ChartType params_chartType
        = ( 0 == chart )
        ? params.chartType()
        : params.additionalChartType();


    // store whether we are calculating Ordinate-like axis values
    const bool bSteadyCalc = para.axisSteadyValueCalc();

    // store whether logarithmic calculation is wanted
    isLogarithmic = bSteadyCalc &&
            (KDChartParams::Line == params_chartType) &&
            (KDChartAxisParams::AxisCalcLogarithmic == para.axisCalcMode());

    //qDebug(bSteadyCalc ? "bSteadyCalc":"NOT bSteadyCalc");
    //qDebug(isLogarithmic? "isLogarithmic":"NOT isLogarithmic");

    // store whether this is a vertical axis or a horizontal axis
    const bool bVertAxis = (KDChartAxisParams::AxisPosLeft  == basicPos) ||
                           (KDChartAxisParams::AxisPosRight == basicPos);

    // store the coordinate number to be used for this axis
    const int coordinate = bVertAxis ? 1 : 2;

    // store whether our coordinates are double or QDateTime values
    const QVariant::Type valueType =
        (KDCHART_ALL_DATASETS == dataDataset)
        ? data.cellsValueType(                            coordinate )
        : data.cellsValueType( dataDataset, dataDataset2, coordinate );
    isDateTime = valueType == QVariant::DateTime;
    bool bIsDouble = valueType == QVariant::Double;

    autoDtLabels = isDateTime && ( KDCHART_AXIS_LABELS_AUTO_DATETIME_FORMAT
            == para.axisLabelsDateTimeFormat() );

    if( autoDtLabels || bSteadyCalc )
        ( ( KDChartAxisParams& ) para ).setAxisLabelsTouchEdges( true );

    bool bStatistical = KDChartParams::HiLo       == params_chartType
                     || KDChartParams::BoxWhisker == params_chartType;

    if ( !bVertAxis && KDChartParams::BoxWhisker == params_chartType
                    && ! para.axisLabelStringCount() ) {
        uint ds1 = (KDCHART_ALL_DATASETS == dataDataset)
            ? 0
            : dataDataset;
        uint ds2 = (KDCHART_ALL_DATASETS == dataDataset)
            ? data.usedRows() - 1
            : dataDataset2;
        for (uint i = ds1; i <= ds2; ++i)
            labelTexts.append(
                    QObject::tr( "Series " ) + QString::number( i + 1 ) );
        bDone = true;
    }

    double nLow   =  1.0 + bSteadyCalc;// ? 0.0 : data.colsScrolledBy();
    double nHigh  = 10.0;
    double nDelta =  1.0;
    if ( !bDone ) {
        bDone = true;

        // look if exact label specification was made via limits and delta
        if (       ! isLogarithmic
                && ! para.axisLabelStringCount()
                && ! ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() )
                && ! ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() )
                && ! ( para.axisValueStart() == para.axisValueEnd() )
                && ! ( KDCHART_AXIS_LABELS_AUTO_DELTA == para.axisValueDelta() )
                && ! ( 0.0 == para.axisValueDelta() ) ) {
            nLow   = para.axisValueStart().toDouble();
            nHigh  = para.axisValueEnd().toDouble();
            nDelta = para.axisValueDelta();
            int behindComma = para.axisDigitsBehindComma();
            int trueBehindComma = -1;
            bool upwards = (nLow < nHigh);
            if( upwards != (0.0 < nDelta) )
                nDelta *= -1.0;
            double nVal = nLow;
            bDone = false;
            bool bShowVeryLastLabel = false;
            //qDebug("\n nLow: %f,  nHigh: %f,  nDelta: %f", nLow, nHigh, nDelta );
            while( bShowVeryLastLabel || (upwards ? (nVal <= nHigh) : (nVal >= nHigh)) ){
                //qDebug("nVal    : %f", nVal );
                labelTexts.append( applyLabelsFormat( nVal,
                                                      divPow10,
                                                      behindComma,
                                                      nDelta,
                                                      trueBehindComma,
                                                      decimalPoint,
                                                      thousandsPoint,
                                                      prefix,
                                                      postfix,
                                                      totalLen,
                                                      padFill,
                                                      blockAlign ) );
                nVal += nDelta;
                //qDebug("nVal-neu: %f", nVal );
                if( ! (upwards ? (nVal <= nHigh) : (nVal >= nHigh)) ){
                    // work around a rounding error: show the last label, even if not nVal == nHigh is not matching exactly
                    if( bShowVeryLastLabel )
                        bShowVeryLastLabel = false;
                    else{
                        QString sHigh(  applyLabelsFormat( nHigh,
                                                           divPow10,
                                                           behindComma,
                                                           nDelta,
                                                           trueBehindComma,
                                                           decimalPoint,
                                                           thousandsPoint,
                                                           prefix,
                                                           postfix,
                                                           totalLen,
                                                           padFill,
                                                           blockAlign ) );
                        QString sValue( applyLabelsFormat( nVal,
                                                           divPow10,
                                                           behindComma,
                                                           nDelta,
                                                           trueBehindComma,
                                                           decimalPoint,
                                                           thousandsPoint,
                                                           prefix,
                                                           postfix,
                                                           totalLen,
                                                           padFill,
                                                           blockAlign ) );
                        bShowVeryLastLabel = (sValue == sHigh);
                        //qDebug("test:                 sHigh: "+sHigh+"   sValue: "+sValue );
                    }
                }
                bDone = true;
            }
            ( ( KDChartAxisParams& ) para ).setTrueAxisLowHighDelta( nLow, nHigh, nDelta );
            //qDebug("\n[Z-0] nLow: %f,  nHigh: %f,  nDelta: %f", nLow, nHigh, nDelta );
        }         // look if a string list was specified
        else if ( para.axisLabelStringCount() ) {
            int nLabels = bSteadyCalc
                ? para.axisLabelStringCount()
                : bStatistical ? data.usedRows() : data.usedCols();
            calculateBasicTextFactors( nTxtHeight, para, averageValueP1000,
                    basicPos, orig, delimLen, nLabels,
                    // start of return parameters
                    pDelimDelta,
                    pTextsX, pTextsY, pTextsW, pTextsH,
                    textAlign );
            bool useShortLabels = false;
            QStringList tmpList( para.axisLabelStringList() );

            // find start- and/or end-entry
            int iStart = 0;
            int iEnd = para.axisLabelStringCount() - 1;
            if(    ! ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() )
                || ! ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) ) {
                const bool testStart = !( QVariant::String == para.axisValueStart().type() );
                const bool testEnd   = !( QVariant::String == para.axisValueEnd().type() );
                QString sStart = testStart
                    ? para.axisValueStart().toString().upper()
                    : QString::null;
                QString sEnd = testEnd
                    ? para.axisValueEnd().toString().upper()
                    : QString::null;

                uint i = 0;
                for ( QStringList::Iterator it = tmpList.begin();
                        it != tmpList.end(); ++it, ++i ) {
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
            if (    para.axisLabelStringCount()
                 && para.axisShortLabelsStringCount()
                 && para.axisLabelStringList() != para.axisShortLabelsStringList() ) {
                QFont font( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() )
                    font.setPixelSize( static_cast < int > ( nTxtHeight ) );
                painter->setFont( font );
                QFontMetrics fm( painter->fontMetrics() );

                QStringList::Iterator it = tmpList.begin();
                for ( int i = 0; i < nLabels; ++i ) {
                    if ( it != tmpList.end() ) {
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
                = ( KDCHART_AXIS_LABELS_AUTO_DELTA == para.axisValueDelta() )
                ? 1.0
                : para.axisValueDelta();
            modf( ddelta, &ddelta );
            bool positive = ( 0.0 <= ddelta );
            int delta = static_cast < int > ( fabs( ddelta ) );
            // find 1st significant entry
            QStringList::Iterator it = positive
                ? tmpList.begin()
                : tmpList.fromLast();
            if ( positive )
                for ( int i = 0; i < (int)tmpList.count(); ++i ) {
                    if ( i >= iStart )
                        break;
                    ++it;
                }
            else
                for ( int i = tmpList.count() - 1; i >= 0; --i ) {
                    if ( i <= iEnd )
                        break;
                    --it;
                }
            // transfer the strings
            int meter = delta;
            int i2 = positive ? iStart : iEnd;
            for ( int iLabel = 0; iLabel < nLabels; ) {
                if ( positive ) {
                    if ( it == tmpList.end() ) {
                        it = tmpList.begin();
                        i2 = 0;
                    }
                } else {
                    if ( it == tmpList.begin() ) {
                        it = tmpList.end();
                        i2 = tmpList.count();
                    }
                }
                if ( ( positive && i2 >= iStart )
                        || ( !positive && i2 <= iEnd ) ) {
                    if ( meter >= delta ) {
                        labelTexts << *it;
                        ++iLabel;
                        meter = 1;
                    } else {
                        meter += 1;
                    }
                }
                if ( positive ) {
                    if ( i2 == iEnd ) {
                        it = tmpList.begin();
                        i2 = 0;
                    } else {
                        ++it;
                        ++i2;
                    }
                } else {
                    if ( i2 == iStart ) {
                        it = tmpList.end();
                        i2 = tmpList.count();
                    } else {
                        --it;
                        --i2;
                    }
                }
            }
        } else {
            // find out if the associated dataset contains only strings
            // if yes, we will take these as label texts
            uint dset = ( dataset == KDCHART_ALL_DATASETS ) ? 0 : dataset;
            //qDebug("\ndset: %u", dset);
            bDone = false;
            QVariant value;
            for ( uint col = 0; col < data.usedCols(); ++col ) {
                if( data.cellCoord( dset, col, value, coordinate ) ){
                    if( QVariant::String == value.type() ){
                        const QString sVal = value.toString();
                        if( !sVal.isEmpty() && !sVal.isNull() ){
                            labelTexts.append( sVal );
                            bDone = true;
                        }
                    }else{
                        labelTexts.clear();
                        bDone = false;
                        break;
                    }
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
        if ( data.usedCols() && bSteadyCalc ) {
            // double values for numerical coordinates
            double nLow = 0.01;
            double nHigh = 0.0;
            double orgLow = 0.0;
            double orgHigh = 0.0;
            double nDelta = 0.0;
            double nDist = 0.0;

            //  VERTICAL axes support three modes:
            enum { Normal, Stacked, Percent } mode;

            if( bVertAxis ){
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
                    case KDChartParams::HiLo:
                    case KDChartParams::BoxWhisker:
                        mode = Normal;
                        break;
                    case KDChartParams::Polar:
                        if ( KDChartParams::PolarStacked
                                == params.polarChartSubType() )
                            mode = Stacked;
                        else if ( KDChartParams::PolarPercent
                                == params.polarChartSubType() )
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
            }else
                mode = Normal; // this axis is not a vertical axis

            uint nLabels = 200;

            // find highest and lowest value of associated dataset(s)
            bool bOrdFactorsOk = false;

            if( adjustTheValues ){
                nDelta = fabs( trueDelta );
                pDelimDelta = trueDeltaPix;
                nLow = QMIN( para.trueAxisLow(), para.trueAxisHigh() );
                //qDebug("\nsearching: para.trueAxisLow() %f    para.trueAxisHigh() %f",para.trueAxisLow(),para.trueAxisHigh());
                double orgLow( nLow );
                modf( nLow / nDelta, &nLow );
                nLow *= nDelta;
                if ( nLow > orgLow )
                    nLow -= nDelta;
                if ( 0.0 < nLow && 0.0 >= orgLow )
                    nLow = 0.0;
                nHigh = nLow;
                double dx = fabs( pXDeltaFactor * pDelimDelta );
                double dy = fabs( pYDeltaFactor * pDelimDelta );
                double x = 0.0;
                double y = 0.0;
                nLabels = 1;
                if( axisLength ){
                    do{
                        ++nLabels;
                        nHigh += nDelta;
                        x += dx;
                        y += dy;
                    }while( x < axisLength && y < axisLength );
                    nHigh -= nDelta;
                    --nLabels;
                }
                nDist = nHigh - nLow;
                bOrdFactorsOk = true;

            }

            if( !bOrdFactorsOk ){
                const bool bAutoCalcStart =
                       ( Percent != mode )
                    && ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() );
                const bool bAutoCalcEnd =
                       ( Percent != mode )
                    && ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() );

                if( !bIsDouble && !isDateTime ){
                    // no data at all: let us use our default 0..10 range
                    nLow   = 0.0;
                    nHigh  = 10.0;
                    nDist  = 10.0;
                    nDelta = 1.0;
                    nSubDelimFactor = 0.5;
                    bIsDouble     = true;
                    bOrdFactorsOk = true;
                }else if( mode == Percent ){
                    // precentage mode: we use a 0..100 range
                    nLow   =   0.0;
                    nHigh  = 100.0;
                    nDist  = 100.0;
                    nDelta =  10.0;
                    nSubDelimFactor = 0.25;
                    bOrdFactorsOk = true;
                }else{
                    //qDebug("\ngo:      nLow:  %f  nHigh: %f", nLow, nHigh );
                    // get the raw start value
                    const bool bStackedMode = (mode == Stacked);
                    if( bAutoCalcStart ){
                        if ( dataDataset == KDCHART_ALL_DATASETS ) {
                            if( bIsDouble ){
                                nLow = bStackedMode
                                     ? QMIN( data.minColSum(), 0.0 )
                                     : data.minValue( coordinate,
                                                      isLogarithmic );
                                //qDebug("\n1:      nLow:  %f", nLow );

                            }else{
                                dtLow = data.minDtValue( coordinate );
                            }
                        } else {
                            if( bIsDouble ){
                                nLow = bStackedMode
                                     ? QMIN( data.minColSum( dataDataset, dataDataset2 ),
                                             0.0 )
                                     : data.minInRows( dataDataset,dataDataset2,
                                                       coordinate,
                                                       isLogarithmic );
                            }else{
                                dtLow = data.minDtInRows( dataDataset,dataDataset2,
                                                          coordinate );
                            }
                        }
                    }else{
                        if( bIsDouble ){
                            if( QVariant::Double == para.axisValueStart().type() )
                                nLow  = para.axisValueStart().toDouble();
                        }else{
                            if( QVariant::DateTime == para.axisValueStart().type() )
                                dtLow = para.axisValueStart().toDateTime();
                        }
                    }

                    // get the raw end value
                    if( bAutoCalcEnd ){
                        if ( dataDataset == KDCHART_ALL_DATASETS ) {
                            if( bIsDouble ){
                                nHigh = bStackedMode
                                      ? QMAX( data.maxColSum(), 0.0 )
                                      : data.maxValue( coordinate );
//qDebug("\nnHigh %g", nHigh);
                            }else{
                                dtHigh = data.maxDtValue( coordinate );
                            }
                        } else {
                            if( bIsDouble )
                                nHigh = bStackedMode
                                      ? QMAX( data.maxColSum( dataDataset, dataDataset2 ),
                                              0.0 )
                                      : data.maxInRows( dataDataset,dataDataset2,
                                                        coordinate );
                            else
                                dtHigh = data.maxDtInRows( dataDataset,dataDataset2,
                                                          coordinate );
                        }
                        //qDebug("\nbAutoCalcEnd:\n  nLow:  %f\n  nHigh: %f", nLow, nHigh );
                    }else{
                        if( bIsDouble ){
                            if( QVariant::Double == para.axisValueEnd().type() )
                                nHigh = para.axisValueEnd().toDouble();
                        }else{
                            if( QVariant::DateTime == para.axisValueEnd().type() )
                                dtHigh = para.axisValueEnd().toDateTime();
                        }
                    }
                }


                //qDebug("\nnew:      nLow:  %f  nHigh: %f", nLow, nHigh );

                if( bIsDouble ) {
                    if(    DBL_MAX == nLow
                        || (    ( 0.0 == nHigh || 0 == nHigh )
                             && ( 0.0 == nLow  || 0 == nLow  ) ) ) {
                        // qDebug("NO values or all values have ZERO value, showing 0.0 - 10.0 span");
                        nLow   = 0.0;
                        nHigh  = 10.0;
                        nDist  = 10.0;
                        nDelta = 1.0;
                        nSubDelimFactor = 0.5;
                        bOrdFactorsOk = true;
                        //qDebug("nLow: %f,  nHigh: %f", nLow, nHigh);
                    }else if( nLow == nHigh ){
                        // if both values are equal, but NOT Zero
                        // -> move the appropriate one to Zero
                        if( nLow < 0.0 )
                            nHigh = 0.0;
                        else
                            nLow = 0.0;
                        //qDebug("nLow: %f,  nHigh: %f", nLow, nHigh);
                    }else if( nHigh < nLow ){
                        // make sure nLow is <= nHigh
                        double nTmp = nLow;
                        nLow = nHigh;
                        nHigh = nTmp;
                    }
                } else if( isDateTime ){
                    bool toggleDts = dtLow > dtHigh;
                    if( toggleDts ) {
                        QDateTime dt( dtLow );
                        dtLow = dtHigh;
                        dtHigh = dt;
                    }
                    double secDist = dtLow.secsTo( dtHigh );
                    secDist += (dtHigh.time().msec() - dtLow.time().msec()) / 1000.0;
                    const double aDist = fabs( secDist );
                    const double secMin   = 60.0;
                    const double secHour  = 60.0 * secMin;
                    const double secDay   = 24.0 * secHour;
                    //
                    // we temporarily disable week alignment until bug
                    // is fixed (1st week of year must not start in the
                    // preceeding year but rather be shown incompletely)
                    //
                    //                                 (khz, 2003/10/10)
                    //
                    //const int secWeek  =  7 * secDay;
                    const double secMonth = 30 * secDay;   // approx.
                    const double secYear  = 12 * secMonth; // approx.
                    if(      2.0*secMin > aDist )
                        dtDeltaScale = KDChartAxisParams::ValueScaleSecond;
                    else if( 2.0*secHour > aDist )
                        dtDeltaScale = KDChartAxisParams::ValueScaleMinute;
                    else if( 2.0*secDay > aDist )
                        dtDeltaScale = KDChartAxisParams::ValueScaleHour;
                    // khz: else if( 2*secWeek > aDist )
                    // khz:    dtDeltaScale = KDChartAxisParams::ValueScaleDay;
                    else if( 2.0*secMonth > aDist )
                        dtDeltaScale = KDChartAxisParams::ValueScaleDay;
                    // khz: dtDeltaScale = KDChartAxisParams::ValueScaleWeek;

                    else if( 2.0*secYear > aDist )
                        dtDeltaScale = KDChartAxisParams::ValueScaleMonth;
                    else if( 10.0*secYear > aDist )
                        dtDeltaScale = KDChartAxisParams::ValueScaleQuarter;
                    else
                        dtDeltaScale = KDChartAxisParams::ValueScaleYear;


                    //const int yearLow   = dtLow.date().year();
                    const int monthLow  = dtLow.date().month();
                    // khz: currently unused: const int dowLow    = dtLow.date().dayOfWeek();
                    const int dayLow    = dtLow.date().day();
                    const int hourLow   = dtLow.time().hour();
                    const int minuteLow = dtLow.time().minute();
                    const int secondLow = dtLow.time().second();

                    //const int yearHigh   = dtHigh.date().year();
                    const int monthHigh  = dtHigh.date().month();
                    // khz: currently unused: const int dowHigh    = dtHigh.date().dayOfWeek();
                    const int hourHigh   = dtHigh.time().hour();
                    const int minuteHigh = dtHigh.time().minute();
                    const int secondHigh = dtHigh.time().second();
                    int yearLowD   = 0;
                    int monthLowD  = 0;
                    int dayLowD    = 0;
                    int hourLowD   = 0;
                    int minuteLowD = 0;
                    int secondLowD = 0;
                    int yearHighD   = 0;
                    int monthHighD  = 0;
                    int dayHighD    = 0;
                    int hourHighD   = 0;
                    int minuteHighD = 0;
                    int secondHighD = 0;
                    bool gotoEndOfMonth = false;
                    switch( dtDeltaScale ) {
                        case KDChartAxisParams::ValueScaleSecond:
                            //qDebug("\nKDChartAxisParams::ValueScaleSecond");
                            if( 5.0 < aDist ){
                                secondLowD = secondLow % 5;
                                if( secondHigh % 5 )
                                    secondHighD = 5 - secondHigh % 5;
                            }
                            break;
                        case KDChartAxisParams::ValueScaleMinute:
                            //qDebug("\nKDChartAxisParams::ValueScaleMinute");
                            secondLowD = secondLow;
                            secondHighD = 59-secondHigh;
                            break;
                        case KDChartAxisParams::ValueScaleHour:
                            //qDebug("\nKDChartAxisParams::ValueScaleHour");
                            minuteLowD = minuteLow;
                            secondLowD = secondLow;
                            minuteHighD = 59-minuteHigh;
                            secondHighD = 59-secondHigh;
                            break;
                        case KDChartAxisParams::ValueScaleDay:
                            //qDebug("\nKDChartAxisParams::ValueScaleDay");
                            hourLowD   = hourLow;
                            minuteLowD = minuteLow;
                            secondLowD = secondLow;
                            hourHighD   = 23-hourHigh;
                            minuteHighD = 59-minuteHigh;
                            secondHighD = 59-secondHigh;
                            break;
                        case KDChartAxisParams::ValueScaleWeek:
                            //qDebug("\nKDChartAxisParams::ValueScaleWeek");
                            // khz: week scaling is disabled at the moment
                            /*
                            dayLowD = dowLow - 1;
                            hourLowD   = hourLow;
                            minuteLowD = minuteLow;
                            secondLowD = secondLow;
                            if( 7 > dowHigh )
                            dayHighD = 7 - dowHigh + 1;
                            */
                            break;
                        case KDChartAxisParams::ValueScaleMonth:
                            //qDebug("\nKDChartAxisParams::ValueScaleMonth");
                            if( 1 < dayLow )
                                dayLowD = dayLow - 1;
                            hourLowD   = hourLow;
                            minuteLowD = minuteLow;
                            secondLowD = secondLow;
                            gotoEndOfMonth = true;
                            break;
                        case KDChartAxisParams::ValueScaleQuarter:
                            //qDebug("\nKDChartAxisParams::ValueScaleQuarter");
                            monthLowD = ( monthLow - 1 ) % 3;
                            dayLowD    = dayLow;
                            hourLowD   = hourLow;
                            minuteLowD = minuteLow;
                            secondLowD = secondLow;
                            if( ( monthHigh - 1 ) % 3 )
                                monthHighD = 3 - ( monthHigh - 1 ) % 3;
                            gotoEndOfMonth = true;
                            break;
                        case KDChartAxisParams::ValueScaleYear:
                            //qDebug("\nKDChartAxisParams::ValueScaleYear");
                            monthLowD  = monthLow;
                            dayLowD    = dayLow;
                            hourLowD   = hourLow;
                            minuteLowD = minuteLow;
                            secondLowD = secondLow;
                            if( 12 > monthHigh )
                                monthHighD = 12 - monthHigh;
                            gotoEndOfMonth = true;
                            break;
                        default:
                            /* NOOP */
                            break;
                    }
                    dtLow  = dtLow.addSecs(   -1 * (secondLowD + 60*minuteLowD + 3600*hourLowD) );
                    dtLow  = dtLow.addDays(   -1 * dayLowD   );
                    dtAddMonths( dtLow, -1 * monthLowD, dtLow );
                    dtAddYears(  dtLow, -1 * yearLowD,  dtLow );
                    dtHigh = dtHigh.addSecs(   secondHighD + 60*minuteHighD + 3600* hourHighD );
                    dtHigh = dtHigh.addDays(   dayHighD   );
                    dtAddMonths( dtHigh, monthHighD, dtHigh );
                    dtAddYears(  dtHigh, yearHighD,  dtHigh );
                    if( gotoEndOfMonth ){
                        dtHigh.setDate( QDate( dtHigh.date().year(),
                                    dtHigh.date().month(),
                                    dtHigh.date().daysInMonth() ) );
                        dtHigh.setTime( QTime( 23, 59, 59 ) );
                    }
                    if( toggleDts ) {
                        QDateTime dt( dtLow );
                        dtLow = dtHigh;
                        dtHigh = dt;
                    }
                    // secDist = dtLow.secsTo( dtHigh );

                    // NOTE: nSubDelimFactor is not set here since it
                    //        cannot be used for QDateTime values.
                    nSubDelimFactor = 0.0;
                    bOrdFactorsOk = true;
                }


                if( !bOrdFactorsOk ) {
                    // adjust one or both of our limit values
                    // according to max-empty-inner-span settings
                    nDist = nHigh - nLow;
                    if( !isLogarithmic ){
                        // replace nLow (or nHigh, resp.) by zero if NOT ALL OF
                        // our values are located outside of the 'max. empty
                        //  inner space' (i.e. percentage of the y-axis range
                        // that may to contain NO data entries)
                        int maxEmpty = para.axisMaxEmptyInnerSpan();
                        if( bAutoCalcStart ) {
                            //qDebug("\nbAutoCalcStart:\n  nLow:  %f\n  nHigh: %f", nLow, nHigh );
                            if( 0.0 < nLow ) {
                                if(    maxEmpty == KDCHART_AXIS_IGNORE_EMPTY_INNER_SPAN
                                    || maxEmpty > ( nLow / nHigh * 100.0 ) )
                                    nLow = 0.0;
                                else if( nDist / 100.0 < nLow )
                                    nLow -= nDist / 100.0; // shift lowest value
                            }
                            else if( nDist / 100.0 < fabs( nLow ) )
                                nLow -= nDist / 100.0; // shift lowest value
                            nDist = nHigh - nLow;
                            //qDebug("* nLow:  %f\n  nHigh: %f", nLow, nHigh );
                        }
                        if( bAutoCalcEnd ) {
                            //qDebug("\nbAutoCalcEnd:\n  nLow:  %f\n  nHigh: %f", nLow, nHigh );
                            if( 0.0 > nHigh ) {
                                if(    maxEmpty == KDCHART_AXIS_IGNORE_EMPTY_INNER_SPAN
                                        || maxEmpty > ( nHigh / nLow * 100.0 ) )
                                    nHigh = 0.0;
                                else if( nDist / 100.0 > nHigh )
                                    nHigh += nDist / 100.0; // shift highest value
                            }
                            else if( nDist / 100.0 < fabs( nHigh ) )
                                 nHigh += nDist / 100.0; // shift highest value
                            nDist = nHigh - nLow;
                            //qDebug("* nLow:  %f\n  nHigh: %f\n\n", nLow, nHigh );
                        }
                    }
                }


                if( isLogarithmic ){
                    if( bIsDouble ) {
                        //qDebug("\n[L--] nLow: %f,  nHigh: %f,  nDelta: %f", nLow, nHigh, nDelta );
                        if( 0.0 == QABS( nLow ) )
                            nLow = -5;
                        else{
                            // find the Low / High values for the log. axis
                            nLow = log10( QABS( nLow ) );
                            //if( 0.0 >= nLow ){
                                //nLow = fastPow10( -nLow );
                            //}
                        }
                        nHigh = log10( QABS( nHigh ) );

                        //qDebug("[L-0] nLow: %f,  nHigh: %f", nLow, nHigh );
                        double intPart=0.0; // initialization necessary for Borland C++
                        double fractPart = modf( nLow, &intPart );
                        //qDebug("  intPart: %f\nfractPart: %f", intPart, fractPart );
                        if( 0.0 > nLow && 0.0 != fractPart )
                            nLow = intPart - 1.0;
                        else
                            nLow = intPart;
                        fractPart = modf( nHigh, &intPart );
                        if( 0.0 != fractPart )
                          nHigh = intPart + 1.0;

                        nDist = nHigh - nLow;
                        nDelta = 1.0;
                        nSubDelimFactor = 0.1;
                        //qDebug("\n[L-1] nLow: %f,  nHigh: %f,  nDelta: %f", nLow, nHigh, nDelta );
                        bOrdFactorsOk = true;
                    }
                }


                if ( !bOrdFactorsOk ) {
                    // adjust one or both of our limit values
                    // according to first two digits of (nHigh - nLow) delta
                    double nDivisor;
                    double nRound;
                    nDist = nHigh - nLow;
                    //qDebug("* nLow:  %f\n  nHigh: %f  nDist: %f\n\n", nLow, nHigh, nDist );
                    // find out factors and adjust nLow and nHigh
                    orgLow  = nLow;
                    orgHigh = nHigh;
                    calculateOrdinateFactors( para, isLogarithmic,
                                              nDist, nDivisor, nRound,
                                              nDelta, nSubDelimFactor,
                                              nLow, nHigh );
                    nLabels = params.roundVal( nDist / nDelta );

                    //qDebug("\n0.  nOrgHigh: %f\n    nOrgLow:  %f",
                    //       orgHigh, orgLow);
                    //qDebug("\n    nDist:    %f\n    nHigh:    %f\n    nLow:     %f",
                    //       nDist, nHigh, nLow);
                    //qDebug("    nDelta: %f", nDelta);
                    //qDebug("    nRound: %f", nRound);
                    //qDebug("    nLabels: %u", nLabels);

                    if( para.axisSteadyValueCalc() ) {
                        ++nLabels;
                        //qDebug("*   nLabels: %u", nLabels );
                    }
                }


                // calculate the amount of nLabels to be written we could take
                // based on the space we have for writing the label texts
                if( ! (    KDCHART_AXIS_LABELS_AUTO_DELTA
                        == para.axisValueDelta() ) ){
                    nDist = nHigh - nLow;
                    nDelta = para.axisValueDelta();
                    nLabels = params.roundVal( nDist / nDelta );

                    //qDebug("\nI nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u",
                    //       nLow, nHigh, nDelta, nLabels );

                    if( para.axisSteadyValueCalc() ) {
                        ++nLabels;

                        //qDebug("* nLabels: %u", nLabels );

                    }
                }

                // make sure labels fit into available height, if vertical axis
                if( bVertAxis ) {
                    double areaHeight = para.axisTrueAreaRect().height();
                    double nDivisor;
                    double nRound;
                    orgLow = nLow;
                    orgHigh = nHigh;
                    //qDebug("\ncalc ordinate 0.  nDist: %f\n  nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u", nDist, nLow, nHigh, nDelta, nLabels );
                    bool bTryNext = false;
                    uint minLabels = para.axisSteadyValueCalc() ? 3 : 2;
                    // the following must be processed at least twice - to avoid rounding errors
                    int pass = 0;
                    do{
                        nDist = nHigh - nLow;
                        nLow = orgLow;
                        nHigh = orgHigh;
                        /*
                        qDebug("\n=============================================================================\ncalc ordinate 1.  nDist: %f\n  nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u",
                        nDist, nLow, nHigh, nDelta, nLabels );
                        */
                        calculateOrdinateFactors( para, isLogarithmic,
                                nDist, nDivisor, nRound,
                                nDelta,
                                nSubDelimFactor, nLow, nHigh,
                                bTryNext );
                        nLabels = params.roundVal( nDist / nDelta );

                        //qDebug("\ncalc ordinate 2.  nDist: %f\n+ nLow: %f\n  nHigh: %f\n  nDelta: %f\n  nLabels: %u",
                        //nDist, nLow, nHigh, nDelta, nLabels );
                        //QString sDelta;sDelta.setNum( nDelta, 'f', 24 );
                        //QString sLow;    sLow.setNum( nLow,   'f', 24 );
                        //qDebug("nLow: %f,  sLow: %s,  sDelta: %s", nLow, sLow.latin1(), sDelta.latin1());

                        // special case: End values was set by the user, but no Detla values was set.
                        if( !bAutoCalcEnd && orgHigh > nLow + nLabels * nDelta ) {
                            ++nLabels;
                            //qDebug("\nnLabels: %u\n", nLabels );
                        }
                        if( para.axisSteadyValueCalc() ) {
                            ++nLabels;
                            //qDebug("\nnLabels: %u\n", nLabels );
                        }
                        //qDebug("calc ordinate n.  nDist = nHigh - nLow: %f = %f - %f",nDist, nHigh, nLow);
                        //qDebug("    nRound: %f\n", nRound);
                        bTryNext = true;
                        ++pass;
                    }while (    ( pass < 2 )
                             || (    ( minLabels < nLabels )
                                  && ( areaHeight < ( nTxtHeight * 1.5 ) * nLabels ) ) );
                }
            }

            // finally we can build the texts
            if( bIsDouble ) {
                int trueBehindComma = -1;
                double nVal = nLow;
                for ( uint i = 0; i < nLabels; ++i ) {
		  if( isLogarithmic ) {
                        labelTexts.append( applyLabelsFormat(
                            fastPow10( static_cast < int > ( nVal ) ),
                            divPow10,
                            behindComma,
                            1.0 == nDelta ? KDCHART_AXIS_LABELS_AUTO_DELTA : nDelta,
                            trueBehindComma,
                            decimalPoint,
                            thousandsPoint,
                            prefix,
                            postfix,
                            totalLen,
                            padFill,
                            blockAlign ) );
                  }  else {
                        labelTexts.append( applyLabelsFormat( nVal,
                                                              divPow10,
                                                              behindComma,
                                                              nDelta,
                                                              trueBehindComma,
                                                              decimalPoint,
                                                              thousandsPoint,
                                                              prefix,
                                                              postfix,
                                                              totalLen,
                                                              padFill,
                                                              blockAlign ) );
		  }
                    nVal += nDelta;
                }

                // save our true Low and High value
                //qDebug(para.axisSteadyValueCalc()?"\ntrue " : "\nfalse");
                //qDebug("nVal: %f,  nDelta: %f", nVal, nDelta );
                if ( para.axisSteadyValueCalc() ) {
                    nHigh = nVal - nDelta;
                }
                ( ( KDChartAxisParams& ) para ).setTrueAxisLowHighDelta( nLow, nHigh, nDelta );
                //qDebug("[Z] nLow: %f,  nHigh: %f,  nDelta: %f", nLow, nHigh, nDelta );

            } else {
                bool goDown = dtLow > dtHigh;
                int mult = goDown ? -1 : 1;
                QDateTime dt( dtLow );
                nLabels = 0;
                /*
                   qDebug("dtLow:  ");
                   qDebug(dtLow.toString(  Qt::ISODate ));
                   qDebug("dtHigh: ");
                   qDebug(dtHigh.toString( Qt::ISODate ));
                   */
                bool bDone=false;
                while( !bDone ) {
                    /*
                       qDebug("dtLow: %i %i %i    %i:%i:%i",
                       dtLow.date().year(),
                       dtLow.date().month(),
                       dtLow.date().day(),
                       dtLow.time().hour(),
                       dtLow.time().minute(),
                       dtLow.time().second());
                       qDebug("dtHigh: %i %i %i    %i:%i:%i",
                       dtHigh.date().year(),
                       dtHigh.date().month(),
                       dtHigh.date().day(),
                       dtHigh.time().hour(),
                       dtHigh.time().minute(),
                       dtHigh.time().second());
                       qDebug("dt: %i %i %i    %i:%i:%i",
                       dt.date().year(),
                       dt.date().month(),
                       dt.date().day(),
                       dt.time().hour(),
                       dt.time().minute(),
                       dt.time().second());
                       */
                    ++nLabels;
                    if( autoDtLabels )
                        labelTexts.append( "x" );
                    else
#if COMPAT_QT_VERSION >= 0x030000
                        labelTexts.append( dt.toString( Qt::ISODate ) );
#else
                    labelTexts.append( dateTimeToString( dt ) );
#endif
                    bDone = (goDown ? (dt < dtLow ) : (dt > dtHigh));
                    /*if( bDone ){
                      dtHigh = dt;
                      }else*/{
                          switch( dtDeltaScale ) {
                              case KDChartAxisParams::ValueScaleSecond:
                                  dtAddSecs( dt, 1 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleMinute:
                                  dtAddSecs( dt, 60 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleHour:
                                  dtAddSecs( dt, 3600 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleDay:
                                  dtAddDays( dt, 1 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleWeek:
                                  dtAddDays( dt, 7 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleMonth:
                                  dtAddMonths( dt,1 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleQuarter:
                                  dtAddMonths( dt,3 * mult, dt );
                                  break;
                              case KDChartAxisParams::ValueScaleYear:
                                  dtAddYears( dt, 1 * mult, dt );
                                  break;
                              default:
                                  dtAddDays( dt, 1 * mult, dt );
                                  break;
                          }
                      }
                }
                //if( autoDtLabels )
                //    labelTexts.append( "x" );
                ( ( KDChartAxisParams& ) para ).setTrueAxisDtLowHighDeltaScale(
                                                                               dtLow, dtHigh,
                                                                               dtDeltaScale );
                // note: pDelimDelta will be calculated below,
                //       look for "COMMOM CALC OF NLABELS, DELIM DELTA..."
            }
            bDone = true;
        }

        // let's generate some strings
        if ( !bDone ) {
            // default scenario for abscissa axes
            uint count = bStatistical
                ? (data.usedRows() ? data.usedRows() : 1)
                : (data.usedCols() ? data.usedCols() : 1);
            //double start( 1.0 );
            double start( 1.0 + (bSteadyCalc ? 0.0 : static_cast < double >(data.colsScrolledBy())) );
//qDebug("colsScrolledBy: %i", data.colsScrolledBy());
//if(bVertAxis)
//qDebug("vert nVal starting: %f",start);
//else
//qDebug("horz nVal starting: %f",start);
//if(bSteadyCalc)
//qDebug("bSteadyCalc");
//else
//qDebug("not bSteadyCalc");
            double delta( 1.0 );
            double finis( start + delta * ( count - 1 ) );
            const bool startIsDouble = QVariant::Double == para.axisValueStart().type();
            const bool endIsDouble   = QVariant::Double == para.axisValueEnd().type();

            bool deltaIsAuto = true;
            if ( !( KDCHART_AXIS_LABELS_AUTO_DELTA == para.axisValueDelta() ) ) {
                delta = para.axisValueDelta();
                deltaIsAuto = false;
            }
            if ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueStart() ) {
                if ( ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) ) {
                    finis = start + delta * ( count - 1 );
                } else {
                    if( endIsDouble ){
                        finis = para.axisValueEnd().toDouble();
                        start = finis - delta * ( count - 1 );
//qDebug("1 changing:   start: %f",start);
                    } else {
                        //
                        //
                        //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                        //
                        //
                    }
                }
            }else{
                if ( startIsDouble ) {
                    start = para.axisValueStart().toDouble() + (bSteadyCalc ? 0.0 : static_cast < double >(data.colsScrolledBy()));
//qDebug("2 changing:   start: %f",start);
                } else {
                    //
                    //
                    //   F E A T U R E   P L A N N E D   F O R   F U T U R E . . .
                    //
                    //
                }
                if ( !( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) ) {
                    if (endIsDouble ) {
                        finis = para.axisValueEnd().toDouble();
                        if ( deltaIsAuto ) {
                            delta = ( finis - start ) / count;
                        } else {
                            count = static_cast < uint > (
                                    ( finis - start ) / delta );
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
                    finis = start + delta * ( count - 1 );
                }
            }
            QString prefix( QObject::tr( "Item " ) );
            QString postfix;


            if ( startIsDouble && endIsDouble ) {
                int precis =
                    KDCHART_AXIS_LABELS_AUTO_DIGITS == para.axisDigitsBehindComma()
                    ? 0
                    : para.axisDigitsBehindComma();
                double s = start;
                double f = finis;
//qDebug("label loop:   s: %f   f: %f",s,f);
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
                    font.setPixelSize( static_cast < int > ( nTxtHeight ) );
                painter->setFont( font );
                QFontMetrics fm( painter->fontMetrics() );

                if ( fm.width( prefix +
                            QString::number( -fabs( ( s + f ) / 2.0 + delta ),
                                'f', precis ) )
                        > pTextsW ) {
                    prefix = "";
                    postfix = "";
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

            
            
                // pending(KHZ): make sure this branch will ever be reached
            
                
                
                // check the text widths largest entry
                // to make sure it will fit into the available space
                calculateBasicTextFactors( nTxtHeight, para,
                        averageValueP1000,
                        basicPos, orig, delimLen,
                        count,
                        // start of return parameters
                        pDelimDelta,
                        pTextsX, pTextsY, pTextsW, pTextsH,
                        textAlign );
                QFont font( para.axisLabelsFont() );
                if ( para.axisLabelsFontUseRelSize() )
                    font.setPixelSize( static_cast < int > ( nTxtHeight ) );
                painter->setFont( font );
                QFontMetrics fm( painter->fontMetrics() );

                if ( fm.width( prefix + QString::number( count - 1 ) )
                        > pTextsW ) {
                    prefix = "";
                    postfix = "";
                }
                // now transfer the strings into labelTexts
                for ( uint i = 1; i <= count; ++i )
                    labelTexts.append(
                            prefix + QString::number( i ) + postfix );
            }
        }
    }

    /*
        finishing: COMMOM CALC OF NLABELS, DELIM DELTA...
    */
    uint nLabels = labelTexts.count()
        ? labelTexts.count()
        : 0;
    ( ( KDChartAxisParams& ) para ).setAxisLabelTexts( &labelTexts );

    if( !adjustTheValues ){

        calculateBasicTextFactors( nTxtHeight, para, averageValueP1000,
                basicPos, orig, delimLen, nLabels,
                // start of return parameters
                pDelimDelta,
                pTextsX, pTextsY, pTextsW, pTextsH,
                textAlign );
    }

    ( ( KDChartAxisParams& ) para ).setTrueAxisDeltaPixels( pDelimDelta );

    //qDebug("\nsetting:   para.trueAxisLow() %f    para.trueAxisHigh() %f",para.trueAxisLow(),para.trueAxisHigh());
    //qDebug("\npDelimDelta: %f", pDelimDelta );

    /*
       qDebug( "Found label texts:" );
       for ( QStringList::Iterator it = labelTexts.begin();
       it != labelTexts.end(); ++it )
       qDebug( ">>>  %s", (*it).latin1() );
       qDebug( "\n" );
       */
//qDebug("\nleaving KDChartAxesPainter::calculateLabelTexts() :   nTxtHeight: "+QString::number(nTxtHeight));
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
/**** static ****/
void KDChartAxesPainter::calculateBasicTextFactors( double nTxtHeight,
        const KDChartAxisParams& para,
        double /*averageValueP1000*/,
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
            /*
              qDebug("pTextsW %f    wid %f    nLabels %u", pTextsW, wid, nLabels );
            */
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
            pDelimDelta = wid / divi;

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
        default: {
            qDebug( "IMPLEMENTATION ERROR: KDChartAxesPainter::calculateBasicTextFactors() unhandled enum value." );
            // NOOP since the 'basicPos' does not support more that these four values.
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
  variable "trueBehindComma" that has to be initialized with a value
  smaller than zero.

  \note This function is reserved for internal use.
  */
QString KDChartAxesPainter::truncateBehindComma( const double nVal,
        const int    behindComma,
        const double nDelta,
        int& trueBehindComma )
{
    const int  nTrustedPrecision = 6; // when using 15 we got 1.850000 rounded to 1.849999999999999

    const bool bUseAutoDigits = KDCHART_AXIS_LABELS_AUTO_DIGITS == behindComma;
    const bool bAutoDelta     = KDCHART_AXIS_LABELS_AUTO_DELTA == nDelta;
    QString sVal;
    sVal.setNum( nVal, 'f', bUseAutoDigits ? nTrustedPrecision
                                           : QMIN(behindComma, nTrustedPrecision) );
    //qDebug("nVal: %f    sVal: "+sVal, nVal );
    //qDebug( QString("                     %1").arg(sVal));
    if ( bUseAutoDigits ) {
        int comma = sVal.find( '.' );
        if ( -1 < comma ) {
            if ( bAutoDelta ) {
                int i = sVal.length();
                while ( 1 < i
                        && '0' == sVal[ i - 1 ] )
                    --i;
                sVal.truncate( i );
                if ( '.' == sVal[ i - 1 ] )
                    sVal.truncate( i - 1 );
            } else {
                if ( 0 > trueBehindComma ) {
                    QString sDelta = QString::number( nDelta, 'f', nTrustedPrecision );
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
                // now we cut off the too-many digits behind the comma
                int nPos = comma + ( trueBehindComma ? trueBehindComma + 1 : 0 );
                sVal.truncate( nPos );
            }
        }
    }
    //qDebug( QString("                       -  %1").arg(trueBehindComma));
    return sVal;
}


QString KDChartAxesPainter::applyLabelsFormat( const double nVal,
                        int   divPow10,
                        int   behindComma,
                        double nDelta,
                        int& trueBehindComma,
                        const QString& decimalPoint,
                        const QString& thousandsPoint,
                        const QString& prefix,
                        const QString& postfix,
                        int            totalLen,
                        const QChar&   padFill,
                        bool           blockAlign )
{

    QString sVal = truncateBehindComma( nVal / fastPow10( divPow10 ),
                                        behindComma,
                                        nDelta,
                                        trueBehindComma );
   
    int posComma = sVal.find( '.' );
    if( 0 <= posComma ){
        sVal.replace( posComma, 1, decimalPoint);
    }else{
        posComma = sVal.length();
    }
    if( thousandsPoint.length() ){
        const int minLen = (0 < sVal.length() && '-' == sVal[0])
                         ? 4
                         : 3;
        int n = posComma; // number of digits at the left side of the comma
        while( minLen < n ){
            n -= 3;
            sVal.insert(n, thousandsPoint);
        }
    }
    sVal.append( postfix );
    int nFill = totalLen - (sVal.length() + prefix.length());
    if( 0 > nFill )
        nFill = 0;
    if( !blockAlign )
        sVal.prepend( prefix );
    for(int i=0; i < nFill; ++i)
        sVal.prepend( padFill );
    if( blockAlign )
        sVal.prepend( prefix );
    if ( totalLen > 0 )
        sVal.truncate( totalLen );
    /*Pending Michel: Force non fractional values
     *In case it is a fractional value  
     *and the user has set axisLabelsDigitsBehindComma() == 0
     *return an empty string 
     */
    if ( behindComma == 0 && QString::number(nVal).find('.') > 0 )
      sVal = QString::null;//sVal = "";
    return sVal;
}

/**
  Calculates the factors to be used for calculating ordinate labels texts.

  \note This function is reserved for internal use.
  */
void KDChartAxesPainter::calculateOrdinateFactors(
        const KDChartAxisParams& para,
        bool isLogarithmic,
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
        sDistDigis2.setNum( nDist, 'f', 24);
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
                    nDivisor = fastPow10( (int)sDistDigis2.length() - 2 );
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

    // make sure its a whole number > 0 if its a log axis. Just round up.
    if( isLogarithmic )
        nDelta = static_cast < int > ( nDelta ) < nDelta
               ? static_cast < int > ( nDelta ) + 1
               : static_cast < int > ( nDelta );

    bool bInvertedAxis = ( 0.0 > nDist );
    if( bInvertedAxis )
        nDelta *= -1.0;

    /*
       qDebug("  n D i s t       :  %f", nDist   );
       qDebug("  n D i v i s o r :  %f", nDivisor);
       qDebug("  n R o u n d     :  %f", nRound  );
       qDebug("  n D e l t a     :  %f", nDelta  );
       qDebug("  nHigh           :  %f", nHigh   );
       qDebug("  nLow            :  %f", nLow    );
       */
    if(    KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueStart()
        || !para.axisValueStartIsExact() ) {
        double orgLow( nLow );
        modf( nLow / nDelta, &nLow );
        nLow *= nDelta;
        if( bInvertedAxis ){
            if ( nLow < orgLow )
                nLow += nDelta;
            if ( 0.0 > nLow && 0.0 <= orgLow )
                nLow = 0.0;
        }else{
            if ( nLow > orgLow )
                nLow -= nDelta;
            if ( 0.0 < nLow && 0.0 >= orgLow )
                nLow = 0.0;
        }
    }
    if ( KDCHART_AXIS_LABELS_AUTO_LIMIT == para.axisValueEnd() ) {
        double orgHigh( nHigh );
        modf( nHigh / nDelta, &nHigh );
        nHigh *= nDelta;
        if( bInvertedAxis ){
            if ( nHigh > orgHigh )
                nHigh -= nDelta;
            if ( 0.0 < nHigh && 0.0 >= orgHigh )
                nHigh = 0.0;
        }else{
            if ( nHigh < orgHigh )
                nHigh += nDelta;
            if ( 0.0 > nHigh && 0.0 <= orgHigh )
                nHigh = 0.0;
        }
    }
    
    //qDebug("  n H i g h       :  %f", nHigh   );
    //qDebug("  n L o w         :  %f\n\n", nLow    );
    
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

/**** static ****/
void KDChartAxesPainter::dtAddSecs( const QDateTime& org, const int secs, QDateTime& dest )
{
    //qDebug("entering KDChartAxesPainter::dtAddSecs() ..");
    int s = org.time().second();
    int m = org.time().minute();
    int h = org.time().hour();
    int days = 0;
    if( -1 < secs ){
        int mins = (s + secs) / 60;
        if( 0 == mins )
            s += secs;
        else{
            s = (s + secs) % 60;
            int hours = (m + mins) / 60;
            if( 0 == hours )
                m += mins;
            else{
                m = (m + mins) % 60;
                days = (h + hours) / 24;
                if( 0 == days )
                    h += hours;
                else{
                    h = (h + hours) % 24;
                }
            }
        }
    }
    dest.setTime( QTime(h,m,s) );
    dest.setDate( org.date() );
    if( days )
        dtAddDays( dest, days, dest );
    //qDebug(".. KDChartAxesPainter::dtAddSecs() done.");
}

/**** static ****/
void KDChartAxesPainter::dtAddDays( const QDateTime& org, const int days, QDateTime& dest )
{
    //qDebug("entering KDChartAxesPainter::dtAddDays() ..");
    int d = org.date().day();
    int m = org.date().month();
    int y = org.date().year();
    int dd = (-1 < days) ? 1 : -1;
    int di = 0;
    while( di != days ){
        d += dd;
        // underrunning day?
        if( 1 > d ){
            if( 1 < m ){
                --m;
                d = QDate( y,m,1 ).daysInMonth();
            }
            else{
                --y;
                m = 12;
                d = 31;
            }
            // overrunning day?
        }else if( QDate( y,m,1 ).daysInMonth() < d ){
            if( 12 > m )
                ++m;
            else{
                ++y;
                m = 1;
            }
            d = 1;
        }
        di += dd;
    }
    dest = QDateTime( QDate( y,m,d ), org.time() );
    //qDebug(".. KDChartAxesPainter::dtAddDays() done.");
}

/**** static ****/
void KDChartAxesPainter::dtAddMonths( const QDateTime& org, const int months, QDateTime& dest )
{
    //qDebug("entering KDChartAxesPainter::dtAddMonths() ..");
    int d = org.date().day();
    int m = org.date().month();
    int y = org.date().year();
    int md = (-1 < months) ? 1 : -1;
    int mi = 0;
    while( mi != months ){
        m += md;
        if( 1 > m ){
            --y;
            m = 12;
        }else if( 12 < m ){
            ++y;
            m = 1;
        }
        mi += md;
    }
    // QMIN takes care for intercalary day
    dest = QDateTime( QDate( y,m,QMIN( d, QDate( y,m,1 ).daysInMonth() ) ),
            org.time() );
    //qDebug(".. KDChartAxesPainter::dtAddMonths() done.");
}

/**** static ****/
void KDChartAxesPainter::dtAddYears( const QDateTime& org, const int years, QDateTime& dest )
{
    //qDebug("entering KDChartAxesPainter::dtAddYears() ..");
    int d = org.date().day();
    int m = org.date().month();
    int y = org.date().year() + years;
    dest.setTime( org.time() );
    // QMIN takes care for intercalary day
    dest = QDateTime( QDate( y,m,QMIN( d, QDate( y,m,d ).daysInMonth() ) ),
            org.time() );
    //qDebug(".. KDChartAxesPainter::dtAddYears() done.");
}



void KDChartAxesPainter::calculateAbscissaInfos( const KDChartParams& params,
                                                 const KDChartTableDataBase& data,
                                                 uint datasetStart,
                                                 uint datasetEnd,
                                                 double logWidth,
                                                 const QRect& dataRect,
                                                 abscissaInfos& infos )
{
    if( params.axisParams( KDChartAxisParams::AxisPosBottom ).axisVisible()
            && ( KDChartAxisParams::AxisTypeUnknown
                != params.axisParams( KDChartAxisParams::AxisPosBottom ).axisType() ) )
        infos.abscissaPara = &params.axisParams( KDChartAxisParams::AxisPosBottom );
    else
        if( params.axisParams( KDChartAxisParams::AxisPosBottom2 ).axisVisible()
                && ( KDChartAxisParams::AxisTypeUnknown
                    != params.axisParams( KDChartAxisParams::AxisPosBottom2 ).axisType() ) )
            infos.abscissaPara = &params.axisParams( KDChartAxisParams::AxisPosBottom2 );
        else
            if( params.axisParams( KDChartAxisParams::AxisPosTop ).axisVisible()
                    && ( KDChartAxisParams::AxisTypeUnknown
                        != params.axisParams( KDChartAxisParams::AxisPosTop ).axisType() ) )
                infos.abscissaPara = &params.axisParams( KDChartAxisParams::AxisPosTop );
            else
                if( params.axisParams( KDChartAxisParams::AxisPosTop2 ).axisVisible()
                        && ( KDChartAxisParams::AxisTypeUnknown
                            != params.axisParams( KDChartAxisParams::AxisPosTop2 ).axisType() ) )
                    infos.abscissaPara = &params.axisParams( KDChartAxisParams::AxisPosTop2 );
                else
                    // default is bottom axis:
                    infos.abscissaPara = &params.axisParams( KDChartAxisParams::AxisPosBottom );

    if( infos.abscissaPara->axisLabelsTouchEdges() )
        infos.bCenterThePoints = false;

    infos.bAbscissaDecreasing = infos.abscissaPara->axisValuesDecreasing();
    infos.bAbscissaIsLogarithmic
        = KDChartAxisParams::AxisCalcLogarithmic == infos.abscissaPara->axisCalcMode();


    // Number of values: If -1, use all values, otherwise use the
    // specified number of values.
    infos.numValues = 0;
    if ( params.numValues() > -1 )
        infos.numValues = params.numValues();
    else
        infos.numValues = data.usedCols();

    QVariant::Type type2Ref = QVariant::Invalid;
    infos.bCellsHaveSeveralCoordinates =
        data.cellsHaveSeveralCoordinates( datasetStart, datasetEnd,
                &type2Ref );

    infos.numLabels = (infos.abscissaPara &&
                    infos.abscissaPara->axisLabelTexts())
        ? infos.abscissaPara->axisLabelTexts()->count()
        : infos.numValues;
    if( 0 >= infos.numLabels )
        infos.numLabels = 1;

    infos.bAbscissaHasTrueAxisValues =
        infos.abscissaPara && (0.0 != infos.abscissaPara->trueAxisDelta());
    infos.abscissaStart = infos.bAbscissaHasTrueAxisValues
        ? infos.abscissaPara->trueAxisLow()
        : 0.0;
    infos.abscissaEnd   = infos.bAbscissaHasTrueAxisValues
        ? infos.abscissaPara->trueAxisHigh()
        : 1.0 * (infos.numLabels - 1);
    infos.abscissaSpan  = fabs( infos.abscissaEnd - infos.abscissaStart );
    infos.abscissaDelta = infos.bAbscissaHasTrueAxisValues
                        ? infos.abscissaPara->trueAxisDelta()
                        : (   ( 0.0 != infos.abscissaSpan  )
                                    ? ( infos.abscissaSpan / infos.numLabels )
                                    : infos.abscissaSpan );

    //qDebug( bAbscissaDecreasing ? "bAbscissaDecreasing =  TRUE" : "bAbscissaDecreasing = FALSE");
    //qDebug( abscissaHasTrueAxisValues ? "abscissaHasTrueAxisValues =  TRUE" : "abscissaHasTrueAxisValues = FALSE");
    //qDebug( "abscissaDelta = %f", abscissaDelta);

    infos.bAbscissaHasTrueAxisDtValues =
        (QVariant::DateTime == type2Ref) &&
        infos.abscissaPara &&
        infos.abscissaPara->trueAxisDtLow().isValid();
    if( infos.bAbscissaHasTrueAxisDtValues ){
        infos.numLabels = 200;
        infos.bCenterThePoints = false;
    }

    infos.dtLowPos = infos.bAbscissaHasTrueAxisDtValues
        ? infos.abscissaPara->axisDtLowPosX() - dataRect.x()
        : 0.0;
    infos.dtHighPos = infos.bAbscissaHasTrueAxisDtValues
        ? infos.abscissaPara->axisDtHighPosX() - dataRect.x()
        : logWidth;
    infos.abscissaDtStart = infos.bAbscissaHasTrueAxisDtValues
        ? infos.abscissaPara->trueAxisDtLow()
        : QDateTime();
    infos.abscissaDtEnd = infos.bAbscissaHasTrueAxisDtValues
        ? infos.abscissaPara->trueAxisDtHigh()
        : QDateTime();

    // adjust the milli seconds:
    infos.abscissaDtStart.setTime(
        QTime( infos.abscissaDtStart.time().hour(),
               infos.abscissaDtStart.time().minute(),
               infos.abscissaDtStart.time().second(),
               0 ) );
    infos.abscissaDtEnd.setTime(
        QTime( infos.abscissaDtEnd.time().hour(),
               infos.abscissaDtEnd.time().minute(),
               infos.abscissaDtEnd.time().second(),
               999 ) );
    //qDebug(infos.abscissaDtStart.toString("yyyy-MM-dd-hh:mm:ss.zzz"));
    //qDebug(infos.abscissaDtEnd.toString("yyyy-MM-dd-hh:mm:ss.zzz"));

    infos.bScaleLessThanDay = ( infos.bAbscissaHasTrueAxisDtValues
            ? infos.abscissaPara->trueAxisDtDeltaScale()
            : KDChartAxisParams::ValueScaleDay )
        < KDChartAxisParams::ValueScaleDay;

    if( infos.bAbscissaHasTrueAxisDtValues ){
        if( infos.bScaleLessThanDay  ){
            infos.abscissaDtSpan = infos.abscissaDtStart.secsTo( infos.abscissaDtEnd );
            /*  NOTE: We do *not* add the milli seconds because they aren't covered
                      by the span indicated by infos.dtHighPos - infos.dtLowPos.
            if( infos.abscissaDtStart.time().msec() || infos.abscissaDtEnd.time().msec() )
                infos.abscissaDtSpan +=
                    ( infos.abscissaDtEnd.time().msec() -
                      infos.abscissaDtStart.time().msec() ) / 1000.0;
            */
        }
        else{
            infos.abscissaDtSpan = infos.abscissaDtStart.daysTo( infos.abscissaDtEnd );
            if( infos.abscissaDtStart.time().msec() || infos.abscissaDtEnd.time().msec() )
                infos.abscissaDtSpan +=
                    ( infos.abscissaDtEnd.time().msec() -
                      infos.abscissaDtStart.time().msec() ) / (86400.0 * 1000.0);
            if( infos.abscissaDtEnd.time().second() )
                infos.abscissaDtSpan += infos.abscissaDtEnd.time().second() / 86400.0;
            if( infos.abscissaDtEnd.time().minute() )
                infos.abscissaDtSpan += infos.abscissaDtEnd.time().minute() / 1440.0;
            if( infos.abscissaDtEnd.time().hour() )
                infos.abscissaDtSpan += infos.abscissaDtEnd.time().hour()   / 24.0;
        }
    }else
        infos.abscissaDtSpan = 10.0;
    if( 0 == infos.abscissaDtSpan || 0.0 == infos.abscissaDtSpan )
        infos.abscissaDtSpan = 1.0;
    //qDebug("abscissaDtSpan: %f", infos.abscissaDtSpan);

    infos.abscissaDtPixelsPerScaleUnit = (infos.dtHighPos - infos.dtLowPos) / infos.abscissaDtSpan;
    if( infos.bAbscissaHasTrueAxisDtValues )
        infos.abscissaDelta = 20.0;

    infos.pointDist
        = ( infos.abscissaPara && (0.0 != infos.abscissaPara->trueAxisDeltaPixels()) )
        ? infos.abscissaPara->trueAxisDeltaPixels()
        : ( logWidth /
                (
                  (1 > ((double)(infos.numLabels) - (infos.bCenterThePoints ? 0.0 : 1.0)))
                ? ((double)(infos.numLabels) - (infos.bCenterThePoints ? 0.0 : 1.0))
                : 1 ) );

    infos.abscissaPixelsPerUnit = ( 0.0 != infos.abscissaDelta  )
                                ? ( infos.pointDist / infos.abscissaDelta )
                                : infos.pointDist;

    //const double infos.abscissaZeroPos2 = -1.0 * infos.abscissaPixelsPerUnit * infos.abscissaStart;
    infos.abscissaZeroPos = infos.abscissaPara->axisZeroLineStartX() - dataRect.x();
    //qDebug("abscissaZeroPos %f    abscissaZeroPos2 %f",abscissaZeroPos,abscissaZeroPos2);

    /*
       qDebug(abscissaPara ?
       "\nabscissaPara: OK" :
       "\nabscissaPara: leer");
       qDebug(abscissaHasTrueAxisValues ?
       "abscissaHasTrueAxisValues: TRUE" :
       "abscissaHasTrueAxisValues: FALSE");
       qDebug("abscissaStart: %f", abscissaStart);
       qDebug("abscissaEnd  : %f",   abscissaEnd);
       qDebug("abscissaPara->trueAxisDelta(): %f", abscissaPara->trueAxisDelta());
       qDebug("numValues  : %u,      numLabels  : %u",   numValues, numLabels);
    */
}



bool KDChartAxesPainter::calculateAbscissaAxisValue( const QVariant& value,
                                                     abscissaInfos& ai,
                                                     int colNumber,
                                                     double& xValue )
{
    if( ai.bCellsHaveSeveralCoordinates ) {
        if( QVariant::Double == value.type() ) {
            double dVal = value.toDouble();
            if( ai.bAbscissaIsLogarithmic ){
                if( 0.0 < dVal )
                    xValue = ai.abscissaPixelsPerUnit * log10( dVal );
                else
                    xValue = -10250.0;
            }else{
                xValue = ai.abscissaPixelsPerUnit * dVal;
            }
            xValue *= ai.bAbscissaDecreasing ? -1.0 : 1.0;
            xValue += ai.abscissaZeroPos;
        }
        else if( ai.bAbscissaHasTrueAxisDtValues &&
                 QVariant::DateTime == value.type() ) {
            const QDateTime dtVal = value.toDateTime();
            double dT = ( ai.bScaleLessThanDay )
                      ? ai.abscissaDtStart.secsTo( dtVal )
                      : ai.abscissaDtStart.daysTo( dtVal );
            /*
            qDebug("abscissaDtStart:  %i %i %i   %i:%i:%i.%i",
            ai.abscissaDtStart.date().year(),
            ai.abscissaDtStart.date().month(),
            ai.abscissaDtStart.date().day(),
            ai.abscissaDtStart.time().hour(),
            ai.abscissaDtStart.time().minute(),
            ai.abscissaDtStart.time().second(),
            ai.abscissaDtStart.time().msec());
            //qDebug("days to = %f",dT);
            qDebug("                        dtVal: %i %i %i   %i:%i:%i.%i",
            dtVal.date().year(),
            dtVal.date().month(),
            dtVal.date().day(),
            dtVal.time().hour(),
            dtVal.time().minute(),
            dtVal.time().second(),
            dtVal.time().msec());
            */
            xValue = ai.abscissaDtPixelsPerScaleUnit * dT;
            if( dtVal.time().msec() )
                xValue += (ai.abscissaDtPixelsPerScaleUnit * dtVal.time().msec())
                    / (   ai.bScaleLessThanDay
                        ? 1000.0
                        : (1000.0 * 86400.0) );
            //qDebug("xValue: %f",xValue);
            if( !ai.bScaleLessThanDay ){
                if( dtVal.time().second() )
                    xValue += (ai.abscissaDtPixelsPerScaleUnit * dtVal.time().second())
                        / 86400.0;
                if( dtVal.time().minute() )
                    xValue += (ai.abscissaDtPixelsPerScaleUnit * dtVal.time().minute())
                        / 1440.0;
                if( dtVal.time().hour() )
                    xValue += (ai.abscissaDtPixelsPerScaleUnit * dtVal.time().hour())
                        / 24.0;
            }
            xValue *= ai.bAbscissaDecreasing ? -1.0 : 1.0;
            xValue += ai.dtLowPos;
            // qDebug("xValue = dtLowPos + abscissaDtPixelsPerScaleUnit * dT\n%f = %f + %f * %f",
            // xValue, dtLowPos, abscissaDtPixelsPerScaleUnit, dT);
        }
        else
            return false;
    } else
        xValue = ai.pointDist * ( double ) colNumber;
    return true;
}



/*************************************************************************/
/*************************************************************************/
/*************************************************************************/
/***                                                                   ***/
/***  Framework for data drawing using cartesian axes (Bar, Line, ...) ***/
/***                                                                   ***/
/*************************************************************************/
/*************************************************************************/
/*************************************************************************/

/**
  Paints the actual data area and registers the region for the data
  points if \a regions is not 0.

  \param painter the QPainter onto which the chart should be painted
  \param data the data that will be displayed as a chart
  \param paint2nd specifies whether the main chart or the additional chart is to be drawn now
  \param regions a pointer to a list of regions that will be filled
  with regions representing the data segments, if not null
  */
void KDChartAxesPainter::paintData( QPainter* painter,
        KDChartTableDataBase* data,
        bool paint2nd,
        KDChartDataRegionList* regions )
{
    bool bNormalMode = isNormalMode();

    uint chart = paint2nd ? 1 : 0;

    // find out the ordinate axis (or axes, resp.) belonging to this chart
    // (up to 4 ordinates might be in use: 2 left ones and 2 right ones)
    uint axesCount;
    KDChartParams::AxesArray ordinateAxes;
    ordinateAxes.resize( KDCHART_CNT_ORDINATES );
    if( !params()->chartAxes( chart, axesCount, ordinateAxes ) ) {
        // no axis - no fun!
        return;
        // We cannot draw data without an axis having calculated high/low
        // values and position of the zero line before.

        // PENDING(khz) Allow drawing without having a visible axis!
    }

    //const KDChartParams::ChartType params_chartType
    //  = paint2nd ? params()->additionalChartType() : params()->chartType();

    double logWidth = _dataRect.width();
    double areaWidthP1000 = logWidth / 1000.0;

    int nClipShiftUp = clipShiftUp(bNormalMode, areaWidthP1000);
    QRect ourClipRect( _dataRect );
    if ( 0 < ourClipRect.top() ) {
        ourClipRect.setTop( ourClipRect.top() - nClipShiftUp );
        ourClipRect.setHeight( ourClipRect.height() + nClipShiftUp - 1 );
    } else
        ourClipRect.setHeight( ourClipRect.height() + nClipShiftUp / 2 - 1 );

    // protect axes ?
    //ourClipRect.setBottom( ourClipRect.bottom() - 1 );
    //ourClipRect.setLeft( ourClipRect.left() + 1 );
    //ourClipRect.setRight( ourClipRect.right() - 1 );

    const QWMatrix & world = painter->worldMatrix();
    ourClipRect =
#if COMPAT_QT_VERSION >= 0x030000
    world.mapRect( ourClipRect );
#else
    world.map( ourClipRect );
#endif
    painter->setClipRect( ourClipRect );
    painter->translate( _dataRect.x(), _dataRect.y() );

    painter->setPen( params()->outlineDataColor() );

    // find out which datasets are to be represented by this chart
    uint chartDatasetStart, chartDatasetEnd;
    findChartDatasets( data, paint2nd, chart, chartDatasetStart, chartDatasetEnd );

    // Note: 'aI' is *not* the axis number!
    for( uint aI = 0; aI < axesCount; ++aI ) {
        // 'axis' is the REAL axis number!
        uint axis = ordinateAxes.at( aI );

        const KDChartAxisParams* axisPara = &params()->axisParams( axis );

        uint datasetStart, datasetEnd;
        uint axisDatasetStart, axisDatasetEnd;
        uint dummy;
        if( params()->axisDatasets( axis,
                                    axisDatasetStart,
                                    axisDatasetEnd, dummy )
            && ( KDCHART_ALL_DATASETS != axisDatasetStart ) ) {

            if( KDCHART_NO_DATASET == axisDatasetStart ){
                //==========
                continue; //  NO DATASETS  -->  STOP PROCESSING THIS AXIS
                //==========
            }

            if(    axisDatasetStart >= chartDatasetStart
                && axisDatasetStart <= chartDatasetEnd )
                datasetStart = QMAX( axisDatasetStart, chartDatasetStart );
            else if(    axisDatasetStart <= chartDatasetStart
                     && axisDatasetEnd   >= chartDatasetStart )
                datasetStart = chartDatasetStart;
            else
                datasetStart = 20;
            if(    axisDatasetEnd >= chartDatasetStart
                && axisDatasetEnd <= chartDatasetEnd )
                datasetEnd = QMIN( axisDatasetEnd, chartDatasetEnd );
            else if(    axisDatasetEnd   >= chartDatasetEnd
                     && axisDatasetStart <= chartDatasetEnd )
                datasetEnd = chartDatasetEnd;
            else
                datasetEnd = 0;
        } else {
            datasetStart = chartDatasetStart;
            datasetEnd   = chartDatasetEnd;
        }

        //qDebug("\n=========================================================="
        //       "\naxis   %u   axisDatasetStart %u   axisDatasetEnd %u   /   chartDatasetStart %u   chartDatasetEnd %u",
        //axis, axisDatasetStart, axisDatasetEnd, chartDatasetStart, chartDatasetEnd );

        double logHeight   = axisPara->axisTrueAreaRect().height();
        double axisYOffset = axisPara->axisTrueAreaRect().y() - _dataRect.y();

        //qDebug("\n==========================================================\naxis   %u   logHeight %f   axisDatasetStart %u   chartDatasetStart %u   axisDatasetEnd %u   chartDatasetEnd %u",
        //axis, logHeight, axisDatasetStart, chartDatasetStart, axisDatasetEnd, chartDatasetEnd );
        //if( KDCHART_ALL_DATASETS == axisDatasetStart )
        //    qDebug("  ALL DATASETS");
        //if( KDCHART_NO_DATASET == axisDatasetStart )
        //    qDebug("  N O   DATESETS");

        double maxColumnValue = axisPara->trueAxisHigh();
        double minColumnValue = axisPara->trueAxisLow();
        double columnValueDistance = maxColumnValue - minColumnValue;


        // call the chart type specific data painter:
        specificPaintData( painter,
                           ourClipRect,
                           data,
                           regions,
                           axisPara,
                           bNormalMode,
                           chart,
                           logWidth,
                           areaWidthP1000,
                           logHeight,
                           axisYOffset,
                           minColumnValue,
                           maxColumnValue,
                           columnValueDistance,
                           chartDatasetStart,
                           chartDatasetEnd,
                           datasetStart,
                           datasetEnd );
    }
    painter->translate( - _dataRect.x(), - _dataRect.y() );
}
