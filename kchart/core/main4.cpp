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
	KChartData data4( 2 ); // two datasets
	data4.setXValue( 0, "1st" );
	data4.setXValue( 1, "2nd" );
	data4.setXValue( 2, "3rd" );
	data4.setXValue( 3, "4th" );
	data4.setXValue( 4, "5th" );
	data4.setXValue( 5, "6th" );
	data4.setXValue( 6, "7th" );
	data4.setXValue( 7, "8th" );
	data4.setXValue( 8, "9th" );
	data4.setYValue( 0, 0, 5 );
	data4.setYValue( 0, 1, 12 );
	data4.setYValue( 0, 2, 24 );
	data4.setYValue( 0, 3, 33 );
	data4.setYValue( 0, 4, 19 );
	data4.setYValue( 0, 5, 8 );
	data4.setYValue( 0, 6, 6 );
	data4.setYValue( 0, 7, 15 );
	data4.setYValue( 0, 8, 21 );
	data4.setYValue( 1, 0, 1 );
	data4.setYValue( 1, 1, 2 );
	data4.setYValue( 1, 2, 5 );
	data4.setYValue( 1, 3, 6 );
	data4.setYValue( 1, 4, 3 );
	data4.setYValue( 1, 5, 1.5 );
	data4.setYValue( 1, 6, 1 );
	data4.setYValue( 1, 7, 3 );
	data4.setYValue( 1, 8, 4 );
	KChartWidget cw4;
	cw4.setChartData( &data4 );
	cw4.chart()->setXLabel( "X Label" );
	cw4.chart()->setY1Label( "Y1 Label" );
	cw4.chart()->setY2Label( "Y2 Label" );
	cw4.chart()->setTitle( "Using two axes" );
	cw4.chart()->setYTicksNum( 8 );
	cw4.chart()->setYLabelSkip( 2 );
	cw4.chart()->setY1MaxValue( 40 );
	cw4.chart()->setY2MaxValue( 8 );
	cw4.chart()->setLongTicks( true );
	cw4.chart()->setTwoAxes( true );
	cw4.chart()->setLegendPlacement( RightTop );
	QStrList legends;
	legends.append( "Left axis" );
	legends.append( "Right axis" );
	cw4.chart()->setLegends( legends );

    cw4.resize( 400, 300 );

    a.setMainWidget( &cw4 );
    cw4.show();
    return a.exec();
}
