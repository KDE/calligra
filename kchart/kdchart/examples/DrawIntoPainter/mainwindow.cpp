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
#include "framewidget.h"

#include <KDChartChart>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartLineDiagram>
#include <KDChartLineAttributes>
#include <KDChartTextAttributes>
#include <KDChartDataValueAttributes>
#include <KDChartThreeDLineAttributes>
#include <KDChartMarkerAttributes>
#include <KDChartFrameAttributes>
#include <KDChartBackgroundAttributes>
#include <KDChartLegend>

#include <QDebug>
#include <QPainter>

using namespace KDChart;

static QPixmap drawIntoPixmap( const QSize& size, KDChart::Chart* chart )
{
    QPixmap pix( size );
    pix.fill( Qt::white );
    QPainter painter( &pix );
    chart->paint( &painter, QRect( 0, 0, size.width(), size.height() ) );
    return pix;
}

// When set, this example uses FrameWidget which uses Chart::paint to paint itself.
// When not set, this example uses a Chart widget directly.
#define USE_FRAME_WIDGET 1


MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
#ifdef USE_FRAME_WIDGET
    FrameWidget* chartFrameWidget = new FrameWidget();
    chartLayout->addWidget( chartFrameWidget );
#endif
    hSBar->setVisible( false );
    vSBar->setVisible( false );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );

    CartesianAxis *xAxis = new CartesianAxis( m_lines );
    CartesianAxis *yAxis = new CartesianAxis ( m_lines );
    CartesianAxis *axisTop = new CartesianAxis ( m_lines );
    CartesianAxis *axisRight = new CartesianAxis ( m_lines );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    axisTop->setPosition( KDChart::CartesianAxis::Top );
    axisRight->setPosition( KDChart::CartesianAxis::Right );

    m_lines->addAxis( xAxis );
    m_lines->addAxis( yAxis );
    m_lines->addAxis( axisTop );
    m_lines->addAxis( axisRight );

    m_chart = new Chart();
    //m_chart->setGlobalLeading(10,10,10,10); // by default there is no leading

#ifdef USE_FRAME_WIDGET
    chartFrameWidget->setChart( m_chart );
    // make sure, we re-draw after changing one of the chart's properties
    connect( m_chart,          SIGNAL( propertiesChanged() ),
             chartFrameWidget, SLOT(   update() ) ) ;
#else
    chartLayout->addWidget( m_chart );
#endif

    m_chart->coordinatePlane()->replaceDiagram( m_lines );

    for ( int iColumn = 0; iColumn<m_lines->model()->columnCount(); ++iColumn ){
        QPen pen(m_lines->pen( iColumn ));
        pen.setWidth(4);
        m_lines->setPen( iColumn, pen );
    }

    FrameAttributes faChart( m_chart->frameAttributes() );
    faChart.setPen( QPen(QColor(0x60,0x60,0xb0), 8) );
    m_chart->setFrameAttributes( faChart );

    BackgroundAttributes baChart( m_chart->backgroundAttributes() );
    baChart.setVisible( true );
    baChart.setBrush( QColor(0xd0,0xd0,0xff) );
    m_chart->setBackgroundAttributes( baChart );

    // Set up the legend
    m_legend = new Legend( m_lines, m_chart );

    m_legend->setPosition( Position::South );
    m_legend->setAlignment( Qt::AlignRight );
    m_legend->setShowLines( false );
    m_legend->setTitleText( tr( "Legend" ) );
    m_legend->setOrientation( Qt::Horizontal );

    const QColor legendColor(0xff,0xe0,0x80);
    FrameAttributes faLegend( m_legend->frameAttributes() );
    faLegend.setPen( QPen(legendColor, 1) );
    m_legend->setFrameAttributes( faLegend );

    BackgroundAttributes baLegend( m_legend->backgroundAttributes() );
    baLegend.setVisible( true );
    baLegend.setBrush( legendColor );
    m_legend->setBackgroundAttributes( baLegend );

    m_chart->addLegend( m_legend );


    //m_chart->setGeometry(10,10,50,50);

    // for illustration we paint the same chart at different sizes:
    QSize size1 = QSize( 200, 200 );
    QSize size2 = QSize( 800, 800 );
    m_pix1 = drawIntoPixmap( size1, m_chart );
    m_pix2 = drawIntoPixmap( size2, m_chart );
    m_pix2 = m_pix2.scaled( size1 );

    m_smallChart1 = new QLabel( this );
    m_smallChart1->setWindowTitle( "200x200" );
    m_smallChart1->setPixmap( m_pix1 );
    m_smallChart1->setFixedSize( m_pix1.size() );
    m_smallChart1->move( width() - m_pix1.width()*2,
                         height()/2 - m_pix1.height()-5 );
    m_smallChart1->show();

    m_smallChart2 = new QLabel( this );
    m_smallChart2->setWindowTitle( "800x800 scaled down" );
    m_smallChart2->setPixmap( m_pix2 );
    m_smallChart2->setFixedSize( m_pix2.size() );
    m_smallChart2->move( width() - m_pix2.width()*2,
                         height()/2 + 5 );
    m_smallChart2->show();

    faChart.setPen( QPen(QColor(0xb0,0xb0,0xff), 8) );
    m_chart->setFrameAttributes( faChart );
}

void MainWindow::on_lineTypeCB_currentIndexChanged( const QString & text )
{
    if ( text == "Normal" )
        m_lines->setType( LineDiagram::Normal );
    else if ( text == "Stacked" )
        m_lines->setType( LineDiagram::Stacked );
    else if ( text == "Percent" )
        m_lines->setType( LineDiagram::Percent );
    else
        qWarning (" Does not match any type");
}


void MainWindow::on_paintLegendCB_toggled( bool checked )
{
    m_legend->setVisible( checked );
}

void MainWindow::on_paintValuesCB_toggled( bool checked )
{
    //testing
    const int colCount = m_lines->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        QBrush brush = qVariantValue<QBrush>( m_lines->model()->headerData( iColumn, Qt::Vertical, DatasetBrushRole ) );
        DataValueAttributes a = m_lines->dataValueAttributes( iColumn );
        if ( !paintMarkersCB->isChecked() ) {
            MarkerAttributes ma = a.markerAttributes();
            ma.setVisible( false );
            a.setMarkerAttributes( ma );
        }
        TextAttributes ta = a.textAttributes();
        ta.setRotation( 0 );
        ta.setFont( QFont( "Comic", 10 ) );
        ta .setPen( QPen( brush.color() ) );
        if ( checked )
            ta.setVisible( true );
        else
            ta.setVisible( false );

        a.setTextAttributes( ta );
        a.setVisible( true );
        m_lines->setDataValueAttributes( iColumn, a);
    }
}


void MainWindow::on_paintMarkersCB_toggled( bool checked )
{
   //testing
    DataValueAttributes a( m_lines->dataValueAttributes() );
    // dont paint the values
    if ( !paintValuesCB->isChecked() ) {
        TextAttributes ta = a.textAttributes();
        ta.setVisible( false );
        a.setTextAttributes( ta );
    }
    MarkerAttributes ma( a.markerAttributes() );
    MarkerAttributes::MarkerStylesMap map;
    map.insert( 0, MarkerAttributes::MarkerSquare );
    map.insert( 1, MarkerAttributes::MarkerCircle );
    map.insert( 2, MarkerAttributes::MarkerRing );
    map.insert( 3, MarkerAttributes::MarkerCross );
    map.insert( 4, MarkerAttributes::MarkerDiamond );
    ma.setMarkerStylesMap( map );

    switch ( markersStyleCB->currentIndex() ) {
        case 0:{
                   break; }
        case 1:{
                   ma.setMarkerStyle( MarkerAttributes::MarkerCircle );
                   break; }
        case 2:{
                   ma.setMarkerStyle( MarkerAttributes::MarkerSquare );
                   break; }
        case 3:{
                   ma.setMarkerStyle( MarkerAttributes::MarkerDiamond );
                   break; }
        case 4:{
                   ma.setMarkerStyle( MarkerAttributes::Marker1Pixel );
                   break; }
        case 5:{
                   ma.setMarkerStyle( MarkerAttributes::Marker4Pixels );
                   break; }
        case 6:{
                   ma.setMarkerStyle( MarkerAttributes::MarkerRing );
                   break; }
        case 7:{
                   ma.setMarkerStyle( MarkerAttributes::MarkerCross );
                   break; }
        case 8:{
                   ma.setMarkerStyle( MarkerAttributes::MarkerFastCross );
                   break; }
    }
    ma.setMarkerSize( QSize( markersWidthSB->value(), markersHeightSB->value() ) );

    if ( checked )
        ma.setVisible( true );
    else
        ma.setVisible( false );

    a.setMarkerAttributes( ma );
    a.setVisible( true );

    // make a special one for certain values
    DataValueAttributes yellowAttributes( a );
    MarkerAttributes yellowMarker( yellowAttributes.markerAttributes() );
    yellowMarker.setMarkerColor( Qt::yellow );
    yellowAttributes.setMarkerAttributes( yellowMarker );

    const int rowCount = m_lines->model()->rowCount();
    const int colCount = m_lines->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        DataValueAttributes colAttributes( a );
        if ( markersStyleCB->currentIndex() == 0 ) {
            MarkerAttributes ma( colAttributes.markerAttributes() );
            ma.setMarkerStyle( ma.markerStylesMap().value(iColumn) );
            colAttributes.setMarkerAttributes( ma );
        }
        for ( int j=0; j< rowCount; ++j ) {
            QModelIndex index = m_lines->model()->index( j, iColumn, QModelIndex() );
            QBrush brush = qVariantValue<QBrush>( m_lines->model()->headerData( iColumn, Qt::Vertical, DatasetBrushRole ) );
            double value = m_lines->model()->data( index ).toDouble();
            /* Set a specific color - marker for a specific value */
            if ( value == 8 ) {
                m_lines->setDataValueAttributes( index, yellowAttributes );
            }
        }
        m_lines->setDataValueAttributes( iColumn, colAttributes );
    }
}

void MainWindow::on_markersStyleCB_currentIndexChanged( const QString & text )
{
    Q_UNUSED( text );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
}


void MainWindow::on_markersWidthSB_valueChanged( int i )
{
    Q_UNUSED( i );
    markersHeightSB->setValue( markersWidthSB->value() );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
}

void MainWindow::on_markersHeightSB_valueChanged( int i )
{
    Q_UNUSED( i );
    markersWidthSB->setValue( markersHeightSB->value() );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
}


void MainWindow::on_displayAreasCB_toggled( bool checked )
{
    const int rowCount = m_lines->model()->rowCount();
    const int colCount = m_lines->model()->columnCount();
     for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
         for ( int j=0; j< rowCount; ++j ) {
             LineAttributes la( m_lines->lineAttributes( iColumn ) );
             if ( checked  ) {
                 la.setDisplayArea( true );
                 la.setTransparency( transparencySB->value() );
             } else
                 la.setDisplayArea( false );
             m_lines->setLineAttributes( iColumn,  la );
         }
     }
}

void MainWindow::on_transparencySB_valueChanged( int alpha )
{
    const int rowCount = m_lines->model()->rowCount();
    const int colCount = m_lines->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        for ( int j=0; j< rowCount; ++j ) {
            LineAttributes la = m_lines->lineAttributes( iColumn );
            la.setTransparency( alpha );
            m_lines->setLineAttributes( la );
        }
    }
    on_displayAreasCB_toggled( true );
}

void MainWindow::on_zoomFactorSB_valueChanged( double factor )
{
    if ( factor > 1 ) {
        hSBar->setVisible( true );
        vSBar->setVisible( true );
    } else {
        hSBar->setValue( 500 );
        vSBar->setValue( 500 );
        hSBar->setVisible( false );
        vSBar->setVisible( false );
    }
    m_chart->coordinatePlane()->setZoomFactorX( factor );
    m_chart->coordinatePlane()->setZoomFactorY( factor );
}

void MainWindow::on_hSBar_valueChanged( int hPos )
{
    m_chart->coordinatePlane()->setZoomCenter( QPointF(hPos/1000.0, vSBar->value()/1000.0) );
    m_chart->update();
}

void MainWindow::on_vSBar_valueChanged( int vPos )
{
    m_chart->coordinatePlane()->setZoomCenter( QPointF( hSBar->value()/1000.0, vPos/1000.0) );
}

void MainWindow::on_savePB_clicked()
{
    qDebug() << "Painting into PNG";
    QPixmap qpix(600,600);
    QPainter painter(&qpix);
    painter.setBrush(Qt::white);
    painter.fillRect( 0, 0, 600, 600, Qt::white);
    m_chart->paint(
            &painter,
            QRect(100, 100, 400, 400) );
    painter.end();
    qpix.save("kdchart-demo.png", "PNG");
    qDebug() << "Painting into PNG - done";
}

void MainWindow::resizeEvent ( QResizeEvent * )
{
    m_smallChart1->move( width() - m_pix1.width()*2,
                         height()/2 - m_pix1.height()-5 );
    m_smallChart2->move( width() - m_pix2.width()*2,
                         height()/2 + 5 );
}


