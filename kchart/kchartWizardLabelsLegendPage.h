#ifndef _kchartWIZARDLABELSLEGENDPAGE_H
#define _kchartWIZARDLABELSLEGENDPAGE_H

#include <qwidget.h>

class QLineEdit;
class QRadioButton;
class KChartPart;
class kchartWidget;

class kchartWizardLabelsLegendPage : public QWidget
{
  Q_OBJECT 

public:
  kchartWizardLabelsLegendPage( QWidget* parent, KChartPart* chart );
  ~kchartWizardLabelsLegendPage();

  void placementChanged( int );

private:
  KChartPart* _chart;
  kchartWidget* preview;
  QLineEdit* _xlabelED;
  QLineEdit* _ylabelED;
  QLineEdit* _titleED;
  QRadioButton* _righttopRB;
  QRadioButton* _rightcenterRB;
  QRadioButton* _rightbottomRB;
  QRadioButton* _bottomleftRB;
  QRadioButton* _bottomcenterRB;
  QRadioButton* _bottomrightRB;
};


#endif
