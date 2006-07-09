#ifndef _kchartWIZARDSELECTSETUPDATAPAGE_H
#define _kchartWIZARDSELECTSETUPDATAPAGE_H

#include <qwidget.h>

class QRadioButton;
class QCheckBox;

namespace KChart
{

class kchartWidget;
class KChartPart;
class KChartWizard;

class KChartWizardSetupDataPage : public QWidget
{
  Q_OBJECT 

public:
  KChartWizardSetupDataPage( QWidget* parent, KChartPart* chart );
  ~KChartWizardSetupDataPage();
  
private slots:
  void dataInRowsClicked();
  void dataInColsClicked();
  void firstColumnIsDescriptionToggled( bool );
  void firstRowIsDescriptionToggled( bool );

private:
  KChartPart* _chart;
  KChartWizard* _parent;

  kchartWidget* preview;
  QRadioButton* datacol;
  QRadioButton* datarow;
  QCheckBox* rowdescript;
  QCheckBox* coldescript;
};

}  //namespace KChart

#endif
