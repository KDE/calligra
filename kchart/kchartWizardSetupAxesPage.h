#ifndef _kchartWIZARDSETUPAXESPAGE_H
#define _kchartWIZARDSETUPAXESPAGE_H

#include <qwidget.h>
#include <qstring.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <kcolorbtn.h>
#include <qcolor.h>

class KChartPart;
class kchartWidget;

class kchartWizardSetupAxesPage : public QWidget
{
  Q_OBJECT

public:
  kchartWizardSetupAxesPage( QWidget* parent, KChartPart* chart );
  ~kchartWizardSetupAxesPage();

public slots:
      void apply();
      void changeLabelColor(const QColor &);
      void changeLabelFont();
      void changeBorderColor(const QColor &);
      void changeGridColor(const QColor &);
private:
  kchartWidget* preview;
  KChartPart* _chart;
  QCheckBox* grid;
  QCheckBox* border;
  QLineEdit *y_interval;
  QLineEdit *y_max;
  QLineEdit *y_min;
  QLineEdit *ylabel_fmt;
  QSpinBox *angle;
  QPushButton *ylabelFont;
  KColorButton *borderColor;
  KColorButton *gridColor;
  KColorButton *ylabelColor;
  QFont ylabel;
  QColor ycolor;
  QColor colorGrid;
  QColor colorBorder;
};

#endif
