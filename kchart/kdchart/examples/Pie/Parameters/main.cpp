#include <QtGui>
#include <KDChartChart>
#include <KDChartPieDiagram>
#include <KDChartDataValueAttributes>
#include <KDChartPieAttributes>

using namespace KDChart;

class ChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit ChartWidget(QWidget* parent=0)
        : QWidget(parent)
    {

        m_model.insertRows( 0, 1, QModelIndex() );
        m_model.insertColumns(  0,  6,  QModelIndex() );
        for (int row = 0; row < 1; ++row) {
            for (int column = 0; column < 6; ++column) {
                QModelIndex index = m_model.index(row, column, QModelIndex());
                m_model.setData(index, QVariant(row+1 * column+1) );
            }
        }
        // We need a Polar plane for the Pie type
        PolarCoordinatePlane* polarPlane = new PolarCoordinatePlane( &m_chart );
        // replace the default Cartesian plane with
        // our Polar plane
        m_chart.replaceCoordinatePlane( polarPlane );

        // assign the model to our pie diagram
        PieDiagram* diagram = new PieDiagram;
        diagram->setModel(&m_model);

        // Configure some Pie specifical attributes

        // explode a section
        PieAttributes pa(  diagram->pieAttributes() );
        pa.setExplodeFactor( 0.1 );

        // Assign the attributes
        // to the diagram
        diagram->setPieAttributes( 1,  pa );

        // Configure a generic attribute
        // available to all chart types
        QPen sectionPen;
        sectionPen.setWidth( 5 );
        sectionPen.setStyle( Qt::DashLine );
        sectionPen.setColor( Qt::magenta );

        diagram->setPen( 1, sectionPen );

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
