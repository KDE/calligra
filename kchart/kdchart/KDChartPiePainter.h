/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTPIEPAINTER_H__
#define __KDCHARTPIEPAINTER_H__

#include <KDChartPainter.h>
#include <KDChartTable.h>

class KDChartParams;

class KDChartPiePainter : public KDChartPainter
{
    friend class KDChartPainter;
protected:
    KDChartPiePainter( KDChartParams* params );
    virtual ~KDChartPiePainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
    virtual void drawOnePie( QPainter* painter, KDChartTableData* data,
                             uint dataset, uint pie, uint threeDPieHeight,
                             KDChartDataRegionList* regions = 0 );
    virtual void draw3DEffect( QPainter* painter, const QRect& drawPosition,
                               uint dataset, uint pie, uint threeDPieHeight,
                               bool explode,
                               KDChartDataRegionList* regions = 0 );
    void drawStraightEffectSegment( QPainter* painter, const QRect& rect,
                                    uint dataset, uint pie, int
                                    threeDHeight, int angle,
                                    KDChartDataRegionList* regions = 0 );
    void drawArcEffectSegment( QPainter* painter, const QRect& rect,
                               uint dataset, uint pie,
                               int threeDHeight, int startAngle,
                               int endAngle,
                               KDChartDataRegionList* regions = 0 );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;

    uint findPieAt( int angle );
    uint findLeftPie( uint pie );
    uint findRightPie( uint pie );
    QPoint pointOnCircle( const QRect& rect, int angle );

    QArray < int > _startAngles;
    QArray < int > _angleLens;

    QRect _position;
    uint _size;
    uint _numValues; // PENDING(kalle) Move to base class
}
;

#endif
