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
                             uint dataset, uint pie, uint chart,
                             uint threeDPieHeight,
                             KDChartDataRegionList* regions = 0 );
    virtual void draw3DEffect( QPainter* painter, const QRect& drawPosition,
                               uint dataset, uint pie, uint chart,
                               uint threeDPieHeight,
                               bool explode,
                               QRegion* region = 0 );
    void drawStraightEffectSegment( QPainter* painter, const QRect& rect,
                                    uint dataset, uint pie, uint chart,
                                    int threeDHeight, int angle,
                                    QRegion* region = 0 );
    void drawArcEffectSegment( QPainter* painter, const QRect& rect,
                               uint dataset, uint pie, uint chart,
                               int threeDHeight, int startAngle,
                               int endAngle,
                               QRegion* region = 0 );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;

    uint findPieAt( int angle );
    uint findLeftPie( uint pie );
    uint findRightPie( uint pie );

    QMemArray < int > _startAngles;
    QMemArray < int > _angleLens;

    QRect _position;
    uint _size;
    uint _numValues; // PENDING(kalle) Move to base class
}
;

#endif
