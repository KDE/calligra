#ifndef _kchartWIZARDLABELSLEGENDPAGE_H
#define _kchartWIZARDLABELSLEGENDPAGE_H

#include <qwidget.h>
#include <qpushbutton.h>
#include <kcolorbtn.h>
#include <qcolor.h>
class QLineEdit;
class QRadioButton;
class KChartPart;
class kchartWidget;

class kchartWizardLabelsLegendPage : public QWidget
{
  Q_OBJECT

public:
  kchartWizardLabelsLegendPage( QWidget* parent, KChartPart* chart );
  ~kchartWizardLabelsLegendPage();

  //void placementChanged( int );
public slots:
  void apply();
  void changeXLabelFont();
  void changeYLabelFont();
  void changeTitleFont();
  void changeXLabelColor(const QColor &);
  void changeYLabelColor(const QColor &);
  void changeTitleColor(const QColor &);
private:
  KChartPart* _chart;
  kchartWidget* preview;
  QLineEdit* _xlabelED;
  QLineEdit* _ylabelED;
  QLineEdit* _titleED;
  QRadioButton* _righttopRB;
  QRadioButton* _rightcenterRB;
  QRadioButton* _rightbottomRB;
  QRadioButton* _bottomleftRB;
  QRadioButton* _bottomcenterRB;
  QRadioButton* _bottomrightRB;
  QPushButton *xtitlefont;
  QPushButton *ytitlefont;
  QPushButton *titlefont;
  KColorButton *xtitlecolor;
  KColorButton *ytitlecolor;
  KColorButton *titlecolor;
  QFont xlabel;
  QFont ylabel;
  QFont title;
  QColor x_color;
  QColor y_color;
  QColor title_color;

};


#endif
