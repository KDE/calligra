#ifndef _KCWIZARDAXESPAGE_H
#define _KCWIZARDAXESPAGE_H

#include <QWidget>
//Added by qt3to4:
#include <QPaintEvent>

#include <kcolorbutton.h>

class QColor;
class QLineEdit;
class QSpinBox;
class QPushButton;
class QCheckBox;

namespace KChart
{

class KChartPart;
class kchartWidget;

class KCWizardAxesPage : public QWidget
{
  Q_OBJECT

public:
  KCWizardAxesPage( QWidget* parent, KChartPart* chart );
  ~KCWizardAxesPage();
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
  QCheckBox* gridX;
  QCheckBox* gridY;
  QCheckBox* border;
  QLineEdit *y_interval;
  QLineEdit *y_max;
  QLineEdit *y_min;
  QLineEdit *ylabel_fmt;
#if 0
  QLineEdit *ylabel2_fmt;
#endif
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

}  //namespace KChart

#endif
