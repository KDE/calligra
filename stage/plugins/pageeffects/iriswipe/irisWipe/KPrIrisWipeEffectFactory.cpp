/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrIrisWipeEffectFactory.h"

//KF5 includes
#include <klocalizedstring.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define IrisWipeEffectId "IrisWipeEffect"

KPrIrisWipeEffectFactory::KPrIrisWipeEffectFactory()
: KPrPageEffectFactory( IrisWipeEffectId, i18n("Iris") )
{

     QPainterPath shape;

    //rectangle
    shape.addRect( -25, -25, 50, 50 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Rectangle, "irisWipe", "rectangle", false ) );

    //rectangle reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, RectangleReverse, "irisWipe", "rectangle", true ) );

    //diamond
    shape = QPainterPath();
    shape.moveTo( 0, -25);
    shape.lineTo( 25, 0 );
    shape.lineTo( 0, 25 );
    shape.lineTo(-25, 0 );
    shape.closeSubpath();
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Diamond, "irisWipe", "diamond", false ) );

    //diamond reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, DiamondReverse, "irisWipe", "diamond", true ) );
}

KPrIrisWipeEffectFactory::~KPrIrisWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Rectangular" ),
    I18N_NOOP( "Rectangular Reverse" ),
    I18N_NOOP( "Diamond" ),
    I18N_NOOP( "Diamond Reverse" )
};

QString KPrIrisWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
