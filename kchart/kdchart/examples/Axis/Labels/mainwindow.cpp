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
#include "AdjustedCartesianAxis.h"

#include <KDChartAbstractCoordinatePlane>
#include <KDChartBackgroundAttributes>
#include <KDChartChart>
#include <KDChartDataValueAttributes>
#include <KDChartHeaderFooter>
#include <KDChartLegend>
#include <KDChartLineDiagram>
#include <KDChartLineAttributes>
#include <KDChartTextAttributes>
#include <KDChartThreeDLineAttributes>
#include <KDChartMarkerAttributes>

#include <QDebug>
#include <QPainter>
#include <QLinearGradient>


MainWindow::MainWindow( QWidget* parent ) :
    QWidget( parent )
{
    setupUi( this );

    QHBoxLayout* chartLayout = new QHBoxLayout( chartFrame );
    m_chart = new KDChart::Chart();
    chartLayout->addWidget( m_chart );
    hSBar->setVisible( false );
    vSBar->setVisible( false );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new KDChart::LineDiagram();
    m_lines->setModel( &m_model );

    KDChart::CartesianAxis *xAxis = new KDChart::CartesianAxis( m_lines );
    AdjustedCartesianAxis *yAxis = new AdjustedCartesianAxis( m_lines );
    yAxis->setBounds(3, 6);
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    xAxis->setTitleText ( "a default Abscissa axis at the bottom" );
    yAxis->setTitleText ( "a custom Ordinate axis at the left side" );

    // add a header
    KDChart::HeaderFooter* headerFooter = new KDChart::HeaderFooter( m_chart );
    headerFooter->setType( KDChart::HeaderFooter::Header );
    headerFooter->setPosition( KDChart::Position::North );
    headerFooter->setText( "Line diagram using a custom axis class" );
    m_chart->addHeaderFooter( headerFooter );
    // adjust the text colour of the header
    KDChart::TextAttributes textAttrs( headerFooter->textAttributes() );
    textAttrs.setPen( QPen( Qt::red ) );
    headerFooter->setTextAttributes( textAttrs );

    // add a footer
    headerFooter = new KDChart::HeaderFooter( m_chart );
    headerFooter->setType( KDChart::HeaderFooter::Footer );
    headerFooter->setPosition( KDChart::Position::West );
    headerFooter->setText( "see also kdchartserializer/examples/customAxisClass/" );
    m_chart->addHeaderFooter( headerFooter );
    // adjust the text attributes: declare a tiny fixed font size and white color
    textAttrs = headerFooter->textAttributes();
    KDChart::Measure me( 7.0, KDChartEnums::MeasureCalculationModeAbsolute );
    textAttrs.setFontSize( me );
    textAttrs.setMinimalFontSize( me );
    textAttrs.setPen( QPen( Qt::white ) );
    headerFooter->setTextAttributes( textAttrs );
    // adjust ther background colour
    KDChart::BackgroundAttributes ba = headerFooter->backgroundAttributes();
    ba.setVisible( true );
    ba.setBrush( QBrush( QColor(0x60,0x60,0x60) ) );
    headerFooter->setBackgroundAttributes( ba );


// set the following to 0, to have only one of the axes with background
#if 1
    // colourize the axes' backgrounds
    ba = yAxis->backgroundAttributes();
    ba.setVisible( true );
    ba.setBrush( QBrush( QColor(0xff,0xff,0xc0) ) );
    yAxis->setBackgroundAttributes( ba );
    xAxis->setBackgroundAttributes( ba );
#else
    // colourize the Ordinate axis' background
    QLinearGradient linearGrad(QPointF(0, 100), QPointF(0, 400));
    linearGrad.setColorAt(0.0, QColor(0xff,0xff,0xc0));
    linearGrad.setColorAt(1.0, QColor(0xa0,0xc0,0xff));
    ba = yAxis->backgroundAttributes();
    ba.setVisible( true );
    ba.setBrush( linearGrad );
    yAxis->setBackgroundAttributes( ba );
#endif

    // add 1 pixel space at the left and at the top edge, because the
    // axis area would otherwise overwrite the left/top borders
    m_chart->setGlobalLeading(1,1,0,0);

// set the following to 0, to see the default Abscissa labels (== X headers, as read from the data file)
#if 1
    QStringList daysOfWeek;
    daysOfWeek << "Monday" << "Tuesday" << "Wednesday" << "Thursday" << "Friday" << "Saturday" << "Sunday";
    xAxis->setLabels( daysOfWeek );
    QStringList shortDays;
    shortDays << "Mon" << "Tue" << "Wed" << "Thu" << "Fri" << "Sat" << "Sun";
    xAxis->setShortLabels( shortDays );
#endif

    m_lines->addAxis( xAxis );
    m_lines->addAxis( yAxis );

    m_chart->coordinatePlane()->replaceDiagram( m_lines );
    // Set up the legend
    m_legend = new KDChart::Legend( m_lines, m_chart );
    m_chart->addLegend( m_legend );
    m_legend->hide();
}
