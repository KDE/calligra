/* $Id$ */

#include "KChartPiePainter.h"
#include "KChart.h"

#include <qpainter.h>
#include <qstrlist.h>

KChartPiePainter::KChartPiePainter( KChart* chart ) :
  KChartPainter( chart )
{
  fatal( "Sorry, not implemented: KChartPiePainter::KChartPiePainter" );
}


KChartPiePainter::~KChartPiePainter()
{
  fatal( "Sorry, not implemented: KChartPiePainter::~KChartPiePainter" );
}


void KChartPiePainter::paintChart( QPaintDevice* /* paintdev */ )
{
  fatal( "Sorry, not implemented: KChartPiePainter::paintGraph" );
}


void KChartPiePainter::drawData( QPainter* )
{
  fatal( "Sorry, not implemented: KChartPiePainter::drawData" );
}


QColor KChartPiePainter::chooseDataColor( int /* dataset */ )
{
  fatal( "Sorry, not implemented: KChartPiePainter::chooseDataColor" );
  return QColor();
}


bool KChartPiePainter::setupCoords( QPaintDevice* )
{
  fatal( "Sorry, not implemented: KChartPiePainter::setupCoords" );
  return false;
}


void KChartPiePainter::drawText( QPainter* /* painter */ )
{
  fatal( "Sorry, not implemented: KChartPiePainter::drawText" );
}


void KChartPiePainter::setXLabel( const char* )
{
}


QString KChartPiePainter::xLabel() const
{
  return "";
}


void KChartPiePainter::setYLabel( const char* )
{
}


QString KChartPiePainter::yLabel() const
{
  return "";
}

void KChartPiePainter::setY1Label( const char* )
{
}

QString KChartPiePainter::y1Label() const
{
  return "";
}

void KChartPiePainter::setY2Label( const char* )
{
}

QString KChartPiePainter::y2Label() const
{
  return "";
}


void KChartPiePainter::setYMaxValue( double )
{
}

double KChartPiePainter::yMaxValue() const
{
  return 0;
}

void KChartPiePainter::setYMinValue( double )
{
}

double KChartPiePainter::yMinValue() const
{
  return 0;
}

void KChartPiePainter::setY1MaxValue( double )
{
}

double KChartPiePainter::y1MaxValue() const
{
  return 0;
}

void KChartPiePainter::setY1MinValue( double )
{
}

double KChartPiePainter::y1MinValue() const
{
  return 0;
}

void KChartPiePainter::setY2MaxValue( double )
{
}

double KChartPiePainter::y2MaxValue() const
{
  return 0;
}

void KChartPiePainter::setY2MinValue( double )
{
}

double KChartPiePainter::y2MinValue() const
{
  return 0;
}


void KChartPiePainter::setYTicksNum( int )
{
}


int KChartPiePainter::yTicksNum() const
{
  return 0;
}


void KChartPiePainter::setXLabelSkip( int )
{
}

int KChartPiePainter::xLabelSkip() const
{
  return 0;
}

void KChartPiePainter::setYLabelSkip( int )
{
}

int KChartPiePainter::yLabelSkip() const
{
  return 0;
}


void KChartPiePainter::setOverwrite( OverwriteMode )
{
}

OverwriteMode KChartPiePainter::overwrite() const
{
  return SideBySide;
}


void KChartPiePainter::setTwoAxes( bool )
{
}

bool KChartPiePainter::twoAxes() const
{
  return false;
}


void KChartPiePainter::setLongTicks( bool )
{
}

bool KChartPiePainter::longTicks() const
{
  return false;
}


void KChartPiePainter::setLegends( QStrList )
{
}

QStrList KChartPiePainter::legends() const
{
  return QStrList();
}


void KChartPiePainter::setLegendPlacement( LegendPlacement )
{
}

LegendPlacement KChartPiePainter::legendPlacement() const
{
  return BottomLeft;
}



void KChartPiePainter::setZeroAxisOnly( bool )
{
}

bool KChartPiePainter::zeroAxisOnly() const
{
  return false;
}


void KChartPiePainter::setBoxAxis( bool )
{
}

bool KChartPiePainter::boxAxis() const
{
  return false;
}


void KChartPiePainter::setXTicks( bool )
{
}

bool KChartPiePainter::xTicks() const
{
  return false;
}


void KChartPiePainter::setLegendMarkerWidth( int )
{
}

int KChartPiePainter::legendMarkerWidth() const
{
  return 0;
}


void KChartPiePainter::setLegendMarkerHeight( int )
{
}

int KChartPiePainter::legendMarkerHeight() const
{
  return 0;
}



