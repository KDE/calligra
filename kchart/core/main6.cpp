/* $Id $ */
/****************************************************************
**
** Demo program for KChart
**
****************************************************************/

#include <qapp.h>
#include <qstrlist.h>

#include "KChartWidget.h"
#include "KChartData.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

	// setup some chart data
	KChartData data6( 2 ); // two datasets
	data6.setXValue( 0, "1st" );
	data6.setXValue( 1, "2nd" );
	data6.setXValue( 2, "3rd" );
	data6.setXValue( 3, "4th" );
	data6.setXValue( 4, "5th" );
	data6.setXValue( 5, "6th" );
	data6.setXValue( 6, "7th" );
	data6.setXValue( 7, "8th" );
	data6.setXValue( 8, "9th" );
	data6.setYValue( 0, 0, 5 );
	data6.setYValue( 0, 1, 12 );
	//	data6.setYValue( 0, 2, 24 );
	data6.setYValue( 0, 3, 33 );
	data6.setYValue( 0, 4, 19 );
	data6.setYValue( 0, 5, 8 );
	data6.setYValue( 0, 6, 5 );
	data6.setYValue( 0, 7, 15 );
	data6.setYValue( 0, 8, 21 );
	data6.setYValue( 1, 0, -6 );
	data6.setYValue( 1, 1, -5 );
	data6.setYValue( 1, 2, -9 );
	data6.setYValue( 1, 3, -8 );
	data6.setYValue( 1, 4, -11 );
	data6.setYValue( 1, 5, -9.3 );
	//	data6.setYValue( 1, 6, 1 );
	data6.setYValue( 1, 7, -9 );
	data6.setYValue( 1, 8, -12 );
	KChartWidget cw6;
	cw6.setChartData( &data6 );
	cw6.chart()->setXLabel( "Day" );
	cw6.chart()->setY1Label( "AUD" );
	cw6.chart()->setTitle( "Credits and Debits" );
	cw6.chart()->setYTicksNum( 12 );
	cw6.chart()->setYLabelSkip( 2 );
	cw6.chart()->setOverwrite( InFront );
	QStrList legends;
	legends.append( "credits" );
	legends.append( "debits" );
	cw6.chart()->setLegends( legends );
	KChartColorArray colorarray;
	colorarray.setColor( 0, green );
	colorarray.setColor( 1, red );
	cw6.chart()->setDataColors( &colorarray );
	cw6.chart()->setAxisLabelColor( black );
	cw6.chart()->setLegendPlacement( RightBottom );
	cw6.chart()->setZeroAxisOnly( 0 );

    cw6.resize( 400, 300 );

    a.setMainWidget( &cw6 );
    cw6.show();
    return a.exec();
}
