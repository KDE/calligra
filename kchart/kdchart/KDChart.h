/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHART_H__
#define __KDCHART_H__

#include "KDChartTable.h"
#include "KDChartUnknownTypeException.h"
#include "KDChartParams.h"
#include "KDChartDataRegion.h"

class KDChartPainter;
class QPainter;

class KDChart
{
    friend class KDChartCleanup;
public:
    static void paint( QPainter*, KDChartParams*, KDChartTableData*,
                       KDChartDataRegionList* regions = 0 )
#ifdef USE_EXCEPTIONS
    throw( KDChartUnknownTypeException )
#endif
    ;

private:
    KDChart(); // prevent instantiations

    static KDChartPainter* cpainter;
    static KDChartPainter* cpainter2;
    static KDChartParams::ChartType cpainterType;
    static KDChartParams::ChartType cpainterType2;
};

#endif
