/* $Id$ */

#include "KChartLinesPainter.h"
#include "KChart.h"

#include <qpainter.h>

#include <math.h>

KChartLinesPainter::KChartLinesPainter( KChart* chart ) :
  KChartAxesPainter( chart )
{
  _linewidth = 1;
  PenStyle* myint = new PenStyle;
  *myint = SolidLine;
  _linetypes.setAutoDelete( true );
  _linetypes.append( myint );
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

  y += (int)rint( _legendelementheight / 2 );
  painter->setPen( QPen( datacolor, _linewidth, type ) );
  painter->drawLine( x, y, x + _legendmarkerwidth, y );
}


PenStyle KChartLinesPainter::chooseLineType( int dataset )
{
  if( _linetypes.count() > 0 )
	return *_linetypes.at( dataset % _linetypes.count() );

  // 5 is the number of defined pen styles in qpen.h, excluding NoPen
  return (PenStyle)( dataset % 5 + 1 );
}


void KChartLinesPainter::drawLine( QPainter* painter, const QPoint& begin,
								   const QPoint& end, PenStyle type,
								   const QColor& datacolor )
{
  QPen pen( datacolor, _linewidth, type );

  painter->setPen( pen );
  painter->drawLine( begin, end );
}


void KChartLinesPainter::setLineWidth( int width )
{
  _linewidth = width;
}

int KChartLinesPainter::lineWidth() const
{
  return _linewidth;
}

  
void KChartLinesPainter::setLineTypes( PenStyle types[], int number )
{
  _linetypes.clear();
  for( int i = 0; i < number; i++ ) 
	_linetypes.append( new PenStyle( types[i] ) );
}

void KChartLinesPainter::lineTypes( PenStyle types[], int& number )
{
  for( uint i = 0; i < _linetypes.count(); i++ )
	types[ i ] = *_linetypes.at( i );

  number = _linetypes.count();
}
