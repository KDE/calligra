/* -*- Mode: C++ -*-

  $Id$

  KDChart - a multi-platform charting engine

  Copyright (C) 2001 by Klarälvdalens Datakonsult AB
*/

#ifndef __KDCHARTPOLARPAINTER_H__
#define __KDCHARTPOLARPAINTER_H__

#include "KDChartPainter.h"
#include <KDChartTable.h>
#include <KDChartParams.h>

class KDChartPolarPainter : public KDChartPainter
{
    friend class KDChartPainter;

protected:
    KDChartPolarPainter( KDChartParams* params );
    virtual ~KDChartPolarPainter();

    virtual void paintData( QPainter* painter, KDChartTableData* data,
                            bool paint2nd,
                            KDChartDataRegionList* regions = 0 );
  void drawMarker( QPainter* painter,
            KDChartParams::PolarMarkerStyle style,
            const QColor& color, const QPoint& p,
            uint dataset, uint value, uint chart,
            double minSizeP1000,
            QRegion & region );
  virtual QString fallbackLegendText( uint dataset ) const;
  virtual uint numLegendFallbackTexts( KDChartTableData* data ) const;

private:
  QPoint polarToXY( int radius, int angle );
  void paintCircularAxisLabel( QPainter* painter,
                               bool rotate,
                               int txtAngle,
                               QPoint center,
                               double currentRadiusPPU,
                               const QString& txt,
                               int align,
                               int step );
};

#endif
