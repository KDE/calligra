#ifndef _KCHARTWIZARDSETUPAXESPAGE_H
#define _KCHARTWIZARDSETUPAXESPAGE_H

#include <qwidget.h>

class KChart;
class KChartWidget;

class KChartWizardSetupAxesPage : public QWidget
{
public:
  KChartWizardSetupAxesPage( QWidget* parent, KChart* chart );
  ~KChartWizardSetupAxesPage();

private:
  KChartWidget* preview;
  KChart* _chart;
};

#endif
