#ifndef _KCHARTWIZARDSELECTDATAPAGE_H
#define _KCHARTWIZARDSELECTDATAPAGE_H

#include <qwidget.h>

class QLineEdit;

class KChartWizardSelectDataPage : public QWidget
{
public:
  KChartWizardSelectDataPage( QWidget* parent );

private:
  QLineEdit* rangeED;
};


#endif
