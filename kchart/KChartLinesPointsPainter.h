/* $Id$ */

#ifndef _KCHARTLINESPOINTSPAINTER_H
#define _KCHARTLINESPOINTSPAINTER_H

#include "KChartLinesPainter.h"
#include "KChartPointsPainter.h"

class KChart;
class QPainter;
class table_t;

class KChartLinesPointsPainter : public virtual KChartLinesPainter, 
								 virtual KChartPointsPainter
{
public:
  KChartLinesPointsPainter( KoDiagrammParameters* chart );
  ~KChartLinesPointsPainter();

  virtual void drawData( QPainter* painter, table_t* table );
  virtual void drawLegendMarker( QPainter* painter, int number, 
								 int x, int y, table_t* table );

private:
};

#endif
