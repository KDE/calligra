/* $Id$ */

#ifndef _KCHARTAREAPAINTER_H
#define _KCHARTAREAPAINTER_H

#include "KChartAxesPainter.h"

class KChart;
class QPainter;
class table_t;
struct KoChartParameters;

class KChartAreaPainter : public KChartAxesPainter
{
public:
  KChartAreaPainter( KoChartParameters* chart );
  ~KChartAreaPainter();

  virtual void drawData( QPainter* painter, table_t* table );

private:
};

#endif
