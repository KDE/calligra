/* This file is part of the KDE project
   Copyright 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; only
   version 2 of the License.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/
#include <QStringList>

#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

#include "TableShape.h"
#include "TableToolFactory.h"

#include "TableShapeFactory.h"

using namespace KSpread;

K_EXPORT_COMPONENT_FACTORY( tableshape, KGenericFactory<KSpread::TableShapePlugin>( "TableShape" ) )

TableShapePlugin::TableShapePlugin( QObject * parent,  const QStringList & list )
{
    KoShapeRegistry::instance()->add( new TableShapeFactory( parent, list ) );
#if 0
    KoToolRegistry::instance()->add( new TableToolFactory( parent, list ) );
# endif

}


TableShapeFactory::TableShapeFactory( QObject* parent, const QStringList& list )
    : KoShapeFactory( parent, TableShapeId, i18n( "Table" ) )
{
    setToolTip( i18n( "A shape that shows a table" ) );
    setIcon( "tableshape" );

#if 0
    KoShapeTemplate t;
    t.name = "Simple table";
    t.toolTip = "Table shape containing some cells";
    KoProperties *props = new KoProperties();
    t.properties = props;
    props->setProperty( "columns", 2 );
    props->setProperty( "rows", 2 );
    addTemplate( t );


#endif
}

KoShape* TableShapeFactory::createDefaultShape() const
{
    TableShape* shape = new TableShape();
    shape->setShapeId( shapeId() );
    return shape;
}

KoShape* TableShapeFactory::createShape( const KoProperties* params ) const
{
    TableShape* shape = new TableShape();
    shape->setShapeId( shapeId() );
    return shape;
}

#include "TableShapeFactory.moc"
