#ifndef _kchartWIZARDLABELSLEGENDPAGE_H
#define _kchartWIZARDLABELSLEGENDPAGE_H

#include <qwidget.h>
#include <kcolorbtn.h>
//#include <qpushbutton.h>
//#include <qcolor.h>

class QColor;
class QPushButton;
class QLineEdit;
class QRadioButton;
class KChartPart;
//class kchartWidget;

class KChartWizardLabelsLegendPage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardLabelsLegendPage( QWidget* parent, KChartPart* chart );
  ~KChartWizardLabelsLegendPage();

  bool ytitle2;
public slots:
  void apply();
  void changeXLabelFont();
  void changeYLabelFont();
  void changeTitleFont();
  void changeXLabelColor(const QColor &);
  void changeYLabelColor(const QColor &);
  void changeTitleColor(const QColor &);
  void changeYTitle2Color(const QColor &);
protected:
  void paintEvent( QPaintEvent * );

private:
  KChartPart* _chart;
  //kchartWidget* preview;
  QLineEdit* _xlabelED;
  QLineEdit* _ylabelED;
  QLineEdit* _titleED;
  QLineEdit* _ylabel2ED;
  QPushButton *xtitlefont;
  QPushButton *ytitlefont;
  QPushButton *titlefont;
  QPushButton *ytitle2font;
  KColorButton *xtitlecolor;
  KColorButton *ytitlecolor;
  KColorButton *titlecolor;
  KColorButton *ytitle2color;

  QFont xlabel;
  QFont ylabel;
  QFont title;
  QColor x_color;
  QColor y_color;
  QColor y_color2;
  QColor title_color;

};


#endif
