#include <QtGui>
#include <KDChartChart>
#include <KDChartBarDiagram>


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

    KDChart::BarDiagram* diagram = new KDChart::BarDiagram;
    diagram->setModel(&m_model);

    m_chart.coordinatePlane()->replaceDiagram(diagram);

    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(&m_chart);
    setLayout(l);
    m_timer = new QTimer(this);
    connect( m_timer, SIGNAL( timeout() ),
             this, SLOT( slotTimeout() ) );
    m_timer->start( 200 );
  }

private slots:
      void slotTimeout() {
          QModelIndex index = m_model.index( 0, 1, QModelIndex());
          double value = ( m_model.data( index ).toInt() % 24 ) +1;
          m_model.setData( index, value );
      }

private:
  KDChart::Chart m_chart;
  QStandardItemModel m_model;
  QTimer *m_timer;
};

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    ChartWidget w;
    w.show();

    return app.exec();
}

#include "main.moc"
