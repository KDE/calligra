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
#include <KDChartMarkerAttributes>


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
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );
    m_chart->coordinatePlane()->replaceDiagram( m_lines );
    m_chart->setGlobalLeading( 20, 20, 20, 20 );

    on_paintLinesCB_toggled( false );
    on_paintMarkersCB_toggled( true );
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
        ta.setFont( QFont( "Comic" ) );
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


void MainWindow::on_paintLinesCB_toggled(  bool checked )
{
    const int colCount = m_lines->model()->columnCount(m_lines->rootIndex());
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        DataValueAttributes a( m_lines->dataValueAttributes( iColumn ) );
        QBrush lineBrush( m_lines->brush( iColumn ) );
        if ( checked ) {
            QPen linePen( lineBrush.color() );
            m_lines->setPen(  iColumn,  linePen );
        }
        else
            m_lines->setPen( iColumn,  Qt::NoPen );
    }
        m_chart->update();
}


void MainWindow::on_paintMarkersCB_toggled( bool checked )
{
    // set up a map with different marker styles
    MarkerAttributes::MarkerStylesMap map;
    map.insert( 0, MarkerAttributes::MarkerSquare );
    map.insert( 1, MarkerAttributes::MarkerCircle );
    map.insert( 2, MarkerAttributes::MarkerRing );
    map.insert( 3, MarkerAttributes::MarkerCross );
    map.insert( 4, MarkerAttributes::MarkerDiamond );


    const int colCount = m_lines->model()->columnCount(m_lines->rootIndex());
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        DataValueAttributes dva( m_lines->dataValueAttributes( iColumn ) );
        QBrush lineBrush( m_lines->brush( iColumn ) );
        TextAttributes ta ( dva.textAttributes() );
        if (  paintValuesCB->isChecked() )
            ta.setVisible(  true );
        else
            ta.setVisible( false );
        MarkerAttributes ma( dva.markerAttributes() );
        ma.setMarkerStylesMap( map );
        ma.setMarkerSize( QSize( markersWidthSB->value(),
                                 markersHeightSB->value() ) );

        switch ( markersStyleCB->currentIndex() ) {
        case 0:
            break;
        case 1:
            ma.setMarkerStyle( MarkerAttributes::MarkerCircle );
            break;
        case 2:
            ma.setMarkerStyle( MarkerAttributes::MarkerSquare );
            break;
        case 3:
            ma.setMarkerStyle( MarkerAttributes::MarkerDiamond );
            break;
        case 4:
            ma.setMarkerStyle( MarkerAttributes::Marker1Pixel );
            break;
        case 5:
            ma.setMarkerStyle( MarkerAttributes::Marker4Pixels );
            break;
        case 6:
            ma.setMarkerStyle( MarkerAttributes::MarkerRing );
            break;
        case 7:
            ma.setMarkerStyle( MarkerAttributes::MarkerCross );
            break;
        case 8:
            ma.setMarkerStyle( MarkerAttributes::MarkerFastCross );
            break;
        }

        QPen markerPen( lineBrush.color() );
        ma.setPen( markerPen );
        ma.setVisible(  true );
        dva.setTextAttributes( ta );
        dva.setMarkerAttributes( ma );

        if (  checked )
            dva.setVisible( true );
        else
            dva.setVisible(  false );
        m_lines->setDataValueAttributes( iColumn, dva );
    }

    m_chart->update();
}


void MainWindow::on_markersStyleCB_currentIndexChanged( const QString & text )
{
    Q_UNUSED( text );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
}


void MainWindow::on_markersWidthSB_valueChanged( int i )
{
    Q_UNUSED( i );
    markersHeightSB->setValue( markersWidthSB->value() );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
}

void MainWindow::on_markersHeightSB_valueChanged( int /*i*/ )
{
    markersWidthSB->setValue( markersHeightSB->value() );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
}
