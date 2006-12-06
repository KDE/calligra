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
#include <KoShapeFactory.h>
#include <KoLineBorder.h>
#include <KoProperties.h>

#include "star/KoStarShapeFactory.h"
#include "star/KoStarShape.h"

#include <klocale.h>

KoStarShapeFactory::KoStarShapeFactory( QObject *parent )
    : KoShapeFactory( parent, KoStarShapeId, i18n( "A star shape" ) )
{
    setToolTip( i18n( "A star" ) );
    setIcon("star");

    KoShapeTemplate t;
    t.id = KoPathShapeId;
    t.name = i18n("Star");
    t.toolTip = i18n("A star");
    t.icon = "star";
    KoProperties *props = new KoProperties();
    props->setProperty( "corners", 5 );
    QVariant v;
    v.setValue( QColor( Qt::yellow ) );
    props->setProperty( "background", v );
    t.properties = props;
    addTemplate(t);

    t.id = KoPathShapeId;
    t.name = i18n("Flower");
    t.toolTip = i18n("A flower");
    t.icon = ""; //TODO add it
    props = new KoProperties();
    props->setProperty( "corners", 5 );
    props->setProperty( "baseRadius", 10.0 );
    props->setProperty( "tipRadius", 50.0 );
    props->setProperty( "baseRoundness", 0.0 );
    props->setProperty( "tipRoundness", 40.0 );
    v.setValue( QColor( Qt::magenta ) );
    props->setProperty( "background", v );
    t.properties = props;
    addTemplate(t);

    t.id = KoPathShapeId;
    t.name = "Polygon";
    t.toolTip = "A polygon";
    t.icon = ""; //TODO add it
    props = new KoProperties();
    props->setProperty( "corners", 5 );
    props->setProperty( "convex", true );
    props->setProperty( "tipRadius", 50.0 );
    props->setProperty( "tipRoundness", 0.0 );
    v.setValue( QColor( Qt::blue ) );
    props->setProperty( "background", v );
    t.properties = props;
    addTemplate(t);
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
    KoStarShape *star = new KoStarShape();
    if( ! star )
        return 0;

    star->setCornerCount( params->getInt("corners", 5 ) );
    star->setConvex( params->getBool( "convex", false ) );
    star->setBaseRadius( params->getDouble( "baseRadius", 25.0 ) );
    star->setTipRadius( params->getDouble( "tipRadius", 50.0 ) );
    star->setBaseRoundness( params->getDouble( "baseRoundness", 0.0 ) );
    star->setTipRoundness( params->getDouble( "tipRoundness", 0.0 ) );
    star->setBorder( new KoLineBorder( 1.0 ) );
    star->setShapeId( KoPathShapeId );
    QVariant v;
    if( params->getProperty( "background", v ) )
        star->setBackground( v.value<QColor>() );

    return star;
}

#include "KoStarShapeFactory.moc"
