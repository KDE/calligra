#ifndef _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qwidget.h>

#include <KChartTypes.h>

class QButtonGroup;
class QPushButton;
class KChart;

class KChartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardSelectChartSubTypePage( QWidget* parent, KChart* chart );
  void setNumber( int );

public slots:
  void createChildren( int no );

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
