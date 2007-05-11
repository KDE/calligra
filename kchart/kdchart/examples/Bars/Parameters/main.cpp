#include <QtGui>
#include <KDChartChart>
#include <KDChartBarDiagram>
#include <KDChartDataValueAttributes>

using namespace KDChart;

class ChartWidget : public QWidget {
  Q_OBJECT
public:
  explicit ChartWidget(QWidget* parent=0)
    : QWidget(parent)
  {

    m_model.insertRows( 0, 2, QModelIndex() );
    m_model.insertColumns(  0,  3,  QModelIndex() );
    for (int row = 0; row < 3; ++row) {
            for (int column = 0; column < 3; ++column) {
                QModelIndex index = m_model.index(row, column, QModelIndex());
                m_model.setData(index, QVariant(row+1 * column) );
            }
    }

    BarDiagram* diagram = new KDChart::BarDiagram;
    diagram->setModel(&m_model);

    BarAttributes ba( diagram->barAttributes() );
    //set the bar width and
    //implicitely enable it
    ba.setFixedBarWidth( 500 );
    ba.setUseFixedBarWidth( true );
    //configure gab between values
    //and blocks
    ba.setGroupGapFactor( 0.50 );
    ba.setBarGapFactor( 0.125 );

    //assign to the diagram
    diagram->setBarAttributes(  ba );

    // display the values
    DataValueAttributes dva( diagram->dataValueAttributes() );
    TextAttributes ta( dva.textAttributes() );
    //rotate if you wish
    //ta.setRotation( 0 );
    ta.setFont( QFont( "Comic", 9 ) );
    ta .setPen( QPen( QColor( Qt::darkGreen ) ) );
    ta.setVisible( true );
    dva.setTextAttributes( ta );
    dva.setVisible( true );
    diagram->setDataValueAttributes( dva );

    //draw a surrounding line around bars
    QPen linePen;
    linePen.setColor( Qt::magenta );
    linePen.setWidth( 4 );
    linePen.setStyle( Qt::DotLine );
    //draw only around a dataset
    //to draw around all the bars
    // call setPen( myPen );
    diagram->setPen( 1,  linePen );

    m_chart.coordinatePlane()->replaceDiagram(diagram);
    m_chart.setGlobalLeadingTop( 40 );

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(&m_chart);
    setLayout(l);
  }

private:
  Chart m_chart;
  QStandardItemModel m_model;
};

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    ChartWidget w;
    w.show();

    return app.exec();
}

#include "main.moc"
