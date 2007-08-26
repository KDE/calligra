
#include "KCWizardDataPage.h"
#include "kchart_view.h"
#include "kchart_part.h"


#include <QCheckBox>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <q3listbox.h>
#include <q3buttongroup.h>
#include <QPushButton>
#include <qradiobutton.h>
#include <QLineEdit>
#include <qpainter.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kglobal.h>
#include <klocale.h>
#include <kdialog.h>
#include <kvbox.h>

#include "kchart_params.h"

namespace KChart
{

KCWizardDataPage::KCWizardDataPage( QWidget* parent, KChartPart* chart )
  : QWidget( parent ),
    m_chart( chart )
{
    QVBoxLayout  * layout2;

    QVBoxLayout  *layout = new QVBoxLayout( );
    layout->setMargin( KDialog::marginHint() );
    layout->setSpacing( KDialog::spacingHint() );
    this->setLayout( layout );

    // The Data Area
    QGroupBox *gb1 = new QGroupBox( i18n( "Data Area" ) );
    layout->addWidget( gb1 );

    layout2 = new QVBoxLayout( );
    layout2->setMargin( KDialog::marginHint() );
    layout2->setSpacing( KDialog::spacingHint() );
    gb1->setLayout( layout2 );

    KHBox   *hbox = new KHBox( gb1 );
    (void) new QLabel( i18n("Area: "), hbox);
    m_dataArea = new QLineEdit( hbox );
    layout2->addWidget( hbox );

    // The row/column as label checkboxes. 
    m_firstRowAsLabel = new QCheckBox( i18n( "First row as label" ) );
    layout2->addWidget( m_firstRowAsLabel );
    m_firstColAsLabel = new QCheckBox( i18n( "First column as label" ) );
    layout2->addWidget( m_firstColAsLabel );

    layout2->addStretch( 1 );

    // The Data Format groupbox
    QGroupBox *gb2 = new QGroupBox( i18n( "Data Format" ) );
    layout->addWidget( gb2 );

    layout2 = new QVBoxLayout( );
    layout2->setMargin( KDialog::marginHint() );
    layout2->setSpacing( KDialog::spacingHint() );
    gb2->setLayout( layout2 );

    QButtonGroup *bg = new QButtonGroup( );
    bg->setExclusive( true );

    m_rowMajor = new QRadioButton( i18n( "Data in rows" ), gb2 );
    layout2->addWidget( m_rowMajor );
    bg->addButton( m_rowMajor );

    m_colMajor = new QRadioButton( i18n( "Data in columns" ), gb2 );
    m_colMajor->resize( m_colMajor->sizeHint() );
    layout2->addWidget( m_colMajor );
    bg->addButton( m_colMajor );

    this->setWhatsThis( i18n("This configuration page can be used to swap the interpretation of rows and columns."));
    m_rowMajor->setWhatsThis( i18n("By default one row is considered to be a data set and each column holds the individual values of the data series. This sets the data in rows on your chart."));

    m_colMajor->setWhatsThis( i18n("Here you can choose to have each column hold one data set. Note that the values are not really swapped but only their interpretation."));
    m_colMajor->resize( m_colMajor->sizeHint() );

    QLabel *lbl = new QLabel( i18n( 
        "\n"
	"If the selected data area does not match the data you want,\n"
	"select the data now.\n"
	"\n"
	"Include cells that you want to use as row and column labels,\n"
	"if you want them in the chart.\n"
	), this);
    layout->addWidget( lbl);

    layout->addStretch( 1 );

    if ( m_chart->params()->dataDirection() == KChartParams::DataColumns)
      m_colMajor->setChecked(true);
    else
      m_rowMajor->setChecked(true);
}


QString KCWizardDataPage::dataArea() const
{
    return m_dataArea->text();
}

void KCWizardDataPage::setDataArea( const QString &area )
{
    m_dataArea->setText( area );
}


void KCWizardDataPage::apply()
{
    if (m_rowMajor->isChecked())
      m_chart->params()->setDataDirection( KChartParams::DataRows );
    else
      m_chart->params()->setDataDirection( KChartParams::DataColumns );

    m_chart->params()->setFirstRowAsLabel( m_firstRowAsLabel->isChecked() );
    m_chart->params()->setFirstColAsLabel( m_firstColAsLabel->isChecked() );
}


}  //KChart namespace

#include "KCWizardDataPage.moc"
