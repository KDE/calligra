#ifndef __main_h
#define __main_h
#include "KChartWizard.h"
#include "KChart.h"
#include "KChartWidget.h"
#include "KChartData.h"

class MyReceiver : public QObject
{
  Q_OBJECT
public:
  MyReceiver( KChartWizard * _wiz )
    : wiz ( _wiz ) {}
  ~MyReceiver() {}
    
public slots:
  void showChart();
private:
 KChartWizard * wiz;
};

#endif

