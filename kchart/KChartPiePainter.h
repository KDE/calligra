/* $Id$ */

#ifndef _KCHARTPIEPAINTER_H
#define _KCHARTPIEPAINTER_H

#include "KChartPainter.h"

class QPaintDevice;
class QPainter;
class table_t;

class KChartPiePainter : public KChartPainter
{
public:
  KChartPiePainter( KoChartParameters* chart );
  ~KChartPiePainter();

  virtual void paintChart( QPainter* painter, table_t* table );
  virtual void drawData( QPainter* painter, table_t* table );
  virtual void drawText( QPainter* painter, table_t* table );
  virtual void drawPie( QPainter* painter, table_t* table );

protected:
  QPoint cartesian( int, int, int, int, int );
  void putLabel( QPainter* painter, int x, int y, const char* label );
  const int _angleoffset;
  bool setupCoords( QPaintDevice* );

  int _xcenter;
  int _ycenter;

private:
};

#endif
