
#include "KCWizardSubtypePage.h"
#include "kchart_view.h"
#include "kchart_part.h"

#include <QObject>
#include <QLayout>
#include <QButtonGroup>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QLabel>
#include <q3ptrcollection.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>
#include <kvbox.h>

#include "kchart_params.h"


namespace KChart
{

KCWizardSubtypePage::KCWizardSubtypePage( QWidget* parent, KChartPart* chart )
  : QWidget( parent ),
    m_chart( chart )
{
    // FIXME: Use KChartPart::chartType() instead, when implemented.
    m_charttype  = (KChartParams::ChartType) m_chart->params()->chartType();
    chartSubType = true;

    QVBoxLayout *lay1 = new QVBoxLayout( this );
    lay1->setMargin( KDialog::marginHint() );
    lay1->setSpacing( KDialog::spacingHint() );

    QGroupBox *box = new QGroupBox( i18n( "Chart Sub Type" ) );
    lay1->addWidget( box );

    QVBoxLayout  *lay2 = new QVBoxLayout( );
    box->setLayout( lay2 );

    QButtonGroup *grp = new QButtonGroup( );
    grp->setExclusive( true );

    m_normal  = new QRadioButton( i18n( "Normal" ),  box );
    lay2->addWidget( m_normal );
    grp->addButton( m_normal );
    m_stacked = new QRadioButton( i18n( "Stacked" ), box );
    lay2->addWidget( m_stacked );
    grp->addButton( m_stacked );
    m_percent = new QRadioButton( i18n( "Percent" ), box );
    lay2->addWidget( m_percent );
    grp->addButton( m_percent );

    // The Number of lines.
    KHBox   *hbox = new KHBox( this );
    (void) new QLabel( i18n( "Number of lines: "), hbox );
    m_numLines    = new QSpinBox( hbox );

    if( ( m_chart->chartType() == KChartParams::Bar &&
          m_chart->params()->barChartSubType() == KDChartParams::BarNormal ) 
	|| ( m_chart->chartType() == KChartParams::Line &&
	     m_chart->params()->lineChartSubType() == KDChartParams::LineNormal )
	|| ( m_chart->chartType() == KChartParams::Polar &&
	     m_chart->params()->polarChartSubType() == KDChartParams::PolarNormal )
	|| ( m_chart->chartType() == KChartParams::Area &&
	     m_chart->params()->areaChartSubType() == KDChartParams::AreaNormal ) )
        m_normal->setChecked( true );
    else if( ( m_chart->params()->chartType() == KChartParams::Bar &&
          m_chart->params()->barChartSubType() == KDChartParams::BarStacked ) ||
        ( m_chart->params()->chartType() == KChartParams::Line &&
          m_chart->params()->lineChartSubType() == KDChartParams::LineStacked ) ||
             ( m_chart->params()->chartType() == KChartParams::Polar &&
               m_chart->params()->polarChartSubType() == KDChartParams::PolarStacked ) ||
        ( m_chart->params()->chartType() == KChartParams::Area &&
          m_chart->params()->areaChartSubType() == KDChartParams::AreaStacked ) )
        m_stacked->setChecked( true );
    else if( ( m_chart->params()->chartType() == KChartParams::Bar &&
          m_chart->params()->barChartSubType() == KDChartParams::BarPercent ) ||
        ( m_chart->params()->chartType() == KChartParams::Line &&
          m_chart->params()->lineChartSubType() == KDChartParams::LinePercent ) ||
             ( m_chart->params()->chartType() == KChartParams::Polar &&
               m_chart->params()->polarChartSubType() == KDChartParams::PolarPercent ) ||
        ( m_chart->params()->chartType() == KChartParams::Area &&
          m_chart->params()->areaChartSubType() == KDChartParams::AreaPercent ) )
        m_percent->setChecked( true );
    else {
        kDebug(35001)<<"Error in chart_type\n";
    }

    m_numLines->setValue( m_chart->params()->barNumLines() );

    if( m_chart->params()->chartType() == KChartParams::HiLo)
    {
        if( m_chart->params()->hiLoChartSubType()==KDChartParams::HiLoNormal)
            m_normal->setChecked( true );
        else if(m_chart->params()->hiLoChartSubType()==KDChartParams::HiLoClose)
            m_stacked->setChecked(true);
        else if(m_chart->params()->hiLoChartSubType()==KDChartParams::HiLoOpenClose)
            m_percent->setChecked(true);
    }

    changeSubTypeName( m_chart->chartType());
    if(!chartSubType)
        box->setEnabled(false);

    lay1->addWidget( hbox );
}



void KCWizardSubtypePage::apply()
{
    if (chartSubType) {
        if( m_normal->isChecked() )
            switch( m_chart->params()->chartType() ) {
            case KChartParams::Bar:
                m_chart->params()->setBarChartSubType( KDChartParams::BarNormal );
		// FIXME: Error controls.
		m_chart->params()->setBarNumLines( m_numLines->value() );
                break;
            case KChartParams::Line:
                m_chart->params()->setLineChartSubType( KDChartParams::LineNormal );
                break;
            case KChartParams::Area:
                m_chart->params()->setAreaChartSubType( KDChartParams::AreaNormal );
                break;
            case KChartParams::HiLo:
                m_chart->params()->setHiLoChartSubType(KDChartParams::HiLoNormal);
                break;
            case KChartParams::Polar:
                m_chart->params()->setPolarChartSubType( KDChartParams::PolarNormal );
            default:
                kDebug( 35001 ) << "Error in group button\n";
            }
        else if ( m_stacked->isChecked() )
            switch( m_chart->params()->chartType() ) {
            case KChartParams::Bar:
                m_chart->params()->setBarChartSubType( KDChartParams::BarStacked );
                break;
            case KChartParams::Line:
                m_chart->params()->setLineChartSubType( KDChartParams::LineStacked );
                break;
            case KChartParams::Area:
                m_chart->params()->setAreaChartSubType( KDChartParams::AreaStacked );
                break;
            case KChartParams::HiLo:
                m_chart->params()->setHiLoChartSubType( KDChartParams::HiLoClose);
                break;
            case KChartParams::Polar:
                m_chart->params()->setPolarChartSubType( KDChartParams::PolarStacked );
                break;
            default:
                kDebug( 35001 ) << "Error in group button\n";
            }
        else if( m_percent->isChecked() )
            switch( m_chart->params()->chartType() ) {
            case KChartParams::Bar:
                m_chart->params()->setBarChartSubType( KDChartParams::BarPercent );
                break;
            case KChartParams::Line:
                m_chart->params()->setLineChartSubType( KDChartParams::LinePercent );
                break;
            case KChartParams::Area:
                m_chart->params()->setAreaChartSubType( KDChartParams::AreaPercent );
                break;
            case KChartParams::HiLo:
                m_chart->params()->setHiLoChartSubType( KDChartParams::HiLoOpenClose);
                break;
            case KChartParams::Polar:
                m_chart->params()->setPolarChartSubType( KDChartParams::PolarPercent );
                break;
            default:
                kDebug( 35001 ) << "Error in group button\n";
            }
        else
            kDebug(35001)<<"Error in groupbutton\n";
    }
}

void KCWizardSubtypePage::changeSubTypeName( KChartParams::ChartType _type)
{
    if (_type==KChartParams::HiLo)
    {
	m_stacked->setText( i18n("HiLoClose") );
        m_percent->setText( i18n("HiLoOpenClose") );
    }
    else
    {
        m_stacked->setText( i18n( "Stacked" ) );
        m_percent->setText( i18n( "Percent" ) );
    }
}

}  //KChart namespace

#include "KCWizardSubtypePage.moc"
