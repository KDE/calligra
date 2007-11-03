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

// QT
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
#include "ChartShape.h"
#include "ChartTypeToolFactory.h"
#include "ChartLegendToolFactory.h"
#include "ChartTypeConfigWidget.h"
#include "ChartLegendConfigWidget.h"


using namespace KChart;

K_EXPORT_COMPONENT_FACTORY( chartshape, KGenericFactory<ChartShapePlugin>( "ChartShape" ) )

ChartShapePlugin::ChartShapePlugin( QObject * parent,  const QStringList& )
{
    // Register all the chart shape.factory.
    KoShapeRegistry::instance()->add( new ChartShapeFactory( parent ) );

    // Register all tools for the chart shape.
    KoToolRegistry::instance()->add( new ChartTypeToolFactory( parent ) );
    KoToolRegistry::instance()->add( new ChartLegendToolFactory( parent ) );
}


ChartShapeFactory::ChartShapeFactory( QObject* parent )
    : KoShapeFactory( parent, ChartShapeId, i18n( "Chart" ) )
{
    setToolTip( i18n( "A shape that shows a chart" ) );

    KIconLoader::global()->addAppDir("kchart");
    setIcon( "kchart" );

    // Default 'app specific' config pages i.e. unless an app defines
    // other config pages, these are used.
    QList<KoShapeConfigFactory*> panelFactories;
    // panelFactories.append( new ChartDataConfigFactory() );
    setOptionPanels( panelFactories );
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
        if ( row > 0 )
            m_chartData->setItem( row, 0,
                                  new QStandardItem( i18n( "Row %1", row ) ) );

        for ( uint col = 1; col < 5; ++col ) {
            if ( row == 0 )
                // First row has column labels
                m_chartData->setData( m_chartData->index( 0, col ),
                                      i18n( "Column %1", col ),
                                      Qt::EditRole | Qt::DisplayRole );
            else
                m_chartData->setData( m_chartData->index( row, col ),
                                      QString::number( row + col ),
                                      Qt::EditRole | Qt::DisplayRole );
        }
    }

    // We want KChart to take over and handle this model itself
    shape->setInternalModel( m_chartData );
    shape->setFirstRowIsLabel( true );
    shape->setFirstColumnIsLabel( true );
    shape->setSize( QSizeF( CM_TO_POINT( 12 ), CM_TO_POINT( 8 ) ) );

    return shape;
}

KoShape* ChartShapeFactory::createShape( const KoProperties* params ) const
{
    Q_UNUSED( params );

    return new ChartShape();
}


QList<KoShapeConfigWidgetBase*> ChartShapeFactory::createShapeOptionPanels()
{
    QList<KoShapeConfigWidgetBase*>  answer;
    answer.append(new ChartTypeConfigWidget());
    answer.append(new ChartLegendConfigWidget());

    // TODO Stefan: Axes
    return answer;
}

#include "ChartShapeFactory.moc"
