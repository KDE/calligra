/* $Id$ */

#include "KChartPointsPainter.h"
#include "KoDiagrammParameters.h"

#include <qpainter.h>

KChartPointsPainter::KChartPointsPainter( KoDiagrammParameters* chart ) :
  KChartAxesPainter( chart )
{
  fatal( "Sorry, not implemented: KChartPointsPainter::KChartPointsPainter" );
}


KChartPointsPainter::~KChartPointsPainter()
{
  fatal( "Sorry, not implemented: KChartPointsPainter::~KChartPointsPainter" );
}


void KChartPointsPainter::drawData( QPainter*, table_t* /*table*/ )
{
  fatal( "Sorry, not implemented: KChartPointsPainter::drawData" );
}

void KChartPointsPainter::drawLegendMarker( QPainter* /*painter*/, int /*number*/, 
											int /*x*/, int /*y*/,
											table_t* /*table*/ )
{
  fatal( "Sorry, not implemented: KChartPointsPainter::drawLegendMarker" );
}


