/* $Id$ */

#ifndef _KCHARTPIEPAINTER_H
#define _KCHARTPIEPAINTER_H

#include "KChartPainter.h"

class KChart;
class QPaintDevice;
class QPainter;

class KChartPiePainter : public KChartPainter
{
public:
  KChartPiePainter( KChart* chart );
  ~KChartPiePainter();

  virtual void paintChart( QPaintDevice* paintdev );
  virtual void drawData( QPainter* painter );
  virtual void drawText( QPainter* painter );
  virtual QColor chooseDataColor( int dataset );

protected:
  bool setupCoords( QPaintDevice* );

private:
};

#endif
