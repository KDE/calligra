/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTRINGPAINTER_H__
#define __KDCHARTRINGPAINTER_H__

#include <KDChartPainter.h>
#include <KDChartTable.h>

class KDChartParams;

class KDChartRingPainter : public KDChartPainter
{
    friend class KDChartPainter;
protected:
    KDChartRingPainter( KDChartParams* params );
    virtual ~KDChartRingPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
    void drawOneSegment( QPainter* painter,
                         uint outerRadius, uint innerRadius,
                         uint startAngle, uint endAngle,
                         uint dataset, uint value,
                         bool explode,
                         KDChartDataRegionList* regions = 0 );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;

    QRect _position;
    uint _size;
    uint _numValues; // PENDING(kalle) Move to base class
}
;

#endif
