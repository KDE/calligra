#include <QtGui>
#include <KDChartChart>
#include <KDChartBarDiagram>
#include <KDChartHeaderFooter>
#include <KDChartPosition>
#include <KDChartBackgroundAttributes>
#include <KDChartFrameAttributes>
#include <KDChartGridAttributes>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartAbstractCartesianDiagram>
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
                m_model.setData(index, QVariant(row+10 * column) );
            }
    }

    BarDiagram* diagram = new BarDiagram;
    diagram->setModel(&m_model);

    /*
     * AXIS
     */

    CartesianAxis *xAxis = new CartesianAxis( diagram );
    CartesianAxis *yAxis = new CartesianAxis ( diagram );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );

    /*
     * AXIS LABELS
     */

    // set the following to 0, to see the default Abscissa labels
    // (== X headers, as read from the data file)
#if 1
    QStringList months;
    months << "January" << "February";
    xAxis->setLabels( months );
    QStringList shortMonths;
    shortMonths << "Jan" << "Feb";
    xAxis->setShortLabels( shortMonths );
#endif

    /*
     * DATA VALUES SETTINGS
     */

    // We set the DataValueAttributes on a per-column basis here,
    // because we want the texts to be printed in different
    //colours - according to their respective dataset's colour.
    const QFont font(QFont( "Comic", 10 ));
    const int colCount = diagram->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        QBrush brush( diagram->brush( iColumn ) );
        DataValueAttributes a( diagram->dataValueAttributes( iColumn ) );
        TextAttributes ta( a.textAttributes() );
        ta.setRotation( 0 );
        ta.setFont( font );
        ta .setPen( QPen( brush.color() ) );
        ta.setVisible( true );
        a.setTextAttributes( ta );
        a.setSuffix( "%" );
        a.setVisible( true );
        diagram->setDataValueAttributes( iColumn, a);
    }

    /*
     *  VERTICAL RANGE SETTINGS
     */

    // m_chart.coordinatePlane() returns an abstract plane.
    // We need to cast in order to be able set our own boundaries.
    CartesianCoordinatePlane* plane1 =
        static_cast <CartesianCoordinatePlane*>(m_chart.coordinatePlane());

    plane1->setVerticalRange( QPair<qreal,  qreal>( 0.0,  100.0 ) );
    plane1->replaceDiagram(diagram);

    /*
     * HEADER SETTINGS
     */

    // Add at one Header and set it up
    HeaderFooter* header = new HeaderFooter( &m_chart );
    header->setPosition( Position::North );
    header->setText( "A Bar Chart with Adjusted Vertical Range" );
    m_chart.addHeaderFooter( header );

    // Configure the Header text attributes
    TextAttributes hta;
    hta.setPen( QPen(  Qt::red ) );

    // let the header resize itself
    // together with the widget.
    // so-called relative size
    Measure m( 35.0 );
    m.setRelativeMode( header->autoReferenceArea(),
                       KDChartEnums::MeasureOrientationMinimum );
    hta.setFontSize( m );
    // min font size
    m.setValue( 3.0 );
    m.setCalculationMode( KDChartEnums::MeasureCalculationModeAbsolute );
    hta.setMinimalFontSize( m  );
    header->setTextAttributes( hta );

    // Configure the Header's Background
    BackgroundAttributes hba;
    hba.setBrush( Qt::white );
    hba.setVisible( true );
    header->setBackgroundAttributes(  hba );

    // Configure the header Frame attributes
    FrameAttributes hfa;
    hfa.setPen( QPen ( QBrush( Qt::darkGray ), 2 ) );
    hfa.setPadding( 2 );
    hfa.setVisible( true );
    header->setFrameAttributes(  hfa );

    /*
     * GRIDS SETTINGS
     */

    // retrieve your grid attributes
    // display grid and sub-grid
    GridAttributes ga ( plane1->gridAttributes( Qt::Vertical ) );

    // Configure a sub-grid pen
    QPen subGridPen( Qt::darkGray );
    subGridPen.setStyle( Qt::DotLine );
    ga.setSubGridPen(  subGridPen );

    // change step and substep width
    // to fit the boundaries
    ga.setGridStepWidth( 10.0 );
    ga.setGridSubStepWidth( 5.0 );
    ga.setGridVisible(  true );
    ga.setSubGridVisible( true );



    // Assign your grid to the plane
    plane1->setGridAttributes( Qt::Vertical,  ga );

    QVBoxLayout* l = new QVBoxLayout(this);
    m_chart.setGlobalLeadingTop( 5 );
    l->addWidget(&m_chart);
    setLayout(l);
  }

private:
  Chart m_chart;
  QStandardItemModel m_model;
    QPixmap pixmap;
};

int main( int argc, char** argv ) {
    QApplication app( argc, argv );

    ChartWidget w;
    w.show();

    return app.exec();
}

#include "main.moc"
