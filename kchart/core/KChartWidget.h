/* $Id$ */

#ifndef _KCHARTWIDGET_H
#define _KCHARTWIDGET_H

#include <qwidget.h>

#include "KChart.h"

class KChartWidget : public QWidget
{
	Q_OBJECT

public:
  KChartWidget( KChartType charttype = Bars, QWidget* parent = 0, 
				const char* name = 0, WFlags f = 0 );
  KChartWidget( KChart* chart, QWidget* parent = 0,
				const char* name = 0, WFlags f = 0 );

  ~KChartWidget();

	void setChartType( KChartType charttype );
	KChartType chartType() const;

	void setChartData( KChartData* );
	KChartData* chartData() const;

  KChart* chart() const { return _chart; }

protected:
  void paintEvent( QPaintEvent* event );
  void resizeEvent( QResizeEvent* event );

private:
  KChart* _chart;
  bool _externalchart;

private slots:
  void newData();
};


#endif
