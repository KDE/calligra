#ifndef _KCHARTWIZARDSELECTSETUPDATAPAGE_H
#define _KCHARTWIZARDSELECTSETUPDATAPAGE_H

#include <qwidget.h>

class QRadioButton;
class QCheckBox;

class KChartWizardSetupDataPage : public QWidget
{
public:
  KChartWizardSetupDataPage( QWidget* parent );

private:
  QRadioButton* datacol;
  QRadioButton* datarow;
  QCheckBox* rowdescript;
  QCheckBox* coldescript;
  QCheckBox* grid;
};

#endif
