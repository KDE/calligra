/* $Id$ */

#include "KChartPainter.h"
#include "KoDiagrammParameters.h"

#include <qcolor.h>
#include <qfont.h>
#include <qfontmet.h>
#include <qpainter.h>

#include <values.h>

KChartPainter::KChartPainter( KoDiagrammParameters* chart ) :
  _chart( chart )
{
}


KChartPainter::~KChartPainter()
{
}


bool KChartPainter::checkData( table_t* table )
{
  // We don't do much here, because we allow quite a lot of things
  // like datasets not having the same size etc. Therefore, we just
  // check if there is at least one dataset.
  return ( table->data.size() > 0 );
}


void KChartPainter::setTitleFont( QFont font )
{
  _chart->setTitleFont( font );
}


QPainter* KChartPainter::setupPaintDev( QPaintDevice* paintdev )
{
  // Create a new painter. Will be deleted from the
  // outside. QPainter::begin() is called implicitly.
  QPainter* painter = new QPainter( paintdev );

  // Set some initial values
  painter->setPen( _chart->_fgcolor );
  //painter->setBackgroundColor( _chart->_bgcolor );

  // clear the background
  //painter->eraseRect( painter->window() );

  // (Other colors to be set when used.)
  _chart->_transparency = true;
  painter->setBackgroundMode( _chart->_transparency ? Qt::TransparentMode :
							  Qt::OpaqueMode );

  return painter;
}


QColor KChartPainter::chooseDataColor( int dataset )
{
  return _chart->_datacolors.color( dataset % _chart->_datacolors.count() );
}



uint KChartPainter::numDatasets( table_t* table )
{
	return table->data.size();
}


uint KChartPainter::maxPos( table_t* table )
{
	uint ret = 0;
	for( data_t::iterator lit = table->data.begin();
		 lit != table->data.end(); ++lit ) {
		ret = QMAX( ret, lit->size() );
	}
	return ret - 1;
}


double KChartPainter::maxValue( line_t& line )
{
	double ret = MINDOUBLE;
	for( line_t::iterator lit = line.begin();
		 lit != line.end();
		 ++lit ) {
		ret = QMAX( ret, *lit );
	}
	return ret;
}


double KChartPainter::minValue( line_t& line )
{
	double ret = MAXDOUBLE;
	for( line_t::iterator lit = line.begin();
		 lit != line.end();
		 ++lit ) {
		ret = QMIN( ret, *lit );
	}
	return ret;
}


void KChartPainter::minMaxOverallYValue( table_t* table,
										 double& min, double& max )
{
	min = MAXDOUBLE;
	max = MINDOUBLE;

	for( data_t::iterator lit = table->data.begin();
		 lit != table->data.end(); ++lit ) {
		for( line_t::iterator iter = lit->begin();
			 iter != lit->end(); ++iter ) {
			max = QMAX( max, (*iter) );	
			min = QMIN( min, (*iter) );
		}
	}
}
