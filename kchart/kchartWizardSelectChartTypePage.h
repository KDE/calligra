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
  int pos1, pos2;
  int xsize;
  int ysize;
  int ylabelsize;
  int xstep;
  int ystep;


  addButton(QString name, QString icon_name, int type);
};


#endif
