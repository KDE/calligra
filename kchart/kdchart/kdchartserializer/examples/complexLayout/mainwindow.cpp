/****************************************************************************
 ** Copyright (C) 2006 Klarälvdalens Datakonsult AB.  All rights reserved.
 **
 ** This file is part of the KD Chart library.
 **
 ** This file may be distributed and/or modified under the terms of the
 ** GNU General Public License version 2 as published by the Free Software
 ** Foundation and appearing in the file LICENSE.GPL included in the
 ** packaging of this file.
 **
 ** Licensees holding valid commercial KD Chart licenses may use this file in
 ** accordance with the KD Chart Commercial License Agreement provided with
 ** the Software.
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ** See http://www.kdab.net/kdchart for
 **   information about KDChart Commercial License Agreements.
 **
 ** Contact info@kdab.net if any conditions of this
 ** licensing are not clear to you.
 **
 **********************************************************************/

#include <QtGui>

#include "mainwindow.h"

#include <KDChartBarDiagram>
#include <KDChartChart>
#include <KDChartDataValueAttributes>
#include <KDChartFrameAttributes>
#include <KDChartHeaderFooter>
#include <KDChartLegend>
#include <KDChartLineDiagram>
#include <KDChartPieDiagram>
#include <KDChartPolarCoordinatePlane>

#include <KDChartSerializer>

#include <QStandardItemModel>

using namespace KDChart;


MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    m_chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    m_chartLayout->addWidget( m_chart );

    m_chart->setGlobalLeading( 20,  20,  20,  20 );


    initializeDataModels();


    // Set up the diagrams
    BarDiagram* bars = new BarDiagram();
    bars->setModel( m_model1 );

    LineDiagram* lines = new LineDiagram();
    lines->setModel( m_model1 );
    lines->setReferenceDiagram( bars );

    // increase the line width
    for ( int iRow = 0; iRow < lines->model()->rowCount(); ++iRow ) {
        QPen pen( lines->pen( iRow ) );
        pen.setWidth(2);
        pen.setColor( pen.color().dark(133) );
        lines->setPen( iRow, pen );
    }

    PieDiagram* pie = new PieDiagram();
    pie->setModel( m_model2 );

    // Assign some axes
    //CartesianAxis *xAxis  = new CartesianAxis(  lines );
    //CartesianAxis *yAxis  = new CartesianAxis ( lines );
    CartesianAxis *xAxis  = new CartesianAxis(  bars );
    CartesianAxis *yAxis  = new CartesianAxis ( bars );
    CartesianAxis *xAxis2 = new CartesianAxis(  lines );
    CartesianAxis *yAxis2 = new CartesianAxis ( lines );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    xAxis2->setPosition( KDChart::CartesianAxis::Top );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    yAxis2->setPosition( KDChart::CartesianAxis::Right );
    bars->addAxis(  xAxis );
    bars->addAxis(  yAxis );
    //lines->addAxis( xAxis );
    //lines->addAxis( yAxis );
    lines->addAxis( xAxis2 );
    lines->addAxis( yAxis2 );


    // Initially a default CartesianCoordinatePlane was created,
    // so we just store its pointer as plane1:
    CartesianCoordinatePlane* plane1 =
            static_cast<CartesianCoordinatePlane*>(m_chart->coordinatePlane());

    // Create an extra plane for the pie diagram:
    PolarCoordinatePlane* plane2 = new PolarCoordinatePlane( m_chart );
    m_chart->addCoordinatePlane( plane2/*, 1*/);

    //plane1->replaceDiagram( lines );
    plane1->replaceDiagram( bars );
    plane1->addDiagram(     lines );
    plane2->replaceDiagram( pie );

    // Change the orientation of the planes' layout
    QBoxLayout* planeLayout = dynamic_cast<QBoxLayout*>(m_chart->coordinatePlaneLayout());
    if( planeLayout )
        planeLayout->setDirection( QBoxLayout::LeftToRight );



    Legend* legend1 = new Legend( lines, m_chart );
    legend1->setPosition( Position::South );
    legend1->setAlignment( Qt::AlignLeft );
    legend1->setShowLines( false );
    legend1->setTitleText( tr( "Legend 1" ) );
    legend1->setText( 0, tr( "The red one" ) );
    legend1->setText( 1, tr( "green" ) );
    legend1->setText( 2, tr( "blue" ) );
    legend1->setText( 3, tr( "turquoise" ) );
    legend1->setOrientation( Qt::Horizontal );
    m_chart->addLegend( legend1 );

    Legend* legend2 = new Legend( pie, m_chart );
    legend2->setPosition( Position::East );
    legend2->setAlignment( Qt::AlignTop );
    legend2->setShowLines( false );
    legend2->setTitleText( tr( "L. 2" ) );
    legend2->setOrientation( Qt::Vertical );
    m_chart->addLegend( legend2 );

    KDChart::HeaderFooter* headerFooter = new KDChart::HeaderFooter( m_chart );
    m_chart->addHeaderFooter( headerFooter );
    headerFooter->setText( "Several coordinate planes / diagrams / legends" );
    KDChart::TextAttributes textAttrs( headerFooter->textAttributes() );
    textAttrs.setPen( QPen( Qt::red ) );
    headerFooter->setTextAttributes( textAttrs );
    headerFooter->setType( KDChart::HeaderFooter::Header );
    headerFooter->setPosition( KDChart::Position::North );

    // have some leading and a framing line around the pie diagram
    FrameAttributes fa = m_chart->coordinatePlanes().at(1)->frameAttributes();
    fa.setVisible(true);
    fa.setPen(QPen(Qt::black));
    fa.setPadding(8);
    m_chart->coordinatePlanes().at(1)->setFrameAttributes(fa);

    // assign some bg colors
    BackgroundAttributes ba = m_chart->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(255,255,200)));
    m_chart->setBackgroundAttributes(ba);

    ba = m_chart->coordinatePlane()->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(200,255,200)));
    m_chart->coordinatePlane()->setBackgroundAttributes(ba);

    ba = legend1->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(200,200,255)));
    legend1->setBackgroundAttributes(ba);
    legend2->setBackgroundAttributes(ba);
}


void MainWindow::initializeDataModels()
{
    // model 1:
    static const int nLines = 6;
    static const int nDatasets = 4;
    static const qreal linesData[nLines][nDatasets] = {
        {29.5  ,  30  ,  29.5  ,  29.7},
        {30    ,  29.5,  30.5  ,  29.8},
        {30.5  ,  31  ,  29.7  ,  30},
        {29.7  ,  29.3,  30    ,  31},
        {30.3  ,  31  ,  30    ,  29.6},
        {29.4  ,  30  ,  34    ,  29.5}
    };
    m_model1 = new QStandardItemModel( nLines, nDatasets, this );
    for( int iL=0;  iL < nLines;  ++iL )
        for( int iD=0;  iD < nDatasets;  ++iD )
            m_model1->setData( m_model1->index( iL, iD ), linesData[ iL ][ iD ] );

    // model 2:
    static const int nPieslices = 9;
    static const qreal pieData[nPieslices] = {
        1, 5, 3, 2, 4, 7, 3, 6, 5
    };
    m_model2 = new QStandardItemModel( 1, nPieslices, this );
    for( int i=0;  i < nPieslices;  ++i )
        m_model2->setData( m_model2->index( 0, i ), pieData[ i ] );
}


void MainWindow::on_pushButtonSave_clicked()
{
    saveAs();
}

void MainWindow::on_pushButtonClear_clicked()
{
    removeTheChart();
}

void MainWindow::on_pushButtonLoad_clicked()
{
    load();
}


void MainWindow::removeTheChart()
{
    // note: This does not delete the data model.
    if( m_chart ){
        m_chartLayout->removeWidget( m_chart );
        delete m_chart;
        m_chart = 0;
    }
}


void MainWindow::load()
{
    QString fileName =
            QFileDialog::getOpenFileName(this, tr("Open KD Chart 2 File"),
                                         QDir::currentPath(),
                                         tr("KDC2 Files (*.kdc2 *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("KD Chart Serializer"),
                             tr("Cannot read file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
        return;
    }

    // note: We do NOT set any default data-model for the serializer here
    //       because we assign the data-models by another way, see below.
    KDChart::Serializer serializer( 0, 0 );

    if( serializer.read( &file ) ){
        if( serializer.chart() &&
            serializer.chart()->coordinatePlane() &&
            serializer.chart()->coordinatePlane()->diagram() )
        {
            // Retrieve the chart read from file:
            KDChart::Chart* newChart = serializer.chart();

            // Remove the current chart and delete it:
            removeTheChart();

            CoordinatePlaneList planes( newChart->coordinatePlanes() );
            for( int iPlane=0; iPlane<planes.count(); ++iPlane){
                AbstractDiagramList diags( planes.at(iPlane)->diagrams() );
                for( int iDiag=0; iDiag<diags.count(); ++iDiag){
                    AbstractDiagram* diagram = diags.at( iDiag );
                    if( dynamic_cast<KDChart::BarDiagram*>( diagram ) ||
                        dynamic_cast<KDChart::LineDiagram*>( diagram ) )
                        diagram->setModel( m_model1 );
                    else
                        diagram->setModel( m_model2 );
                }
            }

            // From now on use the chart read from file:
            m_chart = newChart;
            m_chartLayout->addWidget( m_chart );

            m_chart->update();
        }else{
            QMessageBox::warning( this, tr("KD Chart Serializer"),
                                  tr("ERROR: Parsed chart in file %1 has no diagram.")
                                  .arg(fileName) );
        }
    }else{
        QMessageBox::warning( this, tr("KD Chart Serializer"),
                              tr("ERROR: Cannot read file %1.")
                              .arg(fileName) );
    }
    file.close();
}

void MainWindow::saveAs()
{
    QString fileName =
            QFileDialog::getSaveFileName(this, tr("Save KD Chart 2 File"),
                                         QDir::currentPath(),
                                         tr("KDC2 Files (*.kdc2 *.xml)"));
    if (fileName.isEmpty())
        return;

    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("KD Chart Serializer"),
                             tr("Cannot write file %1:\n%2.")
                                     .arg(fileName)
                                     .arg(file.errorString()));
        return;
    }

    KDChart::Serializer serializer( m_chart );
    if( serializer.write( &file ) )
        QMessageBox::information( this, tr("KD Chart Serializer"),
                                  tr("File saved") );
    else
        QMessageBox::warning( this, tr("KD Chart Serializer"),
                              tr("ERROR: Cannot write file %1.") );
    file.close();
}

