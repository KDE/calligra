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
#ifndef __KDCHARTPAINTER_H__
#define __KDCHARTPAINTER_H__

#include <QMap>
#include <qrect.h>
#include <qdatetime.h>
#include <qregion.h>
#include <QString>
//Added by qt3to4:
#include <Q3PointArray>

#include "KDChartGlobal.h"
#include "KDChartUnknownTypeException.h"
#include "KDChartNotEnoughSpaceException.h"
#include "KDChartTable.h"
#include "KDChartDataRegion.h"
#include "KDChartParams.h"

class KDChartTableDataBase;
class KDChartCustomBox;
class KDChartTextPiece;
class KDChartPropertySet;
class KDChartAxisParams;
class QPainter;


struct internal__KDChart__CalcValues {
    bool processThisAxis;
    bool bSteadyCalc;
    bool bLogarithmic;
    bool bDecreasing;
    KDChartAxisParams::AxisPos basicPos;
    QPoint orig;
    QPoint dest;
    double pXDeltaFactor;
    double pYDeltaFactor;
    double pXDelimDeltaFaktor;
    double pYDelimDeltaFaktor;
    double nSubDelimFactor;
    double pDelimDelta;
    double nTxtHeight;
    double pTextsX;
    double pTextsY;
    double pTextsW;
    double pTextsH;
    int textAlign;
    bool isDateTime;
    bool autoDtLabels;
    QDateTime dtLow;
    QDateTime dtHigh;
    KDChartAxisParams::ValueScale dtDeltaScale;
    double nLow;
    double nHigh;
    double nDelta;
    double nDeltaPix;
    double pLastX;
    double pLastY;
};


class KDCHART_EXPORT KDChartPainter
{
    public:
        static KDChartPainter* create( KDChartParams* params,
                                       bool make2nd = false );

        virtual ~KDChartPainter();

        static void registerPainter( const QString& painterName,
                                     KDChartPainter* painter );
        static void unregisterPainter( const QString& painterName );

        virtual void setupGeometry ( QPainter* painter,
                                     KDChartTableDataBase* data,
                                     const QRect& rect );

        virtual void paint( QPainter* painter, KDChartTableDataBase* data,
                            bool paintFirst,
                            bool paintLast,
                            KDChartDataRegionList* regions = 0,
                            const QRect* rect = 0,
                            bool mustCalculateGeometry = true );

        virtual QRect outermostRect() const { return _outermostRect; }

        static void drawMarker( QPainter* painter,
                                 int style,
                                 const QColor& color,
                                 const QPoint& p,
                                 const QSize& size,
                                 uint align = Qt::AlignCenter );

    protected:
        KDChartPainter( KDChartParams* );

        // Note: dataRow, dataCol (and data3rd, resp.)
        //       must only be set if area == KDChartEnums::AreaChartDataRegion
        virtual void paintArea( QPainter* painter,
                                uint area,
                                KDChartDataRegionList* regions = 0,
                                uint dataRow = 0,
                                uint dataCol = 0,
                                uint data3rd = 0 );

        virtual void paintDataRegionAreas( QPainter* painter,
                                           KDChartDataRegionList* regions );

        virtual void paintAreaWithGap( QPainter* painter,
                                       QRect rect,
                                       const KDChartParams::KDChartFrameSettings& settings );
        virtual void paintCustomBoxes( QPainter* painter,
                                       KDChartDataRegionList* regions );

        virtual void paintData( QPainter* painter, KDChartTableDataBase* data,
                                bool paint2nd,
                                KDChartDataRegionList* regions = 0 ) = 0;
        virtual void paintDataValues( QPainter* painter,
                                      KDChartTableDataBase* data,
                                      KDChartDataRegionList* regions );
        virtual void paintAxes( QPainter* painter,
                                KDChartTableDataBase* data );
        virtual void paintLegend( QPainter* painter,
                                  KDChartTableDataBase* data );
        virtual void paintHeaderFooter( QPainter* painter,
                                        KDChartTableDataBase* data );
        virtual bool axesOverlapping( int axis1, int axis2 );

        virtual void findChartDatasets( KDChartTableDataBase* data,
                                        bool paint2nd,
                                        uint chart,
                                        uint& chartDatasetStart,
                                        uint& chartDatasetEnd );

        virtual void calculateAllAxesRects( QPainter* painter,
                                            bool finalPrecision,
                                            KDChartTableDataBase* data );

        virtual QPoint calculateAnchor( const KDChartCustomBox & box,
                KDChartDataRegionList* regions = 0 ) const;
        virtual QRect calculateAreaRect( bool & allCustomBoxes,
                                         uint area,
                                         uint dataRow,
                                         uint dataCol,
                                         uint data3rd,
                                         KDChartDataRegionList* regions ) const;

        virtual QString fallbackLegendText( uint dataset ) const;
        virtual uint numLegendFallbackTexts( KDChartTableDataBase* data ) const;

        static QPoint pointOnCircle( const QRect& rect, double angle );
        static void makeArc( Q3PointArray& points,
                             const QRect& rect,
                             double startAngle, double angles );

        const KDChartParams* params() const
        {
            return _params;
        }

        QRect _outermostRect; /* The Outermost rect covers the complete
                                 area of the painter. */

        QRect _innermostRect; /* The Innermost rect covers the area of
                                 the painter MINUS the the global
                                 left/top/right/bottom leading.
                                 ALL following ones are INSIDE the Innermost.
                                 */

        QRect _dataRect;

        QRect _axesRect; /* The Axes rect contains the Data rect. Up to 4
                            axes might be at the left and bottom as well
                            as at the right and/or at the to top of the
                            chart.
                            */

        QRect _legendRect; /* The legend position depends on the parameter
                              settings. If it is not directly to the left or
                              to the right of the data display, it will be
                              below the headers and on top of the footers.
                              */
        int _legendEMSpace; // an em space in the legend font
        int _legendSpacing; // the line spacing in the legend font
        int _legendHeight; // the font height in the legend font
        int _legendLeading; // the font leading in the legend font
        //     int _legendTitleSpacing; // the line spacing in the legend title font
        //     int _legendTitleLeading; // the font leading in the legend title font
        KDChartTextPiece* _legendTitle;

        int _numLegendTexts; // the number of legend items to show

        int _logicalWidth;
        int _logicalHeight;
        double _areaWidthP1000;
        double _areaHeightP1000;

        QMap < int, QString > _legendTexts; // precomputed legend texts

        internal__KDChart__CalcValues calcVal[ KDCHART_MAX_AXES ];
        virtual bool calculateAllAxesLabelTextsAndCalcValues(
                        QPainter* painter,
                        KDChartTableDataBase* data,
                        double areaWidthP1000,
                        double areaHeightP1000,
                        double& delimLen );

        virtual void drawExtraLinesAndMarkers(
                        KDChartPropertySet& propSet,
                        const QPen& defaultPen,
                        const KDChartParams::LineMarkerStyle& defaultMarkerStyle,
                        int myPointX,
                        int myPointY,
                        QPainter* painter,
                        const KDChartAxisParams* abscissaPara,
                        const KDChartAxisParams* ordinatePara,
                        const double areaWidthP1000,
                        const double areaHeightP1000,
                        bool bDrawInFront = FALSE );

        static KDChartDataRegion* drawMarker( QPainter* painter,
                                               const KDChartParams* params,
                                               double areaWidthP1000,
                                               double areaHeightP1000,
                                               int deltaX,
                                               int deltaY,
                                               int style,
                                               const QColor& color,
                                               const QPoint& p,
                                               uint dataset, uint value, uint chart,
                                               KDChartDataRegionList* regions = 0,
                                               int* width = 0,
                                               int* height = 0,
                                               uint align = Qt::AlignCenter );

    private:
        // disallow copy-construction and assignment
        KDChartPainter( const KDChartPainter& );
        KDChartPainter& operator=( const KDChartPainter& );


        QMap < QString, KDChartPainter* > _customPainters;
        KDChartParams* _params;

        QRect trueFrameRect( const QRect& orgRect,
                             const KDChartParams::KDChartFrameSettings* settings ) const;

        int legendTitleVertGap() const;
        QFont trueLegendFont() const;
        void calculateHorizontalLegendSize( QPainter* painter,
                                            QSize& size,
                                            bool& legendNewLinesStartAtLeft ) const;
        bool mustDrawVerticalLegend() const;
        void findLegendTexts( KDChartTableDataBase* );
        int calculateHdFtRects( QPainter* painter,
                                double averageValueP1000,
                                int  xposLeft,
                                int  xposRight,
                                bool bHeader,
                                int& yposTop,
                                int& yposBottom );
        bool _legendNewLinesStartAtLeft;
        int _legendTitleHeight;
        int _legendTitleWidth;
        int _legendTitleMetricsHeight; // the font height in the legend title font
        int _hdLeading;
        int _ftLeading;
};

#endif
