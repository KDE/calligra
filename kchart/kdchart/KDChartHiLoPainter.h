/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTHILOPAINTER_H__
#define __KDCHARTHILOPAINTER_H__

#include "KDChartAxesPainter.h"
#include <KDChartTable.h>

#include <qnamespace.h>

class KDChartParams;

class KDChartHiLoPainter : public KDChartAxesPainter, public Qt
{
    friend class KDChartPainter;
protected:
    KDChartHiLoPainter( KDChartParams* params );
    virtual ~KDChartHiLoPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions );

    virtual QString fallbackLegendText( uint dataset ) const;
    virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;
};

#endif
