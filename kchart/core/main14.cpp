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
	KChartData data14( 4 );
	data14.setXValue( 0, "1st" );
	data14.setXValue( 1, "2nd" );
	data14.setXValue( 2, "3rd" );
	data14.setXValue( 3, "4th" );
	data14.setXValue( 4, "5th" );
	data14.setXValue( 5, "6th" );
	data14.setXValue( 6, "7th" );
	data14.setYValue( 0, 0, 0.2 );
	data14.setYValue( 0, 1, 1.34 );
	data14.setYValue( 0, 2, 2.1 );
	data14.setYValue( 0, 3, 2.56 );
	data14.setYValue( 0, 4, 3.03 );
	data14.setYValue( 0, 5, 4.05 );
	data14.setYValue( 0, 6, 5.6 );
	KChartWidget cw14( Pie );
	cw14.setChartData( &data14 );

    cw14.resize( 200, 200 );

    a.setMainWidget( &cw14 );
    cw14.show();
    return a.exec();
}
