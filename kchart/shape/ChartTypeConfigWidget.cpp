
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
};

ChartTypeConfigWidget::ChartTypeConfigWidget()
    : d(new Private)
{
    d->shape = 0;
    d->type = KChart::LineChartType;
    d->subtype = KChart::NormalChartSubtype;
    setObjectName("Chart Type");

    QHBoxLayout* layout = new QHBoxLayout;

    d->leftLayout = new QVBoxLayout;
    d->rightLayout = new QVBoxLayout;

    setLayout(layout);

    layout->addLayout(d->leftLayout);
    layout->addLayout(d->rightLayout);

    /*layout->setMargin(5);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 0);
    layout->setRowStretch(2, 0);*/

    QButtonGroup* mainButtonGroup = new QButtonGroup(this);
    mainButtonGroup->setExclusive(true);
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    //buttonGroup->hide();

    Button* button = new Button(this, i18n("Normal Chart"), KIcon("chart_line"));
    button->setChecked( true );
    d->leftLayout->addWidget(button);
    mainButtonGroup->addButton(button, NormalChartSubtype);
    button = new Button(this, i18n("Stacked Chart"), KIcon("chart_pie"));
    d->leftLayout->addWidget(button);
    mainButtonGroup->addButton(button, StackedChartSubtype);
    button = new Button(this, i18n("Percent Chart"), KIcon("chart_pie"));
    d->leftLayout->addWidget(button);
    mainButtonGroup->addButton(button, PercentChartSubtype);

    button = new Button(this, i18n("Bar Chart"), KIcon("chart_bar"));
    button->setChecked( true );
    d->rightLayout->addWidget(button);
    buttonGroup->addButton(button, BarChartType);
    button = new Button(this, i18n("Line Chart"), KIcon("chart_line"));
    d->rightLayout->addWidget(button);
    buttonGroup->addButton(button, LineChartType);
    button = new Button(this, i18n("Area"), KIcon("chart_area"));
    d->rightLayout->addWidget(button);
    buttonGroup->addButton(button, AreaChartType);

    button = new Button(this, i18n("Pie Chart"), KIcon("chart_pie"));
    d->rightLayout->addWidget(button);
    buttonGroup->addButton(button, CircleChartType);
    button = new Button(this, i18n("Polar Chart"), KIcon("chart_pie"));
    d->rightLayout->addWidget(button);
    buttonGroup->addButton(button, RadarChartType);
/*
    button = new Button(this, i18n("Lines"), KIcon("chart_line"));
    layout->addWidget(button, 0, 1);
    buttonGroup->addButton(button, LineChartType);

    button = new Button(this, i18n("Area"), KIcon("chart_area"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 0, 2);
    buttonGroup->addButton(button, AreaChartType);

    // FIXME: Must rename chart_pie into chart_circle (or should we?)
    button = new Button(this, i18n("Pie"), KIcon("chart_pie"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 1, 0);
    buttonGroup->addButton(button, CircleChartType);

    button = new Button(this, i18n("Ring"), KIcon("chart_ring"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 1, 1);
    buttonGroup->addButton(button, RingChartType);

    // FIXME: Must rename chart_polar into chart_radar (or should we?)
    button = new Button(this, i18n("Polar"), KIcon("chart_polar"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 1, 2);
    buttonGroup->addButton(button, RadarChartType);

    // Scatter chart
    button = new Button(this, i18n("Scatter"), KIcon("chart_scatter"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 2, 0 );
    buttonGroup->addButton(button, ScatterChartType);

    // Bubble chart
    button = new Button(this, i18n("Bubble"), KIcon("chart_bubble"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 2, 1 );
    buttonGroup->addButton(button, BubbleChartType);

    // FIXME: Name change of icon
    button = new Button(this, i18n("Stock"), KIcon("chart_boxwhisker"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 3, 0 );
    buttonGroup->addButton(button, StockChartType);

    // Surface chart
    button = new Button(this, i18n("Surface"), KIcon("chart_surface"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 3, 1 );
    buttonGroup->addButton(button, SurfaceChartType);

    // Gantt chart
    button = new Button(this, i18n("Gantt"), KIcon("chart_gantt"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 3, 2 );
    buttonGroup->addButton(button, GanttChartType);
*/

    // Make the button for the current type selected.
//     QPushButton *current = ((QPushButton*)buttonGroup->button( d->shape->params()->chartType() ));
//     if (current != 0) {
//         current->setChecked( true );
//     }
// 
//     d->type = d->shape->params()->chartType();
    connect(mainButtonGroup, SIGNAL(buttonClicked(int)), 
        this,        SLOT(chartSubtypeSelected(int)));

    connect(buttonGroup, SIGNAL(buttonClicked(int)), 
	    this,        SLOT(chartTypeSelected(int)));
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
}

void ChartTypeConfigWidget::chartSubtypeSelected( int type )
{
    d->subtype = (OdfChartSubtype) type;
    emit chartSubtypeChange( d->subtype );
}

#include "ChartTypeConfigWidget.moc"
