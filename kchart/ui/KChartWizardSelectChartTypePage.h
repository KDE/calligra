#ifndef _KCHARTWIZARDSELECTCHARTTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTTYPEPAGE_H

#include <qpixmap.h>
#include <qwidget.h>

class QFrame;
class QPushButton;
class QButtonGroup;
class KChart;

class KChartWizardSelectChartTypePage : public QWidget
{
  Q_OBJECT 

public:
  KChartWizardSelectChartTypePage( QWidget* parent, KChart* chart );

private slots:
  void chartTypeSelected( int type );

private:
  KChart* _chart;
  QButtonGroup* _typeBG;
  QPushButton* _barsPB;
  QPushButton* _linesPB;
  QPushButton* _linespointsPB;
  QPushButton* _areaPB;
  QPushButton* _pointsPB;
  QPushButton* _piePB;
  QPushButton* _pie3DPB;
  QPixmap _barsPM;
  QPixmap _linesPM;
  QPixmap _linespointsPM;
  QPixmap _areaPM;
  QPixmap _pointsPM;
  QPixmap _piePM;
  QPixmap _pie3DPM;
};


#endif
