#include "KChartWizard.h"
#include "KChart.h"
#include "KChartWidget.h"
#include "KChartData.h"

KChart* chart;
KChartWizard* wiz;

class MyReceiver : public QObject
{
  Q_OBJECT

public slots:
  void showChart();
};


void MyReceiver::showChart()
{
  KChartWidget* cw = new KChartWidget( wiz->chart() );
  cw->resize( 400, 300 );
  cw->show();
}



int main(int argc, char **argv)
{
  KApplication a(argc,argv);

  // setup some chart data
  KChartData data2( 2 ); // two datasets
  data2.setXValue( 0, "1st" );
  data2.setXValue( 1, "2nd" );
  data2.setXValue( 2, "3rd" );
  data2.setXValue( 3, "4th" );
  data2.setXValue( 4, "5th" );
  data2.setXValue( 5, "6th" );
  data2.setXValue( 6, "7th" );
  data2.setXValue( 7, "8th" );
  data2.setXValue( 8, "9th" );
  data2.setYValue( 0, 0, 5 );
  data2.setYValue( 0, 1, 12 );
  data2.setYValue( 0, 2, 24 );
  data2.setYValue( 0, 3, 33 );
  data2.setYValue( 0, 4, 19 );
  data2.setYValue( 0, 5, 8 );
  data2.setYValue( 0, 6, 6 );
  data2.setYValue( 0, 7, 15 );
  data2.setYValue( 0, 8, 21 );
  data2.setYValue( 1, 0, 1 );
  data2.setYValue( 1, 1, 2 );
  data2.setYValue( 1, 2, 5 );
  data2.setYValue( 1, 3, 6 );
  data2.setYValue( 1, 4, 3 );
  data2.setYValue( 1, 5, 1.5 );
  data2.setYValue( 1, 6, 1 );
  data2.setYValue( 1, 7, 3 );
  data2.setYValue( 1, 8, 4 );
  
  chart = new KChart();
  chart->setChartData( &data2 );
  wiz = new KChartWizard( chart, 0, "kchartwizard", false );
  wiz->setDataArea( "A1:C4" );
  a.setMainWidget(wiz);

  MyReceiver* rec = new MyReceiver;
  QObject::connect( wiz, SIGNAL( okclicked() ), rec, SLOT( showChart() ) );

  wiz->adjustSize();
  wiz->show();
  return a.exec();
}

#include "main.moc"


