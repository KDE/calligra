/* $Id$ */

#ifndef _KCHARTBARSPAINTER_H
#define _KCHARTBARSPAINTER_H

#include "KChartAxesPainter.h"

class KChart;
class QPainter;
class table_t;

class KChartBarsPainter : public KChartAxesPainter
{
public:
  KChartBarsPainter( KoDiagrammParameters* chart );
  ~KChartBarsPainter();

  virtual void drawData( QPainter* painter, table_t* table );

private:
};

#endif
