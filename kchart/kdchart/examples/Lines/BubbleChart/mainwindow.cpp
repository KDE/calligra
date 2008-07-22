/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License versions 2.0 or 3.0 as published by the Free Software
 ** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
 ** included in the packaging of this file.  Alternatively you may (at
 ** your option) use any later version of the GNU General Public
 ** License if such license has been publicly approved by
 ** Klarälvdalens Datakonsult AB (or its successors, if any).
 ** 
 ** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
 ** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE. Klarälvdalens Datakonsult AB reserves all rights
 ** not expressly granted herein.
 ** 
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 **********************************************************************/

#include "mainwindow.h"

#include <KDChartChart>
#include <KDChartLineDiagram>
#include <KDChartDataValueAttributes>
#include <KDChartTextAttributes>
#include <KDChartMarkerAttributes>

#include <QStandardItemModel>
#include <QTimer>


using namespace KDChart;



static const int nBubbles = 7;
// we display seven bubbles using the following data structure:
// 0: Y
// 1: X
// 2: size
// 3: color as returned by QColor::name()
// 4: style of surrounding line, can be one of these:
//    "NoPen", "SolidLine", "DashLine", "DotLine", "DashDotLine", "DashDotDotLine"
struct DataType{
    DataType( qreal x_,
              qreal y_,
              qreal size_,
              QString color_,
              QString style_ )
    : x(x_)
    , y(y_)
    , size(size_)
    , color(color_)
    , style(style_)
    {}
    qreal x;
    qreal y;
    qreal size;
    QString color;
    QString style;
};
static const DataType bubblesData[nBubbles] = {
    DataType(0.5, 1.0, 100,"#FF0000", "NoPen"),
    DataType(1.0, 0.5, 60, "#00FF00", "SolidLine"),
    DataType(1.6, 2.0, 28, "#0000FF", "DotLine"),
    DataType(0.7, 0.3, 55, "#FFFF00", "DashLine"),
    DataType(1.3, 2.0, 95, "#FF00FF", "DashDotLine"),
    DataType(2.0, 1.0, 75, "#00FFFF", "DashDotDotLine"),
    DataType(1.4, 1.1, 85, "#FFFFFF", "DotLine")
};

#define ROLE_SIZE Qt::UserRole + 1
#define ROLE_COLOR Qt::UserRole + 2
#define ROLE_STYLE Qt::UserRole + 3



MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent ),
    m_model(0)
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    chartLayout->addWidget( m_chart );
    // Set up the data
    initializeDataModel();
    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setDatasetDimension(2);
    // Register the data model at the diagram
    m_lines->setModel( m_model );
    // Add axes to the diagram
    CartesianAxis *xAxis  = new CartesianAxis( m_lines );
    CartesianAxis *xAxis2 = new CartesianAxis( m_lines );
    CartesianAxis *yAxis  = new CartesianAxis ( m_lines );
    CartesianAxis *yAxis2 = new CartesianAxis ( m_lines );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    xAxis2->setPosition( KDChart::CartesianAxis::Top );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    yAxis2->setPosition( KDChart::CartesianAxis::Right );
    m_lines->addAxis( xAxis );
    m_lines->addAxis( xAxis2 );
    m_lines->addAxis( yAxis );
    m_lines->addAxis( yAxis2 );

    m_chart->coordinatePlane()->replaceDiagram( m_lines );
    m_chart->setGlobalLeading( 20, 20, 20, 20 );

    setMarkerAttributes();

    //adjustSizeOfDataArea();
    QTimer::singleShot(200, this, SLOT(adjustSizeOfDataArea()));
}


void MainWindow::initializeDataModel()
{
    m_model = new QStandardItemModel( nBubbles, 2, this );
    m_model->setHeaderData(0, Qt::Horizontal, tr("Some Bubbles"));
    for( int i=0;  i < nBubbles;  ++i ){
        const QModelIndex indexX = m_model->index(i, 0);
        const QModelIndex indexY = m_model->index(i, 1);

        m_model->setData(indexX, bubblesData[i].x, Qt::DisplayRole);

        m_model->setData(indexY, bubblesData[i].y, Qt::DisplayRole);
        m_model->setData(indexY, bubblesData[i].size,  ROLE_SIZE );
        m_model->setData(indexY, bubblesData[i].color, ROLE_COLOR);
        m_model->setData(indexY, bubblesData[i].style, ROLE_STYLE);
    }
}


void MainWindow::setMarkerAttributes()
{
    TextAttributes ta( m_lines->dataValueAttributes().textAttributes() );
    ta.setRotation( 0 );
    ta.setFont( QFont( "Courier", 0, QFont::DemiBold ) );
    Measure m(10, KDChartEnums::MeasureCalculationModeAbsolute);
    ta.setFontSize(m);
    ta.setMinimalFontSize(m);

    // disable the connecting line
    m_lines->setPen( Qt::NoPen );

    for ( int iRow = 0; iRow<nBubbles; ++iRow ) {
        const QModelIndex indexX = m_lines->model()->index(iRow, 0);
        const QModelIndex indexY = m_lines->model()->index(iRow, 1);
        DataValueAttributes dva( m_lines->dataValueAttributes( indexX ) );
        dva.setVisible( true );
        MarkerAttributes ma( dva.markerAttributes() );
        ma.setVisible( true );
        ma.setMarkerStyle( MarkerAttributes::MarkerCircle );

        // set the size
        const qreal d = m_model->data( indexY, ROLE_SIZE ).toDouble();
        ma.setMarkerSize( QSizeF(d,d) );

        // set the color
        ma.setMarkerColor( m_model->data( indexY, ROLE_COLOR ).toString() );

        // set the surrounding line's style
        Qt::PenStyle style = Qt::SolidLine;
        const QString sPen = m_model->data( indexY, ROLE_STYLE ).toString();
        if( sPen.compare("NoPen") == 0 )
            style = Qt::NoPen;
        else if( sPen.compare("SolidLine") == 0 )
            style = Qt::SolidLine;
        else if( sPen.compare("DashLine") == 0 )
            style = Qt::DashLine;
        else if( sPen.compare("DotLine") == 0 )
            style = Qt::DotLine;
        else if( sPen.compare("DashDotLine") == 0 )
            style = Qt::DashDotLine;
        else if( sPen.compare("DashDotDotLine") == 0 )
            style = Qt::DashDotDotLine;
        QPen pen( style );
        pen.setWidth(2);
        ma.setPen( pen );

        RelativePosition pos( dva.positivePosition() );
        pos.setAlignment( Qt::AlignCenter );
        pos.setHorizontalPadding(0);

        dva.setPositivePosition( pos );
        dva.setMarkerAttributes( ma );
        dva.setTextAttributes( ta );
        m_lines->setDataValueAttributes( indexX, dva );
        m_lines->setDataValueAttributes( indexY, dva );
    }
    m_chart->update();
}


void MainWindow::resizeEvent ( QResizeEvent * )
{
    adjustSizeOfDataArea();
    QTimer::singleShot(200, this, SLOT(adjustSizeOfDataArea()));
}


void MainWindow::adjustSizeOfDataArea()
{
    // find the transformation factors
    // currently used by the coordinate plane
    CartesianCoordinatePlane* plane =
            static_cast<CartesianCoordinatePlane*>(m_chart->coordinatePlane());
    const QPointF pt0 = plane->translate( QPointF(0.0, 0.0) );
    const QPointF pt1 = plane->translate( QPointF(1.0, 1.0) );
    const qreal divX = pt1.x() - pt0.x();
    const qreal divY = pt0.y() - pt1.y();

    qreal x1, x2, y1, y2;
    // find the four most-external bubble edges
    const int nFirstRow=0;
    const int nLastRow=nBubbles-1;

    for ( int iRow = nFirstRow;  iRow <= nLastRow;  ++iRow ) {
        const QModelIndex indexX = m_lines->model()->index(iRow, 0);
        const QModelIndex indexY = m_lines->model()->index(iRow, 1);
        const QSizeF siz( m_lines->dataValueAttributes( indexX ).markerAttributes().markerSize() );
        const qreal dX = siz.width()/2.0/divX;
        const qreal dY = siz.height()/2.0/divY;
        const qreal x( m_model->data( indexX, Qt::DisplayRole ).toDouble() );
        const qreal y( m_model->data( indexY, Qt::DisplayRole ).toDouble() );
        if( iRow == nFirstRow ){
            x1 = x - dX;
            x2 = x + dX;
            y1 = y - dY;
            y2 = y + dY;
        }else{
            x1 = qMin(x1, x - dX);
            x2 = qMax(x2, x + dX);
            y1 = qMin(y1, y - dY);
            y2 = qMax(y2, y + dY);
        }
        //qDebug() << "x:" << x << "y:" << y << "  dx:" << dX << "  dy:" << dY << "  x-range:" << x1 << x2 << "  y-range:" << y1 << y2;
    }

    // adjust the data area, so none of the bubbles will reach
    // out of its borders
    plane->setHorizontalRange( qMakePair(x1, x2) );
    plane->setVerticalRange(   qMakePair(y1, y2) );
    m_chart->update();
}

