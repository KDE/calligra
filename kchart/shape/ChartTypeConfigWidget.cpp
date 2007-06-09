
#include "ChartTypeConfigWidget.h"

#include <QLayout>
#include <QButtonGroup>
#include <q3frame.h>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kvbox.h>

#include "ChartShape.h"

namespace KChart
{

KChartButton::KChartButton(QWidget *parent, const QString & _text, const KIcon &_icon)
    : KVBox(parent)
{
    // The button
    m_button = new QPushButton(this);
    m_button->setIcon( _icon );
    m_button->setCheckable( true );

    // The text
    QLabel *label = new QLabel(_text, this);
    label->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
}

KChartButton::~KChartButton()
{
}


// ================================================================

class ChartTypeConfigWidget::Private
{
public:
    enum ChartType { Bar, Line, Area, HiLo, BoxWhisker, Pie, Ring, Polar };

    ChartShape* shape;
    ChartType type;
    int colPos;
    int rowPos;
    QButtonGroup* typeBG;
    QGridLayout* layout;
};

ChartTypeConfigWidget::ChartTypeConfigWidget()
    : d( new Private() )
{
    d->shape = 0;
    d->typeBG = new QButtonGroup( this );
    d->typeBG->setExclusive( true );
    //d->typeBG->hide();

    d->colPos=0;
    d->rowPos=0;
    d->layout = new QGridLayout( this );
    d->layout->setMargin( 5 );
    d->layout->setRowStretch( 0, 0 );
    d->layout->setRowStretch( 1, 0 );
    d->layout->setRowStretch( 2, 0 );

    setObjectName( i18n( "Chart Type" ) );

    addButton( i18n( "Bar" ),            "chart_bar",    Private::Bar );
    addButton( i18n( "Lines" ),          "chart_line",   Private::Line );
    addButton( i18n( "Area" ),           "chart_area",   Private::Area );

    addButton( i18n("HiLo"),             "chart_hilo",   Private::HiLo );
    addButton( i18n("Box & Whisker "),   "chart_boxwhisker", Private::BoxWhisker );
    incPos();

    addButton( i18n( "Pie" ),            "chart_pie",    Private::Pie );
    addButton( i18n( "Ring" ),           "chart_ring",   Private::Ring );
    addButton( i18n( "Polar" ),          "chart_polar",  Private::Polar);

    // Make the button for the current type selected.
//     QPushButton *current = ((QPushButton*)d->typeBG->button( d->shape->params()->chartType() ));
//     if (current != 0) {
//         current->setChecked( true );
//     }
// 
//     d->type = d->shape->params()->chartType();
    connect( d->typeBG, SIGNAL( clicked( int ) ),
            this,      SLOT( chartTypeSelected( int ) ) );
}

ChartTypeConfigWidget::~ChartTypeConfigWidget()
{
    delete d;
}

void ChartTypeConfigWidget::open( KoShape* shape )
{
    d->shape = dynamic_cast<ChartShape*>( shape );
}

void ChartTypeConfigWidget::save()
{
}

KAction* ChartTypeConfigWidget::createAction()
{
    return 0;
}

void ChartTypeConfigWidget::addButton(const QString &name,
                                                const QString &icon_name,
                                                int type)
{
    KChartButton *button = new KChartButton( this, name, KIcon( icon_name ) );
    d->layout->addWidget(button, d->rowPos, d->colPos);
    d->typeBG->addButton( button->button(), type );

    incPos();
}

void ChartTypeConfigWidget::incPos()
{
    if (d->colPos == 2) {
        d->colPos=0;
        d->rowPos++; //place the next button in the second row
    }
    else
        d->colPos++;
}

void ChartTypeConfigWidget::chartTypeSelected( int type )
{
    d->type = (Private::ChartType) type;
    emit chartChange(type);
}

void ChartTypeConfigWidget::apply()
{
//     d->shape->chart()->setChartType( m_type );
}

}  //namespace KChart

#include "ChartTypeConfigWidget.moc"
