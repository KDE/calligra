
#include "ChartTypeConfigWidget.h"

// Qt
#include <QButtonGroup>
#include <QGridLayout>
#include <QToolButton>

// KDE
#include <klocale.h>
#include <kiconloader.h>

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
        setIconSize(QSize(48, 48));
        setText(text);
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
};

// ================================================================

class ChartTypeConfigWidget::Private
{
public:
    ChartShape* shape;
    OdfChartType type;
};

ChartTypeConfigWidget::ChartTypeConfigWidget()
    : d(new Private)
{
    d->shape = 0;
    setObjectName("Chart Type");

    QGridLayout* layout = new QGridLayout(this);
    layout->setMargin(5);
    layout->setRowStretch(0, 0);
    layout->setRowStretch(1, 0);
    layout->setRowStretch(2, 0);

    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    //buttonGroup->hide();

    Button* button = new Button(this, i18n("Bars"), KIcon("chart_bar"));
    layout->addWidget(button, 0, 0);
    buttonGroup->addButton(button, BarChartType);

    button = new Button(this, i18n("Lines"), KIcon("chart_line"));
    layout->addWidget(button, 0, 1);
    buttonGroup->addButton(button, LineChartType);

    button = new Button(this, i18n("Area"), KIcon("chart_area"));
    button->setEnabled(false); // TODO not supported yet
    layout->addWidget(button, 0, 2);
    buttonGroup->addButton(button, AreaChartType);

#if 0
    // FIXME: Name change of icon
    button = new Button(this, i18n("HiLo"), KIcon("chart_hilo"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 1, 0);
    buttonGroup->addButton(button, StockChartType);
#endif

    // FIXME: Name change of icon
    button = new Button(this, i18n("Stock"), KIcon("chart_boxwhisker"));
    button->setEnabled(false); // TODO not supported yet
    //layout->addWidget(button, 1, 1, 1, 2);
    layout->addWidget(button, 1, 0 );
    buttonGroup->addButton(button, StockChartType);

    // FIXME: Must rename chart_pie into chart_circle (or should we?)
    button = new Button(this, i18n("Pie"), KIcon("chart_pie"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 2, 0);
    buttonGroup->addButton(button, CircleChartType);

    button = new Button(this, i18n("Ring"), KIcon("chart_ring"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 2, 1);
    buttonGroup->addButton(button, RingChartType);

    // FIXME: Must rename chart_polar into chart_radar (or should we?)
    button = new Button(this, i18n("Polar"), KIcon("chart_polar"));
    button->setEnabled(false); // FIXME: not supported yet
    layout->addWidget(button, 2, 2);
    buttonGroup->addButton(button, RadarChartType);

    // Make the button for the current type selected.
//     QPushButton *current = ((QPushButton*)buttonGroup->button( d->shape->params()->chartType() ));
//     if (current != 0) {
//         current->setChecked( true );
//     }
// 
//     d->type = d->shape->params()->chartType();
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
    ChartTypeCommand  command(d->shape->chart());
    command.setChartType(d->type);
    command.redo();
}

KAction* ChartTypeConfigWidget::createAction()
{
    return 0;
}

void ChartTypeConfigWidget::chartTypeSelected( int type )
{
    d->type = (OdfChartType) type;
    emit chartTypeChange(d->type);
}

#include "ChartTypeConfigWidget.moc"
