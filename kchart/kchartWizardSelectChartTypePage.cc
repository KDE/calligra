
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

void KChartWizardSelectChartTypePage::addButton(const QString &name,
                                                const QString &icon_name,
                                                int type)
{
    QFrame* buttonframe = new QFrame( this );
    buttonframe->setGeometry( 10+pos1*xstep,
                              10+pos2*ystep,
                              xsize,
                              ysize );
    buttonframe->setLineWidth( 2 );
    QPushButton *pushbutton = new QPushButton( buttonframe );
    pushbutton->setToggleButton( true );
    pushbutton->resize( buttonframe->contentsRect().width(),
                        buttonframe->contentsRect().height() );
    _typeBG->insert( pushbutton, type );
    pushbutton->setPixmap( BarIcon( icon_name,KIcon::SizeMedium, KIcon::DefaultState,KChartFactory::global() ) );
    QLabel* label = new QLabel(name, this );
    label->setGeometry( 10+pos1*xstep,
                        10+ysize+pos2*ystep,
                        xsize,
                        ylabelsize );
    label->setAlignment( AlignCenter );
    // next position
    if (pos1 == 4) {
        pos1=0;
        pos2++;
    } else pos1++;
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

    pos1=0;
    pos2=0;
    xsize = 94;
    ysize = 94;
    ylabelsize = 20;
    xstep = xsize + 10;
    ystep = ysize + ylabelsize + 10;

    addButton( i18n( "Lines" ), "chart_lines", KDChartParams::Line );
    addButton( i18n( "Area" ),  "chart_area", KDChartParams::Area );
    addButton( i18n( "Bar" ), "chart_bar", KDChartParams::Bar );
    addButton( i18n( "Pie" ), "chart_pie3D", KDChartParams::Pie );
    addButton( i18n( "Ring" ), "chart_ring", KDChartParams::Ring );
    addButton(i18n("Hi-Lo-Close"), "chart_hiloclose", KDChartParams::HiLo );
    addButton(i18n("Polar"), "chart_polar", KDChartParams::Polar);
    addButton(i18n("Box & Whisker "), "chart_bw", KDChartParams::BoxWhisker );

    QPushButton *current = ((QPushButton*)_typeBG->find( _chart->params()->chartType() ));
    if (current != NULL) {
        current->setOn( true );
    }
    _type=_chart->params()->chartType();
    connect( _typeBG, SIGNAL( clicked( int ) ),
             this, SLOT( chartTypeSelected( int ) ) );

    //  parent->resize( 425, 256 );
    parent->resize(xstep*5+50, ystep*4 + 100);
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
