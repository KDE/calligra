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

class KChartWizardSetupAxesPage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardSetupAxesPage( QWidget* parent, KChartPart* chart );
  ~KChartWizardSetupAxesPage();
  bool chart3d;
public slots:
      void apply();
      void changeLabelColor(const QColor &);
      void changeLabelFont();
      void changeBorderColor(const QColor &);
      void changeGridColor(const QColor &);
protected:
      void paintEvent( QPaintEvent *_ev );

private:
  kchartWidget* preview;
  KChartPart* _chart;
  QCheckBox* grid;
  QCheckBox* border;
  QLineEdit *y_interval;
  QLineEdit *y_max;
  QLineEdit *y_min;
  QLineEdit *ylabel_fmt;
  QLineEdit *ylabel2_fmt;
  QSpinBox *angle;
  QSpinBox *depth;
  QSpinBox *barWidth;
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
