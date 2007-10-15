
#include "ChartTypeConfigWidget.h"

// Qt
#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QToolButton>

// KDE
#include <klocale.h>
#include <kiconloader.h>
#include <KDebug>

// KDChart
#include "KDChartChart.h"

// KChart
#include "kchart_global.h"
#include "ChartShape.h"
#include "commands/ChartTypeCommand.h"

using namespace KChart;

class Button : public QToolButton
{
public:
    Button(QWidget* parent, const QString& text, const KIcon& icon)
        : QToolButton(parent)
    {
        setCheckable(true);
        setIcon(icon);
        //setIconSize(QSize(48, 48));
        setToolTip(text);
        setText(text);
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
};

// ================================================================

class ChartTypeConfigWidget::Private
{
public:
    ChartShape* shape;
    OdfChartType type;
    OdfChartSubtype subtype;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
    Ui::ChartTypeConfigWidget ui;
};

ChartTypeConfigWidget::ChartTypeConfigWidget()
    : d(new Private)
{
    d->shape = 0;
    d->type = KChart::LineChartType;
    d->subtype = KChart::NormalChartSubtype;
    setObjectName("Chart Type");
    d->ui.setupUi( this );
    d->ui.typeCombobox->addItem( KIcon("chart_bar"), i18n("Bar Chart"), BarChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_line"), i18n("Line Chart"), LineChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_area"), i18n("Area Chart"), AreaChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_pie"), i18n("Pie Chart"), CircleChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_ring"), i18n("Ring Chart"), RingChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_pie"), i18n("Radar Chart"), RadarChartType );

    connect( d->ui.typeCombobox, SIGNAL( currentIndexChanged( int ) ),
             this, SLOT( chartTypeSelected( int ) ) );

    QButtonGroup *subtypeButtonGroup = new QButtonGroup( this );
    subtypeButtonGroup->setExclusive( true );
    d->ui.subtypeNormal->setChecked( true );
    d->ui.subtypeNormal->setIcon(  KIcon("chart_bar_beside" ) );
    d->ui.subtypeStacked->setIcon( KIcon("chart_bar_layer" ) );
    d->ui.subtypePercent->setIcon( KIcon("chart_bar_percent" ) );

    subtypeButtonGroup->addButton( d->ui.subtypeNormal, NormalChartSubtype );
    subtypeButtonGroup->addButton( d->ui.subtypeStacked, StackedChartSubtype );
    subtypeButtonGroup->addButton( d->ui.subtypePercent, PercentChartSubtype );

    connect( subtypeButtonGroup, SIGNAL( buttonClicked( int ) ),
             this, SLOT( chartSubtypeSelected( int ) ) );
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
    ChartTypeCommand  command( d->shape );
    command.setChartType( d->type, d->subtype );
    command.redo();
}

KAction* ChartTypeConfigWidget::createAction()
{
    return 0;
}

void ChartTypeConfigWidget::chartTypeSelected( int type )
{
    d->type = (OdfChartType) type;
    emit chartTypeChange( d->type );

    switch ( d->shape->lastChartSubtype( d->type ) ) {
        case NormalChartSubtype:
            d->ui.subtypeNormal->setChecked( true );
            break;
        case StackedChartSubtype:
            d->ui.subtypeStacked->setChecked( true );
            break;
        case PercentChartSubtype:
            d->ui.subtypePercent->setChecked( true );
            break;
    }

    if( type != BarChartType && type != LineChartType && type != AreaChartType ) {
        d->ui.subtypeNormal->setEnabled( false );
        d->ui.subtypeStacked->setEnabled( false );
        d->ui.subtypePercent->setEnabled( false );
    } else {
        d->ui.subtypeNormal->setEnabled( true );
        d->ui.subtypeStacked->setEnabled( true );
        d->ui.subtypePercent->setEnabled( true );
    }
}

void ChartTypeConfigWidget::chartSubtypeSelected( int type )
{
    d->subtype = (OdfChartSubtype) type;
    emit chartSubtypeChange( d->subtype );
}

#include "ChartTypeConfigWidget.moc"
