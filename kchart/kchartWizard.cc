
#include "kchartWizard.h"
#include "kchartWizardSelectDataPage.h"
#include "kchartWizardSelectChartTypePage.h"
#include "kchartWizardSelectChartSubTypePage.h"
#include "kchartWizardSetupDataPage.h"
#include "kchartWizardLabelsLegendPage.h"
#include "kchartWizardSetupAxesPage.h"

#include <qlineedit.h>
#include <qwidget.h>
#include <qpushbutton.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <kdebug.h>

#include "kchart_params.h"

namespace KChart
{

KChartWizard::KChartWizard ( KChartPart* chart, QWidget *parent, const char* name,
                             bool modal, WFlags f ) :
    KWizard( parent, name, modal, f ),
    _chart( chart )
{
    // hack

    kdDebug(35001) << "Creating KChartWizard" << endl;

    // First page: select the range
    //     _selectdatapage = new KChartWizardSelectDataPage( this );
    //     addPage( _selectdatapage, i18n( "Select Data" ) );

    // Second page: select the major chart type
    _selectcharttypepage =  new KChartWizardSelectChartTypePage( this, _chart );
    addPage( _selectcharttypepage, i18n( "Select Chart Type" ) );
    finishButton()->setEnabled( TRUE );
    setFinishEnabled(_selectcharttypepage, true);

    // Third page: select the minor chart type
    _selectchartsubtypepage = new KChartWizardSelectChartSubTypePage( this, _chart );
    addPage( _selectchartsubtypepage, i18n( "Select Chart Subtype" ) );

    // Fourth page: data setup
    //_setupdatapage = new KChartWizardSetupDataPage( this, _chart );
    //addPage( _setupdatapage, i18n( "Data Setup" ) );

    // Fifth page: labels/legends setup
    _labelslegendpage = new KChartWizardLabelsLegendPage( this, _chart );
    addPage( _labelslegendpage, i18n( "Labels & Legend" ) );

    // Sixth page: axes setup
    _axespage = new KChartWizardSetupAxesPage( this, _chart );
    addPage( _axespage, i18n( "Setup Axes" ) );
    setFinishEnabled(_axespage, true);
    setNextEnabled(_axespage, false);
    //  connect( this, SIGNAL( finished() ), _selectdatapage, SLOT( apply() ) );
    connect( this ,SIGNAL( finished() ), _labelslegendpage, SLOT( apply() ) );
    connect( this ,SIGNAL( finished() ), _selectcharttypepage, SLOT( apply() ) );
    connect( this ,SIGNAL( finished() ), _axespage,SLOT( apply() ) );
    connect( this ,SIGNAL( finished() ), _selectchartsubtypepage,SLOT( apply()));
    connect( _selectcharttypepage, SIGNAL( chartChange( int ) ),
             this, SLOT( subType( int ) ) );
    adjustSize();

    subType( _chart->params()->chartType() );
    kdDebug(35001) << "kchartwizard created" << endl;
}


KChartWizard::~KChartWizard()
{
    //  delete _selectdatapage;
    delete _selectcharttypepage;
    delete _selectchartsubtypepage;
    //delete _setupdatapage;
    delete _labelslegendpage;
    delete _axespage;
}

void KChartWizard::subType(int _type)
{
    KDChartParams::ChartType type = (KDChartParams::ChartType)_type;
    if( type == KDChartParams::Bar ||
        type == KDChartParams::Line ||
        type == KDChartParams::Area ||
        type == KDChartParams::HiLo ||
        type == KDChartParams::Polar) {
        _selectchartsubtypepage->chartSubType=true;
    } else {
        _selectchartsubtypepage->chartSubType=false;
    }
    _selectchartsubtypepage->changeSubTypeName( type );
    if( ( type == KDChartParams::Bar &&
          _chart->params()->threeDBars() ) ||
        ( type == KDChartParams::Pie &&
          _chart->params()->threeDPies() ) ) {
        _axespage->chart3d = true;
    } else {
        _axespage->chart3d = false;
    }

#if 0				// No second Y axis so far /ingwa
    if( _chart->params()->axisVisible( KDChartAxisParams::AxisPosRight ) ) {
        _labelslegendpage->ytitle2=true;
    } else {
        _labelslegendpage->ytitle2=false;
    }
#endif
}


bool KChartWizard::appropriate( QWidget * w ) const
{
    if ( w == _selectchartsubtypepage )
        // Show the sub-type page only if has anything to show
        return _selectchartsubtypepage->chartSubType;
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
