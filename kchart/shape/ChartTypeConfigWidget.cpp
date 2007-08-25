
#include "ChartTypeConfigWidget.h"

#include <QButtonGroup>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QPixmap>
#include <QToolButton>

#include <klocale.h>
#include <kglobal.h>
#include <kiconloader.h>
#include <kvbox.h>

#include "KDChartBarDiagram.h"
#include "KDChartChart.h"

#include "ChartShape.h"

namespace KChart
{

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

}; // namespace KChart

using namespace KChart;

// ================================================================

class ChartTypeConfigWidget::Private
{
public:
    enum ChartType { Bar, Line, Area, HiLo, BoxWhisker, Pie, Ring, Polar };

    ChartShape* shape;
    ChartType type;
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

    Button* button = new Button(this, i18n("Bar"), KIcon("chart_bar"));
    layout->addWidget(button, 0, 0);
    buttonGroup->addButton(button, Private::Bar);

    button = new Button(this, i18n("Lines"), KIcon("chart_line"));
    layout->addWidget(button, 0, 1);
    buttonGroup->addButton(button, Private::Line);

    button = new Button(this, i18n("Area"), KIcon("chart_area"));
    layout->addWidget(button, 0, 2);
    buttonGroup->addButton(button, Private::Area);

    button = new Button(this, i18n("HiLo"), KIcon("chart_hilo"));
    layout->addWidget(button, 1, 0);
    buttonGroup->addButton(button, Private::HiLo);

    button = new Button(this, i18n("Box && Whisker"), KIcon("chart_boxwhisker"));
    layout->addWidget(button, 1, 1, 1, 2);
    buttonGroup->addButton(button, Private::BoxWhisker);

    button = new Button(this, i18n("Pie"), KIcon("chart_pie"));
    layout->addWidget(button, 2, 0);
    buttonGroup->addButton(button, Private::Pie);

    button = new Button(this, i18n("Ring"), KIcon("chart_ring"));
    layout->addWidget(button, 2, 1);
    buttonGroup->addButton(button, Private::Ring);

    button = new Button(this, i18n("Polar"), KIcon("chart_polar"));
    layout->addWidget(button, 2, 2);
    buttonGroup->addButton(button, Private::Polar);

    // Make the button for the current type selected.
//     QPushButton *current = ((QPushButton*)buttonGroup->button( d->shape->params()->chartType() ));
//     if (current != 0) {
//         current->setChecked( true );
//     }
// 
//     d->type = d->shape->params()->chartType();
    connect(buttonGroup, SIGNAL(clicked(int)), this, SLOT(chartTypeSelected(int)));
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
//     d->shape->chart()->coordinatePlane()->replaceDiagram( new KDChart::BarDiagram() );
}

KAction* ChartTypeConfigWidget::createAction()
{
    return 0;
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

#include "ChartTypeConfigWidget.moc"
