
#include "kchartWizardSelectChartTypePage.h"
#include "kchart_view.h"
#include "kchart_factory.h"
#include "kchart_part.h"

#include <qbuttongroup.h>
#include <qframe.h>
#include <qpushbutton.h>
#include <qlabel.h>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>

namespace KChart
{

KChartButton::KChartButton(QWidget *parent, const QString & _text, const QPixmap &_pixmap)
  : QVBox(parent)
{
  m_button = new QPushButton(this);
  m_button->setPixmap(_pixmap);
  m_button->setToggleButton( true );
  QLabel *label = new QLabel(_text, this);
  label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

KChartButton::~KChartButton()
{
}

void KChartWizardSelectChartTypePage::addButton(const QString &name,
                                                const QString &icon_name,
                                                int type)
{
  KChartButton *button = new KChartButton( this, name, BarIcon( icon_name,KIcon::SizeMedium, KIcon::DefaultState,KChartFactory::global() )  );
  _layout->addWidget(button,r_pos,c_pos);
  _typeBG->insert( button->button(), type );

  if (c_pos == 3)
  {
    c_pos=0;
    r_pos++; //place the next button in the second row
  }
  else
    c_pos++;
}


KChartWizardSelectChartTypePage::KChartWizardSelectChartTypePage( QWidget* parent,
                                                                  KChartPart* chart ) :
    QWidget( parent ),
_chart( chart )
{
    _typeBG = new QButtonGroup( this );
    _typeBG->setExclusive( true );
    _typeBG->hide();
    //  _typeBG->resize

    c_pos=0;
    r_pos=0;
    _layout = new QGridLayout(this, 2, 4, 5);
    _layout->setRowStretch(0,0);
    _layout->setRowStretch(1,0);
    addButton( i18n( "Bar" ), "chart_bar", KDChartParams::Bar );
    addButton( i18n( "Lines" ), "chart_line", KDChartParams::Line );
    addButton( i18n( "Area" ),  "chart_area", KDChartParams::Area );
    addButton(i18n("Hi-Lo-Close"), "chart_hilo", KDChartParams::HiLo );
    addButton(i18n("Box & Whisker "), "chart_boxwhisker", KDChartParams::BoxWhisker );
    addButton( i18n( "Pie" ), "chart_pie", KDChartParams::Pie );
    addButton( i18n( "Ring" ), "chart_ring", KDChartParams::Ring );
    addButton(i18n("Polar"), "chart_polar", KDChartParams::Polar);

    QPushButton *current = ((QPushButton*)_typeBG->find( _chart->params()->chartType() ));
    if (current != NULL) {
        current->setOn( true );
    }
    _type=_chart->params()->chartType();
    connect( _typeBG, SIGNAL( clicked( int ) ),
             this, SLOT( chartTypeSelected( int ) ) );

    ////  parent->resize( 425, 256 );
//     parent->resize(xstep*5+50, ystep*4 + 100);
}


void KChartWizardSelectChartTypePage::chartTypeSelected( int type )
{
    _type=(KDChartParams::ChartType)type;
    emit chartChange(type);
}

void KChartWizardSelectChartTypePage::apply()
{
    _chart->params()->setChartType( _type );
}

}  //namespace KChart

#include "kchartWizardSelectChartTypePage.moc"
