#ifndef kchartWIZARD_H
#define kchartWIZARD_H

#include <klocale.h>
#include "kwizard.h"
#include "kchart_part.h"

class KChartPart;
class kchartWizardSetupAxesPage;
class kchartWizardLabelsLegendPage;
class kchartWizardSelectChartSubTypePage;
class kchartWizardSelectDataPage;
class kchartWizardSetupDataPage;
class kchartWizardSelectChartTypePage;

class kchartWizard : public KWizard
{
    Q_OBJECT
public:
    kchartWizard ( KChartPart* chart, QWidget *parent, const char* name, 
				   bool modal = true, WFlags f = 0 );
    ~kchartWizard();

    KChartPart* chart() const { return _chart; };
  void setDataArea( QString area ); 
  QString dataArea() const;

  enum RowCol { Row, Col };

  void emitNeedNewData( const char* area, int rowcol, bool firstRowIsLegend,
						bool firstColIsLabel );

  virtual bool appropriate( QWidget * w ) const;

signals:
  // valid values for rowcol: Row: data is in rows, Col: data is in cols
  void needNewData( const char* area, int rowcol, bool firstRowIsLegend,
					bool firstColIsLabel );
  void finished();
  void cancelled();
  
protected slots:
  virtual void next();
  virtual void reject();
  virtual void accept();

private:
  KChartPart* _chart;
  kchartWizardSelectDataPage* _selectdatapage;
  kchartWizardSelectChartTypePage* _selectcharttypepage;
  kchartWizardSelectChartSubTypePage* _selectchartsubtypepage;
  kchartWizardSetupDataPage* _setupdatapage;
  kchartWizardLabelsLegendPage* _labelslegendpage;
  kchartWizardSetupAxesPage* _axespage;
};

#endif


