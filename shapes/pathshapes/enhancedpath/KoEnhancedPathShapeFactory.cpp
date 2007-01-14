/* This file is part of the KDE project
 * Copyright (C) 2007 Jan Hambrecht <jaham@gmx.net>
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

#include "enhancedpath/KoEnhancedPathShapeFactory.h"
#include "enhancedpath/KoEnhancedPathShape.h"

#include <KoLineBorder.h>
#include <KoProperties.h>

#include <klocale.h>
#include <kdebug.h>

#include <QString>
#include <QMap>

KoEnhancedPathShapeFactory::KoEnhancedPathShapeFactory( QObject *parent )
    : KoShapeFactory( parent, KoEnhancedPathShapeId, i18n( "An enhanced path shape" ) )
{
    setToolTip( i18n( "An enhanced path" ) );
    setIcon("enhancedpath");

    QString modifiers( "35" );
    QStringList commands;
    commands.append( "M $0 0" );
    commands.append( "L ?Right 0 ?Right $0 width $0 width ?Bottom ?Right ?Bottom" );
    commands.append( "L ?Right height $0 height $0 ?Bottom 0 ?Bottom 0 $0 $0 $0" );
    commands.append( "Z" );

    QStringList handles;
    handles.append( "$0 0 0 ?Half" );

    Formulae formulae;
    formulae["Right"] = "width - $0";
    formulae["Bottom"] = "height - $0";
    formulae["Half"] = "min( 0.5 * height, 0.5 * width )";

    KoShapeTemplate t;
    t.id = KoPathShapeId;
    t.name = i18n("Enhanced Path");
    t.toolTip = i18n("A cross");
    t.icon = "cross";
    t.properties = dataToProperties( modifiers, commands, handles, formulae );

    addTemplate(t);

    modifiers = "60 35";

    commands.clear();
    commands.append( "M $0 $1" );
    commands.append( "L $0 0 width ?HalfHeight $0 height $0 ?LowerCorner 0 ?LowerCorner 0 $1" );
    commands.append( "Z" );

    handles.clear();
    handles.append( "$0 $1 0 width 0 ?HalfHeight" );

    formulae.clear();
    formulae["HalfHeight"] = "0.5 * height";
    formulae["LowerCorner"] = "height - $1";

    t.name = i18n("Enhanced Path");
    t.toolTip = i18n("An arrow");
    t.icon = "arrow";
    t.properties = dataToProperties( modifiers, commands, handles, formulae );

    addTemplate(t);
}

KoShape * KoEnhancedPathShapeFactory::createDefaultShape() const
{
    KoEnhancedPathShape *shape = new KoEnhancedPathShape();
    shape->setBorder( new KoLineBorder( 1.0 ) );
    shape->setShapeId( KoPathShapeId );

    shape->addModifiers( "35" );

    shape->addFormula( "Right", "width - $0" );
    shape->addFormula( "Bottom", "height - $0" );
    shape->addFormula( "Half", "min( 0.5 * height, 0.5 * width )" );

    shape->addCommand( "M $0 0" );
    shape->addCommand( "L ?Right 0 ?Right $0 width $0 width ?Bottom ?Right ?Bottom" );
    shape->addCommand( "L ?Right height $0 height $0 ?Bottom 0 ?Bottom 0 $0 $0 $0" );
    shape->addCommand( "Z" );

    shape->addHandle("$0 0 0 ?Half" );
    shape->resize( QSize(100, 100) );

    return shape;
}

KoShape * KoEnhancedPathShapeFactory::createShape(const KoProperties * params) const
{
    KoEnhancedPathShape *shape = new KoEnhancedPathShape();
    if( ! shape )
        return 0;

    shape->setBorder( new KoLineBorder( 1.0 ) );
    shape->setShapeId( KoPathShapeId );

    shape->addModifiers( params->getString( "modifiers" ) );

    int handleCount = params->getInt( "handleCount" );
    for( int i = 0; i < handleCount; ++i )
        shape->addHandle( params->getString( QString( "handle%1").arg( i ) ) );

    int commandCount = params->getInt( "commandCount" );
    for( int i = 0; i < commandCount; ++i )
        shape->addCommand( params->getString( QString( "command%1").arg( i ) ) );

    int formulaCount = params->getInt( "formulaCount" );
    for( int i = 0; i < formulaCount; ++i )
    {
        QString name = params->getString( QString( "formula%1" ).arg( i ) );
        shape->addFormula( name, params->getString( name ) );
    }

    QVariant color;
    if( params->getProperty( "background", color ) )
        shape->setBackground( color.value<QColor>() );

    shape->resize( QSize(100, 100) );

    return shape;
}

KoProperties* KoEnhancedPathShapeFactory::dataToProperties(
    const QString &modifiers, const QStringList &commands,
    const QStringList &handles, const Formulae & formulae ) const
{
    KoProperties *props = new KoProperties();
    props->setProperty( "modifiers", modifiers );

    props->setProperty( "commandCount", commands.count() );
    for( int i = 0; i < commands.count(); ++i )
        props->setProperty( QString("command%1").arg( i ), QVariant( commands[i] ) );

    props->setProperty( "handleCount", handles.count() );
    for( int i = 0; i < handles.count(); ++i )
        props->setProperty( QString("handle%1").arg( i ), QVariant( handles[i] ) );

    Formulae::const_iterator formula = formulae.begin();
    Formulae::const_iterator lastFormula = formulae.end();
    props->setProperty( "formulaCount", formulae.count() );
    int i = 0;
    for( ; formula != lastFormula; ++formula )
    {
        props->setProperty( QString("formula%1").arg( i++ ), formula.key() );
        props->setProperty( formula.key(), formula.value() );
    }

    QVariant color;
    color.setValue( QColor( Qt::red ) );
    props->setProperty( "background", color );

    return props;
}
#include "KoEnhancedPathShapeFactory.moc"
