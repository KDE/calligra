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
#include <KDChartGridAttributes>

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
    m_model2.loadFromCSV( ":/data2" );

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );

    m_lines2 = new LineDiagram();
    m_lines2->setModel( &m_model2 );

    // We call this "plane2" just for remembering, that we use it
    // in addition to the plane, that's built-in by default.
    plane2 = new CartesianCoordinatePlane( m_chart );

    plane2->setReferenceCoordinatePlane( m_chart->coordinatePlane() );

    CartesianAxis *xAxis = new CartesianAxis( m_lines );
    CartesianAxis *yAxis = new CartesianAxis ( m_lines );
    CartesianAxis *yAxis2 = new CartesianAxis ( m_lines2 );

    xAxis->setPosition ( KDChart::CartesianAxis::Top );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    yAxis2->setPosition ( KDChart::CartesianAxis::Left );

    m_lines->addAxis( yAxis );

    m_lines2->addAxis( xAxis );
    m_lines2->addAxis( yAxis2 );

    m_chart->coordinatePlane()->replaceDiagram( m_lines );
    plane2->replaceDiagram( m_lines2 );
    m_chart->addCoordinatePlane( plane2/*, 1*/);
    m_chart->setGlobalLeading( 20, 20, 20, 20 );
    init();
}

void MainWindow::init()
{
    on_displayGrid2CB_toggled( false );
}


void MainWindow::on_displayGrid1CB_toggled( bool checked )
{
    CartesianCoordinatePlane* plane = static_cast <CartesianCoordinatePlane*>
                                      ( m_chart->coordinatePlane() );

    // disable grids display for plane
    GridAttributes gv ( plane->gridAttributes( Qt::Vertical) );
    gv.setGridVisible( checked ? true : false );
    plane->setGridAttributes(Qt::Vertical, gv );
    plane->setGridAttributes(Qt::Horizontal, gv );
}

void MainWindow::on_displayGrid2CB_toggled( bool checked )
{

    // disable grids display for plane
    GridAttributes gv ( plane2->gridAttributes( Qt::Vertical) );
    gv.setGridVisible( checked ? true : false );
    plane2->setGridAttributes(Qt::Vertical, gv );
    plane2->setGridAttributes(Qt::Horizontal, gv );

}
