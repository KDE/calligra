/* $Id$ */

#ifndef _KCHARTPOINTSPAINTER_H
#define _KCHARTPOINTSPAINTER_H

#include "KChartAxesPainter.h"

class KChart;
class QPainter;

class KChartPointsPainter : public KChartAxesPainter
{
public:
  KChartPointsPainter( KChart* chart );
  ~KChartPointsPainter();

  virtual void drawData( QPainter* painter );
  virtual void drawLegendMarker( QPainter* painter, int number, int x, int y );

private:
};

#endif
