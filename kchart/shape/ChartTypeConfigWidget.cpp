
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
    bool threeDMode;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
    Ui::ChartTypeConfigWidget ui;
};

ChartTypeConfigWidget::ChartTypeConfigWidget()
    : d(new Private)
{
    d->shape = 0;
    d->type = KChart::BarChartType;
    d->subtype = KChart::NormalChartSubtype;
    setObjectName("Chart Type");
    d->ui.setupUi( this );
    d->ui.typeCombobox->addItem( KIcon("chart_bar"), i18n("Bar Chart"), BarChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_line"), i18n("Line Chart"), LineChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_area"), i18n("Area Chart"), AreaChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_pie"), i18n("Pie Chart"), CircleChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_ring"), i18n("Ring Chart"), RingChartType );
    d->ui.typeCombobox->addItem( KIcon("chart_point"), i18n("Scatter Chart"), ScatterChartType );
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

    // The default chart is a BarChartType, so let's prepare for that
    d->ui.threeDLook->show();

    connect( subtypeButtonGroup, SIGNAL( buttonClicked( int ) ),
             this, SLOT( chartSubtypeSelected( int ) ) );

    connect( d->ui.threeDLook, SIGNAL( toggled( bool ) ),
             this, SLOT( setThreeDMode( bool ) ) );
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
    ChartTypeCommand command( d->shape );
    command.setChartType( d->type, d->subtype );
    command.redo();
}

KAction* ChartTypeConfigWidget::createAction()
{
    return 0;
}

void ChartTypeConfigWidget::chartTypeSelected( int type )
{
    d->type = ( OdfChartType )type;
    emit chartTypeChange( d->type );

    switch ( type ) {
        case BarChartType:
            d->ui.optionsBox->show();
            // Hide them for now, as the functionality for it is not yet implemented
            /*
            d->ui.linesInBarChart->show();
            d->ui.linesInBarChartArea->show();
            */
            d->ui.linesInBarChart->hide();
            d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( true );
            break;
        case LineChartType:
            d->ui.optionsBox->show();
            d->ui.linesInBarChart->hide();
            d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( true );
            break;
        case ScatterChartType:
            d->ui.optionsBox->hide();
            d->ui.linesInBarChart->hide();
            d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( false );
            break;
        case AreaChartType:
            d->ui.optionsBox->show();
            d->ui.linesInBarChart->hide();
            d->ui.linesInBarChartArea->hide();
            d->ui.threeDLook->setEnabled( false );
            break;
        case CircleChartType:
            d->ui.optionsBox->hide();
            d->ui.threeDLook->setEnabled( true );
            break;
        default:
            d->ui.optionsBox->hide();
            d->ui.threeDLook->setEnabled( false );
            break;
    }
    updateChartTypeOptions( d->shape->chartTypeOptions( d->type ) );
}

void ChartTypeConfigWidget::chartSubtypeSelected( int type )
{
    d->subtype = (OdfChartSubtype) type;
    emit chartSubtypeChange( d->subtype );
}

void ChartTypeConfigWidget::setThreeDMode( bool threeD )
{
    d->threeDMode = threeD;
    emit threeDModeToggled( threeD );
}

void ChartTypeConfigWidget::updateChartTypeOptions( ChartTypeOptions options )
{
    switch ( options.subtype ) {
        case NormalChartSubtype:
            d->ui.subtypeNormal->blockSignals( true );
            d->ui.subtypeNormal->setChecked( true );
            d->ui.subtypeNormal->blockSignals( false );
            break;
        case StackedChartSubtype:
            d->ui.subtypeStacked->blockSignals( true );
            d->ui.subtypeStacked->setChecked( true );
            d->ui.subtypeStacked->blockSignals( false );
            break;
        case PercentChartSubtype:
            d->ui.subtypePercent->blockSignals( true );
            d->ui.subtypePercent->setChecked( true );
            d->ui.subtypePercent->blockSignals( false );
            break;
    }
    d->ui.threeDLook->setChecked( d->shape->threeDMode() );
}

#include "ChartTypeConfigWidget.moc"
