/* $Id$ */

#ifndef _KCHARTAREAPAINTER_H
#define _KCHARTAREAPAINTER_H

#include "KChartAxesPainter.h"

class KChart;
class QPainter;

class KChartAreaPainter : public KChartAxesPainter
{
public:
  KChartAreaPainter( KChart* chart );
  ~KChartAreaPainter();

  virtual void drawData( QPainter* painter );

private:
};

#endif
