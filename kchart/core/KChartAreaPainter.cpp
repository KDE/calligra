/* $Id$ */

#include "KChartAreaPainter.h"
#include "KChart.h"

#include <qpainter.h>
#include <qpen.h>

KChartAreaPainter::KChartAreaPainter( KChart* chart ) :
  KChartAxesPainter( chart )
{
}


KChartAreaPainter::~KChartAreaPainter()
{
}


void KChartAreaPainter::drawData( QPainter* painter )
{
  for( int ds = 0; 
	   ds < _chart->chartData()->numDatasets(); ds++ ) {
	int pointno = 0;

	// Select a data color
	QColor datacolor = chooseDataColor( ds );
	
	// Create a new point array
	QPointArray points( _chart->chartData()->maxPos() + 1 );

	// Add the first 'zero' point
	QPoint xy = valToPixel( 1, 0, ds );
	points.setPoint( pointno, xy ); pointno++;

	// Add the data points
	for( uint i = 0; i <= _chart->chartData()->maxPos(); i++ ) {
	  if( !_chart->chartData()->hasYValue( ds, i ) )
		continue;
	  
	  xy = valToPixel( i+1, _chart->chartData()->yValue( ds, i ), 
					   ds );
	  points.setPoint( pointno, xy ); pointno++;
	}

	// Add the last zero point
	xy = valToPixel( pointno, 0, ds );
	points.setPoint( pointno, xy );

	// Draw a filled line and a line polygon
	QBrush filledbrush( datacolor, SolidPattern );
	QBrush emptybrush( _accentcolor, NoBrush );
	painter->setPen( datacolor );
	painter->setBrush( filledbrush );
	painter->drawPolygon( points );
	painter->setPen( _accentcolor );
	painter->setBrush( emptybrush );
	painter->drawPolygon( points );

	// Draw the accent lines
	QPen dashedpen( _accentcolor, 0, DashLine );
	painter->setPen( dashedpen );
	for( uint i = 1; i < _chart->chartData()->maxPos(); i++ ) {
	  if( !_chart->chartData()->hasYValue( ds, i ) )
		continue;

	  xy = valToPixel( i+1, _chart->chartData()->yValue( ds, i ),
					   ds );
	  painter->drawLine( xy.x(), xy.y(), xy.x(), _zeropoint );
	}
  }
}




