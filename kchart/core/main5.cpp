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
	KChartData data5( 2 ); // two datasets
	data5.setXValue( 0, "1st" );
	data5.setXValue( 1, "2nd" );
	data5.setXValue( 2, "3rd" );
	data5.setXValue( 3, "4th" );
	data5.setXValue( 4, "5th" );
	data5.setXValue( 5, "6th" );
	data5.setXValue( 6, "7th" );
	data5.setXValue( 7, "8th" );
	data5.setXValue( 8, "9th" );
	data5.setYValue( 0, 0, 5 );
	data5.setYValue( 0, 1, 12 );
	data5.setYValue( 0, 2, 24 );
	data5.setYValue( 0, 3, 33 );
	data5.setYValue( 0, 4, 19 );
	data5.setYValue( 0, 5, 8 );
	data5.setYValue( 0, 6, 6 );
	data5.setYValue( 0, 7, 15 );
	data5.setYValue( 0, 8, 21 );
	data5.setYValue( 1, 0, 1 );
	data5.setYValue( 1, 1, 2 );
	data5.setYValue( 1, 2, 35 );
	data5.setYValue( 1, 3, 16 );
	data5.setYValue( 1, 4, 3 );
	data5.setYValue( 1, 5, 1.5 );
	data5.setYValue( 1, 6, 1 );
	data5.setYValue( 1, 7, 3 );
	data5.setYValue( 1, 8, 4 );
	KChartWidget cw5;
	cw5.setChartData( &data5 );
	cw5.chart()->setXLabel( "X Label" );
	cw5.chart()->setY1Label( "Y Label" );
	cw5.chart()->setTitle( "Bars on top of each other (incremental)" );
	cw5.chart()->setYTicksNum( 8 );
	cw5.chart()->setYLabelSkip( 2 );
	cw5.chart()->setOverwrite( OnTop );
	QStrList legends;
	legends.append( "Offset" );
	legends.append( "Increment" );
	cw5.chart()->setLegends( legends );

    cw5.resize( 400, 300 );

    a.setMainWidget( &cw5 );
    cw5.show();
    return a.exec();
}
