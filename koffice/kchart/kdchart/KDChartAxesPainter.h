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
#ifndef __KDCHARTAXESPAINTER_H__
#define __KDCHARTAXESPAINTER_H__

#include <qpen.h>
#include <qpainter.h>

#include <KDChartPainter.h>
#include <KDChartTable.h>
#include <KDChartAxisParams.h>

class QDateTime;
class KDChartParams;

class KDChartAxesPainter : public KDChartPainter
{
    protected:
        KDChartAxesPainter( KDChartParams* params );
        virtual ~KDChartAxesPainter();

        virtual void paintAxes( QPainter* painter,
                KDChartTableDataBase* data );

        virtual void paintData( QPainter* painter,
                KDChartTableDataBase* data,
                bool paint2nd,
                KDChartDataRegionList* regions );

        // note: Classes inheriting from KDChartAxesPainter
        //       MUST implement the following abstract functions:

        // Returns whether a normal chart is to be drawn
        // or a stacked or percentage (or ... ) chart            .
        virtual bool isNormalMode() const = 0;

        // Returns the number of pixels to be added to the clipping area's height
        // to make room for special arrows indicating oversized, cut data entries.
        // Chart classes not drawing such indicators must return 0.
        virtual int clipShiftUp( bool normalMode, double areaWidthP1000 ) const = 0;

        // The actual, chart type specific drawing of the data.
        virtual void specificPaintData( QPainter* painter,
                const QRect& ourClipRect,
                KDChartTableDataBase* data,
                KDChartDataRegionList* regions,
                const KDChartAxisParams* axisPara,
                bool bNormalMode,
                uint chart,
                double logWidth,
                double areaWidthP1000,
                double logHeight,
                double axisYOffset,
                double minColumnValue,
                double maxColumnValue,
                double columnValueDistance,
                uint chartDatasetStart,
                uint chartDatasetEnd,
                uint datasetStart,
                uint datasetEnd ) = 0;

        virtual bool calculateAllAxesLabelTextsAndCalcValues(
                          QPainter* painter,
                          KDChartTableDataBase* data,
                          double areaWidthP1000,
                          double areaHeightP1000,
                          double& delimLen );

    private:
    public:
        // special infos needed by the different painters' implementations
        // of the specificPaintData() function
        struct abscissaInfos{
            const KDChartAxisParams* abscissaPara;
            bool bCenterThePoints;
            bool bAbscissaDecreasing;
            bool bAbscissaIsLogarithmic;
            bool bCellsHaveSeveralCoordinates;
            int numValues;
            int numLabels;
            bool bAbscissaHasTrueAxisValues;
            double abscissaStart;
            double abscissaEnd;
            double abscissaSpan;
            double abscissaDelta;
            bool bAbscissaHasTrueAxisDtValues;
            double dtLowPos;
            double dtHighPos;
            QDateTime abscissaDtStart;
            QDateTime abscissaDtEnd;
            bool bScaleLessThanDay;
            double abscissaDtSpan;
            double abscissaDtPixelsPerScaleUnit;
            double pointDist;
            double abscissaPixelsPerUnit;
            double abscissaZeroPos;
        };
        static void calculateAbscissaInfos(
                const KDChartParams& params,
                const KDChartTableDataBase& data,
                uint datasetStart,
                uint datasetEnd,
                double logWidth,
                const QRect& dataRect,
                abscissaInfos& infos );
        static bool calculateAbscissaAxisValue( const QVariant& value,
                                                abscissaInfos& ai,
                                                int colNumber,
                                                double& xValue );

        static void saveDrawLine( QPainter& painter,
                QPoint pA,
                QPoint pZ,
                QPen pen );
        static void calculateLabelTexts(
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
                bool adjustTheValues = false,
                double trueDelta     = 0.0,
                double trueDeltaPix  = 0.0 );
                static void calculateBasicTextFactors( double nTxtHeight,
                        const KDChartAxisParams& para,
                        double averageValueP1000,
                        KDChartAxisParams::AxisPos basicPos,
                        const QPoint& orig,
                        double delimLen,
                        uint nLabels,
                        // start of return parameters
                        double& pDelimDelta,
                        double& pTextsX,
                        double& pTextsY,
                        double& pTextsW,
                        double& pTextsH,
                        int& textAlign );
                static QString truncateBehindComma( const double nVal,
                        const int    behindComma,
                        const double nDelta,
                        int& trueBehindComma );
                static QString applyLabelsFormat( const double nVal,
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
                        bool           blockAlign );
                static void calculateOrdinateFactors( const KDChartAxisParams& para,
                        bool bLogarithmic,
                        double& nDist,
                        double& nDivisor,
                        double& nRound,
                        double& nDelta,
                        double& nSubDelimFactor,
                        double& nLow,
                        double& nHigh,
                        bool findNextRound = false );
                static void dtAddSecs(   const QDateTime& org, const int secs,   QDateTime& dest );
                static void dtAddDays(   const QDateTime& org, const int days,   QDateTime& dest );
                static void dtAddMonths( const QDateTime& org, const int months, QDateTime& dest );
                static void dtAddYears(  const QDateTime& org, const int years,  QDateTime& dest );
};

#endif
