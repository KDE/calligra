/* $Id$ */

#include "KChartPointsPainter.h"
#include "KChart.h"

#include <qpainter.h>

KChartPointsPainter::KChartPointsPainter( KChart* chart ) :
  KChartAxesPainter( chart )
{
  fatal( "Sorry, not implemented: KChartPointsPainter::KChartPointsPainter" );
}


KChartPointsPainter::~KChartPointsPainter()
{
  fatal( "Sorry, not implemented: KChartPointsPainter::~KChartPointsPainter" );
}


void KChartPointsPainter::drawData( QPainter* )
{
  fatal( "Sorry, not implemented: KChartPointsPainter::drawData" );
}

void KChartPointsPainter::drawLegendMarker( QPainter* /*painter*/, int /*number*/, 
											int /*x*/, int /*y*/ )
{
  fatal( "Sorry, not implemented: KChartPointsPainter::drawLegendMarker" );
}


