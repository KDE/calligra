/* $Id$ */

#ifndef _KCHARTPIE3DPAINTER_H
#define _KCHARTPIE3DPAINTER_H

#include "KChartPiePainter.h"

class KChart;

class KChartPie3DPainter : public KChartPiePainter
{
public:
  KChartPie3DPainter( KChart* chart );
  ~KChartPie3DPainter();

protected:
	virtual void drawData( QPainter* painter );
	virtual void drawPie( QPainter* painter );
	bool setupCoords( QPaintDevice* );


private:
};

#endif
