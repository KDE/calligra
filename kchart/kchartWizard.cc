
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
    // hack

    kdDebug(35001) << "Creating KChartWizard" << endl;

    // First page: select the range
    m_selectdataformatpage = new KChartWizardSelectDataFormatPage(this, m_chart);
    addPage( m_selectdataformatpage, i18n("Select Data Format"));
    setFinishEnabled(m_selectdataformatpage, true);
    setHelpEnabled(m_selectdataformatpage, false);
    
    // Second page: select the major chart type
    m_selectcharttypepage = new KChartWizardSelectChartTypePage( this, m_chart );
    addPage( m_selectcharttypepage, i18n( "Select Chart Type" ) );
    //finishButton()->setEnabled( TRUE );
    setFinishEnabled(m_selectcharttypepage, true);
    setHelpEnabled(m_selectcharttypepage, false);

    // Third page: select the minor chart type
    m_selectchartsubtypepage = new KChartWizardSelectChartSubTypePage( this, m_chart );
    addPage( m_selectchartsubtypepage, i18n( "Select Chart Sub-type" ) );
    setFinishEnabled(m_selectchartsubtypepage, true);
    setHelpEnabled(m_selectchartsubtypepage, false);

    // Fourth page: data setup
    //_setupdatapage = new KChartWizardSetupDataPage( this, m_chart );
    //addPage( _setupdatapage, i18n( "Data Setup" ) );

    // Fifth page: labels/legends setup
    m_labelslegendpage = new KChartWizardLabelsLegendPage( this, m_chart );
    addPage( m_labelslegendpage, i18n( "Labels & Legend" ) );
    setFinishEnabled(m_labelslegendpage, true);
    setHelpEnabled(m_labelslegendpage, false);

    // Sixth page: axes setup
    m_axespage = new KChartWizardSetupAxesPage( this, m_chart );
    addPage( m_axespage, i18n( "Setup Axes" ) );
    setFinishEnabled(m_axespage, true);
    setNextEnabled(m_axespage, false);
    setHelpEnabled(m_axespage, false);

    // connect( this, SIGNAL( finished() ), _selectdatapage, SLOT( apply() ) );
    connect(this, SIGNAL(finished()), m_labelslegendpage,       SLOT(apply()));
    connect(this, SIGNAL(finished()), m_selectcharttypepage,    SLOT(apply()));
    connect(this, SIGNAL(finished()), m_axespage,               SLOT(apply()));
    connect(this ,SIGNAL(finished()), m_selectchartsubtypepage, SLOT(apply()));
    connect(this ,SIGNAL(finished()), m_selectdataformatpage, SLOT(apply()));
    connect( m_selectcharttypepage, SIGNAL( chartChange( int ) ),
             this,                  SLOT( subType( int ) ) );
    adjustSize();

    subType( m_chart->params()->chartType() );
    kdDebug(35001) << "kchartwizard created" << endl;
}


KChartWizard::~KChartWizard()
{
    //  delete _selectdatapage;
    delete m_selectcharttypepage;
    delete m_selectchartsubtypepage;
    //delete _setupdatapage;
    delete m_labelslegendpage;
    delete m_axespage;
    delete m_selectdataformatpage;
}

void KChartWizard::subType(int _type)
{
    KDChartParams::ChartType type = (KDChartParams::ChartType)_type;
    if (type == KDChartParams::Bar ||
        type == KDChartParams::Line ||
        type == KDChartParams::Area ||
        type == KDChartParams::HiLo ||
        type == KDChartParams::Polar) {
        m_selectchartsubtypepage->chartSubType=true;
    } else {
        m_selectchartsubtypepage->chartSubType=false;
    }
    m_selectchartsubtypepage->changeSubTypeName( type );
    if( ( type == KDChartParams::Bar && m_chart->params()->threeDBars() ) 
	|| ( type == KDChartParams::Pie && m_chart->params()->threeDPies() ) ) {
	m_axespage->chart3d = true;
    } else {
        m_axespage->chart3d = false;
    }

#if 0				// No second Y axis so far /ingwa
    if ( m_chart->params()->axisVisible( KDChartAxisParams::AxisPosRight ) ) {
        m_labelslegendpage->ytitle2=true;
    } else {
        m_labelslegendpage->ytitle2=false;
    }
#endif
}


bool KChartWizard::appropriate( QWidget * w ) const
{
    // Show the sub-type page only if has anything to show
    if ( w == m_selectchartsubtypepage )
        return m_selectchartsubtypepage->chartSubType;
    else
        return true;
}


void KChartWizard::next()
{
    // Some sort of a hack. We want the chart-subtype-page to get
    // dynamically built when it's going to be shown
    //if ( currentPage() == _selectcharttypepage )
    //_selectchartsubtypepage->createChildren();

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

void KChartWizard::setDataArea( const QString &/*area*/ )
{
    //  _selectdatapage->rangeED->setText( area );
}


QString KChartWizard::dataArea() const
{
    return QString("");
    //  return _selectdatapage->rangeED->text();
}


void KChartWizard::emitNeedNewData( const char* area, int rowcol,
				    bool firstRowIsLegend,
				    bool firstColIsLabel )
{
    emit needNewData( area, rowcol, firstRowIsLegend, firstColIsLabel );
}

}  //KChart namespace

#include "kchartWizard.moc"
