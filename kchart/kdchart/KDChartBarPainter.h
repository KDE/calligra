/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTBARPAINTER_H__
#define __KDCHARTBARPAINTER_H__

#include "KDChartAxesPainter.h"
#include <KDChartTable.h>

#include <qnamespace.h>

class KDChartParams;

class KDChartBarPainter : public KDChartAxesPainter, public Qt
{
    friend class KDChartPainter;
protected:
    KDChartBarPainter( KDChartParams* params );
    virtual ~KDChartBarPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions );
};

#endif
