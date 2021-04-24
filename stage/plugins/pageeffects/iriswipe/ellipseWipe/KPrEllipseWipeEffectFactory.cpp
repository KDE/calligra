/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.org>

   SPDX-License-Identifier: LGPL-2.0-or-later

#include "KPrEllipseWipeEffectFactory.h"

#include <cmath>

#include <klocalizedstring.h>

#include "../KPrIrisWipeEffectStrategyBase.h"

#define EllipseWipeEffectFactoryId "EllipseWipeEffectFactory"

KPrEllipseWipeEffectFactory::KPrEllipseWipeEffectFactory()
: KPrPageEffectFactory( EllipseWipeEffectFactoryId, i18n("Ellipse") )
{
    QPainterPath shape;

    //circle
    shape.addEllipse( -25, -25, 50, 50 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Circle, "ellipseWipe", "circle", false ) );

    //circle reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, CircleReverse, "ellipseWipe", "circle", true ) );

    //horizontal
    shape = QPainterPath();
    shape.addEllipse( -25, -12, 50, 24 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Horizontal, "ellipseWipe", "horizontal", false ) );

    //horizontal reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, HorizontalReverse, "ellipseWipe", "horizontal", true ) );

    //vertical
    shape = QPainterPath();
    shape.addEllipse( -12, -25, 24, 50 );
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, Vertical, "ellipseWipe", "vertical", false ) );

    //vertical reverse
    addStrategy(new KPrIrisWipeEffectStrategyBase( shape, VerticalReverse, "ellipseWipe", "vertical", true ) );
}

KPrEllipseWipeEffectFactory::~KPrEllipseWipeEffectFactory()
{
}

static const char* const s_subTypes[] = {
    I18N_NOOP( "Circle" ),
    I18N_NOOP( "Circle Reverse" ),
    I18N_NOOP( "Horizontal" ),
    I18N_NOOP( "Horizontal Reverse" ),
    I18N_NOOP( "Vertical" ),
    I18N_NOOP( "Vertical Reverse" )
};

QString KPrEllipseWipeEffectFactory::subTypeName(int subType) const
{
    if (subType >= 0 && (uint)subType < sizeof s_subTypes / sizeof s_subTypes[0]) {
        return i18n( s_subTypes[subType] );
    } else {
        return i18n( "Unknown subtype" );
    }
}
