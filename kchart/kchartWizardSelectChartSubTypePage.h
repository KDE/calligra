#ifndef _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qradiobutton.h>

class QButtonGroup;
class QPushButton;
class KChartPart;

#include "kchart_params.h"

class KChartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT
public:
  KChartWizardSelectChartSubTypePage( QWidget* parent, KChartPart* chart );
  void changeSubTypeName( KDChartParams::ChartType _type);
  bool chartSubType;
public slots:
  void apply();

private:
  KChartPart* _chart;
  KDChartParams::ChartType _charttype;
  QRadioButton* normal;
  QRadioButton* stacked;
  QRadioButton* percent;
};


#endif
