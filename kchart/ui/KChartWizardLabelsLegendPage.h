#ifndef _KCHARTWIZARDLABELSLEGENDPAGE_H
#define _KCHARTWIZARDLABELSLEGENDPAGE_H

#include <qwidget.h>

class QLineEdit;
class QRadioButton;
class KChart;

class KChartWizardLabelsLegendPage : public QWidget
{
  Q_OBJECT 

public:
  KChartWizardLabelsLegendPage( QWidget* parent, KChart* chart );

  void placementChanged( int );

private:
  KChart* _chart;
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
