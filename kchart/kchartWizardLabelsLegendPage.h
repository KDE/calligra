#ifndef _kchartWIZARDLABELSLEGENDPAGE_H
#define _kchartWIZARDLABELSLEGENDPAGE_H

#include <qwidget.h>
#include <kcolorbutton.h>

class QColor;
class QPushButton;
class QLineEdit;
class QRadioButton;
class KChartPart;

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
  void changeY2LabelFont();
  void changeTitleFont();
  void changeLegendTitleFont();
  void changeLegendTextFont();
  void changeXLabelColor(const QColor &);
  void changeYLabelColor(const QColor &);
  void changeTitleColor(const QColor &);
  void changeYTitle2Color(const QColor &);
  void changeLegendTitleColor(const QColor &);
  void changeLegendTextColor(const QColor &);

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

  QLineEdit *_legendTitleText;

  KColorButton *legendTextColor;
  KColorButton *legendTitleColor;

  QPushButton *legendTextFont;
  QPushButton *legendTitleFont;

  QFont xlabel;
  QFont ylabel;
  QFont ylabel2;
  QFont title;
  QColor x_color;
  QColor y_color;
  QColor y_color2;
  QColor title_color;

  QColor _legendTextColor;
  QColor _legendTitleColor;

  QFont _legendTextFont;
  QFont _legendTitleFont;

};


#endif
