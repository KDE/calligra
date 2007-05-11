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

#include <KDChartChart>
#include <KDChartLineDiagram>
#include <KDChartTextAttributes>
#include <KDChartDataValueAttributes>
#include <KDChartThreeDLineAttributes>
#include <KDChartHeaderFooter>
#include <KDChartLegend>

#include <KDChartSerializer>

#include <QStandardItemModel>

using namespace KDChart;


MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    m_curColumn = -1;
    m_curOpacity = 0;

    m_chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    m_chartLayout->addWidget( m_chart );

    initializeDataModel();

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( m_model );

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

    KDChart::HeaderFooter* headerFooter = new KDChart::HeaderFooter( m_chart );
    m_chart->addHeaderFooter( headerFooter );
    headerFooter->setText( "This is a centered header in red." );
    KDChart::TextAttributes textAttrs( headerFooter->textAttributes() );
    textAttrs.setPen( QPen( Qt::red ) );
    headerFooter->setTextAttributes( textAttrs );
    headerFooter->setType( KDChart::HeaderFooter::Header );
    headerFooter->setPosition( KDChart::Position::North );

    headerFooter = new KDChart::HeaderFooter( m_chart );
    m_chart->addHeaderFooter( headerFooter );
    headerFooter->setText( "... and a blue right-sided footer." );
    textAttrs = headerFooter->textAttributes();
    textAttrs.setPen( QPen( Qt::blue ) );
    headerFooter->setTextAttributes( textAttrs );
    headerFooter->setType( KDChart::HeaderFooter::Footer );
    headerFooter->setPosition( KDChart::Position::SouthEast );


    Legend* legend = new Legend( m_lines, m_chart );
    legend->setPosition( Position::South );
    legend->setAlignment( Qt::AlignCenter );
    legend->setShowLines( false );
    legend->setTitleText( tr( "The Legend" ) );
    legend->setText( 0, tr( "The red one" ) );
    legend->setText( 1, tr( "green" ) );
    legend->setText( 2, tr( "blue" ) );
    legend->setText( 3, tr( "turquoise" ) );

    legend->setOrientation( Qt::Horizontal );
    m_chart->addLegend( legend );

    m_chart->coordinatePlane()->replaceDiagram( m_lines );
    m_chart->setGlobalLeading( 20,  20,  20,  20 );

    // assign some bg colors
    BackgroundAttributes ba = m_chart->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(255,255,200)));
    m_chart->setBackgroundAttributes(ba);

    ba = m_chart->coordinatePlane()->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(200,255,200)));
    m_chart->coordinatePlane()->setBackgroundAttributes(ba);

    ba = legend->backgroundAttributes();
    ba.setVisible(true);
    ba.setBrush(QBrush(QColor(200,200,255)));
    legend->setBackgroundAttributes(ba);
}


void MainWindow::initializeDataModel()
{
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
    m_model = new QStandardItemModel( nLines, nDatasets, this );
    for( int iL=0;  iL < nLines;  ++iL )
        for( int iD=0;  iD < nDatasets;  ++iD )
            m_model->setData( m_model->index( iL, iD ), linesData[ iL ][ iD ] );
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

    m_chart->update();
}

void MainWindow::on_paintValuesCB_toggled( bool checked )
{
    const int colCount = m_lines->model()->columnCount(m_lines->rootIndex());
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        DataValueAttributes a( m_lines->dataValueAttributes( iColumn ) );
        QBrush brush( m_lines->brush( iColumn ) );
        TextAttributes ta( a.textAttributes() );
        ta.setRotation( 0 );
        ta.setFont( QFont( "Comic", 10 ) );
        ta.setPen( QPen( brush.color() ) );

        if ( checked )
            ta.setVisible( true );
        else
            ta.setVisible( false );
        a.setVisible( true );
        a.setTextAttributes( ta );

        m_lines->setDataValueAttributes( iColumn, a );
    }
    m_chart->update();
}

void MainWindow::setHighlightArea( int row, int column, int opacity, bool checked, bool doUpdate )
{
    if( row < 0 ){
        // highlight a complete dataset
        LineAttributes la = m_lines->lineAttributes( column );
        if ( checked ) {
            la.setDisplayArea( true );
            la.setTransparency( opacity );
        }  else {
            la.setDisplayArea( false );
        }
        m_lines->setLineAttributes( column, la );
    }else{
        // highlight two segments only
        if( row ){
            QModelIndex cellIndex( m_lines->model()->index( row-1, column, m_lines->rootIndex() ) );
            if ( checked ) {
                LineAttributes la( m_lines->lineAttributes( cellIndex ) );
                la.setDisplayArea( true );
                la.setTransparency( 255-opacity );
                // set specific line attribute settings for this cell
                m_lines->setLineAttributes( cellIndex, la );
            }  else {
                // remove any cell-specific line attribute settings from the indexed cell
                m_lines->resetLineAttributes( cellIndex );
            }
        }
        if( row < m_lines->model()->rowCount(m_lines->rootIndex()) ){
            QModelIndex cellIndex( m_lines->model()->index( row, column, m_lines->rootIndex() ) );
            if ( checked ) {
                LineAttributes la( m_lines->lineAttributes( cellIndex ) );
                la.setDisplayArea( true );
                la.setTransparency( opacity );
                // set specific line attribute settings for this cell
                m_lines->setLineAttributes( cellIndex, la );
            }  else {
                // remove any cell-specific line attribute settings from the indexed cell
                m_lines->resetLineAttributes( cellIndex );
            }
        }
    }
    if( doUpdate )
        m_chart->update();
}

void MainWindow::on_highlightAreaCB_toggled( bool checked )
{
    setHighlightArea( -1, highlightAreaSB->value(), 127, checked, true );
}

void MainWindow::on_highlightAreaSB_valueChanged( int i )
{
    Q_UNUSED( i );
    if ( highlightAreaCB->isChecked() )
        on_highlightAreaCB_toggled( true );
    else
        on_highlightAreaCB_toggled( false);
}

void MainWindow::on_threeDModeCB_toggled( bool checked )
{
    ThreeDLineAttributes td( m_lines->threeDLineAttributes() );
    td.setDepth( depthSB->value() );
    if ( checked )
        td.setEnabled(  true );
    else
        td.setEnabled(  false );

    m_lines->setThreeDLineAttributes( td );

    m_chart->update();
}

void MainWindow::on_depthSB_valueChanged( int i )
{
    Q_UNUSED( i );
    if ( threeDModeCB->isChecked() )
        on_threeDModeCB_toggled( true );
}

void MainWindow::on_pushButtonLoad_clicked()
{
    load();
}

void MainWindow::on_pushButtonSave_clicked()
{
    saveAs();
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

    KDChart::Serializer serializer( 0, m_model );
    if( serializer.read( &file ) ){
        if( serializer.chart() &&
            serializer.chart()->coordinatePlane() &&
            serializer.chart()->coordinatePlane()->diagram() )
        {
            // Retrieve the chart read from file:
            KDChart::Chart* newChart = serializer.chart();

            // Retrieve the diagram read from file:
            // We assume the file was created by this program, so it is
            // supposed to have just one diagram and that must be a LineDiagram.
            KDChart::LineDiagram* newDiagram =
                    dynamic_cast<KDChart::LineDiagram*>(newChart->coordinatePlane()->diagram());

            if( ! newDiagram ){
                QMessageBox::warning(
                        this,
                        tr("KD Chart Serializer"),
                        tr("The saved XLM file either was not created by this program,<br>"
                           "or it has been modified manually in a wrong way.<br>"
                           "Problem: First diagram in file %1 is not a LineDiagram.").arg(fileName) );
                delete newChart;
            }else{
                // Remove the current chart and delete it:
                m_chartLayout->removeWidget( m_chart );
                delete m_chart;

                // From now on use the chart read from file:
                m_chart = newChart;
                m_chartLayout->addWidget( m_chart );

                // Adjust our auxiliary diagram-ptr:
                m_lines = newDiagram;

                // We do NOT need to tell the diagram to use our data model,
                // because we have specified the model when instantiating Serializer
                // so it got used transparently when the diagram was instantiated.
                //
                // m_lines->setModel( &m_model );

                m_chart->update();
            }
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
                              tr("ERROR: Cannot write file %1.").arg( fileName ) );
    file.close();
}

