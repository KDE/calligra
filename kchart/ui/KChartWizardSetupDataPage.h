#ifndef _KCHARTWIZARDSELECTSETUPDATAPAGE_H
#define _KCHARTWIZARDSELECTSETUPDATAPAGE_H

#include <qwidget.h>

class QRadioButton;
class QCheckBox;

class KChartWizardSetupDataPage : public QWidget
{
  Q_OBJECT 

public:
  KChartWizardSetupDataPage( QWidget* parent );

private slots:
  void dataInRowsClicked();
  void dataInColsClicked();
  void firstColumnIsDescriptionToggled( bool );
  void firstRowIsDescriptionToggled( bool );

private:
  QRadioButton* datacol;
  QRadioButton* datarow;
  QCheckBox* rowdescript;
  QCheckBox* coldescript;
};

#endif
