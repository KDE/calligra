#ifndef _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qwidget.h>

#include <KoChartParameters.h>

class QButtonGroup;
class QPushButton;
class KChart;

class KChartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardSelectChartSubTypePage( QWidget* parent, KChart* chart );

  bool createChildren();

private slots:
  void chartSubTypeSelected( int type );
 
private:
  KChart* _chart;
  KChartType _charttype;
  int _no;

  QButtonGroup* _typeBG;
  QPushButton* _normalbarsPB;
  QPushButton* _barsontopPB;
  QPushButton* _barsinfrontPB;
};


#endif
