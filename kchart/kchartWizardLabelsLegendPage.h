#ifndef _kchartWIZARDLABELSLEGENDPAGE_H
#define _kchartWIZARDLABELSLEGENDPAGE_H

#include <QWidget>
//Added by qt3to4:
#include <QPaintEvent>
#include <kcolorbutton.h>

class QColor;
class QPushButton;
class QLineEdit;
class QRadioButton;

namespace KChart
{

class KChartPart;

class KChartWizardLabelsLegendPage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardLabelsLegendPage( QWidget* parent, KChartPart* chart );
  ~KChartWizardLabelsLegendPage();

#if 0
  bool ytitle2;
#endif
public slots:
  void apply();
  void changeXLabelFont();
  void changeYLabelFont();
#if 0
  //void changeY2LabelFont();
#endif
  void changeTitleFont();
  void changeLegendTitleFont();
  void changeLegendTextFont();
  void changeXLabelColor(const QColor &);
  void changeYLabelColor(const QColor &);
  void changeTitleColor(const QColor &);
#if 0
  //void changeYTitle2Color(const QColor &);
#endif
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
#if 0
  QLineEdit* _ylabel2ED;
#endif
  QPushButton *xtitlefont;
  QPushButton *ytitlefont;
  QPushButton *titlefont;
#if 0
  QPushButton *ytitle2font;
#endif
  KColorButton *xtitlecolor;
  KColorButton *ytitlecolor;
  KColorButton *titlecolor;
#if 0
  KColorButton *ytitle2color;
#endif

  QLineEdit *_legendTitleText;

  KColorButton *legendTextColor;
  KColorButton *legendTitleColor;

  QPushButton *legendTextFont;
  QPushButton *legendTitleFont;

  QFont xlabel;
  QFont ylabel;
#if 0
  QFont ylabel2;
#endif
  QFont title;
  QColor x_color;
  QColor y_color;
#if 0
  QColor y_color2;
#endif
  QColor title_color;

  QColor _legendTextColor;
  QColor _legendTitleColor;

  QFont _legendTextFont;
  QFont _legendTitleFont;

};

}  //KChart namespace

#endif
