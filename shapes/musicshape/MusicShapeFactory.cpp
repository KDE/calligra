/* This file is part of the KDE project
 * Copyright 2007 Marijn Kruisselbrink <m.kruiselbrink@student.tue.nl>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include <QStringList>

#include <kgenericfactory.h>
#include <klocale.h>

#include <KoProperties.h>
#include <KoToolRegistry.h>
#include <KoShapeRegistry.h>

#include "MusicShape.h"
#include "MusicToolFactory.h"

#include "MusicShapeFactory.h"

K_EXPORT_COMPONENT_FACTORY( musicshape, KGenericFactory<MusicShapePlugin>( "MusicShape" ) )

MusicShapePlugin::MusicShapePlugin( QObject * parent,  const QStringList & list )
{
    KoShapeRegistry::instance()->add( new MusicShapeFactory( parent ) );
    KoToolRegistry::instance()->add( new MusicToolFactory( parent ) );
}


MusicShapeFactory::MusicShapeFactory( QObject* parent )
    : KoShapeFactory( parent, MusicShapeId, i18n( "Music Shape" ) )
{
    setToolTip( i18n( "A shape which provides a music editor" ) );
    setIcon( "musicflake" );

}

KoShape* MusicShapeFactory::createDefaultShape() const
{
    MusicShape* shape = new MusicShape();
    return shape;
}

KoShape* MusicShapeFactory::createShape( const KoProperties* params ) const
{
    MusicShape* shape = new MusicShape();
    return shape;
}

#include "MusicShapeFactory.moc"
