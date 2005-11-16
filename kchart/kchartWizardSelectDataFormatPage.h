#ifndef _kchartWIZARDSELECTDATAFORMATPAGE_H
#define _kchartWIZARDSELECTDATAFORMATPAGE_H

#include <qradiobutton.h>

#include "kchart_params.h"

class QButtonGroup;
class QPushButton;

namespace KChart
{

class KChartPart;

class KChartWizardSelectDataFormatPage : public QWidget
{
  Q_OBJECT
public:
  KChartWizardSelectDataFormatPage( QWidget* parent, KChartPart* chart );

public slots:
  void apply();

private:
  KChartPart* _chart;
  
  QRadioButton* m_rowMajor;
  QRadioButton* m_colMajor;
};

}  //KChart namespace

#endif
