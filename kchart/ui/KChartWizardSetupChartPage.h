#ifndef _KCHARTWIZARDSELECTSETUPCHARTPAGE_H
#define _KCHARTWIZARDSELECTSETUPCHARTPAGE_H

#include <qwidget.h>

class QRadioButton;
class QCheckBox;

class KChartWizardSetupChartPage : public QWidget
{
public:
  KChartWizardSetupChartPage( QWidget* parent );

private:
  QRadioButton* datacol;
  QRadioButton* datarow;
  QCheckBox* rowdescript;
  QCheckBox* coldescript;
  QCheckBox* grid;
};

#endif
