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
  virtual void setLineWidth( int width );
  virtual int lineWidth() const;
  virtual void setLineTypes( PenStyle types[], int number );
  virtual void lineTypes( PenStyle types[], int& number );

private:
  PenStyle chooseLineType( int dataset );
  void drawLine( QPainter* painter, const QPoint& begin,
				 const QPoint& end, PenStyle type,
				 const QColor& datacolor );

  int _linewidth;
  QList<PenStyle> _linetypes;
};

#endif
