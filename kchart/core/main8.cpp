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
	KChartData data8( 2 ); // two datasets
	data8.setXValue( 0, "1st" );
	data8.setXValue( 1, "2nd" );
	data8.setXValue( 2, "3rd" );
	data8.setXValue( 3, "4th" );
	data8.setXValue( 4, "5th" );
	data8.setXValue( 5, "6th" );
	data8.setXValue( 6, "7th" );
	data8.setXValue( 7, "8th" );
	data8.setXValue( 8, "9th" );
	data8.setYValue( 0, 0, 5 );
	data8.setYValue( 0, 1, 12 );
	data8.setYValue( 0, 2, 24 );
	data8.setYValue( 0, 3, 33 );
	data8.setYValue( 0, 4, 19 );
	data8.setYValue( 0, 5, 8 );
	data8.setYValue( 0, 6, 6 );
	data8.setYValue( 0, 7, 15 );
	data8.setYValue( 0, 8, 21 );
	data8.setYValue( 1, 0, 1 );
	data8.setYValue( 1, 1, 2 );
	data8.setYValue( 1, 2, 5 );
	data8.setYValue( 1, 3, 6 );
	data8.setYValue( 1, 4, 3 );
	data8.setYValue( 1, 5, 1.5 );
	//	data8.setYValue( 1, 6, 1 );
	data8.setYValue( 1, 7, 3 );
	data8.setYValue( 1, 8, 4 );
	KChartWidget cw8( Area );
	cw8.setChartData( &data8 );
	cw8.chart()->setXLabel( "X Label" );
	cw8.chart()->setYLabel( "Y Label" );
	cw8.chart()->setTitle( "A Points Graph" );
	cw8.chart()->setYMaxValue( 40 );
	cw8.chart()->setYTicksNum( 8 );
	cw8.chart()->setYLabelSkip( 2 );
	cw8.chart()->setLongTicks( true );
	cw8.chart()->setMarkerSize( 6 );
	KChartMarkerList list;
	list.append( 1 );
	list.append( 7 );
	list.append( 5 );
	cw8.chart()->setMarkers( list );
	QStrList legends;
	legends.append( "one" );
	legends.append( "two" );
	cw8.chart()->setLegends( legends );
	cw8.chart()->setLegendPlacement( RightCenter );

    cw8.resize( 400, 300 );

    a.setMainWidget( &cw8 );
    cw8.show();
    return a.exec();
}
