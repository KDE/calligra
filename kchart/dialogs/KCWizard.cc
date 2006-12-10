
#include "KCWizard.h"
#include "KCWizardDataPage.h"
#include "KCWizardCharttype.h"
#include "KCWizardSubtypePage.h"
#include "KCWizardLabelsLegendPage.h"
#include "KCWizardAxesPage.h"
//#include "kchartWizardSetupDataPage.h"

#include <QLineEdit>
#include <QWidget>
#include <QPushButton>

#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "kchart_params.h"

namespace KChart
{

KCWizard::KCWizard ( KChartPart* _chart, QWidget *parent, const char* name,
                             bool modal, Qt::WFlags f ) :
    K3Wizard( parent, name, modal, f ),
    m_chart( _chart )
{
    // First page: select the data range
    m_dataPage = new KCWizardDataPage(this, m_chart);
    addPage( m_dataPage, i18n("Data"));
    setFinishEnabled(m_dataPage, true);
    setHelpEnabled(m_dataPage, false);
    
    // Second page: select the major chart type
    m_chartTypePage = new KCWizardCharttype( this, m_chart );
    addPage( m_chartTypePage, i18n( "Select Chart Type" ) );
    //finishButton()->setEnabled( true );
    setFinishEnabled(m_chartTypePage, true);
    setHelpEnabled(m_chartTypePage, false);

    // Third page: select the minor chart type
    m_chartSubtypePage = new KCWizardSubtypePage( this, m_chart );
    addPage( m_chartSubtypePage, i18n( "Select Chart Sub-type" ) );
    setFinishEnabled(m_chartSubtypePage, true);
    setHelpEnabled(m_chartSubtypePage, false);

    // Fourth page: labels/legends setup
    m_labelsLegendPage = new KCWizardLabelsLegendPage( this, m_chart );
    addPage( m_labelsLegendPage, i18n( "Labels & Legend" ) );
    setFinishEnabled(m_labelsLegendPage, true);
    setHelpEnabled(m_labelsLegendPage, false);

    // Fifth page: axes setup
    m_axespage = new KCWizardAxesPage( this, m_chart );
    addPage( m_axespage, i18n( "Setup Axes" ) );
    setFinishEnabled(m_axespage, true);
    setNextEnabled(m_axespage, false);
    setHelpEnabled(m_axespage, false);

    // connect( this, SIGNAL( finished() ), _selectdatapage, SLOT( apply() ) );
    connect(this, SIGNAL(finished()), m_dataPage,         SLOT(apply()));
    connect(this, SIGNAL(finished()), m_chartTypePage,    SLOT(apply()));
    connect(this ,SIGNAL(finished()), m_chartSubtypePage, SLOT(apply()));
    connect(this, SIGNAL(finished()), m_labelsLegendPage, SLOT(apply()));
    connect(this, SIGNAL(finished()), m_axespage,         SLOT(apply()));

    connect( m_chartTypePage, SIGNAL( chartChange( int ) ),
             this,            SLOT( subType( int ) ) );
    adjustSize();

    subType( m_chart->params()->chartType() );
    kDebug(35001) << "kchartwizard created" << endl;
}


KCWizard::~KCWizard()
{
    //  delete _selectdatapage;
    delete m_chartTypePage;
    delete m_chartSubtypePage;
    //delete _setupdatapage;
    delete m_labelsLegendPage;
    delete m_axespage;
    delete m_dataPage;
}

void KCWizard::subType(int _type)
{
    KChartParams::ChartType  type = (KChartParams::ChartType) _type;
    if (type == KChartParams::Bar ||
        type == KChartParams::Line ||
        type == KChartParams::Area ||
        type == KChartParams::HiLo ||
        type == KChartParams::Polar) {
        m_chartSubtypePage->chartSubType = true;
    } else {
        m_chartSubtypePage->chartSubType = false;
    }
    m_chartSubtypePage->changeSubTypeName( type );
    if( ( type == KChartParams::Bar && m_chart->params()->threeDBars() ) 
	|| ( type == KChartParams::Pie && m_chart->params()->threeDPies() ) ) {
	m_axespage->chart3d = true;
    } else {
        m_axespage->chart3d = false;
    }

#if 0				// No second Y axis so far /ingwa
    if ( m_chart->params()->axisVisible( KDChartAxisParams::AxisPosRight ) ) {
        m_labelsLegendPage->ytitle2 = true;
    } else {
        m_labelsLegendPage->ytitle2 = false;
    }
#endif
}


bool KCWizard::appropriate( QWidget * w ) const
{
    // Show the sub-type page only if has anything to show
    if ( w == m_chartSubtypePage )
        return m_chartSubtypePage->chartSubType;
    else
        return true;
}


void KCWizard::next()
{
    // Some sort of a hack. We want the chart-subtype-page to get
    // dynamically built when it's going to be shown
    //if ( currentPage() == _charttypePage )
    //_chartSubtypePage->createChildren();

    Q3Wizard::next();
}

void KCWizard::accept()
{
    emit finished();
    Q3Wizard::accept();
}

void KCWizard::reject()
{
    emit cancelled();
    Q3Wizard::reject();
}

void KCWizard::setDataArea( const QString &area )
{
    m_dataPage->setDataArea( area );
}


QString KCWizard::dataArea() const
{
    return m_dataPage->dataArea();
}


void KCWizard::emitNeedNewData( const char* area, int rowcol,
				    bool firstRowIsLegend,
				    bool firstColIsLabel )
{
    emit needNewData( area, rowcol, firstRowIsLegend, firstColIsLabel );
}

}  //KChart namespace

#include "KCWizard.moc"
