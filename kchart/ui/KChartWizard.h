#ifndef KCHARTWIZARD_H
#define KCHARTWIZARD_H

#include <klocale.h>
#include "kwizard.h"

class KChart;
class KChartWizardSetupAxesPage;
class KChartWizardLabelsLegendPage;
class KChartWizardSelectChartSubTypePage;
class KChartWizardSelectDataPage;
class KChartWizardSetupDataPage;
class KChartWizardSelectChartTypePage;

class KChartWizard : public KWizard
{
    Q_OBJECT
public:
    KChartWizard ( KChart* chart, QWidget *parent, const char* name, 
				   bool modal = true, WFlags f = 0 );
    ~KChartWizard();

  KChart* chart() const { return _chart; }
  void setDataArea( QString area ); 
  QString dataArea() const;

  enum RowCol { Row, Col };

  void emitNeedNewData( const char* area, int rowcol, bool firstRowIsLegend,
						bool firstColIsLabel );

signals:
  // valid values for rowcol: Row: data is in rows, Col: data is in cols
  void needNewData( const char* area, int rowcol, bool firstRowIsLegend,
					bool firstColIsLabel );

private:
  /*
  QWidget* selectDataPage() const {
	return _selectdatapagew;
  }
  QWidget* selectChartTypePage() const {
	return _selectcharttypepage;
  }
  QWidget* selectChartSubTypePage() const {
	return _selectchartsubtypepage;
  }
  QWidget* setupDataPage() const {
	return _setupdatapage;
  }
  QWidget* labelsLegendPage() const {
	return _labelslegendpage;
  }
  QWidget* axesPage() const {
	return _axespage;
  }
  */

  KChart* _chart;
  QWidget* _selectdatapage;
  QWidget* _selectcharttypepage;
  QWidget* _selectchartsubtypepage;
  QWidget* _setupdatapage;
  QWidget* _labelslegendpage;
  QWidget* _axespage;
};

#endif


