#ifndef _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTSUBTYPEPAGE_H

#include <qwidget.h>

#include <kchartparams.h>

class QButtonGroup;
class QPushButton;
class KChartPart;

class kchartWizardSelectChartSubTypePage : public QWidget
{
  Q_OBJECT

public:
  kchartWizardSelectChartSubTypePage( QWidget* parent, KChartPart* chart );

  bool createChildren();

private slots:
  void chartSubTypeSelected( int type );
 
private:
  KChartPart* _chart;
  KChartType _charttype;
  int _no;

  QButtonGroup* _typeBG;
  QPushButton* _normalbarsPB;
  QPushButton* _barsontopPB;
  QPushButton* _barsinfrontPB;
};


#endif
