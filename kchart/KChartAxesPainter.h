/* $Id$ */

#ifndef _KCHARTAXESPAINTER_H
#define _KCHARTAXESPAINTER_H

#include "KChartPainter.h"
#include "KoChartParameters.h"

#include <qfont.h>
#include <qstrlist.h>

class QPaintDevice;
class QPainter;
class table_t;

// The base class for all chart painters
class KChartAxesPainter : public KChartPainter
{
public:
  KChartAxesPainter( KoChartParameters* chart );
  virtual ~KChartAxesPainter();

  virtual void paintChart( QPainter* painter, table_t* table );
  virtual void drawData( QPainter* painter, table_t* table ) = 0;
  virtual void drawText( QPainter* painter, table_t* table );
  virtual void drawAxes( QPainter* painter, table_t* table );
  virtual void drawTicks( QPainter* painter, table_t* table );
  virtual void drawLegend( QPainter* painter, table_t* table );
  virtual void drawLegendMarker( QPainter* painter, int number, 
								 int x, int y, table_t* table );

	KChartType _charttype;

protected:
  QPoint valToPixel( uint pos, double value, uint dataset );
  virtual bool setupCoords( QPaintDevice*, table_t* table );
  virtual void setupLegend( QPaintDevice*, table_t* table );
  virtual bool setMaxMin( table_t* table );
  virtual void createYLabels();
  double upperBound( double );
  double lowerBound( double );
};

#endif
