#ifndef _KCWIZARDDATAPAGE_H
#define _KCWIZARDDATAPAGE_H


#include "kchart_params.h"

class QLineEdit;
class QRadioButton;
class QCheckBox;
class Q3ButtonGroup;
class QPushButton;


namespace KChart
{

class KChartPart;

class KCWizardDataPage : public QWidget
{
  Q_OBJECT
public:
  KCWizardDataPage( QWidget* parent, KChartPart* chart );

  QString  dataArea() const;
  void     setDataArea( const QString &area );

public slots:
  void apply();

private:
  KChartPart   *m_chart;
  
  QLineEdit    *m_dataArea;
  QRadioButton *m_rowMajor;
  QRadioButton *m_colMajor;
  QCheckBox    *m_firstRowAsLabel;
  QCheckBox    *m_firstColAsLabel;
};

}  //KChart namespace

#endif
