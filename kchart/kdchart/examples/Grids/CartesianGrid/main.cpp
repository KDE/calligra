#include <QtGui>
#include <KDChartChart>
#include <KDChartLineDiagram>
#include <KDChartHeaderFooter>
#include <KDChartPosition>
#include <KDChartBackgroundAttributes>
#include <KDChartFrameAttributes>
#include <KDChartGridAttributes>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartCartesianCoordinatePlane>
#include <KDChartAbstractCartesianDiagram>
#include <KDChartDataValueAttributes>
#include <KDChartPosition>


using namespace KDChart;

class ChartWidget : public QWidget {
  Q_OBJECT
public:
  explicit ChartWidget(QWidget* parent=0)
    : QWidget(parent)
  {

    m_model.insertRows( 0, 6, QModelIndex() );
    m_model.insertColumns(  0,  4,  QModelIndex() );
    for (int row = 0; row < 6; ++row) {
            for (int column = 0; column < 4; ++column) {
                QModelIndex index = m_model.index(row, column, QModelIndex());
                m_model.setData(index, QVariant(row*0.5 + column) );
            }
    }

    LineDiagram* diagram = new LineDiagram;
    diagram->setModel(&m_model);

    CartesianAxis *xAxis = new CartesianAxis( diagram );
    CartesianAxis *yAxis = new CartesianAxis ( diagram );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    diagram->addAxis( xAxis );
    diagram->addAxis( yAxis );

    m_chart.coordinatePlane()->replaceDiagram(diagram);

    /* Header */

   // Add at one Header and set it up
    HeaderFooter* header = new HeaderFooter( &m_chart );
    header->setPosition( Position::North );
    header->setText( "A Line Chart with Grid Configured" );
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


    // diagram->coordinatePlane returns an abstract plane one.
    // if we want to specify the orientation we need to cast
    // as follow
    CartesianCoordinatePlane* plane = static_cast <CartesianCoordinatePlane*>
		    ( diagram->coordinatePlane() );

    /* Configure grid steps and pen */

    // Vertical
    GridAttributes gv ( plane->gridAttributes( Qt::Vertical ) );

    // Configure a grid pen
    // I know it is horrible
    // just for demo'ing
    QPen gridPen(  Qt::gray );
    gridPen.setWidth( 2 );
    gv.setGridPen(  gridPen );

    // Configure a sub-grid pen
    QPen subGridPen( Qt::darkGray );
    subGridPen.setStyle( Qt::DotLine );
    gv.setSubGridPen(  subGridPen );

    // Display a blue zero line
    gv.setZeroLinePen( QPen( Qt::blue ) );

    // change step and substep width
    // or any of those.
    gv.setGridStepWidth( 1.0 );
    gv.setGridSubStepWidth( 0.5 );
    gv.setGridVisible(  true );
    gv.setSubGridVisible( true );

    // Horizontal

    GridAttributes gh = plane->gridAttributes( Qt::Horizontal );
    gh.setGridPen( gridPen );
    gh.setGridStepWidth(  0.5 );
    gh.setSubGridPen(  subGridPen );
    gh.setGridSubStepWidth( 0.1 );

    plane->setGridAttributes( Qt::Vertical,  gv );
    plane->setGridAttributes( Qt::Horizontal,  gh );

    // Data Values Display and position
    const int colCount = diagram->model()->columnCount(diagram->rootIndex());
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        DataValueAttributes a( diagram->dataValueAttributes( iColumn ) );
        RelativePosition pos ( a.position( true ) );
        pos.setAlignment( Qt::AlignRight );
        a.setPositivePosition( pos );
        QBrush brush( diagram->brush( iColumn ) );
        TextAttributes ta( a.textAttributes() );
        ta.setRotation( 0 );
        ta.setFont( QFont( "Comic", 10 ) );
        ta.setPen( QPen( brush.color() ) );
        ta.setVisible( true );
        a.setVisible( true );
        a.setTextAttributes( ta );
        diagram->setDataValueAttributes( iColumn, a );
    }


    QVBoxLayout* l = new QVBoxLayout(this);
    l->addWidget(&m_chart);
    m_chart.setGlobalLeadingRight( 20 );
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
