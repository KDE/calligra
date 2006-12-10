#ifndef kchartWIZARD_H
#define kchartWIZARD_H

#include <klocale.h>
#include "k3wizard.h"
#include "kchart_part.h"

namespace KChart
{

class KChartPart;
class KCWizardDataPage;
class KCWizardCharttype;	/* FIXME: Add ...Page suffix. */
class KCWizardSubtypePage;
class KCWizardLabelsLegendPage;
class KCWizardAxesPage;
class KChartWizardSetupDataPage;

class KCWizard : public K3Wizard
{
    Q_OBJECT
public:
    KCWizard ( KChartPart* chart, QWidget *parent, const char* name,
		   bool modal = true, Qt::WFlags f = 0 );
    ~KCWizard();

    KChartPart* chart() const { return m_chart; };

    // Set and get the string for the data area.
    void setDataArea( const QString &area );
    QString dataArea() const;

    enum RowCol { Row, Col };

    void emitNeedNewData( const char* area, int rowcol, 
			  bool firstRowIsLegend, bool firstColIsLabel );

    virtual bool appropriate( QWidget * w ) const;

signals:
    // valid values for rowcol: Row: data is in rows, Col: data is in cols
    void needNewData( const char* area, int rowcol, 
		      bool firstRowIsLegend, bool firstColIsLabel );
    void finished();
    void cancelled();

protected slots:
    virtual void next();
    virtual void reject();
    virtual void accept();
    virtual void subType(int);

private:
    // The chart itself.
    KChartPart                *m_chart;

    KCWizardDataPage          *m_dataPage;
    KCWizardCharttype         *m_chartTypePage;
    KCWizardSubtypePage       *m_chartSubtypePage;
    KCWizardLabelsLegendPage  *m_labelsLegendPage;
    KCWizardAxesPage          *m_axespage;
};

}  //KChart namespace

#endif


