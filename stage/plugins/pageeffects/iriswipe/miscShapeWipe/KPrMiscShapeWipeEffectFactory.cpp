
/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>

   SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrMiscShapeWipeEffectFactory.h"

#include <klocalizedstring.h>

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

static const char* const s_subTypes[] = {
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
