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
#include <KDChartAbstractCoordinatePlane>
#include <KDChartLineDiagram>
#include <KDChartLegend>

#include <QDebug>
#include <QPainter>

using namespace KDChart;

MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    m_chart->setGlobalLeadingTop( 10 );
    m_chart->setGlobalLeadingRight( 10 );
    chartLayout->addWidget( m_chart );
    hSBar->setVisible( false );
    vSBar->setVisible( false );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );

    CartesianAxis *xAxis = new CartesianAxis( m_lines );
    CartesianAxis *yAxis = new CartesianAxis ( m_lines );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );

    xAxis->setTitleText ( "Abscissa axis at the bottom" );
    yAxis->setTitleText ( "Ordinate axis at the left side" );

    m_lines->addAxis( xAxis );
    m_lines->addAxis( yAxis );
    m_chart->coordinatePlane()->replaceDiagram( m_lines );

    // Set up the legend
    m_legend = new Legend( m_lines, m_chart );
    m_chart->addLegend( m_legend );
    m_legend->setPosition( KDChartEnums::PositionEast );
    m_legend->setAlignment( Qt::AlignTop );
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
    m_chart->update();
}

void MainWindow::on_adjustGridCB_toggled( bool checked )
{
    static_cast <CartesianCoordinatePlane*>( m_chart->coordinatePlane() )
            ->setAutoAdjustGridToZoom( checked );
    m_chart->update();
}

void MainWindow::on_hSBar_valueChanged( int hPos )
{
    m_chart->coordinatePlane()->setZoomCenter( QPointF(hPos/1000.0, vSBar->value()/1000.0) );
    m_chart->update();
}

void MainWindow::on_vSBar_valueChanged( int vPos )
{
    m_chart->coordinatePlane()->setZoomCenter( QPointF( hSBar->value()/1000.0, vPos/1000.0) );
    m_chart->update();
}

