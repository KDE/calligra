/* $Id$ */

#ifndef _KCHARTLINESPOINTSPAINTER_H
#define _KCHARTLINESPOINTSPAINTER_H

#include "KChartLinesPainter.h"
#include "KChartPointsPainter.h"

class KChart;
class QPainter;

class KChartLinesPointsPainter : public virtual KChartLinesPainter, 
								 virtual KChartPointsPainter
{
public:
  KChartLinesPointsPainter( KChart* chart );
  ~KChartLinesPointsPainter();

  virtual void drawData( QPainter* painter );
  virtual void drawLegendMarker( QPainter* painter, int number, int x, int y );

private:
};

#endif
