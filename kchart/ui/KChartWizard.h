#ifndef KCHARTWIZARD_H
#define KCHARTWIZARD_H

#include "kwizard.h"

class KChart;

class KChartWizard : public KWizard
{
    Q_OBJECT
public:
    KChartWizard ( KChart* chart, QWidget *parent, const char* name, 
				   bool modal = true, WFlags f = 0 );
    ~KChartWizard();

  KChart* chart() const { return _chart; }

private:
  KChart* _chart;
  KWizardPage* _selectdatapage;
  KWizardPage* _selectcharttypepage;
  KWizardPage* _selectchartsubtypepage;
  KWizardPage* _setupdatapage;
  KWizardPage* _labelslegendpage;
  KWizardPage* _axespage;
};

#endif


