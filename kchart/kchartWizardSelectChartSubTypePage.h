#ifndef _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qradiobutton.h>

class QButtonGroup;
class QPushButton;
class KChartPart;

class KChartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT
public:
  KChartWizardSelectChartSubTypePage( QWidget* parent, KChartPart* chart );

  bool chartSubType;
public slots:
  void apply();

private:
  KChartPart* _chart;
#warning "Put back in"
  //  KChartType _charttype;
  QRadioButton *depth;
  QRadioButton *sum;
  QRadioButton *beside;
  QRadioButton *layer;
  QRadioButton *percent;
};


#endif
