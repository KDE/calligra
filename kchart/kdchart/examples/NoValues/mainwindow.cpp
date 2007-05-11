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

