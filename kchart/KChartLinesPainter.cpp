/* $Id$ */

#include "KChartLinesPainter.h"
#include "KoChartParameters.h"

#include <qpainter.h>

#include <math.h>

KChartLinesPainter::KChartLinesPainter( KoChartParameters* chart ) :
  KChartAxesPainter( chart )
{
}


KChartLinesPainter::~KChartLinesPainter()
{
}

void KChartLinesPainter::drawData( QPainter* painter, table_t* table )
{
  for( uint dataset = 0; dataset < numDatasets( table );
	   dataset++ ) {
	QColor datacolor = chooseDataColor( dataset );
	QPoint begin = valToPixel( 1, table->data[dataset][0],
							   dataset );
	Qt::PenStyle type = chooseLineType( dataset );

	for( uint i = 0; i <= maxPos( table ); i++ ) {
		// PENDING(kalle) How do I know this?
// 	  if( !_chart->chartData()->hasYValue( dataset, i ) )
// 		continue;

	  QPoint end = valToPixel( i+1, table->data[dataset][i],
							   dataset );

	  drawLine( painter, begin, end, type, datacolor, table );
	  begin = end;
	}
  }
}


void KChartLinesPainter::drawLegendMarker( QPainter* painter, int number,
					   int x, int y, table_t* /*table*/ )
{
  QColor datacolor = chooseDataColor( number );
  Qt::PenStyle type = chooseLineType( number );

  y += (int)rint( _chart->_legendelementheight / 2 );
  painter->setPen( QPen( datacolor, _chart->_linewidth, type ) );
  painter->drawLine( x, y, x + _chart->_legendmarkerwidth, y );
}


Qt::PenStyle KChartLinesPainter::chooseLineType( int dataset )
{
  if( _chart->_linetypes.count() > 0 )
	return *_chart->_linetypes.at( dataset % _chart->_linetypes.count() );

  // 5 is the number of defined pen styles in qpen.h, excluding NoPen
  return (Qt::PenStyle)( dataset % 5 + 1 );
}


void KChartLinesPainter::drawLine( QPainter* painter, const QPoint& begin,
				   const QPoint& end, Qt::PenStyle type,
				   const QColor& datacolor, table_t* /*table*/ )
{
  QPen pen( datacolor, _chart->_linewidth, type );

  painter->setPen( pen );
  painter->drawLine( begin, end );
}


