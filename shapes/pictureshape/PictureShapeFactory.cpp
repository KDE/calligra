/* This file is part of the KDE project
   Copyright 2007 Montel Laurent <montel@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

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

#include "PictureShape.h"
#include "PictureToolFactory.h"

#include "PictureShapeFactory.h"

K_EXPORT_COMPONENT_FACTORY( pictureshape, KGenericFactory<PictureShapePlugin>( "PictureShape" ) )

PictureShapePlugin::PictureShapePlugin( QObject * parent,  const QStringList & list )
{
    KoShapeRegistry::instance()->add( new PictureShapeFactory( parent, list ) );
    KoToolRegistry::instance()->add( new PictureToolFactory( parent, list ) );
}


PictureShapeFactory::PictureShapeFactory( QObject* parent, const QStringList& list )
    : KoShapeFactory( parent, PictureShapeId, i18n( "Picture Shape" ) )
{
    setToolTip( i18n( "A Shape which display a picture" ) );
    setIcon( "pictureshape" );

}

KoShape* PictureShapeFactory::createDefaultShape() const
{
    PictureShape* shape = new PictureShape();
    return shape;
}

KoShape* PictureShapeFactory::createShape( const KoProperties* params ) const
{
    PictureShape* shape = new PictureShape();
    return shape;
}

#include "PictureShapeFactory.moc"

