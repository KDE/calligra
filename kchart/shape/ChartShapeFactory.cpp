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

#include <kiconloader.h>
#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

#include "ChartShape.h"
#include "ChartTypeToolFactory.h"
//#include "ChartDataConfigFactory.h"
#include "ChartTypeConfigWidget.h"

using namespace KChart;

K_EXPORT_COMPONENT_FACTORY( chartshape, KGenericFactory<ChartShapePlugin>( "ChartShape" ) )

ChartShapePlugin::ChartShapePlugin( QObject * parent,  const QStringList& )
{
    KoShapeRegistry::instance()->add( new ChartShapeFactory( parent ) );

    KoToolRegistry::instance()->add( new ChartTypeToolFactory( parent ) );
}


ChartShapeFactory::ChartShapeFactory( QObject* parent )
    : KoShapeFactory( parent, ChartShapeId, i18n( "Chart" ) )
{
    setToolTip( i18n( "A shape that shows a chart" ) );
    KIconLoader::global()->addAppDir("kchart");
    setIcon( "kchart" );
    // default 'app specific' config pages
    // i.e. unless an app defines other config pages, these are used.
    QList<KoShapeConfigFactory*> panelFactories;
//     panelFactories.append( new ChartDataConfigFactory() );
    setOptionPanels( panelFactories );
}

KoShape* ChartShapeFactory::createDefaultShape() const
{
    ChartShape* shape = new ChartShape();
    // Fill cells with data if there is none.
    QStandardItemModel *m_chartData = new QStandardItemModel();
    m_chartData->setRowCount( 4 );
    m_chartData->setColumnCount( 4 );

    // Insert example data
    for (uint row = 0; row < 4; row++) {
        for (uint col = 0; col < 4; col++) {

            m_chartData->setItem( row, col,
                  new QStandardItem( QString::number( row + col ) ) );
            // Fill column label, but only on the first iteration.
            if (row == 0) {
                m_chartData->setHeaderData( col, Qt::Horizontal,
                        i18n("Column %1", col + 1) );
            }
        }

        // Fill row label.
        m_chartData->setHeaderData( row, Qt::Vertical,
                    i18n("Row %1", row + 1) );
    }
    shape->setModel( m_chartData );
    return shape;
}

KoShape* ChartShapeFactory::createShape( const KoProperties* params ) const
{
    Q_UNUSED( params );
    ChartShape* shape = new ChartShape();
    return shape;
}

QList<KoShapeConfigWidgetBase*> ChartShapeFactory::createShapeOptionPanels()
{
    kDebug() << "### createShapeOptionPanels";
    QList<KoShapeConfigWidgetBase*> answer;
    answer.append(new ChartTypeConfigWidget());
    // TODO Stefan: SubType
    // TODO Stefan: Labels/Legend
    // TODO Stefan: Axes
    return answer;
}

#include "ChartShapeFactory.moc"
