
#include "kchartWizardSelectChartSubTypePage.h"
#include "kchart_view.h"
#include "kchart_part.h"

#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qlabel.h>
#include <qptrcollection.h>
#include <qobjectlist.h>
#include <qlayout.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>

#include "kchart_params.h"

namespace KChart
{

KChartWizardSelectChartSubTypePage::KChartWizardSelectChartSubTypePage( QWidget* parent,
                                                                        KChartPart* chart ) :
  QWidget( parent ),
  _chart( chart )
{
    _charttype = _chart->params()->chartType();
    chartSubType=true;

    QGridLayout *grid1 = new QGridLayout(this,2,2,KDialog::marginHint(), KDialog::spacingHint());

    QVBoxLayout *lay1 = new QVBoxLayout(  );
    lay1->setMargin( KDialog::marginHint() );
    lay1->setSpacing( KDialog::spacingHint() );

    QButtonGroup *grp = new QButtonGroup( 1, QGroupBox::Horizontal, i18n( "Chart Sub Type" ),this );
    grp->setRadioButtonExclusive( TRUE );
    grp->layout();
    lay1->addWidget(grp);
    normal = new QRadioButton( i18n( "Normal" ), grp );
    stacked = new QRadioButton( i18n( "Stacked" ), grp );
    percent = new QRadioButton( i18n( "Percent" ), grp );

    if( ( _chart->params()->chartType() == KDChartParams::Bar &&
          _chart->params()->barChartSubType() == KDChartParams::BarNormal ) ||
        ( _chart->params()->chartType() == KDChartParams::Line &&
          _chart->params()->lineChartSubType() == KDChartParams::LineNormal ) ||
        ( _chart->params()->chartType() == KDChartParams::Polar &&
          _chart->params()->polarChartSubType() == KDChartParams::PolarNormal ) ||
        ( _chart->params()->chartType() == KDChartParams::Area &&
          _chart->params()->areaChartSubType() == KDChartParams::AreaNormal ) )
        normal->setChecked( true );
    else if( ( _chart->params()->chartType() == KDChartParams::Bar &&
          _chart->params()->barChartSubType() == KDChartParams::BarStacked ) ||
        ( _chart->params()->chartType() == KDChartParams::Line &&
          _chart->params()->lineChartSubType() == KDChartParams::LineStacked ) ||
             ( _chart->params()->chartType() == KDChartParams::Polar &&
               _chart->params()->polarChartSubType() == KDChartParams::PolarStacked ) ||
        ( _chart->params()->chartType() == KDChartParams::Area &&
          _chart->params()->areaChartSubType() == KDChartParams::AreaStacked ) )
        stacked->setChecked( true );
    else if( ( _chart->params()->chartType() == KDChartParams::Bar &&
          _chart->params()->barChartSubType() == KDChartParams::BarPercent ) ||
        ( _chart->params()->chartType() == KDChartParams::Line &&
          _chart->params()->lineChartSubType() == KDChartParams::LinePercent ) ||
             ( _chart->params()->chartType() == KDChartParams::Polar &&
               _chart->params()->polarChartSubType() == KDChartParams::PolarPercent ) ||
        ( _chart->params()->chartType() == KDChartParams::Area &&
          _chart->params()->areaChartSubType() == KDChartParams::AreaPercent ) )
        percent->setChecked( true );
    else {
        kdDebug(35001)<<"Error in stack_type\n";
    }

    if( _chart->params()->chartType() == KDChartParams::HiLo)
    {
        if( _chart->params()->hiLoChartSubType()==KDChartParams::HiLoNormal)
            normal->setChecked( true );
        else if(_chart->params()->hiLoChartSubType()==KDChartParams::HiLoClose)
            stacked->setChecked(true);
        else if(_chart->params()->hiLoChartSubType()==KDChartParams::HiLoOpenClose)
            percent->setChecked(true);
    }

    changeSubTypeName( _chart->params()->chartType());
    if(!chartSubType)
        grp->setEnabled(false);

    grid1->addWidget(grp,0,0);
}



void KChartWizardSelectChartSubTypePage::apply()
{
    if(chartSubType) {
        if( normal->isChecked() )
            switch( _chart->params()->chartType() ) {
            case KDChartParams::Bar:
                _chart->params()->setBarChartSubType( KDChartParams::BarNormal );
                break;
            case KDChartParams::Line:
                _chart->params()->setLineChartSubType( KDChartParams::LineNormal );
                break;
            case KDChartParams::Area:
                _chart->params()->setAreaChartSubType( KDChartParams::AreaNormal );
                break;
            case KDChartParams::HiLo:
                _chart->params()->setHiLoChartSubType(KDChartParams::HiLoNormal);
                break;
            case KDChartParams::Polar:
                _chart->params()->setPolarChartSubType( KDChartParams::PolarNormal );
            default:
                kdDebug( 35001 ) << "Error in group button\n";
            }
        else if( stacked->isChecked() )
            switch( _chart->params()->chartType() ) {
            case KDChartParams::Bar:
                _chart->params()->setBarChartSubType( KDChartParams::BarStacked );
                break;
            case KDChartParams::Line:
                _chart->params()->setLineChartSubType( KDChartParams::LineStacked );
                break;
            case KDChartParams::Area:
                _chart->params()->setAreaChartSubType( KDChartParams::AreaStacked );
                break;
            case KDChartParams::HiLo:
                _chart->params()->setHiLoChartSubType( KDChartParams::HiLoClose);
                break;
            case KDChartParams::Polar:
                _chart->params()->setPolarChartSubType( KDChartParams::PolarStacked );
                break;
            default:
                kdDebug( 35001 ) << "Error in group button\n";
            }
        else if( percent->isChecked() )
            switch( _chart->params()->chartType() ) {
            case KDChartParams::Bar:
                _chart->params()->setBarChartSubType( KDChartParams::BarPercent );
                break;
            case KDChartParams::Line:
                _chart->params()->setLineChartSubType( KDChartParams::LinePercent );
                break;
            case KDChartParams::Area:
                _chart->params()->setAreaChartSubType( KDChartParams::AreaPercent );
                break;
            case KDChartParams::HiLo:
                _chart->params()->setHiLoChartSubType( KDChartParams::HiLoOpenClose);
                break;
            case KDChartParams::Polar:
                _chart->params()->setPolarChartSubType( KDChartParams::PolarPercent );
                break;
            default:
                kdDebug( 35001 ) << "Error in group button\n";
            }
        else
            kdDebug(35001)<<"Error in groupbutton\n";
    }
}

void KChartWizardSelectChartSubTypePage::changeSubTypeName( KDChartParams::ChartType _type)
{
    if(_type==KDChartParams::HiLo)
    {
        stacked->setText(i18n("HiLoClose"));
        percent->setText(i18n("HiLoOpenClose"));
    }
    else
    {
        stacked->setText( i18n( "Stacked" ));
        percent->setText( i18n( "Percent" ));
    }
}

}  //KChart namespace

#include "kchartWizardSelectChartSubTypePage.moc"
