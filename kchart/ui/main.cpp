#include "KChartWizard.h"

int main(int argc, char **argv)
{
  KApplication a(argc,argv);
  KChartWizard *wiz = new KChartWizard(0, "kwizardtest", false);
  a.setMainWidget(wiz);
  wiz->adjustSize();
  wiz->show();
  return a.exec();
}



