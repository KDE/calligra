/* $Id$ */

#include "KChartBarsPainter.h"
#include "KChart.h"
#include "KChartData.h"

#include <qpainter.h>

#include <math.h> // rint

KChartBarsPainter::KChartBarsPainter( KChart* chart ) :
  KChartAxesPainter( chart )
{
}


KChartBarsPainter::~KChartBarsPainter()
{
}

void KChartBarsPainter::drawData( QPainter* painter )
{
  int left = 0, right = 0;

  if( ( _chart->_overwrite == InFront ) || ( _chart->_overwrite == OnTop ) ) {
	// loop over positions
	for( uint i = 0; i <= _chart->chartData()->maxPos(); i++ ) {
	  int bottom = _chart->_zeropoint;

	  // loop over datasets
	  for( int dataset = 0; dataset < _chart->chartData()->numDatasets(); 
		   dataset++ ) {
		// Don't draw anything if there is no point defined
		if( !_chart->chartData()->hasYValue( dataset, i ) )
		  continue;

		// Choose a colour for the dataset being drawn
		QColor datacolor = chooseDataColor( dataset );
		painter->setPen( datacolor );
		// Create two brushes for filled and non-filled rectangles
		QBrush filledbrush( datacolor, SolidPattern );
		QBrush emptybrush( datacolor, NoBrush );
		
		// get coordinates of top and center of bar
		QPoint xpt = valToPixel( i+1, 
								 _chart->chartData()->yValue( dataset, i ), 
								 dataset );

		// calculate left and right of bar
		int left = xpt.x() - (int)rint( _chart->_xstep/2 );
		int right = xpt.x() + (int)rint( _chart->_xstep/2 );
		
		// calculate new top
		int t = xpt.y();
		if( _chart->_overwrite == OnTop )
		  t -= _chart->_zeropoint - bottom;
		xpt.setY( t );

		// draw the bar
		if( _chart->chartData()->yValue( dataset, i ) >= 0 ) {
		  QRect rect( QPoint( left, xpt.y() ), QPoint( right, bottom ) );
		  painter->setBrush( filledbrush );
		  painter->setPen( datacolor );
		  painter->drawRect( rect );
		  painter->setBrush( emptybrush );
		  painter->setPen( _chart->_accentcolor );
		  painter->drawRect( rect );
		} else {
		  QRect rect( QPoint( left, bottom ), QPoint( right, xpt.y() ) );
		  painter->setBrush( filledbrush );
		  painter->setPen( datacolor );
		  painter->drawRect( rect );
		  painter->setBrush( emptybrush );
		  painter->setPen( _chart->_accentcolor );
		}

		// reset bottom to the top
		if( _chart->_overwrite == OnTop )
		  bottom = xpt.y();
	  }
	}

	// redraw the zero axis
	painter->setPen( _chart->_fgcolor );
	painter->drawLine( _chart->_left, _chart->_zeropoint, 
					   _chart->_right, _chart->_zeropoint );
  } else {
	// loop over the dataset
	for( int dataset = 0; dataset < _chart->chartData()->numDatasets(); 
		 dataset++ ) {
	  // Choose a colour for the dataset being drawn
	  QColor datacolor = chooseDataColor( dataset );
	  painter->setPen( datacolor );
	  // Create two brushes for filled and non-filled rectangles
	  QBrush filledbrush( datacolor, SolidPattern );
	  QBrush emptybrush( datacolor, NoBrush );

	  // loop over the positions in the current dataset
	  for( uint i = 0; i <= _chart->chartData()->maxPos(); i++ ) {
		// skip positions which have no data in this dataset
		if( !_chart->chartData()->hasYValue( dataset, i ) )
		  continue;
		
		// get coordinates of top and center of bar
		QPoint coords = valToPixel( i+1, 
									_chart->chartData()->yValue( dataset, i ),
									dataset );
		// calculate left and right
		left = coords.x() - _chart->_xstep/2 + 
		  (int)rint( dataset * _chart->_xstep / _chart->chartData()->numDatasets() );
		right = coords.x() - _chart->_xstep/2 +
		  (int)rint( (dataset+1) * _chart->_xstep / _chart->chartData()->numDatasets() );

		// and finally draw the bar
		painter->setBrush( filledbrush );
		painter->setPen( _chart->_accentcolor );
		if( _chart->chartData()->yValue( dataset, i ) >= 0 ) {
		  // positive value
		  QRect rect( QPoint( left, coords.y() ), 
					  QPoint( right, _chart->_zeropoint ) );
		  painter->drawRect( rect );
		} else {
		  // negative value
		  QRect rect( QPoint( left, _chart->_zeropoint ),
					  QPoint( right, coords.y() ) );
		  painter->drawRect( rect );
		}
	  }
	}
	
	// redraw the zero axis
	painter->setPen( _chart->_fgcolor );
	painter->drawLine( left, _chart->_zeropoint, right, _chart->_zeropoint );
  }
}


