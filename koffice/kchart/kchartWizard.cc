
#include "kchartWizard.h"
#include "kchartWizardSelectDataPage.h"
#include "kchartWizardSelectChartTypePage.h"
#include "kchartWizardSelectChartSubTypePage.h"
#include "kchartWizardSetupDataPage.h"
#include "kchartWizardLabelsLegendPage.h"
#include "kchartWizardSetupAxesPage.h"
#include "kchartWizardSelectDataFormatPage.h"

#include <qlineedit.h>
#include <qwidget.h>
#include <qpushbutton.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "kchart_params.h"

namespace KChart
{

KChartWizard::KChartWizard ( KChartPart* _chart, QWidget *parent, const char* name,
                             bool modal, WFlags f ) :
    KWizard( parent, name, modal, f ),
    m_chart( _chart )
{
    // First page: select the data range
    m_dataFormatPage = new KChartWizardSelectDataFormatPage(this, m_chart);
    addPage( m_dataFormatPage, i18n("Data"));
    setFinishEnabled(m_dataFormatPage, true);
    setHelpEnabled(m_dataFormatPage, false);
    
    // Second page: select the major chart type
    m_chartTypePage = new KChartWizardSelectChartTypePage( this, m_chart );
    addPage( m_chartTypePage, i18n( "Select Chart Type" ) );
    //finishButton()->setEnabled( TRUE );
    setFinishEnabled(m_chartTypePage, true);
    setHelpEnabled(m_chartTypePage, false);

    // Third page: select the minor chart type
    m_chartSubtypePage = new KChartWizardSelectChartSubTypePage( this, m_chart );
    addPage( m_chartSubtypePage, i18n( "Select Chart Sub-type" ) );
    setFinishEnabled(m_chartSubtypePage, true);
    setHelpEnabled(m_chartSubtypePage, false);

    // Fourth page: labels/legends setup
    m_labelsLegendPage = new KChartWizardLabelsLegendPage( this, m_chart );
    addPage( m_labelsLegendPage, i18n( "Labels & Legend" ) );
    setFinishEnabled(m_labelsLegendPage, true);
    setHelpEnabled(m_labelsLegendPage, false);

    // Fifth page: axes setup
    m_axespage = new KChartWizardSetupAxesPage( this, m_chart );
    addPage( m_axespage, i18n( "Setup Axes" ) );
    setFinishEnabled(m_axespage, true);
    setNextEnabled(m_axespage, false);
    setHelpEnabled(m_axespage, false);

    // connect( this, SIGNAL( finished() ), _selectdatapage, SLOT( apply() ) );
    connect(this, SIGNAL(finished()), m_dataFormatPage,   SLOT(apply()));
    connect(this, SIGNAL(finished()), m_chartTypePage,    SLOT(apply()));
    connect(this ,SIGNAL(finished()), m_chartSubtypePage, SLOT(apply()));
    connect(this, SIGNAL(finished()), m_labelsLegendPage, SLOT(apply()));
    connect(this, SIGNAL(finished()), m_axespage,         SLOT(apply()));

    connect( m_chartTypePage, SIGNAL( chartChange( int ) ),
             this,            SLOT( subType( int ) ) );
    adjustSize();

    subType( m_chart->params()->chartType() );
    kdDebug(35001) << "kchartwizard created" << endl;
}


KChartWizard::~KChartWizard()
{
    //  delete _selectdatapage;
    delete m_chartTypePage;
    delete m_chartSubtypePage;
    //delete _setupdatapage;
    delete m_labelsLegendPage;
    delete m_axespage;
    delete m_dataFormatPage;
}

void KChartWizard::subType(int _type)
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


bool KChartWizard::appropriate( QWidget * w ) const
{
    // Show the sub-type page only if has anything to show
    if ( w == m_chartSubtypePage )
        return m_chartSubtypePage->chartSubType;
    else
        return true;
}


void KChartWizard::next()
{
    // Some sort of a hack. We want the chart-subtype-page to get
    // dynamically built when it's going to be shown
    //if ( currentPage() == _charttypePage )
    //_chartSubtypePage->createChildren();

    QWizard::next();
}

void KChartWizard::accept()
{
    emit finished();
    QWizard::accept();
}

void KChartWizard::reject()
{
    emit cancelled();
    QWizard::reject();
}

void KChartWizard::setDataArea( const QString &area )
{
    m_dataFormatPage->setDataArea( area );
}


QString KChartWizard::dataArea() const
{
    return m_dataFormatPage->dataArea();
}


void KChartWizard::emitNeedNewData( const char* area, int rowcol,
				    bool firstRowIsLegend,
				    bool firstColIsLabel )
{
    emit needNewData( area, rowcol, firstRowIsLegend, firstColIsLabel );
}

}  //KChart namespace

#include "kchartWizard.moc"
