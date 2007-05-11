#include <QtGui>
#include <KDChartChart>
#include <KDChartPolarDiagram>
#include <KDChartDataValueAttributes>

using namespace KDChart;

class ChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChartWidget(QWidget* parent=0)
        : QWidget(parent)
    {

        // initialize the ItemModel and fill in some data
        m_model.insertRows( 0, 10 );
        m_model.insertColumns(  0,  5 );
        int value = 0;
        for ( int column = 0; column < m_model.columnCount(); ++column ) {
            for ( int row = 0; row < m_model.rowCount(); ++row ) {
                QModelIndex index = m_model.index( row, column );
                m_model.setData( index, QVariant( value++  ) );
            }
        }
        // We need a Polar plane for the Polar type
        PolarCoordinatePlane* polarPlane = new PolarCoordinatePlane( &m_chart );
        // replace the default Cartesian plane with
        // our Polar plane
        m_chart.replaceCoordinatePlane( polarPlane );

        // assign the model to our polar diagram
        PolarDiagram* diagram = new PolarDiagram;
        diagram->setModel(&m_model);

        // Configure some Polar specifical attributes

        // Display data values
        // not implemented yet - disable for now
        const QFont font(QFont( "Comic", 10 ));
        const int colCount = diagram->model()->columnCount();
        for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
            DataValueAttributes dva( diagram->dataValueAttributes( iColumn ) );
            TextAttributes ta( dva.textAttributes() );
            ta.setRotation( 0 );
            ta.setFont( font );
            ta .setPen( QPen( Qt::gray  ) );
            ta.setVisible( true );
            dva.setTextAttributes( ta );
            dva.setVisible( true );
            diagram->setDataValueAttributes( iColumn, dva);
        }


        // Assign our diagram to the Chart
        m_chart.coordinatePlane()->replaceDiagram(diagram);

        QVBoxLayout* l = new QVBoxLayout(this);
        l->addWidget(&m_chart);
        m_chart.setGlobalLeadingTop( 5 );
        m_chart.setGlobalLeadingBottom( 5 );
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
