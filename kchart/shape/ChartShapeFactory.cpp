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

#include <QStringList>

#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

#include "ChartShape.h"
#include "ChartTypeConfigWidget.h"

using namespace KChart;

K_EXPORT_COMPONENT_FACTORY( chartshape, KGenericFactory<ChartShapePlugin>( "ChartShape" ) )

ChartShapePlugin::ChartShapePlugin( QObject * parent,  const QStringList& )
{
    KoShapeRegistry::instance()->add( new ChartShapeFactory( parent ) );
}


ChartShapeFactory::ChartShapeFactory( QObject* parent )
    : KoShapeFactory( parent, ChartShapeId, i18n( "Chart" ) )
{
    setToolTip( i18n( "A shape that shows a chart" ) );
    setIcon( "kchart" );
}

KoShape* ChartShapeFactory::createDefaultShape() const
{
    ChartShape* shape = new ChartShape();
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
    QList<KoShapeConfigWidgetBase*> answer;
    answer.append(new ChartTypeConfigWidget());
    // TODO Stefan: SubType
    // TODO Stefan: Labels/Legend
    // TODO Stefan: Axes
    return answer;
}

#include "ChartShapeFactory.moc"
