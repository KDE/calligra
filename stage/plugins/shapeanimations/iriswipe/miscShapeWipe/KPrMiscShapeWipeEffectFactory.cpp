
/* This file is part of the KDE project
   Copyright (C) 2008 Carlos Licea <carlos.licea@kdemail.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software itation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software itation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrMiscShapeWipeEffectFactory.h"

#include <klocale.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define MiscShapeWipeEffectFactoryId "MiscShapeWipeEffect"

KPrMiscShapeWipeEffectFactory::KPrMiscShapeWipeEffectFactory()
: KPrPageEffectFactory( MiscShapeWipeEffectFactoryId, i18n("Misc") )
{
     QPainterPath shape;

    //heart
//     shape.moveTo( 0, 0 );
//     shape.quadTo( -6, -12, -12, 0 );
//     shape.lineTo( 0, 12 );
//     shape.lineTo( 12, 0 );
//     shape.quadTo( 6, -12, 0, 0 );
//     addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Heart, "miscShapeWipe", "heart", false ) );

    //keyhole
    shape = QPainterPath();
    shape.moveTo( 0, -12 );
    shape.lineTo( -6, 12 );
    shape.lineTo( 6, 12 );
    shape.closeSubpath();
    QPainterPath shape2;
    shape2.addEllipse( -6, -12, 12, 12 );
    shape = shape.united( shape2 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Keyhole, "miscShapeWipe", "keyhole", false ) );

    //keyhole reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, KeyholeReverse, "miscShapeWipe", "keyhole", true ) );
}

KPrMiscShapeWipeEffectFactory::~KPrMiscShapeWipeEffectFactory()
{
}

static const char* s_subTypes[] = {
    I18N_NOOP( "Heart" ),
    I18N_NOOP( "Heart Reverse" ),
    I18N_NOOP( "Keyhole" ),
    I18N_NOOP( "Keyhole Reverse" )
};

QString KPrMiscShapeWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
