#ifndef _KCHARTWIZARDSELECTCHARTTYPEPAGE_H
#define _KCHARTWIZARDSELECTCHARTTYPEPAGE_H

#include <qwidget.h>

class QFrame;

class KChartWizardSelectChartTypePage : public QWidget
{
public:
  KChartWizardSelectChartTypePage( QWidget* parent );

private:
  QFrame* newFrame( int x, int y );

  QFrame* frame1;
  QFrame* frame2;
  QFrame* frame3;
  QFrame* frame4;
  QFrame* frame5;
  QFrame* frame6;
  QFrame* frame7;
  QFrame* frame8;
  QFrame* frame9;
  QFrame* frame10;
  QFrame* frame11;
  QFrame* frame12;
  QFrame* frame13;
  QFrame* frame14;
  QFrame* frame15;
};


#endif
