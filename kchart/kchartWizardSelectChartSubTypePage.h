#ifndef _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <kchartparams.h>
#include <qradiobutton.h>

class QButtonGroup;
class QPushButton;
class KChartPart;

class kchartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT
public:
  kchartWizardSelectChartSubTypePage( QWidget* parent, KChartPart* chart );

  bool chartSubType;
public slots:
  void apply();

private:
  KChartPart* _chart;
  KChartType _charttype;
  QRadioButton *depth;
  QRadioButton *sum;
  QRadioButton *beside;
  QRadioButton *layer;
  QRadioButton *percent;
};


#endif
