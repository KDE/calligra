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
#include <KDChartCartesianAxis>
#include <KDChartBarDiagram>
#include <KDChartLegend>


#include <QDebug>
#include <QPen>
#include <QHBoxLayout>
#include <QStandardItemModel>

using namespace KDChart;

MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    QHBoxLayout* chartLayout = new QHBoxLayout( this );
    m_chart = new Chart();
    m_chart->setGlobalLeading( 5, 5, 5, 5 );
    chartLayout->addWidget( m_chart );

    m_model = new QStandardItemModel(0, 0, this); // model contains no data at all

    // Set up the diagram
    m_bars = new BarDiagram();
    m_bars->setModel( m_model );
    CartesianAxis *xAxis = new CartesianAxis( m_bars );
    CartesianAxis *yAxis = new CartesianAxis ( m_bars );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    xAxis->setTitleText ( "Abscissa axis at the bottom" );
    yAxis->setTitleText ( "Ordinate axis at the left side" );
    m_bars->addAxis( xAxis );
    m_bars->addAxis( yAxis );

    m_chart->coordinatePlane()->replaceDiagram( m_bars );

    Legend* legend = new Legend( m_bars, m_chart );
    legend->setPosition( Position::South );
    legend->setAlignment( Qt::AlignCenter );
    legend->setShowLines( true );
    legend->setTitleText("This is the legend - showing no data either");
    legend->setOrientation( Qt::Horizontal );
    legend->addDiagram( m_bars );
    m_chart->addLegend( legend );
}

