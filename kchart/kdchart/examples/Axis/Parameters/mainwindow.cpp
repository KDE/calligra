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
#include <KDChartLineAttributes>
#include <KDChartTextAttributes>
#include <KDChartDataValueAttributes>
#include <KDChartThreeDLineAttributes>
#include <KDChartMarkerAttributes>
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
    m_chart->setGlobalLeading( 20,  20,  20,  20 );
    chartLayout->addWidget( m_chart );
    hSBar->setVisible( false );
    vSBar->setVisible( false );

    m_model.loadFromCSV( ":/data" );

    // Set up the diagram
    m_lines = new LineDiagram();
    m_lines->setModel( &m_model );
    //CartesianAxisList List = m_lines->axesList();
    CartesianAxis *xAxis = new CartesianAxis( m_lines );
    CartesianAxis *yAxis = new CartesianAxis ( m_lines );
    CartesianAxis *axisTop = new CartesianAxis ( m_lines );
    CartesianAxis *axisRight = new CartesianAxis ( m_lines );
    xAxis->setPosition ( KDChart::CartesianAxis::Bottom );
    yAxis->setPosition ( KDChart::CartesianAxis::Left );
    axisTop->setPosition( KDChart::CartesianAxis::Top );
    axisRight->setPosition( KDChart::CartesianAxis::Left);//Right );

    xAxis->setTitleText ( "Abscissa axis at the bottom" );
    yAxis->setTitleText ( "Ordinate axis at the left side" );
    axisTop->setTitleText ( "Abscissa axis at the top" );
    axisRight->setTitleText ( "Ordinate axis at the right side" );
    TextAttributes taTop ( xAxis->titleTextAttributes () );
    taTop.setPen( QPen( Qt::red ) );
    axisTop->setTitleTextAttributes ( taTop );
    TextAttributes taRight ( xAxis->titleTextAttributes () );
    Measure me( taRight.fontSize() );
    me.setValue( me.value() * 1.5 );
    taRight.setFontSize( me );
    axisRight->setTitleTextAttributes ( taRight );

    m_lines->addAxis( xAxis );
    m_lines->addAxis( yAxis );
    m_lines->addAxis( axisTop );
    m_lines->addAxis( axisRight );
    m_chart->coordinatePlane()->replaceDiagram( m_lines );

    // Set up the legend
    m_legend = new Legend( m_lines, m_chart );
    m_chart->addLegend( m_legend );
    m_legend->hide();
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


void MainWindow::on_paintLegendCB_toggled( bool checked )
{
    m_legend->setVisible( checked );
    m_chart->update();
}

void MainWindow::on_paintValuesCB_toggled( bool checked )
{
    // We set the DataValueAttributes on a per-column basis here,
    // because we want the texts to be printed in different
    // colours - according to their respective dataset's colour.
    const QFont font(QFont( "Comic", 10 ));
    const int colCount = m_lines->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        QBrush brush( m_lines->brush( iColumn ) );
        DataValueAttributes a( m_lines->dataValueAttributes( iColumn ) );
        if ( ! paintMarkersCB->isChecked() ) {
            MarkerAttributes ma( a.markerAttributes() );
            ma.setVisible( false );
            a.setMarkerAttributes( ma );
        }
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
        m_lines->setDataValueAttributes( iColumn, a);
    }
    m_chart->update();
}


void MainWindow::on_paintMarkersCB_toggled( bool checked )
{
    // build a map with different markert types
    MarkerAttributes::MarkerStylesMap map;
    map.insert( 0, MarkerAttributes::MarkerSquare );
    map.insert( 1, MarkerAttributes::MarkerCircle );
    map.insert( 2, MarkerAttributes::MarkerRing );
    map.insert( 3, MarkerAttributes::MarkerCross );
    map.insert( 4, MarkerAttributes::MarkerDiamond );

    DataValueAttributes attrs( m_lines->dataValueAttributes()  );
    TextAttributes ta( attrs.textAttributes() );
    MarkerAttributes ma( attrs.markerAttributes() );
    ma.setMarkerStylesMap( map );
    // next: Specify column- / cell-specific attributes!
    const int rowCount = m_lines->model()->rowCount();
    const int colCount = m_lines->model()->columnCount();
    for ( int iColumn = 0; iColumn<colCount; ++iColumn ) {
        // Specify column-specific attributes!
        if ( markersStyleCB->currentIndex() == 0 )
            ma.setMarkerStyle( ma.markerStylesMap().value( iColumn ) );
        else {
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
        }
        if ( checked ) {
            ma.setVisible( true );
            attrs.setVisible(  true );
            if ( paintValuesCB->isChecked() )
                ta.setVisible(  true );
            else
                ta.setVisible(  false );
        }
        else
            ma.setVisible( false );

        ma.setMarkerSize( QSize( markersWidthSB->value(), markersHeightSB->value() ) );
        attrs.setTextAttributes( ta );
        attrs.setMarkerAttributes( ma );
        // column attributes:
        m_lines->setDataValueAttributes( iColumn, attrs );

        // Specify cell-specific attributes for some values!
        for ( int j=0; j< rowCount; ++j ) {
            const QModelIndex index( m_lines->model()->index( j, iColumn, QModelIndex() ) );
            //const QBrush brush( m_lines->brush( index ) );
            const double value = m_lines->model()->data( index ).toDouble();
            /* Set a specific color - marker for a specific value */
            if ( value == 8 ) {
                // retrieve cell specific attributes
                // or fall back to column settings
                // or fall back to global settings:
                DataValueAttributes yellowAttributes( m_lines->dataValueAttributes( index ) );
                MarkerAttributes yellowMarker( yellowAttributes.markerAttributes() );
                yellowMarker.setMarkerColor( Qt::yellow );
                yellowMarker.setMarkerSize( QSize( markersWidthSB->value(), markersHeightSB->value() ) );
                yellowAttributes.setMarkerAttributes( yellowMarker );
                if ( checked ) {
                    yellowMarker.setVisible( true );
                    yellowAttributes.setVisible(  true );
                 }
                else {
                    yellowMarker.setVisible( false );
                    yellowAttributes.setVisible(  false );
                 }
                //cell specific attributes:
                m_lines->setDataValueAttributes( index, yellowAttributes );
            }
        }
    }

    m_chart->update();
}


void MainWindow::on_markersStyleCB_currentIndexChanged( const QString & text )
{
    Q_UNUSED( text );
    if ( paintMarkersCB->isChecked() )
        on_paintMarkersCB_toggled( true );
    else
        on_paintMarkersCB_toggled( false );
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


void MainWindow::on_displayAreasCB_toggled( bool checked )
{
    LineAttributes la( m_lines->lineAttributes() );
    if ( checked  ) {
        la.setDisplayArea( true );
        la.setTransparency( transparencySB->value() );
    } else
        la.setDisplayArea( false );
    m_lines->setLineAttributes( la );
    m_chart->update();
}

void MainWindow::on_transparencySB_valueChanged( int alpha )
{
    LineAttributes la( m_lines->lineAttributes() );
    la.setTransparency( alpha );
    m_lines->setLineAttributes( la );
    on_displayAreasCB_toggled( true );
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

