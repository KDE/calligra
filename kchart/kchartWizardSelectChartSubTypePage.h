#ifndef _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qradiobutton.h>

#include "kchart_params.h"

class QButtonGroup;
class QPushButton;

namespace KChart
{

class KChartPart;

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

}  //KChart namespace

#endif
