/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTLINESPAINTER_H__
#define __KDCHARTLINESPAINTER_H__

#include "KDChartAxesPainter.h"
#include <KDChartTable.h>
#include <KDChartParams.h>

class KDChartLinesPainter : public KDChartAxesPainter
{
    friend class KDChartPainter;

protected:
    KDChartLinesPainter( KDChartParams* params );
    virtual ~KDChartLinesPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
    virtual void drawMarker( QPainter* painter,
                             KDChartParams::LineMarkerStyle style,
                             const QColor& color, const QPoint& p,
                             uint dataset, uint value,
                             KDChartDataRegionList* regions = 0 );
    void paintDataInternal( QPainter* painter,
                            KDChartTableData* data,
                            bool centerThePoints,
                            bool drawMarkers,
                            bool isArea,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
};

#endif
