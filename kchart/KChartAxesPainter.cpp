/* $Id$ */

#include "KChartAxesPainter.h"
#include "KChartAreaPainter.h"
#include "KChartBarsPainter.h"
#include "KoDiagrammParameters.h"
#include "koDiagramm.h"

#include <qfontmet.h>
#include <qpaintd.h>
#include <qpainter.h>
#include <qpdevmet.h>
#include <qstrlist.h>

#include <math.h> // rint

KChartAxesPainter::KChartAxesPainter( KoDiagrammParameters* chart ) :
  KChartPainter( chart )
{
}


KChartAxesPainter::~KChartAxesPainter()
{
}


QPoint KChartAxesPainter::valToPixel( uint pos, double value, uint dataset )
{
  double ymin = ( _chart->_twoaxes && ( dataset == 2 ) ) ? _chart->_ymins[1] :
	_chart->_ymins[0];
  double ymax = ( _chart->_twoaxes && ( dataset == 2 ) ) ? _chart->_ymaxs[1] : _chart->_ymaxs[0];
  double ystep = ( _chart->_bottom - _chart->_top ) / ( ymax - ymin );
  return QPoint( (int)rint( _chart->_left + pos * _chart->_xstep ),
				 (int)rint( _chart->_bottom - ( value - ymin ) * ystep ) );
}



void KChartAxesPainter::paintChart( QPainter* painter, table_t* table )
{
  if( !checkData( table ) )
	return;

  setupLegend( painter->device(), table );
  if( !setupCoords( painter->device(), table ) ) // this can go wrong if the paintable size
	                             // is too small
	return;
  drawText( painter, table );
  drawAxes( painter, table );
  drawTicks( painter, table );
  drawData( painter, table );
  drawLegend( painter, table );
}


void KChartAxesPainter::setupLegend( QPaintDevice* /*paintdev*/, 
				     table_t* table )
{
  // No need to setup anything if there are no legends
  if( _chart->_legends.count() == 0 )
	return;

  int maxlen = 0;
  uint num = 0;
  // QPaintDeviceMetrics pdm( paintdev );
  
  const char* curlegend  = _chart->_legends.first();
  while( curlegend ) {
	// Check whether there is a legend defined at this position
	if( curlegend[0] != '\0' ) {
	  int len = strlen( curlegend );
	  maxlen = ( maxlen > len ) ? maxlen : len;
	  num++;
	}

	// We don't need more legends than we have datasets.
	if( num >= numDatasets( table ) )
	  break;
	
	curlegend = _chart->_legends.next();
  }

  _chart->_legendnum = num;

  // calculate the height and width of each element
  int textwidth = maxlen * _chart->_legendfontwidth;
  int legendheight = QMAX( _chart->_legendfontheight, _chart->_legendmarkerheight );

  _chart->_legendelementwidth = textwidth + _chart->_legendmarkerwidth +
	3 * _chart->_legendspacing;
  _chart->_legendelementheight = legendheight + 2 * _chart->_legendspacing;

  // position the legend to the right of the graph
  if( ( _chart->_legendplacement == RightTop ) ||
	  ( _chart->_legendplacement == RightCenter ) ||
	  ( _chart->_legendplacement == RightBottom ) ) {
	// Always work in one column
	_chart->_legendcols = 1;
	_chart->_legendrows = num;

	// Martien Verbruggen says: Just for completeness, might use this
	// in later versions
	_chart->_legendxsize = _chart->_legendcols * _chart->_legendelementwidth;
	_chart->_legendysize = _chart->_legendrows * _chart->_legendelementheight;

	// Adjust the right margin for the rest of the graph
	_chart->_rightmargin += _chart->_legendxsize;

	// Set the x starting point
	_chart->_legendxstart = _chart->_width - _chart->_rightmargin;
	
	// Set the y starting point, depending on alignment
	if( _chart->_legendplacement == RightTop )
	  _chart->_legendystart = _chart->_topmargin;
	else if( _chart->_legendplacement == RightCenter )
	  _chart->_legendystart = _chart->_height - _chart->_bottommargin - _chart->_legendysize;
	else { // RightBottom
	  int height = _chart->_height - _chart->_topmargin - _chart->_bottommargin;
	  _chart->_legendystart = (int)rint( _chart->_topmargin + height/2 -
								 _chart->_legendysize/2 );
	}
  }
  else { // position the legend below the graph
	// which width can we use
	int width = _chart->_width - _chart->_leftmargin - _chart->_rightmargin;

	// compute a number of columns if none is given by the user
	if ( _chart->_legendcols == -1 )
	  _chart->_legendcols = (int)rint( width/_chart->_legendelementwidth );
	_chart->_legendcols = QMIN( _chart->_legendcols, (int)num );
	
	_chart->_legendrows = (int)rint( num/_chart->_legendcols) + ( (num % _chart->_legendcols) ?
	  1 : 0 );

	_chart->_legendxsize = _chart->_legendcols * _chart->_legendelementwidth;
	_chart->_legendysize = _chart->_legendrows * _chart->_legendelementheight;

	// Adjust the bottom margin for the rest of the graph
	_chart->_bottommargin += _chart->_legendysize;

	// Set the y starting point
	_chart->_legendystart = _chart->_height - _chart->_bottommargin;

	// Set the x starting point, depending on alignment
	if( _chart->_legendplacement == BottomRight )
	  _chart->_legendxstart = _chart->_width - _chart->_rightmargin - _chart->_legendxsize;
	else if( _chart->_legendplacement == BottomLeft )
	  _chart->_legendxstart = _chart->_leftmargin;
	else // BottomCenter
	  _chart->_legendxstart = (int)rint( _chart->_leftmargin + width/2 -
								 _chart->_legendxsize/2 );
  }
}



bool KChartAxesPainter::setupCoords( QPaintDevice* /*paintdev*/, 
				     table_t* table )
{
  // sanity check
  if( numDatasets( table ) != 2 )
	_chart->_twoaxes = false;

  // Set text heights
  if( _chart->_title.isEmpty() )
	_chart->_textfontheight = 0;
  if( _chart->_xlabel.isEmpty() )
	_chart->_xlabelfontwidth = 0;

  if( _chart->_y1label.isEmpty() && !_chart->_ylabel.isEmpty() )
	_chart->_y1label = _chart->_ylabel;

  _chart->_ylabelfontheight1 = !_chart->_y1label.isEmpty();
  _chart->_ylabelfontheight2 = !_chart->_y2label.isEmpty();

  if( !_chart->_xplotvalues )
	_chart->_xaxisfontheight = 0;
  if( !_chart->_yplotvalues ) {
	_chart->_yaxisfontheight = 0;
	_chart->_yaxisfontwidth = 0;
  }

  bool labelheight = ( _chart->_xlabelfontheight ? 1 : 0 ) +
	( _chart->_xaxisfontheight ? 1 : 0 );

  // calculate top and bottom of bounding box
  // QPaintDeviceMetrics pdm( paintdev );
  _chart->_bottom = _chart->_height - _chart->_bottommargin - 1 -
	( _chart->_xlabelfontheight ? _chart->_xlabelfontheight : 0 ) -
	( _chart->_xaxisfontheight ? _chart->_xaxisfontheight : 0 ) -
	( labelheight ? labelheight * _chart->_textspacing : 0 );

  _chart->_top = _chart->_topmargin +
	( _chart->_titlefontheight ? ( _chart->_titlefontheight + _chart->_textspacing ) : 0 );
  if( _chart->_top == 0 )
	_chart->_top = _chart->_yaxisfontheight/2;

  if( !setMaxMin( table ) ) // can go wrong
	return false;

  // Create the labels for the y axes and calculate the max length
  createYLabels();

  // calculate left and right of bounding box
  int ls = _chart->_yaxisfontwidth * _chart->_ylabellength[0];
  _chart->_left = _chart->_leftmargin +
	( ls ? ( ls + _chart->_axisspace ) : 0 ) +
	( _chart->_ylabelfontheight1 ? ( _chart->_ylabelfontheight + _chart->_textspacing ) : 0 );

  if( _chart->_twoaxes )
	ls = _chart->_yaxisfontwidth * _chart->_ylabellength[1];
  _chart->_right = _chart->_width - _chart->_rightmargin - 1 -
	( _chart->_twoaxes ? 1 : 0 ) * ( ( ls ? ( ls + _chart->_axisspace ) : 0 ) +
							 ( _chart->_ylabelfontheight2 ? ( _chart->_ylabelfontheight2 + _chart->_textspacing ) : 0 ) );

  // calculate the step size for the x data
  int barbardistance = ( _chart->_right - _chart->_left ) / ( maxPos( table ) + 2 );
  _chart->_xstep = (int)rint( (double)barbardistance / ( 1.0 + (
																(double)_chart->_xbardist / 100.0 ) ) );

  // get the zero axis level
  QPoint zero = valToPixel( 0, 0.0, 1 );
  _chart->_zeropoint = zero.y();

  // Check size
  if( ( _chart->_bottom - _chart->_top ) <= 0 )
	return false;
  if( ( _chart->_right - _chart->_left ) <= 0 )
	return false;
	
  // set up the data colour list if it does not exist yet
  if( _chart->_datacolors.count() == 0 ) {
	_chart->_datacolors.setColor( 0, Qt::red );
	_chart->_datacolors.setColor( 1, Qt::green );
	_chart->_datacolors.setColor( 2, Qt::blue );
	_chart->_datacolors.setColor( 3, Qt::yellow );
	_chart->_datacolors.setColor( 4, Qt::magenta );
	_chart->_datacolors.setColor( 5, Qt::cyan );
	_chart->_datacolors.setColor( 6, Qt::darkYellow );
  }

  // more sanity checks
  if( _chart->_xlabelskip < 1 )
	_chart->_xlabelskip = 1;
  if( _chart->_ylabelskip < 1 )
	_chart->_ylabelskip = 1;
  if( _chart->_yticksnum < 1 )
	_chart->_yticksnum = 1;

  return true;
}


bool KChartAxesPainter::setMaxMin( table_t* table )
{
  double max = 0.0, min = 0.0;

  // Some decent values to start with
  if( _chart->_twoaxes ) {
	_chart->_ymaxs[0] = upperBound( maxValue( table->data[0] ) );
	_chart->_ymins[0] = lowerBound( minValue( table->data[0] ) );
	_chart->_ymaxs[1] = upperBound( maxValue( table->data[1] ) );
	_chart->_ymins[1] = lowerBound( minValue( table->data[1] ) );
  } else {
	minMaxOverallYValue( table, min, max );
	_chart->_ymaxs[0] = upperBound( max );
	_chart->_ymins[0] = lowerBound( min );
  }

  // Make sure bars and area always have zero offset
  if( _chart->_ymins[0] >= 0 )
	if( ( _charttype == Bars ) ||
		( _charttype == Area ) )
	  _chart->_ymins[0] = 0;

  // Override defaults with user-supplied values
  if( _chart->_ymin ) {
	_chart->_ymins[0] = _chart->_ymin;
	_chart->_ymins[1] = _chart->_ymin;
  }

  if( _chart->_ymax ) {
	_chart->_ymaxs[0] = _chart->_ymax;
	_chart->_ymaxs[1] = _chart->_ymax;
  }

  if( _chart->_y1min )
	_chart->_ymins[0] = _chart->_y1min;
  if( _chart->_y1max )
	_chart->_ymaxs[0] = _chart->_y1max;
  if( _chart->_y2min )
	_chart->_ymins[1] = _chart->_y2min;
  if( _chart->_y2max )
	_chart->_ymaxs[1] = _chart->_y2max;

  // Do we have sensible values by now?
  if( _chart->_twoaxes ) {
	if( _chart->_ymins[0] > minValue( table->data[0] ) )
	  return false;
	if( _chart->_ymaxs[0] < maxValue( table->data[0] ) )
	  return false;
	if( _chart->_ymins[1] > minValue( table->data[1] ) )
	  return false;
	if( _chart->_ymaxs[1] < maxValue( table->data[1] ) )
	  return false;
  } else {
	if( _chart->_ymins[0] > min )
	  return false;
	if( _chart->_ymaxs[0] < max )
	  return false;
  }

  return true;
}


void KChartAxesPainter::createYLabels()
{
  _chart->_ylabellength[0] = 0;
  _chart->_ylabellength[1] = 0;

  for( int t = 0; t <= _chart->_yticksnum; t++ )
	for( int a = 0; a <= ( _chart->_twoaxes ? 1 : 0 ); a++ ) {
	  double labelno = _chart->_ymins[a] + t *
		( _chart->_ymaxs[a] - _chart->_ymins[a] ) / _chart->_yticksnum;

	  // PENDING(kalle): Support number formats
	  QString labelstr;
	  labelstr.sprintf( "%f", labelno );
	  int len = labelstr.length();

	  _chart->_ylabels[a][t] = labelno;
	
	  if( len > _chart->_ylabellength[a] )
		_chart->_ylabellength[a] = len;
	}
}


double KChartAxesPainter::upperBound( double val )
{
  if( val == 0 )
	return 0.0;

  int sign;
  if( val >= 0 )
	sign = 1;
  else
	sign = -1;
  val = fabs( val );

  double exp = pow( 10, rint( log( val ) / log( 10 ) ) );
  double ret = (rint( val / exp ) + sign ) * exp;

  return sign * ret;
}


double KChartAxesPainter::lowerBound( double val )
{
  if( val == 0 )
	return 0.0;

  int sign;
  if( val >= 0 )
	sign = 1;
  else
	sign = -1;
  val = fabs( val );

  double exp = pow( 10, rint( log( val ) / log( 10 ) ) );
  double ret = (rint( val / exp ) - sign ) * exp;

  return sign * ret;
}



void KChartAxesPainter::drawText( QPainter* painter, table_t* /*table*/ )
{
  // title
  if( _chart->_titlefontheight ) {
	int titlex = _chart->_left + ( _chart->_right - _chart->_left ) / 2 -
	  _chart->_title.length() * _chart->_titlefontwidth / 2;
	int titley = _chart->_top - _chart->_textspacing - _chart->_titlefontheight;

	painter->setFont( _chart->titleFont() );
	painter->setPen( _chart->_textcolor );
	QFontMetrics fm( _chart->titleFont() );
	painter->drawText( titlex, titley, fm.width( _chart->_title ), fm.height(),
					   Qt::AlignLeft, _chart->_title );
  }

  // x label
  if( _chart->_xlabelfontheight ) {
	int xlabelx = 3 * ( _chart->_left  + _chart->_right ) / 4 -
	  _chart->_xlabel.length() * _chart->_xlabelfontwidth / 2;
	// QPaintDeviceMetrics pdm( painter->device() );
	int xlabely = _chart->_height - _chart->_xlabelfontheight - _chart->_bottommargin;

	painter->setFont( _chart->xLabelFont() );
	painter->setPen( _chart->_labelcolor );
	QFontMetrics fm( _chart->xLabelFont() );
	painter->drawText( xlabelx, xlabely, fm.width( _chart->_xlabel ), fm.height(),
					   Qt::AlignLeft, _chart->_xlabel );
  }

  // y label
  if( _chart->_ylabelfontheight1 ) {
	int ylabelx = _chart->_leftmargin;
	int ylabely = ( _chart->_bottom + _chart->_top ) / 2 +
	  _chart->_y1label.length() * _chart->_ylabelfontwidth/2;
	
	painter->setFont( _chart->yLabelFont() );
	painter->setPen( _chart->_labelcolor );
	QFontMetrics fm( _chart->yLabelFont() );
	painter->rotate( -90 );
	QPoint pos( ylabelx, ylabely );
	pos = painter->xFormDev( pos ); // convert position back to user coords
	painter->drawText( pos.x(), pos.y(), fm.width( _chart->_y1label ), fm.height(),
					   Qt::AlignLeft, _chart->_y1label );
	painter->resetXForm();
  }

  // ylabel 2 (if needed)
  if( _chart->_twoaxes && _chart->_ylabelfontheight2 ) {
        // QPaintDeviceMetrics pdm( painter->device() );
	int ylabel2x = _chart->_width - _chart->_ylabelfontheight - _chart->_rightmargin;
	int ylabel2y = ( _chart->_bottom - _chart->_top ) / 2 +
	  _chart->_y2label.length() * _chart->_ylabelfontwidth / 2;

	painter->setFont( _chart->yLabelFont() );
	painter->setPen( _chart->_labelcolor );
	QFontMetrics fm( _chart->yLabelFont() );
	painter->rotate( 90 );
	QPoint pos( ylabel2x, ylabel2y );
	pos = painter->xFormDev( pos ); // convert position back to user coords
	painter->drawText( pos.x(), pos.y(), fm.width( _chart->_y2label ), fm.height(),
					   Qt::AlignLeft, _chart->_y2label );
	painter->resetXForm();
  }
}


void KChartAxesPainter::drawAxes( QPainter* painter, table_t*  )
{
  if( _chart->_boxaxis ) {
	painter->setPen( _chart->_fgcolor );
	painter->setBrush( Qt::NoBrush );
	painter->drawRect( QRect( QPoint( _chart->_left, _chart->_top ),
							  QPoint( _chart->_right, _chart->_bottom) ) );
  } else {
	painter->setPen( _chart->_fgcolor );
	painter->setBrush( Qt::NoBrush );
	painter->drawLine( _chart->_left, _chart->_top, _chart->_left, _chart->_bottom );
	if( !_chart->_zeroaxisonly )
	  painter->drawLine( _chart->_left, _chart->_bottom, _chart->_right, _chart->_bottom );
	if( _chart->_twoaxes )
	  painter->drawLine( _chart->_right, _chart->_bottom, _chart->_right, _chart->_top );
  }

  if( _chart->_zeroaxis || _chart->_zeroaxisonly ) {
	QPoint xy = valToPixel( 0, 0, 1 );
	painter->drawLine( _chart->_left, xy.y(), _chart->_right, xy.y() );
  }
}


void KChartAxesPainter::drawTicks( QPainter* painter, table_t* table )
{
  // Ticks and values for Y axes
  for( int t = 0; t <= _chart->_yticksnum; t++ ) {
	for( int a = 0; a <= ( _chart->_twoaxes ? 1: 0 ); a++ ) {
	  double label = _chart->_ylabels[a][t];

	  QPoint xy = valToPixel( a * ( maxPos( table ) + 2 ),
							  label, a + 1 );

	  if( _chart->_longticks && a == 0 ) {
		painter->setPen( _chart->_fgcolor );
		painter->drawLine( xy.x(), xy.y(), xy.x() + _chart->_right - _chart->_left, xy.y() );
	  } else {
		painter->setPen( _chart->_fgcolor );
		painter->drawLine( xy.x(), xy.y(),
						   xy.x() + (3-2*(a-1))*_chart->_ticklength, xy.y() );
	  }
	
	  // Skip unwanted labels
	  if( ( t % _chart->_ylabelskip ) || ! _chart->_yplotvalues )
		continue;

	  // PENDING(kalle) Use number formatter here
	  QString labelstr;
	  labelstr.sprintf( "%g", label );
	  int x = xy.x();
	  int y = xy.y();
	  x -= (1-a)*labelstr.length() * _chart->_yaxisfontwidth +
		(1-2*a) * _chart->_axisspace;
	  y -= _chart->_yaxisfontheight/2;
	  xy.setX( x );
	  xy.setY( y );
	
	  painter->setFont( _chart->yAxisFont() );
	  painter->setPen( _chart->_axislabelcolor );
	  QFontMetrics fm( _chart->yAxisFont() );
	  painter->drawText( xy.x(), xy.y(),
						 fm.width( labelstr ), fm.height(), Qt::AlignLeft, labelstr );
	}
  }

  // Don't draw the x ticks and values if this is not desired
  if( !_chart->_xplotvalues )
	return;


  // Ticks and values for x axis
  for( uint i = 0; i <= maxPos( table ); i++ ) {
	QPoint xy = valToPixel( i+1, 0, 1 );
	if( !_chart->_zeroaxisonly )
	  xy.setY( _chart->_bottom );

	if( _chart->_xticks ) {
	  if( _chart->_longticks ) {
		painter->setPen( _chart->_fgcolor );
		painter->drawLine( xy.x(), _chart->_bottom, xy.x(), _chart->_top );
	  } else {
		painter->setPen( _chart->_fgcolor );
		painter->drawLine( xy.x(), xy.y(), xy.x(), xy.y() - _chart->_ticklength );
	  }
	}

	// Skip unwanted labels, but always draw last tick
	if( ( i % _chart->_xlabelskip ) && ( i != maxPos( table ) ) )
	  continue;

	int x = xy.x();
	QString text = table->xDesc.at( i );
	x -= _chart->_xaxisfontwidth * text.length()/2;
	xy.setX( x );
	int yt = xy.y() + _chart->_textspacing/2;
	painter->setPen( _chart->_axislabelcolor );
	painter->setFont( _chart->xAxisFont() );
	QFontMetrics fm( _chart->xAxisFont() );
	painter->drawText( xy.x(), yt, fm.width( text ), fm.height(),
					   Qt::AlignLeft, text );
  }
}


void KChartAxesPainter::drawLegend( QPainter* painter, table_t* table )
{
  // No need to do anything if there are no legends
  if( _chart->_legends.count() == 0 )
	return;

  int xl = _chart->_legendxstart + _chart->_legendspacing;
  int y = _chart->_legendystart + _chart->_legendspacing - 1;

  int row = 1;
  int x = xl; // start position of current element

  for( uint legend = 0; legend < _chart->_legends.count(); legend++ ) {
	if( legend  >= numDatasets( table ) )
	  break;

	int xe = x; // position within an element
	if( _chart->_legends.at( legend ) == 0 || strlen( _chart->_legends.at( legend ) ) == 0 )
	  continue;

	drawLegendMarker( painter, legend, xe, y, table );

	xe += _chart->_legendmarkerwidth + _chart->_legendspacing;
	int ys = (int)rint( y + _chart->_legendelementheight/2 - _chart->_legendfontheight/2 );
	
	QFontMetrics fm( _chart->legendFont() );
	painter->setFont( _chart->legendFont() );
	painter->setPen( _chart->_fgcolor );
	painter->drawText( xe, ys, fm.width( _chart->_legends.at( legend ) ),
					   fm.height(), Qt::AlignLeft, _chart->_legends.at( legend ) );

	x += _chart->_legendelementwidth;

	if( ++row > _chart->_legendcols ) {
	  row = 1;
	  y += _chart->_legendelementheight;
	  x = xl;
	}
  }
}


void KChartAxesPainter::drawLegendMarker( QPainter* painter, int number,
					  int x, int y, table_t*  )
{
  QColor datacolor = chooseDataColor( number );

  y += (int)rint( _chart->_legendelementheight/2 - _chart->_legendmarkerheight/2 );

  QBrush filledbrush( datacolor, Qt::SolidPattern );
  QBrush emptybrush( datacolor, Qt::NoBrush );
  QRect rect( QPoint( x, y ),
			  QPoint( x + _chart->_legendmarkerwidth, y + _chart->_legendmarkerheight ) );
  painter->setBrush( filledbrush );
  painter->setPen( datacolor );
  painter->drawRect( rect );
  painter->setBrush( emptybrush );
  painter->setPen( _chart->_accentcolor );
  painter->drawRect( rect );
}


