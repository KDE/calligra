/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTDATAREGION__
#define __KDCHARTDATAREGION__

#include <qregion.h>
#include <qlist.h>

/*!
  \internal
*/
struct KDChartDataRegion
{
    KDChartDataRegion( QRegion reg, uint r, uint c )
    {
        region = reg;
        row = r;
        col = c;
    }

    QRegion region;
    uint row;
    uint col;
};


typedef QList < KDChartDataRegion > KDChartDataRegionList;

#endif
