#include "KChartWizard.h"
#include "KChart.h"

int main(int argc, char **argv)
{
  KApplication a(argc,argv);
  KChart* chart = new KChart();
  KChartWizard *wiz = new KChartWizard( chart, 0, "kchartwizard", false );
  a.setMainWidget(wiz);
  wiz->adjustSize();
  wiz->show();
  return a.exec();
}



