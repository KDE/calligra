#ifndef KCHARTWIZARD_H
#define KCHARTWIZARD_H

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

signals:
  // valid values for rowcol: Row: data is in rows, Col: data is in cols
  void needNewData( const char* area, int rowcol, bool firstRowIsLegend,
					bool firstColIsLabel );

private:
  KChartWizardSelectDataPage* selectDataPage() const {
	return (KChartWizardSelectDataPage*)_selectdatapage->w;
  }
  KChartWizardSelectChartTypePage* selectChartTypePage() const {
	return (KChartWizardSelectChartTypePage*)_selectcharttypepage->w;
  }
  KChartWizardSelectChartSubTypePage* selectChartSubTypePage() const {
	return (KChartWizardSelectChartSubTypePage*)_selectchartsubtypepage->w;
  }
  KChartWizardSetupDataPage* setupDataPage() const {
	return (KChartWizardSetupDataPage*)_setupdatapage->w;
  }
  KChartWizardLabelsLegendPage* labelsLegendPage() const {
	return (KChartWizardLabelsLegendPage*)_labelslegendpage->w;
  }
  KChartWizardSetupAxesPage* axesPage() const {
	return (KChartWizardSetupAxesPage*)_axespage->w;
  }

  KChart* _chart;
  KWizardPage* _selectdatapage;
  KWizardPage* _selectcharttypepage;
  KWizardPage* _selectchartsubtypepage;
  KWizardPage* _setupdatapage;
  KWizardPage* _labelslegendpage;
  KWizardPage* _axespage;
};

#endif


