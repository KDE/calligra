/* $Id$ */

#include "KChartLinesPointsPainter.h"
#include "KChart.h"

#include <qpainter.h>

KChartLinesPointsPainter::KChartLinesPointsPainter( KChart* chart ) :
  KChartLinesPainter( chart ),
  KChartPointsPainter( chart )
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::KChartLinesPointsPainter" );
}


KChartLinesPointsPainter::~KChartLinesPointsPainter()
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::~KChartLinesPointsPainter" );
}


void KChartLinesPointsPainter::drawData( QPainter* )
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::drawData" );
}

void KChartLinesPointsPainter::drawLegendMarker( QPainter* /*painter*/, 
												 int /*number*/, int /*x*/, int /*y*/ )
{
  fatal( "Sorry, not implemented: KChartLinesPointsPainter::drawLegendMarker" );
}
