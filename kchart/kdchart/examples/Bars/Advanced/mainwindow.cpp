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

#include "mainwindow.h"

#include <KDChartChart>
#include <KDChartDatasetProxyModel>
#include <KDChartAbstractCoordinatePlane>
#include <KDChartBarDiagram>
#include <KDChartTextAttributes>
#include <KDChartDataValueAttributes>
#include <KDChartThreeDBarAttributes>


#include <QDebug>
#include <QPainter>

using namespace KDChart;

MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    chartLayout->addWidget( m_chart );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_bars = new BarDiagram();
    m_bars->setModel( &m_model );

    QPen pen(  m_bars->pen() );
    pen.setColor( Qt::darkGray );
    pen.setWidth( 1 );
    m_bars->setPen( pen );
    m_chart->coordinatePlane()->replaceDiagram( m_bars );
    m_chart->setGlobalLeadingTop( 20 );
}



void MainWindow::on_barTypeCB_currentIndexChanged( const QString & text )
{
    if ( text == "Normal" )
        m_bars->setType( BarDiagram::Normal );
    else if ( text == "Stacked" )
        m_bars->setType( BarDiagram::Stacked );
    else if ( text == "Percent" )
        m_bars->setType( BarDiagram::Percent );
    else
        qWarning (" Does not match any type");

    m_chart->update();
}


void MainWindow::on_paintValuesCB_toggled( bool checked )
{
    Q_UNUSED( checked );
    // We set the DataValueAttributes on a per-column basis here,
    // because we want the texts to be printed in different
    // colours - according to their respective dataset's colour.
    const QFont font(QFont( "Comic", 10 ));
    const int colCount = m_bars->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        QBrush brush( m_bars->brush( iColumn ) );
        DataValueAttributes a( m_bars->dataValueAttributes( iColumn ) );
        TextAttributes ta( a.textAttributes() );
        ta.setRotation( 0 );
        ta.setFont( font );
        ta .setPen( QPen( brush.color() ) );
        if ( checked )
            ta.setVisible( true );
        else
            ta.setVisible( false );

        a.setTextAttributes( ta );
        a.setVisible( true );
        m_bars->setDataValueAttributes( iColumn, a);
    }

    m_chart->update();
}


void MainWindow::on_paintThreeDBarsCB_toggled( bool checked )
{
    ThreeDBarAttributes td( m_bars->threeDBarAttributes() );
    double defaultDepth = td.depth();
    if ( checked ) {
        td.setEnabled( true );
        if ( threeDDepthCB->isChecked() )
            td.setDepth( depthSB->value() );
        else
            td.setDepth( defaultDepth );
    } else {
        td.setEnabled( false );
    }
    m_bars->setThreeDBarAttributes( td );
    m_chart->update();
}

void MainWindow::on_markColumnCB_toggled( bool checked )
{
    const int column = markColumnSB->value();
    QPen pen( m_bars->pen( column ) );
    if ( checked ) {
        pen.setColor( Qt::yellow );
        pen.setStyle( Qt::DashLine );
        pen.setWidth( 3 );
        m_bars->setPen( column, pen );
    }  else {
        pen.setColor( Qt::darkGray );
        pen.setStyle( Qt::SolidLine );
        pen.setWidth( 1 );
        m_bars->setPen( column, pen );
    }
    m_chart->update();
}

void MainWindow::on_depthSB_valueChanged( int i )
{
     Q_UNUSED( i );

    if ( threeDDepthCB->isChecked() && paintThreeDBarsCB->isChecked() )
        on_paintThreeDBarsCB_toggled( true );
}

void MainWindow::on_threeDDepthCB_toggled( bool checked )
{
     Q_UNUSED( checked );

    if ( paintThreeDBarsCB->isChecked() )
        on_paintThreeDBarsCB_toggled( true );
}

void MainWindow::on_markColumnSB_valueChanged( int i )
{
    QPen pen( m_bars->pen( i ) );
    markColumnCB->setChecked( pen.color() == Qt::yellow );
}

void MainWindow::on_widthSB_valueChanged( int value )
{
    if (  widthCB->isChecked() ) {
        BarAttributes ba( m_bars->barAttributes() );
        ba.setFixedBarWidth( value );
        ba.setUseFixedBarWidth( true );
        m_bars->setBarAttributes( ba  );
    }
    m_chart->update();
}

void MainWindow::on_widthCB_toggled( bool checked )
{
    if (  checked ){
        on_widthSB_valueChanged( widthSB->value() );
    }else{
        BarAttributes ba( m_bars->barAttributes() );
        ba.setUseFixedBarWidth( false );
        m_bars->setBarAttributes( ba  );
        m_chart->update();
    }
}
