/* $Id$ */

#ifndef _KCHARTLINESPAINTER_H
#define _KCHARTLINESPAINTER_H

#include "KChartAxesPainter.h"

#include <qlist.h>
#include <qpen.h>

class KChart;
class QPainter;

class KChartLinesPainter : public KChartAxesPainter
{
public:
  KChartLinesPainter( KChart* chart );
  ~KChartLinesPainter();

  virtual void drawData( QPainter* painter );
  virtual void drawLegendMarker( QPainter* painter, int number, int x, int y );

private:
  Qt::PenStyle chooseLineType( int dataset );
  void drawLine( QPainter* painter, const QPoint& begin,
				 const QPoint& end, Qt::PenStyle type,
				 const QColor& datacolor );

  int _linewidth;
  QList<Qt::PenStyle> _linetypes;
};

#endif
