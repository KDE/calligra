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

    modifiers = "10 150";

    commands.clear();
    commands.append( "M ?1w8 0" );
    commands.append( "X 0 ?1h8" );
    commands.append( "L ?f2 ?f3 0 ?3h8 0 ?5h8 ?f4 ?f5 0 ?7h8" );
    commands.append( "Y ?1w8 height" );
    commands.append( "L ?f6 ?f7 ?3w8 height ?5w8 height ?f8 ?f9 ?7w8 height" );
    commands.append( "X width ?7h8" );
    commands.append( "L ?f10 ?f11 width ?5h8 width ?3h8 ?f12 ?f13 width ?1h8" );
    commands.append( "Y ?7w8 0" );
    commands.append( "L ?f14 ?f15 ?5w8 0 ?3w8 0 ?f16 ?f17" );
    commands.append( "Z" );
    commands.append( "N" );

    handles.clear();
    handles.append( "$0 $1" );

    formulae.clear();
    formulae["1h8"] = "0.125 * height";
    formulae["3h8"] = "0.400 * height";
    formulae["5h8"] = "0.600 * height";
    formulae["7h8"] = "0.875 * height";
    formulae["1w8"] = "0.125 * width";
    formulae["3w8"] = "0.400 * width";
    formulae["5w8"] = "0.600 * width";
    formulae["7w8"] = "0.875 * width";

    formulae["f0"] = "$0 - 0.5 * width";
    formulae["f1"] = "$1 - 0.5 * height";

    formulae["f2"] = "if(?f18 , $0 , 0)";    // x2
    formulae["f3"] = "if(?f18 , $1 , 0.2625*height )";   // y2

    formulae["f4"] = "if(?f23 , $0 , 0)";       // x5
    formulae["f5"] = "if(?f23 , $1 , 0.7375*height )";  // y5

    formulae["f6"] = "if(?f26 , $0 , 0.2625*width )";   // x8
    formulae["f7"] = "if(?f26 , $1 , height )";  // y8

    formulae["f8"] = "if(?f29 , $0 , 0.7375*width )";  // x11
    formulae["f9"] = "if(?f29 , $1 , height )";  // y11

    formulae["f10"] = "if(?f32 , $0 , width)"; // x14
    formulae["f11"] = "if(?f32 , $1 , 0.7375*height )"; // y14

    formulae["f12"] = "if(?f34 , $0 , width )"; // x17
    formulae["f13"] = "if(?f34 , $1 , 0.2625*height )";  // y17

    formulae["f14"] = "if(?f36 , $0 , 0.7375*width )"; // x20
    formulae["f15"] = "if(?f36 , $1 , 0)";      // y20

    formulae["f16"] = "if(?f38 , $0 , 0.2625*width )";  // x23
    formulae["f17"] = "if(?f38 , $1 , 0)";      // y23

    formulae["f18"] = "if($0 , -1, ?f19 )";
    formulae["f19"] = "if(?f1 , -1, ?f22 )";
    formulae["f20"] = "abs( ?f0 )";
    formulae["f21"] = "abs( ?f1 )";
    formulae["f22"] = "?f20 - ?f21";
    formulae["f23"] = "if( $0, -1, ?f24 )";
    formulae["f24"] = "if( ?f1, ?f22, -1 )";
    formulae["f25"] = "$1 - height";
    formulae["f26"] = "if( ?f25, ?f27, -1 )";
    formulae["f27"] = "if( ?f0, -1, ?f28 )";
    formulae["f28"] = "?f21 - ?f20";
    formulae["f29"] = "if( ?f25, ?f30, -1 )";
    formulae["f30"] = "if( ?f0, ?f28, -1 )";
    formulae["f31"] = "$0 - width";
    formulae["f32"] = "if( ?f31, ?f33, -1)";
    formulae["f33"] = "if( ?f1, ?f22, -1)";
    formulae["f34"] = "if( ?f31, ?f35, -1)";
    formulae["f35"] = "if( ?f1, -1, ?f22 )";
    formulae["f36"] = "if( $1, -1, ?f37 )";
    formulae["f37"] = "if( ?f0, ?f28, -1 )";
    formulae["f38"] = "if( $1, -1, ?f39 )";
    formulae["f39"] = "if( ?f0, -1, ?f28 )";
    formulae["f40"] = "$0";
    formulae["f41"] = "$1";

    t.name = i18n("Enhanced Path");
    t.toolTip = i18n("A callout");
    t.icon = "callout";
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
