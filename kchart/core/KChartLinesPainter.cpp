/* $Id$ */

#include "KChartLinesPainter.h"
#include "KChart.h"

#include <qpainter.h>

#include <math.h>

KChartLinesPainter::KChartLinesPainter( KChart* chart ) :
  KChartAxesPainter( chart )
{
  _chart->_linewidth = 1;
  PenStyle* myint = new PenStyle;
  *myint = SolidLine;
  _chart->_linetypes.setAutoDelete( true );
  _chart->_linetypes.append( myint );
}


KChartLinesPainter::~KChartLinesPainter()
{
}

void KChartLinesPainter::drawData( QPainter* painter )
{
  for( int dataset = 0; dataset < _chart->chartData()->numDatasets();
	   dataset++ ) {
	QColor datacolor = chooseDataColor( dataset );
	QPoint begin = valToPixel( 1, _chart->chartData()->yValue( dataset, 0 ),
							   dataset );
	PenStyle type = chooseLineType( dataset );

	for( uint i = 0; i <= _chart->chartData()->maxPos(); i++ ) {
	  if( !_chart->chartData()->hasYValue( dataset, i ) )
		continue;

	  QPoint end = valToPixel( i+1, _chart->chartData()->yValue( dataset, i ),
							   dataset );

	  drawLine( painter, begin, end, type, datacolor );
	  begin = end;
	}
  }
}


void KChartLinesPainter::drawLegendMarker( QPainter* painter, int number, 
										   int x, int y )
{
  QColor datacolor = chooseDataColor( number );
  PenStyle type = chooseLineType( number );

  y += (int)rint( _chart->_legendelementheight / 2 );
  painter->setPen( QPen( datacolor, _chart->_linewidth, type ) );
  painter->drawLine( x, y, x + _chart->_legendmarkerwidth, y );
}


PenStyle KChartLinesPainter::chooseLineType( int dataset )
{
  if( _chart->_linetypes.count() > 0 )
	return *_chart->_linetypes.at( dataset % _chart->_linetypes.count() );

  // 5 is the number of defined pen styles in qpen.h, excluding NoPen
  return (PenStyle)( dataset % 5 + 1 );
}


void KChartLinesPainter::drawLine( QPainter* painter, const QPoint& begin,
								   const QPoint& end, PenStyle type,
								   const QColor& datacolor )
{
  QPen pen( datacolor, _chart->_linewidth, type );

  painter->setPen( pen );
  painter->drawLine( begin, end );
}


