/* $Id$ */

#include "KChartBarsPainter.h"
#include "koChart.h"
#include "KoChartParameters.h"

#include <qpainter.h>

#include <math.h> // rint

KChartBarsPainter::KChartBarsPainter( KoChartParameters* chart ) :
  KChartAxesPainter( chart )
{
}


KChartBarsPainter::~KChartBarsPainter()
{
}

void KChartBarsPainter::drawData( QPainter* painter, table_t* table )
{
  int left = 0, right = 0;

  if( ( _chart->_overwrite == InFront ) || ( _chart->_overwrite == OnTop ) ) {
	// loop over positions
	for( uint i = 0; i <= maxPos( table ); i++ ) {
	  int bottom = _chart->_zeropoint;

	  // loop over datasets
	  for( uint dataset = 0; dataset < numDatasets( table );
		   dataset++ ) {
		// Don't draw anything if there is no point defined
		  // PENDING(kalle) How do I know this?
// 		if( !_chart->chartData()->hasYValue( dataset, i ) )
// 		  continue;

		// Choose a colour for the dataset being drawn
		QColor datacolor = chooseDataColor( dataset );
		painter->setPen( datacolor );
		// Create two brushes for filled and non-filled rectangles
		QBrush filledbrush( datacolor, Qt::SolidPattern );
		QBrush emptybrush( datacolor, Qt::NoBrush );
		
		// get coordinates of top and center of bar
		QPoint xpt = valToPixel( i+1,
								 table->data[dataset][i],
								 dataset );

		// calculate left and right of bar
		int left = xpt.x() - (int)rint( _chart->_xstep/2 );
		int right = xpt.x() + (int)rint( _chart->_xstep/2 );

		// Move to the right if there is a bar distance set.
		left += (int)rint( ( _chart->_xstep * (
											   ((double)_chart->_xbardist)/100.0 ) ) * (i+1) );
		right += (int)rint( ( _chart->_xstep * (
												((double)_chart->_xbardist)/100.0 ) ) * (i+1) );

		// calculate new top
		int t = xpt.y();
		if( _chart->_overwrite == OnTop )
		  t -= _chart->_zeropoint - bottom;
		xpt.setY( t );

		// draw the bar
		if( table->data[dataset][i] >= 0 ) {
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
	for( uint dataset = 0; dataset < numDatasets( table );
		 dataset++ ) {
	  // Choose a colour for the dataset being drawn
	  QColor datacolor = chooseDataColor( dataset );
	  painter->setPen( datacolor );
	  // Create two brushes for filled and non-filled rectangles
	  QBrush filledbrush( datacolor, Qt::SolidPattern );
	  QBrush emptybrush( datacolor, Qt::NoBrush );

	  // loop over the positions in the current dataset
	  for( uint i = 0; i <= maxPos( table ); i++ ) {
		// skip positions which have no data in this dataset
		  // PENDING(kalle) How do I know this?
// 		if( !_chart->chartData()->hasYValue( dataset, i ) )
// 		  continue;
		
		// get coordinates of top and center of bar
		QPoint coords = valToPixel( i+1,
									table->data[dataset][i],
									dataset );
		// calculate left and right
		left = coords.x() - _chart->_xstep/2 +
		  (int)rint( dataset * _chart->_xstep / numDatasets( table ) );
		right = coords.x() - _chart->_xstep/2 +
		  (int)rint( (dataset+1) * _chart->_xstep / numDatasets( table ) );

		// Move to the right if there is a bar distance set.
		left += (int)rint( ( _chart->_xstep * (
											   ((double)_chart->_xbardist)/100.0 ) ) * (i+1) );
		right += (int)rint( ( _chart->_xstep * (
												((double)_chart->_xbardist)/100.0 ) ) * (i+1) );


		// and finally draw the bar
		painter->setBrush( filledbrush );
		painter->setPen( _chart->_accentcolor );
		if( table->data[dataset][i] >= 0 ) {
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


