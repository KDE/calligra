#ifndef _kchartWIZARDSETUPAXESPAGE_H
#define _kchartWIZARDSETUPAXESPAGE_H

#include <qwidget.h>
#include <qstring.h>

class KChartPart;
class kchartWidget;

class kchartWizardSetupAxesPage : public QWidget
{
  Q_OBJECT

public:
  kchartWizardSetupAxesPage( QWidget* parent, KChartPart* chart );
  ~kchartWizardSetupAxesPage();

public slots:
      void setGrid(bool value);
private:
  kchartWidget* preview;
  KChartPart* _chart;
};

#endif
