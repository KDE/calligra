#ifndef _kchartWIZARDSELECTCHARTTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTTYPEPAGE_H

#include <qwidget.h>

class QFrame;
class QPushButton;
class QButtonGroup;
class KChartPart;

class kchartWizardSelectChartTypePage : public QWidget
{
  Q_OBJECT 

public:
  kchartWizardSelectChartTypePage( QWidget* parent, KChartPart* chart );

private slots:
  void chartTypeSelected( int type );

private:
  KChartPart* _chart;
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
