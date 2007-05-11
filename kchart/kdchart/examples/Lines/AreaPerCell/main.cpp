#include <QtGui>
#include <KDChartChart>
#include <KDChartLineDiagram>

using namespace KDChart;

class ChartWidget : public QWidget {
  Q_OBJECT
public:

  explicit ChartWidget(QWidget* parent=0)
    : QWidget(parent)
  {

    m_model.insertRows( 0,6, QModelIndex() );
    m_model.insertColumns( 0,1, QModelIndex() );

    m_model.setData( m_model.index( 0, 0, QModelIndex()),  15);
    m_model.setData( m_model.index( 1, 0, QModelIndex()),  11);
    m_model.setData( m_model.index( 2, 0, QModelIndex()),  7);
    m_model.setData( m_model.index( 3, 0, QModelIndex()),  3);
    m_model.setData( m_model.index( 4, 0, QModelIndex()),  -1);
    m_model.setData( m_model.index( 5, 0, QModelIndex()),  -5);

    LineDiagram* diagram = new LineDiagram;
    diagram->setModel(&m_model);

    m_chart.coordinatePlane()->replaceDiagram(diagram);

    // paint the areas in a few cells
    // using different brushes
    LineAttributes la3(diagram->lineAttributes(m_model.index(3,0,QModelIndex())));
    la3.setDisplayArea(  true );
    la3.setTransparency( 150 );
    diagram->setBrush(m_model.index(1,0,QModelIndex()),QBrush(Qt::green));
    diagram->setLineAttributes( m_model.index( 1, 0,  QModelIndex()),la3);
    diagram->setBrush(m_model.index(3,0,QModelIndex()),QBrush(Qt::yellow));
    diagram->setLineAttributes(m_model.index(3,0,QModelIndex()),la3);
    diagram->setBrush(m_model.index(4,0,QModelIndex()),QBrush(Qt::red));
    diagram->setLineAttributes(m_model.index(4,0,QModelIndex()),la3);

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
