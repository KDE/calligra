/* $Id$ */

#include "KChartAxesPainter.h"
#include "KChartAreaPainter.h"
#include "KChartBarsPainter.h"
#include "KChart.h"
#include "KChartData.h"

#include <qfontmet.h>
#include <qpaintd.h>
#include <qpainter.h>
#include <qpdevmet.h>
#include <qstrlist.h>

#include <math.h> // rint
#include <typeinfo>

KChartAxesPainter::KChartAxesPainter( KChart* chart ) :
  KChartPainter( chart )
{
  /* Set some defaults that apply for all charts with axes */
  _ticklength = 4; // length of the tick marks
  _longticks = false; // do the ticks cross the whole chart
  _yticksnum = 5; // number of ticks on Y axis
  _xlabelskip = 1; // number of labels skipped on X axis
  _ylabelskip = 1; // number of labels skipped on Y axis
  _xticks = true; // show ticks on X axis?
  _boxaxis = true; // draw right and top of axes?
  _twoaxes = false; // two axes for first and second dataset?
  _xplotvalues = true; // print value on X axis?
  _yplotvalues = true; // print value on Y axis?
  _axisspace = 4; // space between axis and text
  _overwrite = SideBySide; // draw bars on top of each other?
  _zeroaxis = true; // draw zero axis if there are negative values?
  _zeroaxisonly = true; // draw zero axis, but not bottom axis?
  _legendmarkerheight = 8; // legend parameters
  _legendmarkerwidth = 12; 
  _legendspacing = 4;
  _legendplacement = BottomCenter;
  _legendcols = -1; // not user-defined by default
  _legendrows = -1; // not user-defined by default
  // Y number format not yet supported

  /* set some font defaults */
  setXLabelFont( QFont( "courier", 12 ) );
  setYLabelFont( QFont( "courier", 12 ) );
  setXAxisFont( QFont( "courier", 10 ) );
  setYAxisFont( QFont( "courier", 10 ) );
  setLegendFont( QFont( "courier", 10 ) );
}


KChartAxesPainter::~KChartAxesPainter()
{
}


void KChartAxesPainter::setXLabelFont( QFont font )
{
  _xlabelfont = font;

  // compute metrics
  QFontMetrics fm( _xlabelfont );
  _xlabelfontwidth = fm.width( 'm' );
  _xlabelfontheight = fm.height();
}


void KChartAxesPainter::setYLabelFont( QFont font )
{
  _ylabelfont = font;

  // compute metrics
  QFontMetrics fm( _ylabelfont );
  _ylabelfontwidth = fm.width( 'm' );
  _ylabelfontheight = fm.height();
}


void KChartAxesPainter::setXAxisFont( QFont font )
{
  _xaxisfont = font;

  // compute metrics
  QFontMetrics fm( _xaxisfont );
  _xaxisfontwidth = fm.width( 'm' );
  _xaxisfontheight = fm.height();
}


void KChartAxesPainter::setYAxisFont( QFont font )
{
  _yaxisfont = font;

  // compute metrics
  QFontMetrics fm( _yaxisfont );
  _yaxisfontwidth = fm.width( 'm' );
  _yaxisfontheight = fm.height();
}


void KChartAxesPainter::setLegendFont( QFont font )
{
  _legendfont = font;

  // compute metrics
  QFontMetrics fm( _legendfont );
  _legendfontwidth = fm.width( 'm' );
  _legendfontheight = fm.height();
}


QPoint KChartAxesPainter::valToPixel( uint pos, double value, uint dataset )
{
  double ymin = ( _twoaxes && ( dataset == 2 ) ) ? _ymins[1] : _ymins[0];
  double ymax = ( _twoaxes && ( dataset == 2 ) ) ? _ymaxs[1] : _ymaxs[0];
  double ystep = ( _bottom - _top ) / ( ymax - ymin );
  return QPoint( (int)rint( _left + pos * _xstep ),
				 (int)rint( _bottom - ( value - ymin ) * ystep ) );
}



void KChartAxesPainter::paintChart( QPaintDevice* paintdev )
{
  if( !checkData() )
	return;

  setupLegend( paintdev );
  if( !setupCoords( paintdev ) ) // this can go wrong if the paintable size 
	                             // is too small
	return;
  QPainter* painter = setupPaintDev( paintdev ); // creates a painter, sets values and calls begin()

  drawText( painter );
  drawAxes( painter );
  drawTicks( painter );
  drawData( painter ); 
  drawLegend( painter );

  painter->end();
  delete painter; // was allocated by setupPaintDev
}


void KChartAxesPainter::setupLegend( QPaintDevice* paintdev )
{
  // No need to setup anything if there are no legends
  if( _legends.count() == 0 )
	return;
  
  int maxlen = 0;
  int num = 0;
  QPaintDeviceMetrics pdm( paintdev );
  
  const char* curlegend  = _legends.first();
  while( curlegend ) {
	// Check whether there is a legend defined at this position
	if( curlegend[0] != '\0' ) {
	  int len = strlen( curlegend );
	  maxlen = ( maxlen > len ) ? maxlen : len;
	  num++;
	}

	// We don't need more legends than we have datasets.
	if( num >= _chart->chartData()->numDatasets() )
	  break;
	
	curlegend = _legends.next();
  }

  _legendnum = num;

  // calculate the height and width of each element
  int textwidth = maxlen * _legendfontwidth;
  int legendheight = max( _legendfontheight, _legendmarkerheight );

  _legendelementwidth = textwidth + _legendmarkerwidth +
	3 * _legendspacing;
  _legendelementheight = legendheight + 2 * _legendspacing;

  // position the legend to the right of the graph
  if( ( _legendplacement == RightTop ) ||
	  ( _legendplacement == RightCenter ) ||
	  ( _legendplacement == RightBottom ) ) {
	// Always work in one column
	_legendcols = 1;
	_legendrows = num;

	// Martien Verbruggen says: Just for completeness, might use this
	// in later versions
	_legendxsize = _legendcols * _legendelementwidth;
	_legendysize = _legendrows * _legendelementheight;

	// Adjust the right margin for the rest of the graph
	_rightmargin += _legendxsize;

	// Set the x starting point
	_legendxstart = pdm.width() - _rightmargin;
	
	// Set the y starting point, depending on alignment
	if( _legendplacement == RightTop )
	  _legendystart = _topmargin;
	else if( _legendplacement == RightCenter )
	  _legendystart = pdm.height() - _bottommargin - _legendysize;
	else { // RightBottom
	  int height = pdm.height() - _topmargin - _bottommargin;
	  _legendystart = (int)rint( _topmargin + height/2 -
								 _legendysize/2 );
	}
  }
  else { // position the legend below the graph
	// which width can we use
	int width = pdm.width() - _leftmargin - _rightmargin;

	// compute a number of columns if none is given by the user
	if ( _legendcols == -1 )
	  _legendcols = (int)rint( width/_legendelementwidth );
	_legendcols = min( _legendcols, num );
	
	_legendrows = (int)rint( num/_legendcols) + ( (num % _legendcols) ?
	  1 : 0 );

	_legendxsize = _legendcols * _legendelementwidth;
	_legendysize = _legendrows * _legendelementheight;

	// Adjust the bottom margin for the rest of the graph
	_bottommargin += _legendysize;

	// Set the y starting point
	_legendystart = pdm.height() - _bottommargin;

	// Set the x starting point, depending on alignment
	if( _legendplacement == BottomRight )
	  _legendxstart = pdm.width() - _rightmargin - _legendxsize;
	else if( _legendplacement == BottomLeft )
	  _legendxstart = _leftmargin;
	else // BottomCenter
	  _legendxstart = (int)rint( _leftmargin + width/2 -
								 _legendxsize/2 );
  }
}



bool KChartAxesPainter::setupCoords( QPaintDevice* paintdev )
{
  // sanity check
  if( _chart->chartData()->numDatasets() != 2 )
	_twoaxes = false;

  // Set text heights
  if( _title.isEmpty() )
	_textfontheight = 0;
  if( _xlabel.isEmpty() )
	_xlabelfontwidth = 0;
  
  if( _y1label.isEmpty() && !_ylabel.isEmpty() )
	_y1label = _ylabel;

  _ylabelfontheight1 = !_y1label.isEmpty();
  _ylabelfontheight2 = !_y2label.isEmpty();

  if( !_xplotvalues )
	_xaxisfontheight = 0;
  if( !_yplotvalues ) {
	_yaxisfontheight = 0;
	_yaxisfontwidth = 0;
  }

  bool labelheight = ( _xlabelfontheight ? 1 : 0 ) + 
	( _xaxisfontheight ? 1 : 0 );
  
  // calculate top and bottom of bounding box
  QPaintDeviceMetrics pdm( paintdev );
  _bottom = pdm.height() - _bottommargin - 1 -
	( _xlabelfontheight ? _xlabelfontheight : 0 ) -
	( _xaxisfontheight ? _xaxisfontheight : 0 ) -
	( labelheight ? labelheight * _textspacing : 0 );

  _top = _topmargin + 
	( _titlefontheight ? ( _titlefontheight + _textspacing ) : 0 );
  if( _top == 0 )
	_top = _yaxisfontheight/2;

  if( !setMaxMin() ) // can go wrong
	return false;

  // Create the labels for the y axes and calculate the max length
  createYLabels();

  // calculate left and right of bounding box
  int ls = _yaxisfontwidth * _ylabellength[0];
  _left = _leftmargin +
	( ls ? ( ls + _axisspace ) : 0 ) +
	( _ylabelfontheight1 ? ( _ylabelfontheight + _textspacing ) : 0 );
  
  if( _twoaxes )
	ls = _yaxisfontwidth * _ylabellength[1];
  _right = pdm.width() - _rightmargin - 1 -
	( _twoaxes ? 1 : 0 ) * ( ( ls ? ( ls + _axisspace ) : 0 ) +
							 ( _ylabelfontheight2 ? ( _ylabelfontheight2 + _textspacing ) : 0 ) );

  // calculate the step size for the x data
  _xstep = ( _right - _left ) / ( _chart->chartData()->maxPos() + 2 );

  // get the zero axis level
  QPoint zero = valToPixel( 0, 0.0, 1 );
  _zeropoint = zero.y();

  // Check size
  if( ( _bottom - _top ) <= 0 )
	return false;
  if( ( _right - _left ) <= 0 )
	return false;
	
  // set up the data colour list if it does not exist yet
  if( _datacolors.count() == 0 ) {
	_datacolors.setColor( 0, red );
	_datacolors.setColor( 1, green );
	_datacolors.setColor( 2, blue );
	_datacolors.setColor( 3, yellow );
	_datacolors.setColor( 4, magenta );
	_datacolors.setColor( 5, cyan );
	_datacolors.setColor( 6, darkYellow );
  }

  // more sanity checks
  if( _xlabelskip < 1 )
	_xlabelskip = 1;
  if( _ylabelskip < 1 )
	_ylabelskip = 1;
  if( _yticksnum < 1 )
	_yticksnum = 1;

  return true;
}


bool KChartAxesPainter::setMaxMin()
{
  double max = 0.0, min = 0.0;

  // Some decent values to start with
  if( _twoaxes ) {
	_ymaxs[0] = upperBound( _chart->chartData()->maxYValue( 0 ) );
	_ymins[0] = lowerBound( _chart->chartData()->minYValue( 0 ) );
	_ymaxs[1] = upperBound( _chart->chartData()->maxYValue( 1 ) );
	_ymins[1] = lowerBound( _chart->chartData()->minYValue( 1 ) );
  } else {
	_chart->chartData()->minMaxOverallYValue( max, min );
	_ymaxs[0] = upperBound( max );
	_ymins[0] = lowerBound( min );
  }

  // Make sure bars and area always have zero offset
  if( _ymins[0] >= 0 )
	if( ( typeid( *this ) == typeid( KChartBarsPainter ) ) ||
		( typeid( *this ) == typeid( KChartAreaPainter ) ) )
	  _ymins[0] = 0;

  // Override defaults with user-supplied values
  if( _ymin ) {
	_ymins[0] = _ymin;
	_ymins[1] = _ymin;
  } 

  if( _ymax ) {
	_ymaxs[0] = _ymax;
	_ymaxs[1] = _ymax;
  }

  if( _y1min )
	_ymins[0] = _y1min;
  if( _y1max )
	_ymaxs[0] = _y1max;
  if( _y2min )
	_ymins[1] = _y2min;
  if( _y2max )
	_ymaxs[1] = _y2max;

  // Do we have sensible values by now?
  if( _twoaxes ) {
	if( _ymins[0] > _chart->chartData()->minYValue( 0 ) )
	  return false;
	if( _ymaxs[0] < _chart->chartData()->maxYValue( 0 ) )
	  return false;
	if( _ymins[1] > _chart->chartData()->minYValue( 1 ) )
	  return false;
	if( _ymaxs[1] < _chart->chartData()->maxYValue( 1 ) )
	  return false;
  } else {
	if( _ymins[0] > min )
	  return false;
	if( _ymaxs[0] < max )
	  return false;
  }

  return true;
}


void KChartAxesPainter::createYLabels()
{
  _ylabellength[0] = 0;
  _ylabellength[1] = 0;

  for( int t = 0; t <= _yticksnum; t++ ) 
	for( int a = 0; a <= ( _twoaxes ? 1 : 0 ); a++ ) {
	  double labelno = _ymins[a] + t * 
		( _ymaxs[a] - _ymins[a] ) / _yticksnum;

	  // PENDING(kalle): Support number formats
	  QString labelstr;
	  labelstr.sprintf( "%f", labelno );
	  int len = labelstr.length();

	  _ylabels[a][t] = labelno;
	  
	  if( len > _ylabellength[a] )
		_ylabellength[a] = len;
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


void KChartAxesPainter::setXLabel( const char* labelstr )
{
  _xlabel = labelstr;
}


QString KChartAxesPainter::xLabel() const
{
  return _xlabel;
}


void KChartAxesPainter::setYLabel( const char* labelstr )
{
  _ylabel = labelstr;
}


QString KChartAxesPainter::yLabel() const
{
  return _ylabel;
}


void KChartAxesPainter::setY1Label( const char* labelstr )
{
  _y1label = labelstr;
}

QString KChartAxesPainter::y1Label() const
{
  return _y1label;
}

void KChartAxesPainter::setY2Label( const char* labelstr )
{
  _y2label = labelstr;
}

QString KChartAxesPainter::y2Label() const
{
  return _y2label;
}



void KChartAxesPainter::drawText( QPainter* painter )
{
  // title
  if( _titlefontheight ) {
	int titlex = _left + ( _right - _left ) / 2 -
	  _title.length() * _titlefontwidth / 2;
	int titley = _top - _textspacing - _titlefontheight;

	painter->setFont( _titlefont );
	painter->setPen( _textcolor );
	QFontMetrics fm( _titlefont );
	painter->drawText( titlex, titley, fm.width( _title ), fm.height(), 
					   AlignLeft, _title );
  }

  // x label
  if( _xlabelfontheight ) {
	int xlabelx = 3 * ( _left  + _right ) / 4 -
	  _xlabel.length() * _xlabelfontwidth / 2;
	QPaintDeviceMetrics pdm( painter->device() );
	int xlabely = pdm.height() - _xlabelfontheight - _bottommargin;

	painter->setFont( _xlabelfont );
	painter->setPen( _labelcolor );
	QFontMetrics fm( _xlabelfont );
	painter->drawText( xlabelx, xlabely, fm.width( _xlabel ), fm.height(), 
					   AlignLeft,_xlabel );
  }

  // y label
  if( _ylabelfontheight1 ) {
	int ylabelx = _leftmargin;
	int ylabely = ( _bottom + _top ) / 2 +
	  _y1label.length() * _ylabelfontwidth/2;
	
	painter->setFont( _ylabelfont );
	painter->setPen( _labelcolor );
	QFontMetrics fm( _ylabelfont );
	painter->rotate( -90 );
	QPoint pos( ylabelx, ylabely );
	pos = painter->xFormDev( pos ); // convert position back to user coords
	painter->drawText( pos.x(), pos.y(), fm.width( _y1label ), fm.height(),
					   AlignLeft, _y1label );
	painter->resetXForm();
  }

  // ylabel 2 (if needed)
  if( _twoaxes && _ylabelfontheight2 ) {
	QPaintDeviceMetrics pdm( painter->device() );
	int ylabel2x = pdm.width() - _ylabelfontheight - _rightmargin;
	int ylabel2y = ( _bottom - _top ) / 2 +
	  _y2label.length() * _ylabelfontwidth / 2;

	painter->setFont( _ylabelfont );
	painter->setPen( _labelcolor );
	QFontMetrics fm( _ylabelfont );
	painter->rotate( 90 );
	QPoint pos( ylabel2x, ylabel2y );
	pos = painter->xFormDev( pos ); // convert position back to user coords
	painter->drawText( pos.x(), pos.y(), fm.width( _y2label ), fm.height(),
					   AlignLeft,_y2label );
	painter->resetXForm();
  }
}


void KChartAxesPainter::setYMaxValue( double value )
{
  _ymax = value;
}

double KChartAxesPainter::yMaxValue() const
{
  return _ymax;
}

void KChartAxesPainter::setYMinValue( double value )
{
  _ymin = value;
}

double KChartAxesPainter::yMinValue() const
{
  return _ymin;
}

void KChartAxesPainter::setY1MaxValue( double value )
{
  _y1max = value;
}

double KChartAxesPainter::y1MaxValue() const
{
  return _y1max;
}

void KChartAxesPainter::setY1MinValue( double value )
{
  _y1min = value;
}

double KChartAxesPainter::y1MinValue() const
{
  return _y1min;
}

void KChartAxesPainter::setY2MaxValue( double value )
{
  _y2max = value;
}

double KChartAxesPainter::y2MaxValue() const
{
  return _y2max;
}

void KChartAxesPainter::setY2MinValue( double value )
{
  _y2min = value;
}

double KChartAxesPainter::y2MinValue() const
{
  return _y2min;
}


void KChartAxesPainter::drawAxes( QPainter* painter )
{
  if( _boxaxis ) {
	painter->setPen( _fgcolor );
	painter->setBrush( NoBrush );
	painter->drawRect( QRect( QPoint( _left, _top ), 
							  QPoint( _right, _bottom) ) );
  } else {
	painter->setPen( _fgcolor );
	painter->setBrush( NoBrush );
	painter->drawLine( _left, _top, _left, _bottom );
	if( !_zeroaxisonly ) 
	  painter->drawLine( _left, _bottom, _right, _bottom );
	if( _twoaxes )
	  painter->drawLine( _right, _bottom, _right, _top );
  }

  if( _zeroaxis || _zeroaxisonly ) {
	QPoint xy = valToPixel( 0, 0, 1 );
	painter->drawLine( _left, xy.y(), _right, xy.y() );
  }
}


void KChartAxesPainter::drawTicks( QPainter* painter )
{
  // Ticks and values for Y axes
  for( int t = 0; t <= _yticksnum; t++ ) {
	for( int a = 0; a <= ( _twoaxes ? 1: 0 ); a++ ) {
	  double label = _ylabels[a][t];

	  QPoint xy = valToPixel( a * ( _chart->chartData()->maxPos() + 2 ),
							  label, a + 1 );

	  if( _longticks && a == 0 ) {
		painter->setPen( _fgcolor );
		painter->drawLine( xy.x(), xy.y(), xy.x() + _right - _left, xy.y() );
	  } else {
		painter->setPen( _fgcolor );
		painter->drawLine( xy.x(), xy.y(),
						   xy.x() + (3-2*(a-1))*_ticklength, xy.y() );
	  }
	  
	  // Skip unwanted labels
	  if( ( t % _ylabelskip ) || ! _yplotvalues )
		continue;

	  // PENDING(kalle) Use number formatter here
	  QString labelstr;
	  labelstr.sprintf( "%g", label );
	  int x = xy.x();
	  int y = xy.y();
	  x -= (1-a)*labelstr.length() * _yaxisfontwidth +
		(1-2*a) * _axisspace;
	  y -= _yaxisfontheight/2;
	  xy.setX( x );
	  xy.setY( y );
	  
	  painter->setFont( _yaxisfont );
	  painter->setPen( _axislabelcolor );
	  QFontMetrics fm( _yaxisfont );
	  painter->drawText( xy.x(), xy.y(), 
						 fm.width( labelstr ), fm.height(), AlignLeft, labelstr );
	}
  }

  // Don't draw the x ticks and values is this is not desired
  if( !_xplotvalues )
	return;


  // Ticks and values for x axis
  for( uint i = 0; i <= _chart->chartData()->maxPos(); i++ ) {
	QPoint xy = valToPixel( i+1, 0, 1 );
	if( !_zeroaxisonly )
	  xy.setY( _bottom );

	if( _xticks ) {
	  if( _longticks ) {
		painter->setPen( _fgcolor );
		painter->drawLine( xy.x(), _bottom, xy.x(), _top );
	  } else {
		painter->setPen( _fgcolor );
		painter->drawLine( xy.x(), xy.y(), xy.x(), xy.y() - _ticklength );
	  }
	}

	// Skip unwanted labels, but always draw last tick
	if( ( i % _xlabelskip ) && ( i != _chart->chartData()->maxPos() ) )
	  continue;

	int x = xy.x();
	QString text = _chart->chartData()->xValue( i );
	x -= _xaxisfontwidth * text.length()/2;
	xy.setX( x );
	int yt = xy.y() + _textspacing/2;
	painter->setPen( _axislabelcolor );
	painter->setFont( _xaxisfont );
	QFontMetrics fm( _xaxisfont );
	painter->drawText( xy.x(), yt, fm.width( text ), fm.height(), 
					   AlignLeft, text );
  }
}


void KChartAxesPainter::drawLegend( QPainter* painter )
{
  // No need to do anything if there are no legends
  if( _legends.count() == 0 )
	return;

  int xl = _legendxstart + _legendspacing;
  int y = _legendystart + _legendspacing - 1;

  int row = 1;
  int x = xl; // start position of current element

  for( uint legend = 0; legend < _legends.count(); legend++ ) {
	if( (int)legend  >= _chart->chartData()->numDatasets() )
	  break;

	int xe = x; // position within an element
	if( _legends.at( legend ) == 0 || strlen( _legends.at( legend ) ) == 0 )
	  continue;

	drawLegendMarker( painter, legend, xe, y );

	xe += _legendmarkerwidth + _legendspacing;
	int ys = (int)rint( y + _legendelementheight/2 - _legendfontheight/2 );
	
	QFontMetrics fm( _legendfont );
	painter->setFont( _legendfont );
	painter->setPen( _fgcolor );
	painter->drawText( xe, ys, fm.width( _legends.at( legend ) ),
					   fm.height(), AlignLeft, _legends.at( legend ) );

	x += _legendelementwidth;

	if( ++row > _legendcols ) {
	  row = 1;
	  y += _legendelementheight;
	  x = xl;
	}
  }
}


void KChartAxesPainter::drawLegendMarker( QPainter* painter, int number, 
										  int x, int y )
{
  QColor datacolor = chooseDataColor( number );

  y += (int)rint( _legendelementheight/2 - _legendmarkerheight/2 );

  QBrush filledbrush( datacolor, SolidPattern );
  QBrush emptybrush( datacolor, NoBrush );
  QRect rect( QPoint( x, y ),
			  QPoint( x + _legendmarkerwidth, y + _legendmarkerheight ) );
  painter->setBrush( filledbrush );
  painter->setPen( datacolor );
  painter->drawRect( rect );
  painter->setBrush( emptybrush );
  painter->setPen( _accentcolor );
  painter->drawRect( rect );
}


void KChartAxesPainter::setYTicksNum( int ticks )
{
  _yticksnum = ticks;
}


int KChartAxesPainter::yTicksNum() const
{
  return _yticksnum;
}


void KChartAxesPainter::setXLabelSkip( int skip )
{
  _xlabelskip = skip;
}

int KChartAxesPainter::xLabelSkip() const
{
  return _xlabelskip;
}

void KChartAxesPainter::setYLabelSkip( int skip )
{
  _ylabelskip = skip;
}

int KChartAxesPainter::yLabelSkip() const
{
  return _ylabelskip;
}


void KChartAxesPainter::setOverwrite( OverwriteMode over )
{
  _overwrite = over;
}

OverwriteMode KChartAxesPainter::overwrite() const
{
  return _overwrite;
}


void KChartAxesPainter::setTwoAxes( bool twoaxes )
{
  _twoaxes = twoaxes;
}

bool KChartAxesPainter::twoAxes() const
{
  return _twoaxes;
}


void KChartAxesPainter::setLongTicks( bool longticks )
{
  _longticks = longticks;
}

bool KChartAxesPainter::longTicks() const
{
  return _longticks;
}


void KChartAxesPainter::setLegends( QStrList legends )
{
  _legends = legends;
}

QStrList KChartAxesPainter::legends() const
{
  return _legends;
}


void KChartAxesPainter::setLegendPlacement( LegendPlacement placement )
{
  _legendplacement = placement;
}

LegendPlacement KChartAxesPainter::legendPlacement() const
{
  return _legendplacement;
}


void KChartAxesPainter::setZeroAxisOnly( bool only )
{
  _zeroaxisonly = only;
}

bool KChartAxesPainter::zeroAxisOnly() const
{
  return _zeroaxisonly;
}


void KChartAxesPainter::setBoxAxis( bool boxaxis )
{
  _boxaxis = boxaxis;
}

bool KChartAxesPainter::boxAxis() const
{
  return _boxaxis;
}


void KChartAxesPainter::setXTicks( bool xticks )
{
  _xticks = xticks;
}

bool KChartAxesPainter::xTicks() const
{
  return _xticks;
}


void KChartAxesPainter::setLegendMarkerWidth( int width )
{
  _legendmarkerwidth = width;
}

int KChartAxesPainter::legendMarkerWidth() const
{
  return _legendmarkerwidth;
}


void KChartAxesPainter::setLegendMarkerHeight( int height )
{
  _legendmarkerheight = height;
}

int KChartAxesPainter::legendMarkerHeight() const
{
  return _legendmarkerheight;
}
