#ifndef _KCHARTWIZARDSELECTSETUPDATAPAGE_H
#define _KCHARTWIZARDSELECTSETUPDATAPAGE_H

#include <qwidget.h>

class QRadioButton;
class QCheckBox;
class KChartWidget;
class KChartDoc;
class KChartWizard;

class KChartWizardSetupDataPage : public QWidget
{
  Q_OBJECT 

public:
  KChartWizardSetupDataPage( QWidget* parent, KChartDoc* chart );
  ~KChartWizardSetupDataPage();
  
private slots:
  void dataInRowsClicked();
  void dataInColsClicked();
  void firstColumnIsDescriptionToggled( bool );
  void firstRowIsDescriptionToggled( bool );

private:
  KChartDoc* _chart;
  KChartWizard* _parent;
  KChartWidget* preview;
  QRadioButton* datacol;
  QRadioButton* datarow;
  QCheckBox* rowdescript;
  QCheckBox* coldescript;
};

#endif
