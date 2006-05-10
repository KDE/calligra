
#include "kchartWizardSelectChartTypePage.h"
#include "kchart_params.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchart_part.h"

#include <q3buttongroup.h>
#include <q3frame.h>
#include <QPushButton>
#include <QLabel>
//Added by qt3to4:
#include <QPixmap>
#include <Q3GridLayout>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kvbox.h>

namespace KChart
{


KChartButton::KChartButton(QWidget *parent, const QString & _text, const QPixmap &_pixmap)
  : KVBox(parent)
{
  // The button
  m_button = new QPushButton(this);
  m_button->setPixmap(_pixmap);
  m_button->setToggleButton( true );

  // The text
  QLabel *label = new QLabel(_text, this);
  label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

KChartButton::~KChartButton()
{
}


// ================================================================


KChartWizardSelectChartTypePage::KChartWizardSelectChartTypePage( QWidget* parent,
                                                                  KChartPart* chart )
  : QWidget( parent ),
    m_chart( chart )
{
    m_typeBG = new Q3ButtonGroup( this );
    m_typeBG->setExclusive( true );
    m_typeBG->hide();
    //  m_typeBG->resize

    m_colPos=0;
    m_rowPos=0;
    m_layout = new Q3GridLayout(this, 3, 4, 5);
    m_layout->setRowStretch(0, 0);
    m_layout->setRowStretch(1, 0);
    m_layout->setRowStretch(2, 0);

    addButton( i18n( "Bar" ),            "chart_bar",    KChartParams::Bar );
    addButton( i18n( "Lines" ),          "chart_line",   KChartParams::Line );
    addButton( i18n( "Area" ),           "chart_area",   KChartParams::Area );
#if 0
    addButton( i18n( "Bars and Lines" ), "chart_barlines", KChartParams::BarLines );
#endif

    addButton( i18n("Hi-Lo-Close"),      "chart_hilo",   KChartParams::HiLo );
    addButton( i18n("Box & Whisker "),   "chart_boxwhisker", KChartParams::BoxWhisker );
    incPos();
    incPos();

    addButton( i18n( "Pie" ),            "chart_pie",    KChartParams::Pie );
    addButton( i18n( "Ring" ),           "chart_ring",   KChartParams::Ring );
    addButton( i18n( "Polar" ),          "chart_polar",  KChartParams::Polar);

    QPushButton *current = ((QPushButton*)m_typeBG->find( m_chart->params()->chartType() ));
    if (current != NULL) {
	current->setOn( true );
    }
  
    m_type = m_chart->params()->chartType();
    connect( m_typeBG, SIGNAL( clicked( int ) ),
	     this, SLOT( chartTypeSelected( int ) ) );

    ////  parent->resize( 425, 256 );
    //     parent->resize(xstep*5+50, ystep*4 + 100);
}


void KChartWizardSelectChartTypePage::addButton(const QString &name,
                                                const QString &icon_name,
                                                int type)
{
    KChartButton *button = new KChartButton( this, name, 
					     BarIcon( icon_name, 
						      K3Icon::SizeMedium,
						      K3Icon::DefaultState,
						      KChartFactory::global()) );
    m_layout->addWidget(button, m_rowPos, m_colPos);
    m_typeBG->insert( button->button(), type );

    incPos();
}

void KChartWizardSelectChartTypePage::incPos()
{
    if (m_colPos == 3) {
	m_colPos=0;
	m_rowPos++; //place the next button in the second row
    }
    else
	m_colPos++;
}

void KChartWizardSelectChartTypePage::chartTypeSelected( int type )
{
    m_type = (KChartParams::ChartType) type;
    emit chartChange(type);
}

void KChartWizardSelectChartTypePage::apply()
{
    m_chart->params()->setChartType( m_type );
}

}  //namespace KChart

#include "kchartWizardSelectChartTypePage.moc"
