/* $Id$ */

#include "KChart.h"
#include "KChartColorArray.h"
#include "KChartBarsPainter.h"
#include "KChartAreaPainter.h"
#include "KChartLinesPainter.h"
#include "KChartPointsPainter.h"
#include "KChartLinesPointsPainter.h"
#include "KChartPiePainter.h"
#include "KChartPie3DPainter.h"

#include <qapp.h> // fatal
#include <qstrlist.h> // QStrList

#include <kdebug.h>

KChart::KChart( KChartType type ) :
  _charttype( type )
{
  // Create a chart painter object of the appropriate type
  switch( _charttype ) {
  case Bars:
	_cp = new KChartBarsPainter( this );
	break;
  case Lines:
	_cp = new KChartLinesPainter( this );
	break;
  case Points:
	_cp = new KChartPointsPainter( this );
	break;
  case LinesPoints:
	_cp = new KChartLinesPointsPainter( this );
	break;
  case Area:
	_cp = new KChartAreaPainter( this );
	break;
  case Pie:
	_cp = new KChartPiePainter( this );
	break;
  case Pie3D:
	_cp = new KChartPie3DPainter( this );
	break;
  default:
	KDEBUG( KDEBUG_WARN, 34001, "Unknown chart type selected, choosing bars" );
	_cp = new KChartBarsPainter( this );
  };
}


KChart::~KChart()
{
  delete _cp;
}


void KChart::setChartType( KChartType charttype )
{
  _charttype = charttype;
  debug( "Sorry, not implemented: KChart::setChartType\n" );
}

KChartType KChart::chartType() const
{
  return _charttype;
}


void KChart::setChartData( KChartData* chartdata )
{
  _chartdata = chartdata;
}


KChartData* KChart::chartData() const
{
  return _chartdata;
}


void KChart::repaintChart( QPaintDevice* paintdev )
{
  _cp->paintChart( paintdev );
}


void KChart::setTitle( const char* str )
{
  _cp->setTitle( str );
}


QString KChart::title() const
{
  return _cp->title();
}


void KChart::setXLabel( const char* str )
{
  _cp->setXLabel( str );
}


QString KChart::xLabel() const
{
  return _cp->xLabel();
}


void KChart::setYLabel( const char* str )
{
  _cp->setYLabel( str );
}


QString KChart::yLabel() const
{
  return _cp->yLabel();
}

void KChart::setY1Label( const char* str )
{
  _cp->setY1Label( str );
}

QString KChart::y1Label() const
{
  return _cp->y1Label();
}

void KChart::setY2Label( const char* str )
{
  _cp->setY2Label( str );
}


QString KChart::y2Label() const
{
  return _cp->y2Label();
}




void KChart::setTextColor( QColor /* color */ )
{
	fatal( "Sorry, not implemented: KChart::setTextColor\n" );
}

QColor KChart::textColor() const
{
	fatal( "Sorry, not implemented: KChart::textColor\n" );
	return QColor();
}


void KChart::setTitleFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setTitleFont\n" );
}

QFont KChart::titleFont() const
{
	fatal( "Sorry, not implemented: KChart::titleFont\n" );
	return QFont();
}


void KChart::setLabelFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setLabelFont\n" );
}

QFont KChart::labelFont() const
{
	fatal( "Sorry, not implemented: KChart::labelFont\n" );
	return QFont();
}


void KChart::setXLabelFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setXLabelFont\n" );
}

QFont KChart::xLabelFont() const
{
	fatal( "Sorry, not implemented: KChart::xLabelFont\n" );
	return QFont();
}


void KChart::setYLabelFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setYLabelFont\n" );
}

QFont KChart::yLabelFont() const
{
	fatal( "Sorry, not implemented: KChart::yLabelFont\n" );
	return QFont();
}


void KChart::setValueFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setValueFont\n" );
}

QFont KChart::valueFont() const
{
	fatal( "Sorry, not implemented: KChart::valueFont\n" );
	return QFont();
}


void KChart::setXAxisFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setXAxisFont\n" );
}

QFont KChart::xAxisFont() const
{
	fatal( "Sorry, not implemented: KChart::xAxisFont\n" );
	return QFont();
}


void KChart::setYAxisFont( QFont /* font */ )
{
	fatal( "Sorry, not implemented: KChart::setYAxisFont\n" );
}

QFont KChart::yAxisFont() const
{
	fatal( "Sorry, not implemented: KChart::yAxisFont\n" );
	return QFont();
}


void KChart::setMargin( int /* margin */ )
{
	fatal( "Sorry, not implemented: KChart::setMargin\n" );
}

int KChart::margin() const
{
	fatal( "Sorry, not implemented: KChart::margin\n" );
	return 0;
}


void KChart::setDataColors( KChartColorArray* colors )
{
  _cp->setDataColors( colors );
}

KChartColorArray* KChart::dataColors() const
{
  return _cp->dataColors();
}


void KChart::setTickLength( int )
{
	fatal( "Sorry, not implemented: KChart::setTickLength\n" );
}

int KChart::tickLength() const
{
	fatal( "Sorry, not implemented: KChart::tickLength\n" );
	return 0;
}


void KChart::setXTicksEnabled( bool /* enabled */ )
{
	fatal( "Sorry, not implemented: KChart::setXTicksEnabled\n" );
}

bool KChart::xTicksEnabled() const
{
	fatal( "Sorry, not implemented: KChart::xTicksEnabled\n" );
	return false;
}


void KChart::setYTicksEnabled( bool /* enabled */ )
{
	fatal( "Sorry, not implemented: KChart::setYTicksEnabled\n" );
}

bool KChart::yTicksEnabled() const
{
	fatal( "Sorry, not implemented: KChart::yTicksEnabled\n" );
	return false;
}


void KChart::setYTicksNum( int ticks )
{
  _cp->setYTicksNum( ticks );
}


int KChart::yTicksNum() const
{
  return _cp->yTicksNum();
}


void KChart::setXLabelSkip( int skip )
{
  _cp->setXLabelSkip( skip );
}

int KChart::xLabelSkip() const
{
  return _cp->xLabelSkip();
}


void KChart::setYLabelSkip( int skip  )
{
  _cp->setYLabelSkip( skip );
}

int KChart::yLabelSkip() const
{
  return _cp->yLabelSkip();
}


void KChart::setXPlotValues( bool /* plot */ )
{
	fatal( "Sorry, not implemented: KChart::setXPlotValues\n" );
}

bool KChart::xPlotValues() const
{
	fatal( "Sorry, not implemented: KChart::xPlotValues\n" );
	return false;
}


void KChart::setYPlotValues( bool /* plot */ )
{
	fatal( "Sorry, not implemented: KChart::setYPlotValues\n" );
}

bool KChart::yPlotValues() const
{
	fatal( "Sorry, not implemented: KChart::yPlotValues\n" );
	return false;
}


void KChart::setYMaxValue( double value )
{
  _cp->setYMaxValue( value );
}

double KChart::yMaxValue() const
{
  return _cp->yMaxValue();
}


void KChart::setYMinValue( double value )
{
  _cp->setYMinValue( value );
}

double KChart::yMinValue() const
{
  return _cp->yMinValue();
}

void KChart::setY1MaxValue( double value )
{
  _cp->setY1MaxValue( value );
}

double KChart::y1MaxValue() const
{
  return _cp->y1MaxValue();
}

void KChart::setY1MinValue( double value )
{
  _cp->setY1MinValue( value );
}

double KChart::y1MinValue() const
{
  return _cp->y1MinValue();
}

void KChart::setY2MaxValue( double value )
{
  _cp->setY2MaxValue( value );
}

double KChart::y2MaxValue() const
{
  return _cp->y2MaxValue();
}

void KChart::setY2MinValue( double value )
{
  _cp->setY2MinValue( value );
}

double KChart::y2MinValue() const
{
  return _cp->y2MinValue();
}


void KChart::setAxisSpace( int /* space */ )
{
	fatal( "Sorry, not implemented: KChart::setAxisSpace\n" );
}

int KChart::axisSpace() const
{
	fatal( "Sorry, not implemented: KChart::axisSpace\n" );
	return 0;
}


void KChart::setLineType( PenStyle /* linetype */ )
{
	fatal( "Sorry, not implemented: KChart::setLineType\n" );
}

PenStyle KChart::lineType() const
{
	fatal( "Sorry, not implemented: KChart::lineType\n" );
	return SolidLine;
}


void KChart::setLineWidth( int width )
{
  _cp->setLineWidth( width );
}

int KChart::lineWidth() const
{
  return _cp->lineWidth();
}


void KChart::setOverwrite( OverwriteMode over )
{
  _cp->setOverwrite( over );
}


OverwriteMode KChart::overwrite() const
{
  return _cp->overwrite();
}


void KChart::setTwoAxes( bool twoaxes )
{
  _cp->setTwoAxes( twoaxes );
}

bool KChart::twoAxes() const
{
  return _cp->twoAxes();
}


void KChart::setLongTicks( bool longticks )
{
  _cp->setLongTicks( longticks );
}

bool KChart::longTicks() const
{
  return _cp->longTicks();
}


void KChart::setLegends( QStrList legends )
{
  _cp->setLegends( legends );
}


QStrList KChart::legends() const
{
  return _cp->legends();
}


void KChart::setLegendPlacement( LegendPlacement placement )
{
  _cp->setLegendPlacement( placement );
}

LegendPlacement KChart::legendPlacement() const
{
  return _cp->legendPlacement();
}


void KChart::setZeroAxisOnly( bool only )
{
  _cp->setZeroAxisOnly( only );
}

bool KChart::zeroAxisOnly() const
{
  return _cp->zeroAxisOnly();
}


void KChart::setAxisLabelColor( QColor color )
{
  _cp->setAxisLabelColor( color );
}

QColor KChart::axisLabelColor() const
{
  return _cp->axisLabelColor();
}


void KChart::setBoxAxis( bool boxaxis )
{
  _cp->setBoxAxis( boxaxis );
}

bool KChart::boxAxis() const
{
  return _cp->boxAxis();
}


void KChart::setXTicks( bool xticks )
{
  _cp->setXTicks( xticks );
}

bool KChart::xTicks() const
{
  return _cp->xTicks();
}


void KChart::setLineTypes( PenStyle types[], int number )
{
  _cp->setLineTypes( types, number );
}

void KChart::lineTypes( PenStyle types[], int& number )
{
  _cp->lineTypes( types, number );
}


void KChart::setLegendMarkerWidth( int width )
{
  _cp->setLegendMarkerWidth( width );
}

int KChart::legendMarkerWidth() const
{
  return _cp->legendMarkerWidth();
}


void KChart::setLegendMarkerHeight( int height )
{
  _cp->setLegendMarkerHeight( height );
}

int KChart::legendMarkerHeight() const
{
  return _cp->legendMarkerHeight();
}


