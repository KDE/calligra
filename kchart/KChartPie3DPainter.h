/* $Id$ */

#ifndef _KCHARTPIE3DPAINTER_H
#define _KCHARTPIE3DPAINTER_H

#include "KChartPiePainter.h"

class table_t;

class KChartPie3DPainter : public KChartPiePainter
{
public:
  KChartPie3DPainter( KoChartParameters* chart );
  ~KChartPie3DPainter();

protected:
	virtual void drawData( QPainter* painter, table_t* table );
	virtual void drawPie( QPainter* painter, table_t* table );
	bool setupCoords( QPaintDevice* );


private:
};

#endif
