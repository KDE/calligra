#ifndef _kchartWIZARDSELECTDATAPAGE_H
#define _kchartWIZARDSELECTDATAPAGE_H

#include <qwidget.h>

class QLineEdit;

class kchartWizardSelectDataPage : public QWidget
{
friend class kchartWizard;

public:
  kchartWizardSelectDataPage( QWidget* parent );

private:
  QLineEdit* rangeED;
};


#endif
