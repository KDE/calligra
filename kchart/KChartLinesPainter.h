/* $Id$ */

#ifndef _KCHARTLINESPAINTER_H
#define _KCHARTLINESPAINTER_H

#include "KChartAxesPainter.h"

#include <qlist.h>
#include <qpen.h>

class KChart;
class QPainter;
class table_t;

class KChartLinesPainter : public KChartAxesPainter
{
public:
  KChartLinesPainter( KoDiagrammParameters* chart );
  ~KChartLinesPainter();

  virtual void drawData( QPainter* painter, table_t* table );
  virtual void drawLegendMarker( QPainter* painter, int number,
								 int x, int y, table_t* table );

private:
  Qt::PenStyle chooseLineType( int dataset );
  void drawLine( QPainter* painter, const QPoint& begin,
				 const QPoint& end, Qt::PenStyle type,
				 const QColor& datacolor, table_t* table );

  int _linewidth;
  QList<Qt::PenStyle> _linetypes;
};

#endif
