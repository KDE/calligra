/* $Id$ */

#ifndef _KCHARTAXESPAINTER_H
#define _KCHARTAXESPAINTER_H

#include "KChartPainter.h"
#include "KChartData.h"

#include <qfont.h>
#include <qstrlist.h>

class KChart;
class QPaintDevice;
class QPainter;

// The base class for all chart painters
class KChartAxesPainter : public KChartPainter
{
public:
  KChartAxesPainter( KChart* chart );
  virtual ~KChartAxesPainter();

  virtual void paintChart( QPaintDevice* paintdev );
  virtual void drawData( QPainter* painter ) = 0;
  virtual void drawText( QPainter* painter );
  virtual void drawAxes( QPainter* painter );
  virtual void drawTicks( QPainter* painter );
  virtual void drawLegend( QPainter* painter );
  virtual void drawLegendMarker( QPainter* painter, int number, int x, int y );

protected:
  QPoint valToPixel( uint pos, double value, uint dataset );
  virtual bool setupCoords( QPaintDevice* );
  virtual void setupLegend( QPaintDevice* );
  virtual bool setMaxMin();
  virtual void createYLabels();
  double upperBound( double );
  double lowerBound( double );
};

#endif
