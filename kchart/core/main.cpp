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
	KChartData data(1);
	data.setXValue( 0, "1st" );
	data.setXValue( 1, "2nd" );
	data.setXValue( 2, "3rd" );
	data.setXValue( 3, "4th" );
	data.setXValue( 4, "5th" );
	data.setXValue( 5, "6th" );
	data.setXValue( 6, "7th" );
	data.setXValue( 7, "8th" );
	data.setXValue( 8, "9th" );
	data.setYValue( 0, 0, 1.0 );
	data.setYValue( 0, 1, 2.0 );
	data.setYValue( 0, 2, 5.0 );
	data.setYValue( 0, 3, 6.0 );
	data.setYValue( 0, 4, 3.0 );
	data.setYValue( 0, 5, 1.5 );
	data.setYValue( 0, 6, 1.0 );
	data.setYValue( 0, 7, 3.0 );
	data.setYValue( 0, 8, 4.0 );


    KChartWidget cw; // Construct a bar chart (bar = default)
	cw.setChartData( &data );

	// some more settings
	cw.chart()->setTitle( "A Simple Bar Chart" );
	cw.chart()->setXLabel( "X Label" );
	cw.chart()->setYLabel( "Y Label" );
	cw.chart()->setYMaxValue( 8 );
	cw.chart()->setYTicksNum( 8 );
	cw.chart()->setYLabelSkip( 2 );

    cw.resize( 400, 300 );

    a.setMainWidget( &cw );
    cw.show();
    return a.exec();
}
