/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTDATAREGION__
#define __KDCHARTDATAREGION__

#include <qregion.h>
#include <qpointarray.h>

#include "KDChartGlobal.h"

/*!
  \internal
*/
struct KDChartDataRegion
{
    KDChartDataRegion( QRegion reg, uint r, uint c, uint ch )
    {
        region = reg;
        row    = r;
        col    = c;
        chart  = ch;
        negative = false; // default value (useful if value is a string)
        points.resize( 9 );
        startAngle = 1440;
        angleLen   =    1;
    }

    QRegion region;

    // For rectangular data representation  (bar, line, area, point, ...)
    // we use the bounding rect of the above declared 'region'.
    // For curved data representations (pie slice, ring segment, ...)
    // we store the following additional anchor information:

    QPointArray points;  // stores 9 elements: one for each
                         // value of KDChartEnums::PositionFlag

    int startAngle; // Note: 5760 makes a full circle, 2880 is left 'corner'.
    int angleLen;

    uint row;
    uint col;
    // members needed for calculation of data values texts
    uint chart;
    QRegion textRegion;  // for the data values text
    QString text;        // the data values text
    bool    negative;    // stores whether the data value is less than zero
};


typedef QPtrList < KDChartDataRegion > KDChartDataRegionList;

#endif
