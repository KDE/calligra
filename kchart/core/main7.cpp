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
	KChartData data7( 2 ); // two datasets
	data7.setXValue( 0, "1st" );
	data7.setXValue( 1, "2nd" );
	data7.setXValue( 2, "3rd" );
	data7.setXValue( 3, "4th" );
	data7.setXValue( 4, "5th" );
	data7.setXValue( 5, "6th" );
	data7.setXValue( 6, "7th" );
	data7.setXValue( 7, "8th" );
	data7.setXValue( 8, "9th" );
	data7.setYValue( 0, 0, 5 );
	data7.setYValue( 0, 1, 12 );
	data7.setYValue( 0, 2, 24 );
	data7.setYValue( 0, 3, 33 );
	data7.setYValue( 0, 4, 19 );
	data7.setYValue( 0, 5, 8 );
	data7.setYValue( 0, 6, 6 );
	data7.setYValue( 0, 7, 15 );
	data7.setYValue( 0, 8, 21 );
	data7.setYValue( 1, 0, 1 );
	data7.setYValue( 1, 1, 2 );
	data7.setYValue( 1, 2, 5 );
	data7.setYValue( 1, 3, 6 );
	data7.setYValue( 1, 4, 3 );
	data7.setYValue( 1, 5, 1.5 );
	data7.setYValue( 1, 6, 1 );
	data7.setYValue( 1, 7, 3 );
	data7.setYValue( 1, 8, 4 );
	KChartWidget cw7( Area );
	cw7.setChartData( &data7 );
	cw7.chart()->setXLabel( "X Label" );
	cw7.chart()->setYLabel( "Y Label" );
	cw7.chart()->setTitle( "An Area Graph" );
	cw7.chart()->setYMaxValue( 40 );
	cw7.chart()->setYTicksNum( 8 );
	cw7.chart()->setYLabelSkip( 2 );
	QStrList legends;
	legends.append( "one" );
	legends.append( "two" );
	cw7.chart()->setLegends( legends );

    cw7.resize( 400, 300 );

    a.setMainWidget( &cw7 );
    cw7.show();
    return a.exec();
}
