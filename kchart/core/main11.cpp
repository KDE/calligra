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
	KChartData data11( 4 );
	data11.setXValue( 0, "1st" );
	data11.setXValue( 1, "2nd" );
	data11.setXValue( 2, "3rd" );
	data11.setXValue( 3, "4th" );
	data11.setXValue( 4, "5th" );
	data11.setXValue( 5, "6th" );
	data11.setXValue( 6, "7th" );
	data11.setXValue( 7, "8th" );
	data11.setXValue( 8, "9th" );
	data11.setYValue( 0, 0, 1 );
	data11.setYValue( 0, 1, 2 );
	data11.setYValue( 0, 2, 5 );
	data11.setYValue( 0, 3, 6 );
	data11.setYValue( 0, 4, 3 );
	data11.setYValue( 0, 5, 1.5 );
	data11.setYValue( 0, 6, -1 );
	data11.setYValue( 0, 7, -3 );
	data11.setYValue( 0, 8, -4 );
	data11.setYValue( 1, 0, -4 );
	data11.setYValue( 1, 1, -3 );
	data11.setYValue( 1, 2, 1 );
	data11.setYValue( 1, 3, 1 );
	data11.setYValue( 1, 4, -3 );
	data11.setYValue( 1, 5, -1.5 );
	data11.setYValue( 1, 6, -2 );
	data11.setYValue( 1, 7, -1 );
	data11.setYValue( 1, 8, 0 );
	data11.setYValue( 2, 0, 9 );
	data11.setYValue( 2, 1, 8 );
	data11.setYValue( 2, 2, 9 );
	data11.setYValue( 2, 3, 8.4 );
	data11.setYValue( 2, 4, 7.1 );
	data11.setYValue( 2, 5, 7.5 );
	data11.setYValue( 2, 6, 8 );
	data11.setYValue( 2, 7, 3 );
	data11.setYValue( 2, 8, -3 );
	data11.setYValue( 3, 0, 0.1 );
	data11.setYValue( 3, 1, 0.2 );
	data11.setYValue( 3, 2, 0.5 );
	data11.setYValue( 3, 3, 0.4 );
	data11.setYValue( 3, 4, 0.3 );
	data11.setYValue( 3, 5, 0.5 );
	data11.setYValue( 3, 6, 0.1 );
	data11.setYValue( 3, 7, 0 );
	data11.setYValue( 3, 8, 0.4 );
	KChartWidget cw11( Lines );
	cw11.setChartData( &data11 );
	cw11.chart()->setXLabel( "X Label" );
	cw11.chart()->setYLabel( "Y Label success" );
	cw11.chart()->setTitle( "A Multiple Line Graph" );
	cw11.chart()->setYMaxValue( 10 );
	cw11.chart()->setYMinValue( -5 );
	cw11.chart()->setYTicksNum( 3 );
	cw11.chart()->setYLabelSkip( 1 );
	cw11.chart()->setZeroAxisOnly( false );
	cw11.chart()->setLongTicks( true );
	cw11.chart()->setXTicks( false );
	KChartColorArray colors;
	colors.setColor( 0, Qt::darkGreen );
	colors.setColor( 1, Qt::darkRed );
	colors.setColor( 2, Qt::darkYellow );
	colors.setColor( 3, Qt::magenta );
	cw11.chart()->setDataColors( &colors );
	Qt::PenStyle linetypes[4] = { Qt::SolidLine, Qt::DashLine, Qt::DotLine, Qt::DashDotLine };
	cw11.chart()->setLineTypes( linetypes, 4 );
	cw11.chart()->setLegendMarkerWidth( 24 );
	cw11.chart()->setLineWidth( 3 );

    cw11.resize( 400, 300 );

    a.setMainWidget( &cw11 );
    cw11.show();
    return a.exec();
}
