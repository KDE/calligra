#ifndef KCHARTWIZARD_H
#define KCHARTWIZARD_H

#include "kwizard.h"

class KChartWizard : public KWizard
{
    Q_OBJECT
public:
    KChartWizard ( QWidget *parent, const char* name, bool modal = true, 
				  WFlags f = 0 );
    ~KChartWizard();

private:
  KWizardPage* wizpage1;
  KWizardPage* wizpage2;
  KWizardPage* wizpage3;
  KWizardPage* wizpage4;
};

#endif


