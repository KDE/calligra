/* $Id$ */

#ifndef _KCHARTBARSPAINTER_H
#define _KCHARTBARSPAINTER_H

#include "KChartAxesPainter.h"

class KChart;
class QPainter;

class KChartBarsPainter : public KChartAxesPainter
{
public:
  KChartBarsPainter( KChart* chart );
  ~KChartBarsPainter();

  virtual void drawData( QPainter* painter );

private:
};

#endif
