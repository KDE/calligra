/* $Id $ */
/****************************************************************
**
** Demo program for KChart
**
****************************************************************/

#include <qapp.h>

#include "KChartWidget.h"
#include "KChartData.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );

	// setup some chart data
	KChartData data3( 2 ); // two datasets
	data3.setXValue( 0, "1st" );
	data3.setXValue( 1, "2nd" );
	data3.setXValue( 2, "3rd" );
	data3.setXValue( 3, "4th" );
	data3.setXValue( 4, "5th" );
	data3.setXValue( 5, "6th" );
	data3.setXValue( 6, "7th" );
	data3.setXValue( 7, "8th" );
	data3.setXValue( 8, "9th" );
	data3.setYValue( 0, 0, 5 );
	data3.setYValue( 0, 1, 12 );
	data3.setYValue( 0, 2, 24 );
	data3.setYValue( 0, 3, 33 );
	data3.setYValue( 0, 4, 19 );
	data3.setYValue( 0, 5, 8 );
	data3.setYValue( 0, 6, 6 );
	data3.setYValue( 0, 7, 15 );
	data3.setYValue( 0, 8, 21 );
	data3.setYValue( 1, 0, 1 );
	data3.setYValue( 1, 1, 2 );
	data3.setYValue( 1, 2, 5 );
	data3.setYValue( 1, 3, 6 );
	data3.setYValue( 1, 4, 3 );
	data3.setYValue( 1, 5, 1.5 );
	data3.setYValue( 1, 6, 1 );
	data3.setYValue( 1, 7, 3 );
	data3.setYValue( 1, 8, 4 );
	KChartWidget cw3;
	cw3.setChartData( &data3 );
	cw3.chart()->setXLabel( "X Label" );
	cw3.chart()->setYLabel( "Y Label" );
	cw3.chart()->setTitle( "Bars in front of each other" );
	cw3.chart()->setYTicksNum( 8 );
	cw3.chart()->setYLabelSkip( 2 );
	cw3.chart()->setOverwrite( InFront );

    cw3.resize( 400, 300 );

    a.setMainWidget( &cw3 );
    cw3.show();
    return a.exec();
}
