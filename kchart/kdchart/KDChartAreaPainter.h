/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTAREAPAINTER_H__
#define __KDCHARTAREAPAINTER_H__

#include "KDChartLinesPainter.h"
#include <KDChartTable.h>

class KDChartParams;

class KDChartAreaPainter : public KDChartLinesPainter
{
    friend class KDChartPainter;
protected:
    KDChartAreaPainter( KDChartParams* params );
    virtual ~KDChartAreaPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
};

#endif
