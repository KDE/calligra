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
  kchartWidget* preview;
  QLineEdit* _xlabelED;
  QLineEdit* _ylabelED;
  QLineEdit* _titleED;
  QLineEdit* _ylabel2ED;
  /*QRadioButton* _righttopRB;
  QRadioButton* _rightcenterRB;
  QRadioButton* _rightbottomRB;
  QRadioButton* _bottomleftRB;
  QRadioButton* _bottomcenterRB;
  QRadioButton* _bottomrightRB;*/
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
