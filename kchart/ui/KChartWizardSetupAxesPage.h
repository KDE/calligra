#ifndef _KCHARTWIZARDSETUPAXESPAGE_H
#define _KCHARTWIZARDSETUPAXESPAGE_H

#include <qwidget.h>

class KChart;

class KChartWizardSetupAxesPage : public QWidget
{
public:
  KChartWizardSetupAxesPage( QWidget* parent, KChart* chart );

private:
  KChart* _chart;
};

#endif
