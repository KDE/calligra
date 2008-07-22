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

using namespace KDChart;

MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new Chart();
    m_chart->setGlobalLeading( 10,  10,  10,  10 );
    chartLayout->addWidget( m_chart );
    hSBar->setVisible( false );
    vSBar->setVisible( false );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );

    // create and position axis
    CartesianAxis *topAxis = new CartesianAxis( m_lines );
    CartesianAxis *leftAxis = new CartesianAxis ( m_lines );
    CartesianAxis *rightAxis = new CartesianAxis ( m_lines );
    CartesianAxis *bottomAxis = new CartesianAxis ( m_lines );
    topAxis->setPosition ( CartesianAxis::Top );
    leftAxis->setPosition ( CartesianAxis::Left );
    rightAxis->setPosition ( CartesianAxis::Right );
    bottomAxis->setPosition ( CartesianAxis::Bottom );

    // set axis titles
    topAxis->setTitleText ( "Abscissa color configured top position" );
    leftAxis->setTitleText ( "left Ordinate: fonts configured" );
    rightAxis->setTitleText ( "right Ordinate: default settings" );
    bottomAxis->setTitleText ( "Abscissa Bottom" );

    // configure titles text attributes
    TextAttributes taTop ( topAxis->titleTextAttributes () );
    taTop.setPen( QPen( Qt::red ) );
    topAxis->setTitleTextAttributes ( taTop );

    TextAttributes taLeft ( leftAxis->titleTextAttributes () );
    taLeft.setRotation( 180 );
    Measure me( taLeft.fontSize() );
    me.setValue( me.value() * 0.8 );
    taLeft.setFontSize( me );
// Set the following to 1, to hide the left axis title
//  - no matter if a title text is set or not
#if 0
    taLeft.setVisible( false );
#endif
    leftAxis->setTitleTextAttributes ( taLeft );

    TextAttributes taBottom ( bottomAxis->titleTextAttributes () );
    taBottom.setPen(  QPen( Qt::blue ) );
    bottomAxis->setTitleTextAttributes ( taBottom );

    // configure labels text attributes
    TextAttributes taLabels( topAxis->textAttributes() );
    taLabels.setPen(  QPen( Qt::darkGreen ) );
    taLabels.setRotation( 90 );
    topAxis->setTextAttributes( taLabels );
    leftAxis->setTextAttributes( taLabels );
    bottomAxis->setTextAttributes( taLabels );


// Set the following to 0, to see the default Abscissa labels
// (== X headers, as read from the data file)
#if 1
    // configure labels and their shortened versions
    QStringList daysOfWeek;
    daysOfWeek << "M O N D A Y" << "Tuesday" << "Wednesday"
               << "Thursday" << "Friday" ;
    topAxis->setLabels( daysOfWeek );

    QStringList shortDays;
    shortDays << "MON" << "Tue" << "Wed"
              << "Thu" << "Fri";
    topAxis->setShortLabels( shortDays );

    QStringList bottomLabels;
    bottomLabels << "Team A" << "Team B" << "Team C";
    bottomAxis->setLabels( bottomLabels );

    QStringList shortBottomLabels;
    shortBottomLabels << "A" << "B";
    bottomAxis->setShortLabels( shortBottomLabels );
#endif

    // add axis
    m_lines->addAxis( topAxis );
    m_lines->addAxis( leftAxis );
    m_lines->addAxis( rightAxis );
    m_lines->addAxis( bottomAxis );

    // assign diagram to chart view
    m_chart->coordinatePlane()->replaceDiagram( m_lines );
}
