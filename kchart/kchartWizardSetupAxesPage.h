#ifndef _kchartWIZARDSETUPAXESPAGE_H
#define _kchartWIZARDSETUPAXESPAGE_H

#include <qwidget.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>

class KChartPart;
class kchartWidget;

class kchartWizardSetupAxesPage : public QWidget
{
  Q_OBJECT

public:
  kchartWizardSetupAxesPage( QWidget* parent, KChartPart* chart );
  ~kchartWizardSetupAxesPage();

public slots:
      //void setGrid(bool value);
      void apply();
private:
  kchartWidget* preview;
  KChartPart* _chart;
  QCheckBox* grid;
  QCheckBox* border;
  QLineEdit *y_interval;
  QLineEdit *y_max;
  QLineEdit *y_min;
  QSpinBox *angle;
};

#endif
