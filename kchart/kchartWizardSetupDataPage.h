#ifndef _kchartWIZARDSELECTSETUPDATAPAGE_H
#define _kchartWIZARDSELECTSETUPDATAPAGE_H

#include <qwidget.h>

class QRadioButton;
class QCheckBox;
class kchartWidget;
class KChartPart;
class kchartWizard;

class kchartWizardSetupDataPage : public QWidget
{
  Q_OBJECT 

public:
  kchartWizardSetupDataPage( QWidget* parent, KChartPart* chart );
  ~kchartWizardSetupDataPage();
  
private slots:
  void dataInRowsClicked();
  void dataInColsClicked();
  void firstColumnIsDescriptionToggled( bool );
  void firstRowIsDescriptionToggled( bool );

private:
  KChartPart* _chart;
  kchartWizard* _parent;
  kchartWidget* preview;
  QRadioButton* datacol;
  QRadioButton* datarow;
  QCheckBox* rowdescript;
  QCheckBox* coldescript;
};

#endif
