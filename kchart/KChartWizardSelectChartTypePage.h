#ifndef _KCHARTWIZARDSELECTCHARTTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTTYPEPAGE_H

#include <qwidget.h>

class QFrame;
class QPushButton;
class QButtonGroup;
class KChartDoc;

class KChartWizardSelectChartTypePage : public QWidget
{
  Q_OBJECT 

public:
  KChartWizardSelectChartTypePage( QWidget* parent, KChartDoc* chart );

private slots:
  void chartTypeSelected( int type );

private:
  KChartDoc* _chart;
  QButtonGroup* _typeBG;
  QPushButton* _barsPB;
  QPushButton* _linesPB;
  QPushButton* _linespointsPB;
  QPushButton* _areaPB;
  QPushButton* _pointsPB;
  QPushButton* _piePB;
  QPushButton* _pie3DPB;
};


#endif
