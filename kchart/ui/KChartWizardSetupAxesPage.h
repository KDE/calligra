#ifndef _KCHARTWIZARDSETUPAXESPAGE_H
#define _KCHARTWIZARDSETUPAXESPAGE_H

#include <qwidget.h>

class QRadioButton;
class QLineEdit;

class KChartWizardSetupAxesPage : public QWidget
{
public:
  KChartWizardSetupAxesPage( QWidget* parent );

private:
  QRadioButton* descriptYes;
  QRadioButton* descriptNo;
  QLineEdit* title;
  QLineEdit* xaxis;
  QLineEdit* yaxis;
};

#endif
