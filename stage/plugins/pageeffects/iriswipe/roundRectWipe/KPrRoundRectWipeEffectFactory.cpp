// This file is part of the KDE project
// SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrRoundRectWipeEffectFactory.h"

#include <cmath>

#include <klocalizedstring.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define RoundRectWipeEffectFactoryId "RoundRectWipeEffectFactory"

KPrRoundRectWipeEffectFactory::KPrRoundRectWipeEffectFactory()
: KPrPageEffectFactory( RoundRectWipeEffectFactoryId, i18n("RoundRect") )
{
    QPainterPath shape;

    //horizontal
    shape.addRoundedRect( -25, -12, 50, 24, 10, Qt::AbsoluteSize );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Horizontal, "RoundRect", "horizontal", false ) );

    //horizontal reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, HorizontalReverse, "RoundRect", "horizontal", true ) );

    //vertical
    shape = QPainterPath();
    shape.addRoundedRect( -12, -25, 24, 50, 10, Qt::AbsoluteSize );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Vertical, "RoundRect", "vertical", false ) );

    //vertical reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, VerticalReverse, "RoundRect", "vertical", true ) );
}

KPrRoundRectWipeEffectFactory::~KPrRoundRectWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Horizontal Reverse" ),
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Vertical Reverse" )
};

QString KPrRoundRectWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
