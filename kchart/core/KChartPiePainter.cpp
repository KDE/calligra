/* $Id$ */

#include "KChartPiePainter.h"
#include "KChart.h"

#include <qpainter.h>
#include <qstrlist.h>

KChartPiePainter::KChartPiePainter( KChart* chart ) :
  KChartPainter( chart )
{
  fatal( "Sorry, not implemented: KChartPiePainter::KChartPiePainter" );
}


KChartPiePainter::~KChartPiePainter()
{
  fatal( "Sorry, not implemented: KChartPiePainter::~KChartPiePainter" );
}


void KChartPiePainter::paintChart( QPaintDevice* /* paintdev */ )
{
  fatal( "Sorry, not implemented: KChartPiePainter::paintGraph" );
}


void KChartPiePainter::drawData( QPainter* )
{
  fatal( "Sorry, not implemented: KChartPiePainter::drawData" );
}


QColor KChartPiePainter::chooseDataColor( int /* dataset */ )
{
  fatal( "Sorry, not implemented: KChartPiePainter::chooseDataColor" );
  return QColor();
}


bool KChartPiePainter::setupCoords( QPaintDevice* )
{
  fatal( "Sorry, not implemented: KChartPiePainter::setupCoords" );
  return false;
}


void KChartPiePainter::drawText( QPainter* /* painter */ )
{
  fatal( "Sorry, not implemented: KChartPiePainter::drawText" );
}


