#ifndef _kchartWIZARDSELECTCHARTTYPEPAGE_H
#define _kchartWIZARDSELECTCHARTTYPEPAGE_H

#include <qwidget.h>
#include "kchartparams.h"

class QFrame;
class QPushButton;
class QButtonGroup;
class KChartPart;

class KChartWizardSelectChartTypePage : public QWidget
{
  Q_OBJECT

public:
  KChartWizardSelectChartTypePage( QWidget* parent, KChartPart* chart );

public slots:
  void apply();
private slots:
  void chartTypeSelected( int type );
signals:
  void chartChange(int);
private:
  KChartPart* _chart;
  QButtonGroup* _typeBG;
  int pos1, pos2;
  int xsize;
  int ysize;
  int ylabelsize;
  int xstep;
  int ystep;
  KChartType _type;

  void addButton(QString name, QString icon_name, int type);
};


#endif
