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
  _chart->_topmargin = 0;
  _chart->_bottommargin = 0;
  _chart->_leftmargin = 0;
  _chart->_rightmargin = 0;
  
  // the colours
  _chart->_bgcolor = white;
  _chart->_fgcolor = darkBlue;
  _chart->_textcolor = darkBlue;
  _chart->_labelcolor = darkBlue;
  _chart->_axislabelcolor = darkBlue;
  _chart->_accentcolor = gray;

  // draw in transparent mode?
  _chart->_transparency = false;

  // pixels for text spacing
  _chart->_textspacing = 8;

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
  _chart->_titlefont = font;

  // Compute the metrics once and for all
  QFontMetrics fm( font );
  _chart->_titlefontwidth = fm.width( 'm' );
  _chart->_titlefontheight = fm.height();
}


QPainter* KChartPainter::setupPaintDev( QPaintDevice* paintdev )
{
  // Create a new painter. Will be deleted from the
  // outside. QPainter::begin() is called implicitly.
  QPainter* painter = new QPainter( paintdev );

  // Set some initial values
  painter->setPen( _chart->_fgcolor );
  painter->setBackgroundColor( _chart->_bgcolor );
  // (Other colors to be set when used.)
  painter->setBackgroundMode( _chart->_transparency ? TransparentMode :
							  OpaqueMode );

  return painter;
}


QColor KChartPainter::chooseDataColor( int dataset )
{
  return _chart->_datacolors.color( dataset % _chart->_datacolors.count() );
}



