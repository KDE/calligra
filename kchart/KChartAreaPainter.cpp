/* $Id$ */

#include "KChartAreaPainter.h"
#include "koChart.h"
#include "KoChartParameters.h"

#include <qpainter.h>
#include <qpen.h>

KChartAreaPainter::KChartAreaPainter( KoChartParameters* chart ) :
  KChartAxesPainter( chart )
{
	_charttype = Area;
}


KChartAreaPainter::~KChartAreaPainter()
{
}


void KChartAreaPainter::drawData( QPainter* painter, table_t* table )
{
  for( uint ds = 0;
	   ds < numDatasets( table ); ds++ ) {
	int pointno = 0;

	// Select a data color
	QColor datacolor = chooseDataColor( ds );
	
	// Create a new point array
	QPointArray points( maxPos( table ) + 3 );

	// Add the first 'zero' point
	QPoint xy = valToPixel( 1, 0, ds );
	points.setPoint( pointno, xy ); pointno++;

	// Add the data points
	for( uint i = 0; i <= maxPos( table ); i++ ) {
		// PENDING(kalle) How do I know this?
// 	  if( !_chart->chartData()->hasYValue( ds, i ) )
// 		continue;
	
	  xy = valToPixel( i+1, table->data[ds][i],
					   ds );
	  points.setPoint( pointno, xy ); pointno++;
	}

	// Add the last zero point
	xy = valToPixel( pointno-1, 0, ds );
	points.setPoint( pointno, xy );

	// Draw a filled line and a line polygon
	QBrush filledbrush( datacolor, Qt::SolidPattern );
	QBrush emptybrush( _chart->_accentcolor, Qt::NoBrush );
	painter->setPen( datacolor );
	painter->setBrush( filledbrush );
	painter->drawPolygon( points );
	painter->setPen( _chart->_accentcolor );
	painter->setBrush( emptybrush );
	painter->drawPolygon( points );

	// Draw the accent lines
	QPen dashedpen( _chart->_accentcolor, 0, Qt::DashLine );
	painter->setPen( dashedpen );
	for( uint i = 1; i < maxPos( table ); i++ ) {
		// PENDING(kalle) How do I know this?
// 	  if( !_chart->chartData()->hasYValue( ds, i ) )
// 		continue;

	  xy = valToPixel( i+1, table->data[ds][i],
					   ds );
	  painter->drawLine( xy.x(), xy.y(), xy.x(), _chart->_zeropoint );
	}
  }
}




