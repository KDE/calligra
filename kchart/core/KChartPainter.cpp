/* $Id$ */

#include "KChartPainter.h"
#include "KChart.h"
#include "KChartData.h"

#include <qcolor.h>
#include <qfont.h>
#include <qfontmet.h>
#include <qpainter.h>

KChartPainter::KChartPainter( KChart* chart ) :
  _chart( chart )
{
  /* Set some defaults */

  // margins that will be left empty
  _topmargin = 0;
  _bottommargin = 0;
  _leftmargin = 0;
  _rightmargin = 0;
  
  // the colours
  _bgcolor = white;
  _fgcolor = darkBlue;
  _textcolor = darkBlue;
  _labelcolor = darkBlue;
  _axislabelcolor = darkBlue;
  _accentcolor = gray;

  // draw in transparent mode?
  _transparency = false;

  // pixels for text spacing
  _textspacing = 8;

  /* Choose a default title font */
  setTitleFont( QFont( "courier", 16 ) );
}


KChartPainter::~KChartPainter()
{
}


bool KChartPainter::checkData()
{
  // We don't do much here, because we allow quite a lot of things
  // like datasets not having the same size etc. Therefore, we just
  // check if there is at least one dataset.
  return ( _chart->chartData()->numDatasets() > 0 );
}


void KChartPainter::setTitleFont( QFont font )
{
  _titlefont = font;

  // Compute the metrics once and for all
  QFontMetrics fm( font );
  _titlefontwidth = fm.width( 'm' );
  _titlefontheight = fm.height();
}


QPainter* KChartPainter::setupPaintDev( QPaintDevice* paintdev )
{
  // Create a new painter. Will be deleted from the
  // outside. QPainter::begin() is called implicitly.
  QPainter* painter = new QPainter( paintdev );

  // Set some initial values
  painter->setPen( _fgcolor );
  painter->setBackgroundColor( _bgcolor );
  // (Other colors to be set when used.)
  painter->setBackgroundMode( _transparency ? TransparentMode :
							  OpaqueMode );

  return painter;
}


QColor KChartPainter::chooseDataColor( int dataset )
{
  return _datacolors.color( dataset % _datacolors.count() );
}



void KChartPainter::setTitle( const char* titlestring )
{
  _title = titlestring;
}


QString KChartPainter::title() const
{
  return _title;
}


void KChartPainter::setAxisLabelColor( QColor color )
{
  _axislabelcolor = color;
}

QColor KChartPainter::axisLabelColor() const
{
  return _axislabelcolor;
}


void KChartPainter::setDataColors( KChartColorArray* colors )
{
  _datacolors = *colors;
}


KChartColorArray* KChartPainter::dataColors() const
{
  return new KChartColorArray( _datacolors );
}


void KChartPainter::setLineWidth( int )
{
}

int KChartPainter::lineWidth() const
{
  return 0;
}


void KChartPainter::setLineTypes( PenStyle [], int )
{
}

void KChartPainter::lineTypes( PenStyle [], int& number )
{
  number = 0;
}

