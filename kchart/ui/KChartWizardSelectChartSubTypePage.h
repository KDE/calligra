#ifndef _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qwidget.h>

class KChart;

class KChartWizardSelectChartSubTypePage : public QWidget
{
public:
  KChartWizardSelectChartSubTypePage( QWidget* parent, KChart* chart );

private:
  KChart* _chart;
};


#endif
