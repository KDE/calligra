/* $Id$ */

#ifndef _KCHARTPAINTER_H
#define _KCHARTPAINTER_H

#include "KChartColorArray.h"
#include "KChartTypes.h"

#include <qcolor.h>
#include <qfont.h>
#include <qpen.h>

class KChart;
class QPainter;

// The base class for all chart painters
class KChartPainter
{
public:
  KChartPainter( KChart* chart );
  virtual ~KChartPainter();

  void setTitleFont( QFont font );
  virtual bool checkData();
  virtual void paintChart( QPaintDevice* paintdev ) = 0;
  virtual void drawData( QPainter* painter ) = 0;
  virtual void drawText( QPainter* painter ) = 0;
  virtual QPainter* setupPaintDev( QPaintDevice* paintdev );
  virtual QColor chooseDataColor( int dataset );

protected:
  KChart* _chart;
};

#endif
