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
	KChartData data10( 1 );
	data10.setXValue( 0, "Jan" );
	data10.setXValue( 1, "Feb" );
	data10.setXValue( 2, "Mar" );
	data10.setXValue( 3, "Apr" );
	data10.setXValue( 4, "May" );
	data10.setXValue( 5, "Jun" );
	data10.setXValue( 6, "Jul" );
	data10.setXValue( 7, "Aug" );
	data10.setXValue( 8, "Sep" );
	data10.setYValue( 0, 0, -4 );
	data10.setYValue( 0, 1, -3 );
	data10.setYValue( 0, 2, -1 );
	data10.setYValue( 0, 3, 1.5 );
	data10.setYValue( 0, 4, 3 );
	data10.setYValue( 0, 5, 6 );
	data10.setYValue( 0, 6, 5 );
	data10.setYValue( 0, 7, 3 );
	data10.setYValue( 0, 8, 4 );
	KChartWidget cw10( Lines );
	cw10.setChartData( &data10 );
	cw10.chart()->setXLabel( "Month" );
	cw10.chart()->setYLabel( "Measure of success" );
	cw10.chart()->setTitle( "A Simple Line Graph" );
	cw10.chart()->setYMaxValue( 8 );
	cw10.chart()->setYMinValue( -6 );
	cw10.chart()->setYTicksNum( 14 );
	cw10.chart()->setYLabelSkip( 2 );
	cw10.chart()->setBoxAxis( false );
	cw10.chart()->setLineWidth( 3 );

    cw10.resize( 400, 300 );

    a.setMainWidget( &cw10 );
    cw10.show();
    return a.exec();
}
