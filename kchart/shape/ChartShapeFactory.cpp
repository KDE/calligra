/* This file is part of the KDE project
   Copyright 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "ChartShapeFactory.h"

// Qt
#include <QStringList>
#include <QStandardItemModel>

// KDE
#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>

// KOffice
#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

// Chart shape
#include "ChartToolFactory.h"
#include "ChartConfigWidget.h"
#include "ProxyModel.h"
#include "PlotArea.h"
#include "Axis.h"
#include "Legend.h"

// KDChart
#include <KDChartChart>
#include <KDChartAbstractCartesianDiagram>
#include <KDChartLegend>


using namespace KChart;

K_EXPORT_COMPONENT_FACTORY( chartshape, KGenericFactory<ChartShapePlugin>( "ChartShape" ) )

ChartShapePlugin::ChartShapePlugin( QObject * parent,  const QStringList& )
{
    // Register all the chart shape.factory.
    KoShapeRegistry::instance()->add( new ChartShapeFactory( parent ) );

    // Register all tools for the chart shape.
    KoToolRegistry::instance()->add( new ChartToolFactory( parent ) );
}


ChartShapeFactory::ChartShapeFactory( QObject* parent )
    : KoShapeFactory( parent, ChartShapeId, i18n( "Chart" ) )
{
    qDebug() << "================================ Creating ChartShapeFactory instance";
    setOdfElementNames( "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0", QStringList( "object" ) );
    setToolTip( i18n( "A shape that shows a chart" ) );

    KIconLoader::global()->addAppDir("kchart");
    setIcon( "kchart" );

    // Default 'app specific' config pages i.e. unless an app defines
    // other config pages, these are used.
    QList<KoShapeConfigFactory*> panelFactories;
    // panelFactories.append( new ChartDataConfigFactory() );
    setOptionPanels( panelFactories );
}


bool ChartShapeFactory::supports( const KoXmlElement &element ) const
{
    qDebug() << "===============================================";
    qDebug() << element.namespaceURI();
    qDebug() << element.tagName();
    return element.namespaceURI() == "urn:oasis:names:tc:opendocument:xmlns:drawing:1.0" &&
           element.tagName() == "object";
}

KoShape* ChartShapeFactory::createDefaultShape() const
{
    ChartShape* shape = new ChartShape();

    // Fill cells with data if there is none.
    QStandardItemModel  *m_chartData = new QStandardItemModel();
    m_chartData->setRowCount( 4 );
    m_chartData->setColumnCount( 5 );

    // Insert example data
    for ( uint row = 0; row < 4; ++row ) {
        // The first column has row labels, except the upper left
        // corner which is unused.
        if ( row == 1 )
            m_chartData->setItem( row, 0, new QStandardItem( i18n( "January" ) ) );
        else if( row == 2 )
            m_chartData->setItem( row, 0, new QStandardItem( i18n( "July" ) ) );
        else if( row == 3 )
            m_chartData->setItem( row, 0, new QStandardItem( i18n( "December" ) ) );

        for ( uint col = 1; col < 5; ++col ) {
            if ( row == 0 )
                // First row has column labels
                m_chartData->setData( m_chartData->index( 0, col ),
                                      i18n( "Item %1", col ),
                                      Qt::EditRole | Qt::DisplayRole );
            else
                m_chartData->setData( m_chartData->index( row, col ),
                                      QString::number( row + col ),
                                      Qt::EditRole | Qt::DisplayRole );
        }
    }
    
    const QSizeF shapeSize( CM_TO_POINT( 12 ), CM_TO_POINT( 8 ) );

    // We want the chart shape to take over and handle this model itself
    shape->setFirstRowIsLabel( true );
    shape->setFirstColumnIsLabel( true );
    shape->setModel( m_chartData, true );
    shape->setSize( shapeSize );
    
    QPointF plotAreaPos( 0.0, 0.0 );
    QSizeF plotAreaSize( shapeSize );
    QPointF legendPos( 0.0, 0.0 );
    shape->legend()->kdLegend()->forceRebuild();
    shape->legend()->update();
    QSizeF legendSize = shape->legend()->size();

    Legend *legend = shape->legend();
    if ( legend )
    {
        legendPos.ry() = shapeSize.height() / 2.0 - legendSize.height() / 2.0;
        plotAreaSize.rwidth() -= legendSize.width();
    }

    Axis *xAxis = shape->plotArea()->xAxis();
    KoShape *xAxisTitle = shape->plotArea()->xAxis()->title();
    if ( xAxis )
    {
        xAxis->setTitleText( "Month" );
        xAxisTitle->setPosition( QPointF( shapeSize.width() / 2.0 - xAxisTitle->size().width() / 2.0,
                                          shapeSize.height() - xAxisTitle->size().height() ) );
        plotAreaSize.rheight() -= xAxisTitle->size().height();
    }
    
    Axis *yAxis = shape->plotArea()->yAxis();
    KoShape *yAxisTitle = shape->plotArea()->yAxis()->title();
    if ( yAxis )
    {
        yAxis->setTitleText( "Growth in %" );
        yAxisTitle->setPosition( QPointF( -yAxisTitle->size().width() / 2.0 + yAxisTitle->size().height() / 2.0,
                                          shapeSize.height() / 2.0 - yAxisTitle->size().height() / 2.0 ) );
        plotAreaPos.rx() += yAxisTitle->size().height();
        legendPos.rx() += yAxisTitle->size().height();
        plotAreaSize.rwidth() -= yAxisTitle->size().height();
    }
    
    if ( legend )
    {
        legendPos.rx() += plotAreaSize.width();
        legend->setPosition( legendPos );
    }
    
    shape->plotArea()->setPosition( plotAreaPos );
    shape->plotArea()->setSize( plotAreaSize );

    return shape;
}

KoShape* ChartShapeFactory::createShape( const KoProperties* params ) const
{
    Q_UNUSED( params );
    
    // TODO Use KoProperties to create a custom shape
    return createDefaultShape();
}


QList<KoShapeConfigWidgetBase*> ChartShapeFactory::createShapeOptionPanels()
{
    return QList<KoShapeConfigWidgetBase*>();
}

#include "ChartShapeFactory.moc"
