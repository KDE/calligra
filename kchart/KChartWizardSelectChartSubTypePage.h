#ifndef _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qwidget.h>

#include <KoChartParameters.h>

class QButtonGroup;
class QPushButton;
class KChartDoc;

class KChartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardSelectChartSubTypePage( QWidget* parent, KChartDoc* chart );

  bool createChildren();

private slots:
  void chartSubTypeSelected( int type );
 
private:
  KChartDoc* _chart;
  KChartType _charttype;
  int _no;

  QButtonGroup* _typeBG;
  QPushButton* _normalbarsPB;
  QPushButton* _barsontopPB;
  QPushButton* _barsinfrontPB;
};


#endif
