/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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

#include "KoStarShapeFactory.h"
#include "KoStarShape.h"
#include "KoLineBorder.h"

#include <klocale.h>
#include <kgenericfactory.h>

#include <QDebug>

K_EXPORT_COMPONENT_FACTORY(kostarshape,
	         KGenericFactory<KoStarShapeFactory>( "StarShape" ) )

KoStarShapeFactory::KoStarShapeFactory( QObject *parent, const QStringList& )
: KoShapeFactory( parent, KoStarShapeId, i18n( "A star shape" ) )
{
    setToolTip( i18n( "A star" ) );
    setIcon("star");
}

KoShape * KoStarShapeFactory::createDefaultShape()
{
    KoStarShape *star = new KoStarShape();

    star->setBorder( new KoLineBorder( 1.0 ) );
    star->setShapeId( KoPathShapeId );

    return star;
}

KoShape * KoStarShapeFactory::createShape( const KoProperties * params ) const
{
    Q_UNUSED(params);
    return new KoStarShape();
}

