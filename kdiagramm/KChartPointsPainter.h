/* $Id$ */

#ifndef _KCHARTPOINTSPAINTER_H
#define _KCHARTPOINTSPAINTER_H

#include "KChartAxesPainter.h"

class QPainter;
class table_t;

class KChartPointsPainter : public KChartAxesPainter
{
public:
  KChartPointsPainter( KoDiagrammParameters* chart );
  ~KChartPointsPainter();

  virtual void drawData( QPainter* painter, table_t* table );
  virtual void drawLegendMarker( QPainter* painter, int number, 
								 int x, int y, table_t* table );

private:
};

#endif
