/* $Id$ */

#include "KChartLinesPointsPainter.h"
#include "KoChartParameters.h"

#include <qpainter.h>

KChartLinesPointsPainter::KChartLinesPointsPainter( KoChartParameters* chart ) :
  KChartLinesPainter( chart ),
  KChartPointsPainter( chart )
{
	KChartAxesPainter::_charttype = LinesPoints;
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::KChartLinesPointsPainter" );
}


KChartLinesPointsPainter::~KChartLinesPointsPainter()
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::~KChartLinesPointsPainter" );
}


void KChartLinesPointsPainter::drawData( QPainter*, table_t* /* table */ )
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::drawData" );
}

void KChartLinesPointsPainter::drawLegendMarker( QPainter* /*painter*/, 
												 int /*number*/, int
												 /*x*/, int /*y*/,
												 table_t* /*table*/  )
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::drawLegendMarker" );
}
