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
#include "AdjustedCartesianAxis.h"
#include "AdjustedCartesianAxisSerializer.h"

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


MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    m_chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new KDChart::Chart();
    m_chartLayout->addWidget( m_chart );

    // add a small left-side leading because we use a coloured Y axis background
    m_chart->setGlobalLeadingLeft( 1 );


    initializeDataModel();


    // register our own serializer for saving / loading the Y axis:
    KDChart::registerElementSerializer<
            AdjustedCartesianAxisSerializer,
            AdjustedCartesianAxis >( 0 ); // will be un-registred in ~Mainwindow()


    // Set up the diagrams
    KDChart::LineDiagram* lines = new KDChart::LineDiagram();
    lines->setModel( m_model );

    // increase the line width
    for ( int iRow = 0; iRow < lines->model()->rowCount(); ++iRow ) {
        QPen pen( lines->pen( iRow ) );
        pen.setWidth(2);
        pen.setColor( pen.color().dark(133) );
        lines->setPen( iRow, pen );
    }

    // Assign some axes
    KDChart::CartesianAxis *xAxis = new KDChart::CartesianAxis( lines );
    AdjustedCartesianAxis *yAxis = new AdjustedCartesianAxis( lines );
    yAxis->setBounds(29.9, 31.0);
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    yAxis->setTitleText ( "an axis class with customized labels" );

    lines->addAxis( xAxis );
    lines->addAxis( yAxis );


    KDChart::CartesianCoordinatePlane* plane =
            static_cast< KDChart::CartesianCoordinatePlane * >(m_chart->coordinatePlane());

    plane->replaceDiagram( lines );

    KDChart::HeaderFooter* headerFooter = new KDChart::HeaderFooter( m_chart );
    m_chart->addHeaderFooter( headerFooter );
    headerFooter->setText( "Line diagram using a custom axis class" );

    KDChart::TextAttributes textAttrs( headerFooter->textAttributes() );
    textAttrs.setPen( QPen( Qt::red ) );
    headerFooter->setTextAttributes( textAttrs );
    headerFooter->setType( KDChart::HeaderFooter::Header );
    headerFooter->setPosition( KDChart::Position::North );

    // assign some bg colors
    KDChart::BackgroundAttributes ba = yAxis->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(255,255,200)));
    yAxis->setBackgroundAttributes(ba);
}


MainWindow::~MainWindow()
{
    // un-register our own serializer for saving / loading the Y axis:
    KDChart::unregisterElementSerializer<
            AdjustedCartesianAxisSerializer,
            AdjustedCartesianAxis >();
}

void MainWindow::initializeDataModel()
{
    static const int nLines = 6;
    static const int nDatasets = 4;
    static const qreal linesData[nLines][nDatasets] = {
        {29.5  ,  30  ,  29.5  ,  29.7},
        {30    ,  29.4,  30.5  ,  29.8},
        {30.5  ,  31.6,  29.7  ,  30},
        {29.7  ,  28.2,  30    ,  31},
        {30.3  ,  31  ,  30    ,  29.6},
        {29.4  ,  30  ,  34    ,  29.5}
    };
    m_model = new QStandardItemModel( nLines, nDatasets, this );
    for( int iL=0;  iL < nLines;  ++iL )
        for( int iD=0;  iD < nDatasets;  ++iD )
            m_model->setData( m_model->index( iL, iD ), linesData[ iL ][ iD ] );
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

            KDChart::CoordinatePlaneList planes( newChart->coordinatePlanes() );
            for( int iPlane=0; iPlane<planes.count(); ++iPlane){
                KDChart::AbstractDiagramList diags( planes.at(iPlane)->diagrams() );
                for( int iDiag=0; iDiag<diags.count(); ++iDiag){
                    KDChart::AbstractDiagram* diagram = diags.at( iDiag );
                    if( dynamic_cast<KDChart::BarDiagram*>( diagram ) ||
                        dynamic_cast<KDChart::LineDiagram*>( diagram ) )
                        diagram->setModel( m_model );
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

