/* $Id: $ */
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
	KChartData data12( 4 );
	data12.setXValue( 0, "1st" );
	data12.setXValue( 1, "2nd" );
	data12.setXValue( 2, "3rd" );
	data12.setXValue( 3, "4th" );
	data12.setXValue( 4, "5th" );
	data12.setXValue( 5, "6th" );
	data12.setYValue( 0, 0, 4 );
	data12.setYValue( 0, 1, 2 );
	data12.setYValue( 0, 2, 3 );
	data12.setYValue( 0, 3, 4 );
	data12.setYValue( 0, 4, 3 );
	data12.setYValue( 0, 5, 3.5 );
	KChartWidget cw12( Pie3D );
	cw12.chart()->setChartData( &data12 );
	cw12.chart()->setTitle( "A Pie Chart" );
	cw12.chart()->setXLabel( "Label" );
	cw12.chart()->setAxisLabelColor( black );
	cw12.chart()->setPieHeight( 36 );

    cw12.resize( 250, 200 );

    a.setMainWidget( &cw12 );
    cw12.show();
    return a.exec();
}
