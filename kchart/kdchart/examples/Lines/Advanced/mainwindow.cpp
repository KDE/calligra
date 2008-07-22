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
#include <KDChartTextAttributes>
#include <KDChartDataValueAttributes>
#include <KDChartThreeDLineAttributes>


#include <QTimer>

using namespace KDChart;

MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    m_curColumn = -1;
    m_curOpacity = 0;

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    chartLayout->addWidget( m_chart );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );

    CartesianAxis *xAxis = new CartesianAxis( m_lines );
    CartesianAxis *yAxis = new CartesianAxis ( m_lines );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    m_lines->addAxis( xAxis );
    m_lines->addAxis( yAxis );

    m_chart->coordinatePlane()->replaceDiagram( m_lines );
    m_chart->setGlobalLeading( 20,  20,  20,  20 );
    // Instantiate the timer
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(slot_timerFired()));
    timer->start(30);
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

void MainWindow::on_animateAreasCB_toggled( bool checked )
{
    if( checked ){
        highlightAreaCB->setCheckState( Qt::Unchecked );
        m_curRow = 0;
        m_curColumn = 0;
    }else{
        m_curColumn = -1;
    }
    highlightAreaCB->setEnabled( ! checked );
    highlightAreaSB->setEnabled( ! checked );
    // un-highlight all previously highlighted columns
    const int rowCount = m_lines->model()->rowCount();
    const int colCount = m_lines->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ){
        setHighlightArea( -1, iColumn, 127, false, false );
        for ( int iRow = 0; iRow<rowCount; ++iRow )
        //    m_lines->resetLineAttributes( cellIndex );
            setHighlightArea( iRow, iColumn, 127, false, false );
    }
    m_chart->update();
    m_curOpacity = 0;
}

void MainWindow::slot_timerFired()
{
    if( m_curColumn < 0 ) return;
    m_curOpacity += 8;
    if( m_curOpacity > 255 ){
        setHighlightArea( m_curRow, m_curColumn, 127, false, false );
        m_curOpacity = 5;
        ++m_curRow;
        if( m_curRow >= m_lines->model()->rowCount(m_lines->rootIndex()) ){
            m_curRow = 0;
            ++m_curColumn;
            if( m_curColumn >= m_lines->model()->columnCount(m_lines->rootIndex()) )
                m_curColumn = 0;
        }
    }
    setHighlightArea( m_curRow, m_curColumn, m_curOpacity, true, true );
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
