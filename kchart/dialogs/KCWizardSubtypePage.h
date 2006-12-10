#ifndef _KCWIZARDSUBTYPEPAGE_H
#define _KCWIZARDSUBTYPEPAGE_H


#include "kchart_params.h"

class Q3ButtonGroup;
class QPushButton;
class QRadioButton;
class QSpinBox;

namespace KChart
{

class KChartPart;

class KCWizardSubtypePage : public QWidget
{
  Q_OBJECT
public:
  KCWizardSubtypePage( QWidget* parent, KChartPart* chart );
  void changeSubTypeName( KChartParams::ChartType _type);

  bool chartSubType;
public slots:
  void apply();

private:
  KChartPart              *m_chart;
  KChartParams::ChartType  m_charttype;

  // Widgets
  QRadioButton  *m_normal;
  QRadioButton  *m_stacked;
  QRadioButton  *m_percent;
  QSpinBox      *m_numLines;
};

}  //KChart namespace

#endif
